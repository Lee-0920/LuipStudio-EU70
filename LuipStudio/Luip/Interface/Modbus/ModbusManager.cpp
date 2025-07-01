#include <QDebug>
#include <QByteArray>
#include <QDateTime>
#include <QMutexLocker>
#include <QProcess>
#include "Log.h"
#include "LuaEngine/LuaEngine.h"
#include "ModbusManager.h"
#include "oolua.h"
#include "System/SetSystemTime.h"
#include "Interface/HJ212/HJ212Interface.h"

#ifndef  _CS_X86_WINDOWS
#include <sys/time.h>
#endif

using namespace std;
using namespace OOLUA;
using namespace System;
using namespace Controller::API;
using namespace Lua;

namespace Interface
{
namespace Modbus
{

unique_ptr<ModbusManager> ModbusManager::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 ModbusManager 对象（单件）。
 */
ModbusManager* ModbusManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new ModbusManager);
    }

    return m_instance.get();
}

bool ModbusManager::RS232Init()
{
    bool ret = false;

    int connectAddress = 0;
    int baud = 0;
    int RS232Parity = 0;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        Table interconnection, options;
        luaEngine->GetLuaValue(state, "config.interconnection", interconnection);

        interconnection.at("connectAddress", connectAddress);
        interconnection.at("RS232Parity", RS232Parity);

        int index;
        String str;
        interconnection.at("RS232BaudRate", index);

        luaEngine->GetLuaValue(state, "setting.ui.profile.interconnection[5][1].options", options);
        options.at(index + 1, str);
        baud = QString(str.c_str()).toInt();

    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ModbusManager::RS232Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ModbusManager::RS232Init() => %s", e.what());
    }

    char parity = 'N';
    if (RS232Parity == 0)
    {
        parity = 'N';   // 无校验
    }
    else if (RS232Parity == 1)
    {
        parity = 'O';   // 奇校验
    }
    else if (RS232Parity == 2)
    {
        parity = 'E';   // 偶校验
    }

    m_RS232 = new SerialModbus(this, m_map);
    m_RS232->SetAddress(connectAddress);


#ifdef _CS_X86_WINDOWS
    bool isRS232OK = m_RS232->SetCommunicationParam("COM6", baud, parity, 8, 1);
#endif

#ifdef _CS_ARM_LINUX
    bool isRS232OK = m_RS232->SetCommunicationParam("/dev/ttymxc3", baud, parity, 8, 1);
#endif

    if (isRS232OK)
    {
        m_RS232->Start();
        ret = true;
    }
    else
    {
        logger->warn("RS232 Modbus 配置失败");
    }

    return ret;
}

bool ModbusManager::RS485Init()
{
    bool ret = false;

    int connectAddress = 0;
    int baud = 0;
    int RS485Parity = 0;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        Table interconnection, options;
        luaEngine->GetLuaValue(state, "config.interconnection", interconnection);

        interconnection.at("connectAddress", connectAddress);
        interconnection.at("RS485Parity", RS485Parity);
        int index;
        String str;
        interconnection.at("RS485BaudRate", index);
        luaEngine->GetLuaValue(state, "setting.ui.profile.interconnection[4][1].options", options);
        options.at(index + 1, str);
        baud = QString(str.c_str()).toInt();
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ModbusManager::RS485Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ModbusManager::RS485Init() => %s", e.what());
    }

    char parity = 'N';
    if (RS485Parity == 0)
    {
        parity = 'N';   // 无校验
    }
    else if (RS485Parity == 1)
    {
        parity = 'O';   // 奇校验
    }
    else if (RS485Parity == 2)
    {
        parity = 'E';   // 偶校验
    }

    m_RS485 = new SerialModbus(this, m_map);
    m_RS485->SetAddress(connectAddress);


#ifdef _CS_X86_WINDOWS
    bool isRS485OK = m_RS485->SetCommunicationParam("COM4", baud, parity, 8, 1);
#endif

#ifdef _CS_ARM_LINUX
    bool isRS485OK = m_RS485->SetCommunicationParam("/dev/ttymxc4", baud, parity, 8, 1);
#endif

    if (isRS485OK)
    {
       m_RS485->Start();
       ret = true;
    }
    else
    {
        logger->warn("RS485 Modbus 配置失败");
    }

    return ret;
}

bool ModbusManager::TCPInit()
{
    bool ret = false;

    m_TCP = new TCPModbus(this, m_map);
    ret = m_TCP->Start();

    return ret;
}

void ModbusManager::Reboot()
{
    QProcess::execute("reboot");
}

void ModbusManager::MapInit()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        Table registers;
        luaEngine->GetLuaValue(state, "setting.externalInterface.modbus.registers", registers);

        oolua_ipairs(registers)
        {
            Table reg;
            Lua_func_ref read;

            lua->pull(reg);
            reg.at("read",read);

            lua->call(read);
        }
        oolua_ipairs_end()
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ModbusManager::MapInit() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ModbusManager::MapInit() => %s", e.what());
    }
}

bool ModbusManager::Init()
{
    bool CCEPMode = false;
    int hj212Type = (int)HJ212::HJ212Type::None;
    unsigned int inputRegAddr = 0;
    unsigned int inputRegNum = 256;
    unsigned int holdRegAddr = 0;
    unsigned int holdRegNum = 300;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(state, "config.system.hj212Platform.hj212Type", hj212Type);

        luaEngine->GetLuaValue(state, "config.system.CCEPMode", CCEPMode);

        Table modbus;
        luaEngine->GetLuaValue(state, "setting.externalInterface.modbus", modbus);

        modbus.at("inputRegAddr", inputRegAddr);
        modbus.at("inputRegNum", inputRegNum);
        modbus.at("holdRegAddr", holdRegAddr);
        modbus.at("holdRegNum", holdRegNum);

        m_mainState = LuaEngine::Instance()->GetMainState();
        m_readState = LuaEngine::Instance()->CreateThread();;
        m_wirteState = LuaEngine::Instance()->CreateThread();;
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ModbusManager::Init => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ModbusManager::Init => %s", e.what());
    }

    if (hj212Type != (int)HJ212::HJ212Type::None)
    {
        m_map = modbus_mapping_new_start_address(0, 0, 0, 0, 0, 1, 0, 0);
        m_TCP = new TCPModbus(this, m_map);
        return false;
    }
    else
    {
        qDebug("Modbus Enable");
    }

    m_map = modbus_mapping_new_start_address(0, 0, 0, 0, holdRegAddr, holdRegNum, inputRegAddr, inputRegNum);

    if (m_map == NULL)
    {
        qDebug() << "Failed to allocate the mapping " << modbus_strerror(errno);
        return false;
    }

    this->MapInit();

    this->RS485Init();
    QThread::msleep(1000);

//    if(hj212Type == (int)HJ212::HJ212Type::None) //不开启HJ212上传模式
//    {
//        this->RS232Init();
//        QThread::msleep(1000);
//    }

    this->TCPInit();

    return true;
}

SerialModbus *ModbusManager::GetRS232Modbus()
{
    return m_RS232;
}

SerialModbus *ModbusManager::GetRS485Modbus()
{
    return m_RS485;
}

TCPModbus *ModbusManager::GetTCPModbus()
{
    return m_TCP;
}

void ModbusManager::SetDouble(RegisterType type, int addr, double data)
{
    if (type == RegisterType::Input)
    {
         memcpy(&m_map->tab_input_registers[addr], &data, sizeof(double));
    }
    else
    {
         memcpy(&m_map->tab_registers[addr], &data, sizeof(double));
    }
}

double ModbusManager::GetDouble(RegisterType type, int addr) const
{
    double data = 0;

    if (type == RegisterType::Input)
    {
         memcpy(&data, &m_map->tab_input_registers[addr], sizeof(double));
    }
    else
    {
         memcpy(&data, &m_map->tab_registers[addr], sizeof(double));
    }

    return data;
}

void ModbusManager::SetFloat(RegisterType type, int addr, float data)
{
   if (type == RegisterType::Input)
   {
        memcpy(&m_map->tab_input_registers[addr], &data, sizeof(float));
   }
   else
   {
        memcpy(&m_map->tab_registers[addr], &data, sizeof(float));
   }
}

void ModbusManager::SetSmallFloat(RegisterType type, int addr, float data)
{
    int smallFloatHex = 0;
    Uint8 oldHex[4] = {0};
    Uint8 newHex[4] = {0};
    memcpy(oldHex, &data, sizeof(float));

    newHex[0]=oldHex[2];
    newHex[1]=oldHex[3];
    newHex[2]=oldHex[0];
    newHex[3]=oldHex[1];

    memcpy(&smallFloatHex, newHex, sizeof(int));

    if (type == RegisterType::Input)
    {
         memcpy(&m_map->tab_input_registers[addr], &smallFloatHex, sizeof(int));
    }
    else
    {
         memcpy(&m_map->tab_registers[addr], &smallFloatHex, sizeof(int));
    }
}

float ModbusManager::GetFloat(RegisterType type, int addr) const
{
    float data = 0;

    if (type == RegisterType::Input)
    {
         memcpy(&data, &m_map->tab_input_registers[addr], sizeof(float));
    }
    else
    {
         memcpy(&data, &m_map->tab_registers[addr], sizeof(float));
    }

    return data;
}

void ModbusManager::SetInt(RegisterType type, int addr, int data)
{
    if (type == RegisterType::Input)
    {
         memcpy(&m_map->tab_input_registers[addr], &data, sizeof(int));
    }
    else
    {
         memcpy(&m_map->tab_registers[addr], &data, sizeof(int));
    }
}

int ModbusManager::GetInt(RegisterType type, int addr) const
{
    int data = 0;

    if (type == RegisterType::Input)
    {
         memcpy(&data, &m_map->tab_input_registers[addr], sizeof(int));
    }
    else
    {
         memcpy(&data, &m_map->tab_registers[addr], sizeof(int));
    }

    return data;
}

void ModbusManager::SetShort(RegisterType type, int addr, short data)
{
    if (type == RegisterType::Input)
    {
         memcpy(&m_map->tab_input_registers[addr], &data, sizeof(short));
    }
    else
    {
         memcpy(&m_map->tab_registers[addr], &data, sizeof(short));
    }
}

short ModbusManager::GetShort(RegisterType type, int addr) const
{
    short data = 0;

    if (type == RegisterType::Input)
    {
         memcpy(&data, &m_map->tab_input_registers[addr], sizeof(short));
    }
    else
    {
         memcpy(&data, &m_map->tab_registers[addr], sizeof(short));
    }

    return data;
}

void ModbusManager::SetTime(RegisterType type, int addr, unsigned int dateTime)
{
    QDateTime dt = QDateTime::fromTime_t(dateTime);

    short buff[6];

    buff[0] = dt.date().year() - 1970;
    buff[1] = dt.date().month();
    buff[2] = dt.date().day();
    buff[3] = dt.time().hour();
    buff[4] = dt.time().minute();
    buff[5] = dt.time().second();

    if (type == RegisterType::Input)
    {
         memcpy(&m_map->tab_input_registers[addr], &buff, sizeof(short) * 6);
    }
    else
    {
         memcpy(&m_map->tab_registers[addr], &buff, sizeof(short) * 6);
    }
}

void ModbusManager::SetSysTime(RegisterType type, int addr) const
{
#ifdef _CS_ARM_LINUX
    short data[6];

    if (type == RegisterType::Input)
    {
         memcpy(&data, &m_map->tab_input_registers[addr], sizeof(short) * 6);
    }
    else
    {
         memcpy(&data, &m_map->tab_registers[addr], sizeof(short) * 6);
    }

    int year= data[0] + 1970;
    int month = data[1];
    int day= data[2];
    int hour = data[3];
    int minute= data[4];
    int second = data[5];

    System::SetSystemTime::SetTime(year, month, day, hour, minute, second);
    return ;
#else
    (void)type;
    (void)addr;
#endif
}

void ModbusManager::SetHexTime(RegisterType type, int addr, unsigned int dateTime)
{
    QDateTime dt = QDateTime::fromTime_t(dateTime);

    Uint8 buff[6];

    buff[0] = (Uint8)(dt.date().month() & 0xFF);
    buff[1] = (Uint8)((dt.date().year() - 1970) & 0xFF);
    buff[2] = (Uint8)(dt.time().hour() & 0xFF);
    buff[3] = (Uint8)(dt.date().day() & 0xFF);
    buff[4] = (Uint8)(dt.time().second() & 0xFF);
    buff[5] = (Uint8)(dt.time().minute() & 0xFF);

    if (type == RegisterType::Input)
    {
         memcpy(&m_map->tab_input_registers[addr], &buff, sizeof(Uint8) * 6);
    }
    else
    {
         memcpy(&m_map->tab_registers[addr], &buff, sizeof(Uint8) * 6);
    }
}

void ModbusManager::SetHexSysTime(RegisterType type, int addr) const
{
#ifdef _CS_ARM_LINUX
    Uint8 data[6];

    if (type == RegisterType::Input)
    {
         memcpy(&data, &m_map->tab_input_registers[addr], sizeof(Uint8) * 6);
    }
    else
    {
         memcpy(&data, &m_map->tab_registers[addr], sizeof(Uint8) * 6);
    }

    int year = data[1] + 1970;
    int month = data[0];
    int day = data[3];
    int hour = data[2];
    int minute = data[5];
    int second = data[4];

    System::SetSystemTime::SetTime(year, month, day, hour, minute, second);
#else
    (void)type;
    (void)addr;
#endif
}

String ModbusManager::GetTime(RegisterType type, int addr)  const
{
    Uint8 data[6];

    if (type == RegisterType::Input)
    {
         memcpy(&data, &m_map->tab_input_registers[addr], sizeof(Uint8) * 6);
    }
    else
    {
         memcpy(&data, &m_map->tab_registers[addr], sizeof(Uint8) * 6);
    }

    for(int i = 0; i < 6; i++)
    {
        data[i] = ((data[i] >> 4) & 0x0F) * 10 + (data[i] & 0x0F);
    }

    String month = std::to_string(data[0]);
    String year= std::to_string(data[1] + 2000);
    String hour = std::to_string(data[2]);
    String day= std::to_string(data[3]);
    String second = std::to_string(data[4]);
    String minute= std::to_string(data[5]);

    return (year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second);
}

void ModbusManager::SetBCDTime(RegisterType type, int addr, unsigned int dateTime)
{
    QDateTime dt = QDateTime::fromTime_t(dateTime);

    Uint8 buff[6];

    buff[0] = (Uint8)(dt.date().month() & 0xFF);
    buff[1] = (Uint8)((dt.date().year() - 2000) & 0xFF);
    buff[2] = (Uint8)(dt.time().hour() & 0xFF);
    buff[3] = (Uint8)(dt.date().day() & 0xFF);
    buff[4] = (Uint8)(dt.time().second() & 0xFF);
    buff[5] = (Uint8)(dt.time().minute() & 0xFF);

    for (int i = 0; i < 6; i++)
    {
        buff[i] = ((buff[i] / 10) << 4) + ((buff[i] % 10) & 0x0F);
    }

    if (type == RegisterType::Input)
    {
         memcpy(&m_map->tab_input_registers[addr], &buff, sizeof(Uint8) * 6);
    }
    else
    {
         memcpy(&m_map->tab_registers[addr], &buff, sizeof(Uint8) * 6);
    }
}

void ModbusManager::SetBCDTimeToSystem(RegisterType type, int addr)
{
#ifdef _CS_ARM_LINUX
    Uint8 data[6];

    if (type == RegisterType::Input)
    {
         memcpy(&data, &m_map->tab_input_registers[addr], sizeof(Uint8) * 6);
    }
    else
    {
         memcpy(&data, &m_map->tab_registers[addr], sizeof(Uint8) * 6);
    }

    for(int i = 0; i < 6; i++)
    {
        data[i] = ((data[i] >> 4) & 0x0F) * 10 + (data[i] & 0x0F);
    }

    int year = data[1] + 2000;
    int month = data[0];
    int day = data[3];
    int hour = data[2];
    int minute = data[5];
    int second = data[4];

    System::SetSystemTime::SetTime(year, month, day, hour, minute, second);
#else
    (void)type;
    (void)addr;
#endif
}

void ModbusManager::SetCharString(RegisterType type, int addr, String info, int regSize)
{
    QString str = QString(info.c_str());

    QByteArray ba = str.toLatin1();
    const char *ch = ba.data();

    int strLength = str.size();

    if (type == RegisterType::Input)
    {
        memset(&m_map->tab_input_registers[addr], 0, regSize*2);
        memcpy(&m_map->tab_input_registers[addr], ch, strLength);
    }
    else
    {
        memset(&m_map->tab_registers[addr], 0, regSize*2);
        memcpy(&m_map->tab_registers[addr], ch, strLength);
    }
}

void ModbusManager::SetCharStringLittleEndian(RegisterType type, int addr, String info, int regSize)
{
    QString str = QString(info.c_str());

    QByteArray ba = str.toLatin1();
    int len = ba.size();
    if(ba.size()%2 == 1)
    {
        ba.resize(len + 1);
        ba[len] = 0;
    }

    QByteArray temp = ba;
    for(int i = 0; i < ba.size(); i ++)
    {
        if(i%2 == 0)
        {
            ba[i] = temp[i+1];
        }
        else if(i%2 == 1)
        {
            ba[i] = temp[i-1];
        }
    }

    if (type == RegisterType::Input)
    {
        memset(&m_map->tab_input_registers[addr], 0, regSize*2);
        memcpy(&m_map->tab_input_registers[addr], ba.data(), ba.size());
    }
    else
    {
        memset(&m_map->tab_registers[addr], 0, regSize*2);
        memcpy(&m_map->tab_registers[addr], ba.data(), ba.size());
    }
}

void ModbusManager::SetString(RegisterType type, int addr, String info, int regSize)
{
    QString str = QString(info.c_str());
    const QChar *qChar = str.unicode();

    int strLength = str.size();

    if (type == RegisterType::Input)
    {
        memset(&m_map->tab_input_registers[addr], 0, regSize*2);
        memcpy(&m_map->tab_input_registers[addr], qChar, strLength*2);
    }
    else
    {
        memset(&m_map->tab_registers[addr], 0, regSize*2);
        memcpy(&m_map->tab_registers[addr], qChar, strLength*2);
    }
}

/*
*寄存器默认UTF16-BE
* 外设连接读写格式-[UTF16-BE]
* 本机读输入内容需要转小端才能正常显示中文-[UTF16-LE]
*/
String ModbusManager::GetString(RegisterType type, int addr, int regSize) const
{
    unsigned short info[regSize/2];

    if (type == RegisterType::Input)
    {
         memcpy(info, &m_map->tab_input_registers[addr], regSize/2);
    }
    else
    {
         memcpy(info, &m_map->tab_registers[addr], regSize/2);
    }    
    return QString::fromUtf16(info).toStdString();
}

void ModbusManager::SetVersion(RegisterType type, int addr, Version ver)
{
    short data[4];

    data[0] = ver.GetMajor();
    data[1] = ver.GetMinor();
    data[2] = ver.GetRevision();
    data[3] = ver.GetBuild();

    if (type == RegisterType::Input)
    {
         memcpy(&m_map->tab_input_registers[addr], &data, sizeof(short) * 4);
    }
    else
    {
         memcpy(&m_map->tab_registers[addr], &data, sizeof(short) * 4);
    }
}

Version ModbusManager::GetVersion(RegisterType type, int addr) const
{
    Version ver;
    short data[4];

    if (type == RegisterType::Input)
    {
         memcpy(&data, &m_map->tab_input_registers[addr], sizeof(short) * 4);
    }
    else
    {
         memcpy(&data, &m_map->tab_registers[addr], sizeof(short) *4);
    }

    ver.SetMajor(data[0]);
    ver.SetMinor(data[1]);
    ver.SetRevision(data[2]);
    ver.SetBuild(data[3]);


    return ver;
}

void ModbusManager::OnRegisterRead(int funCode, int regAddr, int regNum)
{
    QMutexLocker locker(&m_readMutex);

    LuaEngine::Instance()->AttachCurrentThread(m_readState);

    if (funCode == 0x03 || funCode == 0x04)
    {
        try
        {
            lua_settop(m_readState ,0);//清空栈
            lua_getglobal(m_readState, "ModbusInterface");

            int status = lua_istable(m_readState ,-1);//取栈顶
            if (status != 1)
            {
                logger->warn("ModbusManager::OnRegisterRead() => get 'ModbusInterface' table failed");
                return;
            }

            lua_pushstring(m_readState , "OnRegisterPreread");
            lua_gettable(m_readState ,-2); //取栈顶下一个元素

            status = lua_isfunction(m_readState ,-1);//取栈顶
            if (status != 1)
            {
                logger->warn("ModbusManager::OnRegisterRead() => get 'createFlow' function failed");
                return;
            }

            lua_pushinteger(m_readState, funCode);
            lua_pushinteger(m_readState, regAddr);
            lua_pushinteger(m_readState, regNum);

            int ret = lua_resume(m_readState, m_mainState, 3);
            if (ret != 0)
            {
                logger->warn("ModbusManager::OnRegisterRead() => lua_resume call 'OnRegisterPreread' function failed, error code:%d", ret);
            }
        }
        catch(std::exception e)
        {
            logger->warn("ModbusManager::OnRegisterRead() => %s", e.what());
        }
    }

    LuaEngine::Instance()->DetachCurrentThread();
}

void ModbusManager::OnRegisterWrited(int funCode, int regAddr, int regNum)
{
    QMutexLocker locker(&m_wirteMutex);

    LuaEngine::Instance()->AttachCurrentThread(m_wirteState);

    if (funCode == 0x06 || funCode == 0x10)
    {
        try
        {
            lua_settop(m_wirteState ,0);//清空栈
            lua_getglobal(m_wirteState, "ModbusInterface");

            int status = lua_istable(m_wirteState ,-1);//取栈顶
            if (status != 1)
            {
                logger->warn("ModbusManager::OnRegisterWrited() => get 'ModbusInterface' table failed");
                return;
            }

            lua_pushstring(m_wirteState , "OnRegisterWrited");
            lua_gettable(m_wirteState ,-2); //取栈顶下一个元素

            status = lua_isfunction(m_wirteState ,-1);//取栈顶
            if (status != 1)
            {
                logger->warn("ModbusManager::OnRegisterWrited() => get 'createFlow' function failed");
                return;
            }

            lua_pushinteger(m_wirteState, funCode);
            lua_pushinteger(m_wirteState, regAddr);
            lua_pushinteger(m_wirteState, regNum);

            int ret = lua_resume(m_wirteState, m_mainState, 3);
            if (ret != 0)
            {
                logger->warn("ModbusManager::OnRegisterWrited() => lua_resume call 'OnRegisterWrited' function failed, error code:%d", ret);
            }
        }
        catch(std::exception e)
        {
            logger->warn("ModbusManager::OnRegisterWrited() => %s", e.what());
        }
    }

    LuaEngine::Instance()->DetachCurrentThread();
}

String ModbusManager::FormatTime(String format, unsigned int dateTime)
{
    QDateTime dt = QDateTime::fromTime_t(dateTime);
    return dt.toString(QString::fromStdString(format)).toStdString();
}

}
}
