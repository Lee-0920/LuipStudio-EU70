#include <QDebug>
#include <QDateTime>
#include "Log.h"
#include "LuaEngine/LuaEngine.h"
#include "ResultManager/ResultManager.h"
#include "HJ212Interface.h"

using namespace std;
using namespace System;
using namespace OOLUA;
using namespace Lua;
using namespace Result;

namespace Interface
{
namespace HJ212
{

unique_ptr<HJ212Interface> HJ212Interface::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 CCEPInterface 对象（单件）。
 */
HJ212Interface* HJ212Interface::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new HJ212Interface);
    }

    return m_instance.get();
}

HJ212Interface::HJ212Interface():m_hj212Type(HJ212Type::None),m_hj212Interval(10),m_decimalNum(3),m_dataUpdated(false)
{

}

HJ212Interface::~HJ212Interface()
{

}

bool HJ212Interface::Init()
{
    m_mainState = LuaEngine::Instance()->GetMainState();
    m_newState = LuaEngine::Instance()->CreateThread();
    m_hj212Type = HJ212Type::V2005;
    m_hj212CommunicationMode = 0;

    try
    {
        int hj212DataType = 0;
        int hj212Interval = 0;

        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        Table sysTable;
        luaEngine->GetLuaValue(m_newState, "config.system.hj212Platform", sysTable);
        if (sysTable.safe_at("hj212DataType", hj212DataType))
        {
            m_hj212DataType = (HJ212DataType)hj212DataType;
        }
        else
        {
            m_hj212DataType = HJ212DataType::HourData;
        }

        if (sysTable.safe_at("heartPackTime", hj212Interval))
        {
            m_hj212Interval = hj212Interval;
        }
        else
        {
            m_hj212Interval = 10;
        }

        luaEngine->GetLuaValue(state, "config.system.hj212Platform.hj212CommunicationMode", m_hj212CommunicationMode);

        luaEngine->GetLuaValue(state, "setting.measureResult.decimalNum", m_decimalNum);

        Table itermsTable;
        luaEngine->GetLuaValue(state, "setting.measureResult", itermsTable);

        OOLUA::Lua_func_ref getDecimalNum;
        if (itermsTable.safe_at("getDecimalNum", getDecimalNum))
        {
            unsigned int decimalNum = 0;
            OOLUA::Lua_function call(state);
            call(getDecimalNum);
            OOLUA::pull(state, decimalNum);
            if(m_decimalNum != decimalNum)
            {
                m_decimalNum = decimalNum;
            }
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("HJ212Interface::Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Interface::Init() => %s", e.what());
    }

    this->Start();
    ResultManager::Instance()->Register(this);

    return true;
}

void HJ212Interface::Start()
{
    m_isRuning = true;
    start();
}

void HJ212Interface::Stop()
{
    m_isRuning = false;
}

void HJ212Interface::run()
{
    logger->debug("HJ212Interface Thread ID = %d", QThread::currentThreadId());
    LuaEngine::Instance()->AttachCurrentThread(m_newState);

    int parity = 0;
    int baudRate = 0;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();
        Table interconnection;
        luaEngine->GetLuaValue(state, "config.interconnection", interconnection);

        if (0 == m_hj212CommunicationMode)
        {
            interconnection.at("RS232Parity", parity);
            interconnection.at("RS232BaudRate", baudRate);
        }
        else
        {
            interconnection.at("RS485Parity", parity);
            interconnection.at("RS485BaudRate", baudRate);
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("HJ212Interface::Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Interface::Init() => %s", e.what());
    }

    m_serialPort = new QSerialPort;

#ifdef _CS_X86_WINDOWS
    m_serialPort->setPortName("COM4");
#endif

#ifdef _CS_ARM_LINUX
    if (0 == m_hj212CommunicationMode)
    {
        m_serialPort->setPortName("/dev/ttymxc3");
    }
    else
    {
        m_serialPort->setPortName("/dev/ttymxc4");
    }
#endif

    if (m_serialPort->open(QIODevice::ReadWrite))
    {
        switch (baudRate)
        {
        case 0:
            m_serialPort->setBaudRate(QSerialPort::Baud9600);
            break;
        case 1:
            m_serialPort->setBaudRate(QSerialPort::Baud19200);
            break;
        case 2:
            m_serialPort->setBaudRate(QSerialPort::Baud38400);
            break;
        case 3:
            m_serialPort->setBaudRate(QSerialPort::Baud115200);
            break;
        case 4:
            //Qt自带串口没有230400
            m_serialPort->setBaudRate(QSerialPort::Baud115200);
            break;
        default:
            m_serialPort->setBaudRate(QSerialPort::Baud9600);
            break;
        }

        switch (parity)
        {
        case 0:
            m_serialPort->setParity(QSerialPort::NoParity);
            break;
        case 1:
            m_serialPort->setParity(QSerialPort::OddParity);
            break;
        case 2:
            m_serialPort->setParity(QSerialPort::EvenParity);
            break;
        default:
            m_serialPort->setParity(QSerialPort::NoParity);
            break;
        }

        m_serialPort->setDataBits(QSerialPort::Data8);
        m_serialPort->setStopBits(QSerialPort::OneStop);
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    }
    else
    {
        logger->warn("HJ212 serial port initialize fail");
    }

    m_dataUpdated = true; //第一次上电，先上报

    int minute = 0;
    while(m_isRuning)
    {
        if (m_dataUpdated)  //数据更新发送
        {
            this->UpReportData();
            m_dataUpdated = false;
        }
        else if (minute >= m_hj212Interval && m_hj212Interval > 0)  //周期发送数据
        {
            this->UpReportData();
            minute = 0;
        }

        QThread::sleep(60);

        minute++;
    }

    if (m_serialPort)
    {
        m_serialPort->close();
        delete m_serialPort;
        m_serialPort = nullptr;
    }

    logger->debug("HJ212Interface Thread exit");
}

void HJ212Interface::OnMeasureResultAdded(String name, ResultData::RecordData result)
{
   (void) name;
   (void) result;
   m_dataUpdated = true;
}

void HJ212Interface::OnCalibrateResultAdded(String name, ResultData::RecordData result)
{
    (void) name;
    (void) result;
}

void HJ212Interface::UpReportData()
{
    unsigned int times = 0;
    float consistency = 0;
    int deviceTypeNum = 0;
    int oldTypeNum = 0;
    String flag = "N";

    try
    {
        lua_settop(m_newState ,0);//清空栈
        lua_getglobal(m_newState, "status");

        int status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Interface::UpReportData() => get 'status' table failed");
        }

        lua_pushstring(m_newState , "measure");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Interface::UpReportData() => get 'measure' table failed");
        }

        lua_pushstring(m_newState , "report");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Interface::UpReportData() => get 'report' table failed");
        }

        lua_pushstring(m_newState , "measure");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Interface::UpReportData() => get 'measure' table failed");
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

        lua_pushstring(m_newState , "oldTypeNum");
        lua_gettable(m_newState ,-5); //取栈顶下一个元素

        status = lua_isnumber(m_newState ,-1);//取栈顶
        if (status == 1)
        {
            oldTypeNum = lua_tonumber(m_newState ,-1);
        }

        lua_pushstring(m_newState , "resultInfo");
        lua_gettable(m_newState ,-6); //取栈顶下一个元素

        status = lua_isstring(m_newState ,-1);//取栈顶
        if (status == 1)
        {
            flag = lua_tostring(m_newState ,-1);
        }

    }
    catch(std::exception e)
    {
        logger->warn("CCEPInterface::UpReportMessage() => %s", e.what());
    }

    QByteArray magArray;
    QString str  = "";
    QString mag  = "";
    int strLen = 0;
    Uint16 crc = 0;

    QString qnStr = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");

    QString  dataTimeStr = QDateTime::fromTime_t(times).toString("yyyyMMddhhmmss");

    QString valueStr = QString::number(consistency, 'f', m_decimalNum);

    LuaEngine* luaEngine = LuaEngine::Instance();

    String pw = "123456";
    String mn = "A0000000000000";
    String st = "21";

    Table sysTable;
    luaEngine->GetLuaValue(m_newState, "config.system.hj212Platform", sysTable);
    sysTable.safe_at("pw", pw);
    sysTable.safe_at("mn", mn);
    sysTable.safe_at("st", st);

    if(m_hj212Type == HJ212Type::V2005)
    {
        switch (m_hj212DataType)
        {
//        case (int)HJ212DataType::HourData:
//            str.sprintf("ST=%s;CN=2061;QN=%s;PW=%s;MN=%s;Flag=1;CP=&&DataTime=%s;%03d-Min=%s,%03d-Avg=%s,%03d-Max=%s&&",
//                        st.c_str(), qnStr.toStdString().c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
//                        oldTypeNum, valueStr.toStdString().c_str(), oldTypeNum, valueStr.toStdString().c_str(), oldTypeNum, valueStr.toStdString().c_str());
//            break;
        case HJ212DataType::MinuteData:
            str.sprintf("ST=%s;CN=2051;QN=%s;PW=%s;MN=%s;Flag=1;CP=&&DataTime=%s;%03d-Min=%s,%03d-Avg=%s,%03d-Max=%s&&",
                        st.c_str(), qnStr.toStdString().c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
                        oldTypeNum, valueStr.toStdString().c_str(), oldTypeNum, valueStr.toStdString().c_str(), oldTypeNum, valueStr.toStdString().c_str());
            break;
        case HJ212DataType::RealTimeDate:
            str.sprintf("ST=%s;CN=2011;QN=%s;PW=%s;MN=%s;Flag=1;CP=&&DataTime=%s;%03d-Rtd=%s,%03d-Flag=%s&&",
                        st.c_str(), qnStr.toStdString().c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
                        oldTypeNum, valueStr.toStdString().c_str(), oldTypeNum, flag.c_str());
            break;
        default:    //默认使用小时数据报文
            str.sprintf("ST=%s;CN=2061;QN=%s;PW=%s;MN=%s;Flag=1;CP=&&DataTime=%s;%03d-Min=%s,%03d-Avg=%s,%03d-Max=%s&&",
                        st.c_str(), qnStr.toStdString().c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
                        oldTypeNum, valueStr.toStdString().c_str(), oldTypeNum, valueStr.toStdString().c_str(), oldTypeNum, valueStr.toStdString().c_str());
            break;
        }

    }
    else if(m_hj212Type == HJ212Type::V2017_Dbs)
    {
        switch (m_hj212DataType)
        {
//        case (int)HJ212DataType::HourData:
//            str.sprintf("QN=%s;ST=%s;CN=2061;PW=%s;MN=%s;Flag=9;CP=&&DataTime=%s;w%05d-Avg=%s,w%05d-Flag=%s&&",
//                        qnStr.toStdString().c_str(), st.c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
//                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, flag.c_str());
//            break;
        case HJ212DataType::MinuteData:        //使用实时数据报文
        case HJ212DataType::RealTimeDate:
            str.sprintf("QN=%s;ST=%s;CN=2011;PW=%s;MN=%s;Flag=9;CP=&&DataTime=%s;w%05d-Rtd=%s,w%05d-Flag=%s&&",
                        qnStr.toStdString().c_str(), st.c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, flag.c_str());
            break;
        default:    //默认使用小时数据报文
            str.sprintf("QN=%s;ST=%s;CN=2061;PW=%s;MN=%s;Flag=9;CP=&&DataTime=%s;w%05d-Avg=%s,w%05d-Flag=%s&&",
                        qnStr.toStdString().c_str(), st.c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, flag.c_str());
            break;
        }
    }
    else if(m_hj212Type == HJ212Type::V2017_Wry)
    {
        switch (m_hj212DataType)
        {
//        case (int)HJ212DataType::HourData:
//            str.sprintf("QN=%s;ST=%s;CN=2061;PW=%s;MN=%s;Flag=5;CP=&&DataTime=%s;w%05d-Min=%s,w%05d-Avg=%s,w%05d-Max=%s,w%05d-Flag=%s&&",
//                        qnStr.toStdString().c_str(), st.c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
//                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, valueStr.toStdString().c_str(),
//                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, flag.c_str());
//            break;
        case HJ212DataType::MinuteData:
            str.sprintf("QN=%s;ST=%s;CN=2051;PW=%s;MN=%s;Flag=5;CP=&&DataTime=%s;w%05d-Min=%s,w%05d-Avg=%s,w%05d-Max=%s,w%05d-Flag=%s&&",
                        qnStr.toStdString().c_str(), st.c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, valueStr.toStdString().c_str(),
                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, flag.c_str());
            break;
        case HJ212DataType::RealTimeDate:
            str.sprintf("QN=%s;ST=%s;CN=2011;PW=%s;MN=%s;Flag=5;CP=&&DataTime=%s;w%05d-Rtd=%s,w%05d-Flag=%s&&",
                        qnStr.toStdString().c_str(), st.c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, flag.c_str());
            break;
        default:    //默认使用小时数据报文
            str.sprintf("QN=%s;ST=%s;CN=2061;PW=%s;MN=%s;Flag=5;CP=&&DataTime=%s;w%05d-Min=%s,w%05d-Avg=%s,w%05d-Max=%s,w%05d-Flag=%s&&",
                        qnStr.toStdString().c_str(), st.c_str(), pw.c_str(), mn.c_str(), dataTimeStr.toStdString().c_str(),
                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, valueStr.toStdString().c_str(),
                        deviceTypeNum, valueStr.toStdString().c_str(), deviceTypeNum, flag.c_str());
            break;
        }
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

Uint16 HJ212Interface::CRC(const char *pBuf, int nLen)
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
