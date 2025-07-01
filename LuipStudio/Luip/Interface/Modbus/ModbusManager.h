
#if !defined(MODBUS_MANAGER_H)
#define MODBUS_MANAGER_H
#include <memory>
#include <QMutex>
#include "System/Types.h"
#include "ControllerPlugin/API/Version.h"
#include "IFunCodeNotifiable.h"
#include "SerialModbus.h"
#include "TCPModbus.h"

using System::String;
using Controller::API::Version;

namespace Interface
{
namespace Modbus
{

enum class RegisterType
{
    Input = 0,
    Hold = 1,
};

class ModbusManager : public IFunCodeNotifiable
{
public:
    static ModbusManager *Instance();
    bool Init();
    SerialModbus* GetRS232Modbus();
    SerialModbus* GetRS485Modbus();
    TCPModbus* GetTCPModbus();

    void SetDouble(RegisterType type, int addr, double data);
    double GetDouble(RegisterType type, int addr) const;
    void SetFloat(RegisterType type, int addr, float data);
    void SetSmallFloat(RegisterType type, int addr, float data);
    float GetFloat(RegisterType type, int addr) const;
    void SetInt(RegisterType type, int addr, int data);
    int GetInt(RegisterType type, int addr) const;
    void SetShort(RegisterType type, int addr, short data);
    short GetShort(RegisterType type, int addr) const;
    void SetTime(RegisterType type, int addr, unsigned int dateTime);
    void SetSysTime(RegisterType type, int addr) const;
    void SetHexTime(RegisterType type, int addr, unsigned int dateTime);
    void SetHexSysTime(RegisterType type, int addr) const;
    String GetTime(RegisterType type, int addr) const;
    void SetString(RegisterType type, int addr, String info, int regSize);
    String GetString(RegisterType type, int addr, int regSize) const;
    void SetVersion(RegisterType type, int addr, Version ver);
    Version GetVersion(RegisterType type, int addr) const;
    void OnRegisterRead(int funCode, int regAddr, int regNum);
    void OnRegisterWrited(int funCode, int regAddr, int regNum);
    void SetCharString(RegisterType type, int addr, String info, int regSize);
    void SetCharStringLittleEndian(RegisterType type, int addr, String info, int regSize);
    void Reboot();
    void SetBCDTime(RegisterType type, int addr, unsigned int dateTime);
    void SetBCDTimeToSystem(RegisterType type, int addr);

    String FormatTime(String format, unsigned int time);
private:
    bool RS232Init();
    bool RS485Init();
    bool TCPInit();

    void MapInit();

private:
    static std::unique_ptr<ModbusManager> m_instance;
    modbus_mapping_t * m_map;
    SerialModbus* m_RS232;
    SerialModbus* m_RS485;
    TCPModbus* m_TCP;

    QMutex m_readMutex;
    QMutex m_wirteMutex;

    lua_State * m_mainState;
    lua_State * m_readState;
    lua_State * m_wirteState;
};

}
}

#endif  //MODBUS_MANAGER_H
