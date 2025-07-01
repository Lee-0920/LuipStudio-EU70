#include <QDebug>
#include <QDateTime>
#include "Log.h"
#include "LuaEngine/LuaEngine.h"
#include "ResultManager/ResultManager.h"
#include "CCEPInterface.h"
#include "HJ212Manager/HJ212Manager.h"
using namespace std;
using namespace System;
using namespace OOLUA;
using namespace Lua;
using namespace Result;

namespace Interface
{
namespace CCEP
{

unique_ptr<CCEPInterface> CCEPInterface::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 CCEPInterface 对象（单件）。
 */
CCEPInterface* CCEPInterface::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new CCEPInterface);
    }

    return m_instance.get();
}

CCEPInterface::CCEPInterface():m_dataUpdated(false)
{

}

CCEPInterface::~CCEPInterface()
{

}

bool CCEPInterface::Init()
{
    bool status = false;
    bool CCEPMode = false;
    int hj212Type = (int)HJ212::HJ212Type::None;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(state, "config.system.CCEPMode", CCEPMode);
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.hj212Type", hj212Type);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("CCEPInterface::Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("CCEPInterface::Init() => %s", e.what());
    }

    if (CCEPMode && hj212Type == (int)HJ212::HJ212Type::None) //开启认证模式
    {
        m_mainState = LuaEngine::Instance()->GetMainState();
        m_newState = LuaEngine::Instance()->CreateThread();

        this->Start();
        status = true;

        ResultManager::Instance()->Register(this);
    }

    return status;
}

void CCEPInterface::Start()
{
    m_isRuning = true;
    start();
}

void CCEPInterface::Stop()
{
    m_isRuning = false;
}

void CCEPInterface::run()
{
    logger->debug("CCEPInterface Thread ID = %d", QThread::currentThreadId());
    LuaEngine::Instance()->AttachCurrentThread(m_newState);

    m_serialPort = new QSerialPort;

#ifdef _CS_X86_WINDOWS
    m_serialPort->setPortName("COM6");
#endif

#ifdef _CS_ARM_LINUX
    m_serialPort->setPortName("/dev/ttySAC1");
#endif

    if (m_serialPort->open(QIODevice::ReadWrite))
    {
        m_serialPort->setBaudRate(QSerialPort::Baud9600);
        m_serialPort->setDataBits(QSerialPort::Data8);
        m_serialPort->setParity(QSerialPort::NoParity);
        m_serialPort->setStopBits(QSerialPort::OneStop);
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    }
    else
    {
        logger->warn("CCEP serial port initialize fail");
    }

    int cnt = 0;
    bool isHourSend = true;
    m_dataUpdated = true; //第一次上电，先上报

    while(m_isRuning)
    {
        int minute = QTime::currentTime().minute();

        // 发送心跳包
        if (cnt >= 5)
        {
            cnt = 0;
            this->UpReportMessage(MagType::hend);
        }

        cnt++;

        // 发送数据包
        if (m_dataUpdated)  //数据更新发送
        {
            this->UpReportMessage(MagType::data);
            m_dataUpdated = false;
        }
        else if (minute <= 2 && isHourSend)  //整点发送数据
        {
            isHourSend = false;
            this->UpReportMessage(MagType::data);
        }
        else if (minute > 5)  //整点发送数据
        {
             isHourSend = true;
        }

        QThread::sleep(60);
    }

    if (m_serialPort)
    {
        m_serialPort->close();
        delete m_serialPort;
        m_serialPort = nullptr;
    }

    logger->debug("CCEPInterface Thread exit");
}

void CCEPInterface::OnMeasureResultAdded(String name, ResultData::RecordData result)
{
   (void) name;
   (void) result;
   m_dataUpdated = true;
}

void CCEPInterface::OnCalibrateResultAdded(String name, ResultData::RecordData result)
{
    (void) name;
    (void) result;
}

void CCEPInterface::UpReportMessage(MagType type)
{
    unsigned int times = 0;
    float consistency = 0;
    int deviceTypeNum = 0;

    try
    {
        lua_settop(m_newState ,0);//清空栈
        lua_getglobal(m_newState, "status");

        int status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("CCEPInterface::UpReportMessage() => get 'status' table failed");
        }

        lua_pushstring(m_newState , "measure");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("CCEPInterface::UpReportMessage() => get 'measure' table failed");
        }

        lua_pushstring(m_newState , "report");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("CCEPInterface::UpReportMessage() => get 'report' table failed");
        }

        lua_pushstring(m_newState , "measure");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("CCEPInterface::UpReportMessage() => get 'measure' table failed");
        }

        lua_pushstring(m_newState , "dateTime");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_isnumber(m_newState ,-1);//取栈顶
        if (status == 1)
        {
            times = lua_tonumber(m_newState ,-1);
        }

        lua_pushstring(m_newState , "consistency");
        lua_gettable(m_newState ,-3); //取栈顶下一个元素

        status = lua_isnumber(m_newState ,-1);//取栈顶
        if (status == 1)
        {
            consistency = lua_tonumber(m_newState ,-1);
        }

        lua_pushstring(m_newState , "deviceTypeNum");
        lua_gettable(m_newState ,-4); //取栈顶下一个元素

        status = lua_isnumber(m_newState ,-1);//取栈顶
        if (status == 1)
        {
            deviceTypeNum = lua_tonumber(m_newState ,-1);
        }

    }
    catch(std::exception e)
    {
        logger->warn("CCEPInterface::UpReportMessage() => %s", e.what());
    }

    QDateTime dateTime = QDateTime::fromTime_t(times);

    int year = dateTime.date().year();
    int month = dateTime.date().month();
    int day = dateTime.date().day();
    int hour = dateTime.time().hour();
    int min = dateTime.time().minute();
    int sec = dateTime.time().second();

    QByteArray magArray;
    QString str  = "";
    QString mag  = "";
    int strLen = 0;
    Uint16 crc = 0;

    QString qnStr = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");

    LuaEngine* luaEngine = LuaEngine::Instance();

    String pw = "123456";
    String mn = "A0000000000000";

    Table sysTable;
    luaEngine->GetLuaValue(m_newState, "config.system", sysTable);
    sysTable.safe_at("pw", pw);
    sysTable.safe_at("mn", mn);

    if (type == MagType::data)
    {
        str.sprintf("QN=%s;ST=21;CN=2061;PW=%s;MN=%s;Flag=9;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;w%05d-Avg=%.3f,w%05d-Flag=N&&",
                    qnStr.toStdString().c_str(),pw.c_str(),mn.c_str(),year, month, day, hour, min, sec, deviceTypeNum, consistency, deviceTypeNum);
    }
    else
    {
        str.sprintf("QN=%s;ST=21;CN=9015;PW=%s;MN=%s;Flag=9;CP=&&&&",
                    qnStr.toStdString().c_str(),pw.c_str(),mn.c_str());
    }
    magArray = str.toLatin1();

    const char *cStr = magArray.data();

    strLen = strlen(cStr);
    crc = this->CRC(cStr, strLen);

    mag.sprintf("##%04d%s%04X\r\n", strLen, cStr, crc);

    magArray = mag.toLatin1();

    m_serialPort->write(magArray);
    m_serialPort->waitForBytesWritten(5000);
}

Uint16 CCEPInterface::CRC(const char *pBuf, int nLen)
{
    int i = 0;
    char j = 0;
    uint16_t code = 0xFFFF;

    for(i = 0; i < nLen; i++)
    {
        code = (code>>8)^pBuf[i];
        for(j = 0; j < 8; j++)
        {
            if(code & 1)
            {
                code>>=1;
                code^=0xA001;
            }
            else
            {
                code>>=1;
            }
        }
    }

    return code;
}

}

}
