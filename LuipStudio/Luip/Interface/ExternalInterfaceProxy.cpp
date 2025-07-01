#include "Control/RelayControl.h"
#include "Control/WaterCollector.h"
#include "ControllerPlugin/ControllerPluginProxy.h"
#include "ExternalInterfaceProxy.h"
#include "Current/CurrentResultManager.h"
#include "Modbus/ModbusManager.h"
#include "Communication/CommunicationProxy.h"
#include "Wqimc/WqimcManager.h"
#include "Wqimc/ReagentRemainVector.h"
#include "Interface/HJ212/HJ212Interface.h"

using namespace Interface::Control;
using namespace Interface::Current;
using namespace Interface::HJ212;
using Interface::Modbus::IFunCodeNotifiable;
using Interface::Modbus::RegisterType;
using Interface::Modbus::ModbusManager;
using Interface::Wqimc::Api::Manager::Config;
using Interface::Wqimc::ConfigVector;
using Interface::Wqimc::WqimcManager;
using Interface::Wqimc::Api::Manager::ReagentRemain;
using Interface::Wqimc::ReagentRemainVector;

/**
 * @brief 4-20mA输出结果管理器。
 * @details
 */
OOLUA_PROXY(,CurrentResultManager)
    OOLUA_TAGS(
        //No_default_constructor,
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)  // 单便入口函数
    OOLUA_MFUNC(OutputSample)
    OOLUA_MFUNC(OutputCheck)
    OOLUA_MFUNC(OutputSampleCurrent)
    OOLUA_MFUNC(OutputCheckCurrent)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(CurrentResultManager, OutputSample, OutputCheck, OutputSampleCurrent, OutputCheckCurrent)
OOLUA_EXPORT_FUNCTIONS_CONST(CurrentResultManager)

/**
 * @brief 信号继电器。
 * @details
 */
OOLUA_PROXY(,RelayControl)
    OOLUA_TAGS(
        //No_default_constructor,
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)  // 单便入口函数
    OOLUA_MFUNC(TurnOn)
    OOLUA_MFUNC(TurnOff)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RelayControl, TurnOn, TurnOff)
OOLUA_EXPORT_FUNCTIONS_CONST(RelayControl)

/**
 * @brief 采水继电器。
 * @details
 */
OOLUA_PROXY(,WaterCollector)
    OOLUA_TAGS(
        //No_default_constructor,
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)  // 单便入口函数
    OOLUA_MFUNC(TurnOn)
    OOLUA_MFUNC(TurnOff)
    OOLUA_MFUNC_CONST(GetStatus)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(WaterCollector, TurnOn, TurnOff)
OOLUA_EXPORT_FUNCTIONS_CONST(WaterCollector, GetStatus)

/**
 * @brief HJ212协议类型
 * @details
 */
OOLUA_PROXY(,HJ212Type)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(None)        // 枚举值
        OOLUA_ENUM(V2005)       // 枚举值
        OOLUA_ENUM(V2017_Dbs)   // 枚举值
        OOLUA_ENUM(V2017_Wry)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(HJ212Type)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief HJ212协议数据类型
 * @details
 */
OOLUA_PROXY(,HJ212DataType)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(HourData)        // 枚举值
        OOLUA_ENUM(MinuteData)      // 枚举值
        OOLUA_ENUM(RealTimeDate)    // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(HJ212DataType)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 寄存器类型。
 * @details
 */
OOLUA_PROXY(,RegisterType)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Input)   // 枚举值
        OOLUA_ENUM(Hold)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(RegisterType)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief Modbus管理器
 * @details
 */
OOLUA_PROXY(,ModbusManager)
    OOLUA_TAGS(
        //No_default_constructor,
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)  // 单便入口函数
    OOLUA_MFUNC(SetDouble)
    OOLUA_MFUNC(SetFloat)
    OOLUA_MFUNC(SetSmallFloat)
    OOLUA_MFUNC(SetInt)
    OOLUA_MFUNC(SetShort)
    OOLUA_MFUNC(SetTime)
    OOLUA_MFUNC(SetHexTime)
    OOLUA_MFUNC(SetString)
    OOLUA_MFUNC(SetCharString)
    OOLUA_MFUNC(SetCharStringLittleEndian)
    OOLUA_MFUNC(SetVersion)
    OOLUA_MFUNC(Reboot)
    OOLUA_MFUNC(SetBCDTime)
    OOLUA_MFUNC(SetBCDTimeToSystem)
    OOLUA_MFUNC(FormatTime)
    OOLUA_MFUNC_CONST(GetDouble)
    OOLUA_MFUNC_CONST(GetFloat)
    OOLUA_MFUNC_CONST(GetInt)
    OOLUA_MFUNC_CONST(GetShort)
    OOLUA_MFUNC_CONST(GetTime)
    OOLUA_MFUNC_CONST(GetString)
    OOLUA_MFUNC_CONST(GetVersion)
    OOLUA_MFUNC_CONST(SetSysTime)
    OOLUA_MFUNC_CONST(SetHexSysTime)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ModbusManager, SetDouble, SetFloat,SetSmallFloat, SetInt, SetBCDTime, SetBCDTimeToSystem, SetCharStringLittleEndian,FormatTime,
                       SetShort, SetTime, SetHexTime, SetString, SetCharString, SetVersion, Reboot)
OOLUA_EXPORT_FUNCTIONS_CONST(ModbusManager, GetDouble, GetFloat, GetInt,
                             GetShort, GetTime, GetString, GetVersion, SetSysTime, SetHexSysTime)

OOLUA_PROXY(,Config)
    OOLUA_MFUNC(__set_profile)
    OOLUA_MFUNC(__set_config)
    OOLUA_MFUNC(__set_value)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(Config, __set_profile, __set_config, __set_value)
OOLUA_EXPORT_FUNCTIONS_CONST(Config)

OOLUA_PROXY(,ConfigVector)
    OOLUA_MFUNC(Push)
    OOLUA_MFUNC(Size)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ConfigVector, Push, Size)
OOLUA_EXPORT_FUNCTIONS_CONST(ConfigVector)

OOLUA_PROXY(,ReagentRemain)
    OOLUA_MFUNC(__set_name)
    OOLUA_MFUNC(__set_volume)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ReagentRemain, __set_name, __set_volume)
OOLUA_EXPORT_FUNCTIONS_CONST(ReagentRemain)

OOLUA_PROXY(,ReagentRemainVector)
    OOLUA_MFUNC(Push)
    OOLUA_MFUNC(Size)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ReagentRemainVector, Push, Size)
OOLUA_EXPORT_FUNCTIONS_CONST(ReagentRemainVector)

OOLUA_PROXY(,WqimcManager)
    OOLUA_TAGS(
        //No_default_constructor,
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)  // 单便入口函数
    OOLUA_MFUNC(updateConfigs)
    OOLUA_MFUNC(updateReagentRemain)
    OOLUA_MFUNC(uploadReagentProfile)
    OOLUA_MFUNC(uploadConsumableProfile)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(WqimcManager, updateConfigs, updateReagentRemain, uploadReagentProfile, uploadConsumableProfile)
OOLUA_EXPORT_FUNCTIONS_CONST(WqimcManager)

ExternalInterfaceProxy::ExternalInterfaceProxy()
{

}

void ExternalInterfaceProxy::Proxy(OOLUA::Script *lua)
{
    lua->register_class<CurrentResultManager>();
    lua->register_class_static<CurrentResultManager>("Instance",
             &OOLUA::Proxy_class<CurrentResultManager>::Instance); // 静态函数要单独注册

    lua->register_class<RelayControl>();
    lua->register_class_static<RelayControl>("Instance",
             &OOLUA::Proxy_class<RelayControl>::Instance); // 静态函数要单独注册

    lua->register_class<WaterCollector>();
    lua->register_class_static<WaterCollector>("Instance",
             &OOLUA::Proxy_class<WaterCollector>::Instance); // 静态函数要单独注册

    lua->register_class<HJ212Type>();
    lua->register_class<HJ212DataType>();

    lua->register_class<RegisterType>();

    lua->register_class<ModbusManager>();
    lua->register_class_static<ModbusManager>("Instance",
             &OOLUA::Proxy_class<ModbusManager>::Instance); // 静态函数要单独注册

    lua->register_class<Config>();
    lua->register_class<ConfigVector>();
    lua->register_class<ReagentRemain>();
    lua->register_class<ReagentRemainVector>();
    lua->register_class<WqimcManager>();
    lua->register_class_static<WqimcManager>("Instance",
             &OOLUA::Proxy_class<WqimcManager>::Instance); // 静态函数要单独注册
}
