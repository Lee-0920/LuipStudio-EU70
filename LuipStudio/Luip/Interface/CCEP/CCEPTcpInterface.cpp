#include <QDebug>
#include <QDateTime>
#include "Log.h"
#include "LuaEngine/LuaEngine.h"
#include "ResultManager/ResultManager.h"
#include "CCEPTcpInterface.h"

using namespace std;
using namespace System;
using namespace OOLUA;
using namespace Lua;
using namespace Result;

namespace Interface
{
namespace CCEP
{

unique_ptr<CCEPTcpInterface> CCEPTcpInterface::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 CCEPInterface 对象（单件）。
 */
CCEPTcpInterface* CCEPTcpInterface::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new CCEPTcpInterface);
    }

    return m_instance.get();
}

CCEPTcpInterface::CCEPTcpInterface():m_dataUpdated(false)
{

}

CCEPTcpInterface::~CCEPTcpInterface()
{

}

bool CCEPTcpInterface::Init()
{
    bool status = false;
    bool CCEPMode = false;
//    int hj212Type = 0;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(state, "config.system.CCEPMode", CCEPMode);

        String strIp;
        luaEngine->GetLuaValue(state, "config.system.CCEPAddress", strIp);
        m_ip = strIp;

        int nPort = 5555;
        luaEngine->GetLuaValue(state, "config.system.CCEPPort", nPort);
        m_port = nPort;

//        luaEngine->GetLuaValue(state, "setting.measureResult.decimalNum", m_decimalNum);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("CCEPTcpInterface::Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("CCEPTcpInterface::Init() => %s", e.what());
    }

    if (CCEPMode) //开启认证模式
    {
        m_mainState = LuaEngine::Instance()->GetMainState();
        m_newState = LuaEngine::Instance()->CreateThread();

        this->Start();
        status = true;

        ResultManager::Instance()->Register(this);
    }

    return status;
}

void CCEPTcpInterface::Start()
{
    m_isRuning = true;
    start();
}

void CCEPTcpInterface::Stop()
{
    m_isRuning = false;
}

void CCEPTcpInterface::run()
{
    logger->debug("CCEPTcpInterface Thread ID = %d", QThread::currentThreadId());
    LuaEngine::Instance()->AttachCurrentThread(m_newState);

    m_tcpSocket = new QTcpSocket;

    m_tcpSocket->connectToHost(m_ip.data(), m_port, QIODevice::ReadWrite);
    if(!m_tcpSocket->waitForConnected(1000))
    {
        logger->warn("CCEPTcpInterface connect to %s fail!", m_ip.data());
        m_tcpSocket->close();
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

    if (m_tcpSocket)
    {
        m_tcpSocket->close();
        delete m_tcpSocket;
        m_tcpSocket = nullptr;
    }

    logger->debug("CCEPTcpInterface Thread exit");
}

void CCEPTcpInterface::OnMeasureResultAdded(String name, ResultData::RecordData result)
{
   (void) name;
   (void) result;
   m_dataUpdated = true;
}

void CCEPTcpInterface::OnCalibrateResultAdded(String name, ResultData::RecordData result)
{
    (void) name;
    (void) result;
}

void CCEPTcpInterface::UpReportMessage(MagType type)
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
            logger->warn("CCEPTcpInterface::UpReportData() => get 'status' table failed");
        }

        lua_pushstring(m_newState , "measure");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("CCEPTcpInterface::UpReportData() => get 'measure' table failed");
        }

        lua_pushstring(m_newState , "report");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("CCEPTcpInterface::UpReportData() => get 'report' table failed");
        }

        lua_pushstring(m_newState , "measure");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("CCEPTcpInterface::UpReportData() => get 'measure' table failed");
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
        logger->warn("CCEPTcpInterface::UpReportMessage() => %s", e.what());
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

    // 连接无效，重连
    if (QAbstractSocket::UnconnectedState == m_tcpSocket->state())
    {
        m_tcpSocket->connectToHost(m_ip.data(), m_port, QIODevice::ReadWrite);
        m_tcpSocket->waitForConnected(1000);
    }

    // 连接有效，发送数据
    if (QAbstractSocket::ConnectedState == m_tcpSocket->state())
    {
        int len = m_tcpSocket->write(magArray);
        m_tcpSocket->waitForBytesWritten(5000);
        if(len < 0)
        {
            logger->warn("CCEPTcpInterface write message fail! error = %d", (int)m_tcpSocket->error());
        }
    }
}

Uint16 CCEPTcpInterface::CRC(const char *pBuf, int nLen)
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
