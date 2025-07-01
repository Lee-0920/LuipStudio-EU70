#include "LuaEngine/LuaEngine.h"
#include "API/TemperatureControlInterface.h"
#include "TemperatureController.h"
#include "ControllerPlugin/ControllerPluginProxy.h"
#include "Communication/CommunicationProxy.h"
#include "TemperatureControllerPluginProxy.h"

using Communication::Dscp::DscpAddress;
using namespace Controller;
using namespace Controller::API;
using namespace OOLUA;
using namespace Lua;

/**
 * @brief 温度。
 * @details
 */
OOLUA_PROXY(,Temperature)
    OOLUA_MGET_MSET(thermostatTemp, GetThermostatTemp, SetThermostatTemp)      ///<  恒温室温度，单位为摄氏度
    OOLUA_MGET_MSET(environmentTemp, GetEnvironmentTemp, SetEnvironmentTemp)   ///<  环境温度，单位为摄氏度
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(Temperature, SetThermostatTemp, SetEnvironmentTemp)
OOLUA_EXPORT_FUNCTIONS_CONST(Temperature, GetThermostatTemp, GetEnvironmentTemp)

/**
 * @brief 温度控制参数。
 * @details
 */
OOLUA_PROXY(,ThermostatParam)
    OOLUA_MGET_MSET(proportion, GetProportion, SetProportion)          ///< PID的比例系数
    OOLUA_MGET_MSET(integration, GetIntegration, SetIntegration)       ///< PID的积分系数
    OOLUA_MGET_MSET(differential, GetDifferential, SetDifferential)    ///< PID的微分系数
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ThermostatParam, SetProportion, SetIntegration, SetDifferential)
OOLUA_EXPORT_FUNCTIONS_CONST(ThermostatParam, GetProportion, GetIntegration, GetDifferential)

/**
 * @brief 温度校准参数。
 * @details
 */
OOLUA_PROXY(,TempCalibrateFactor)
    OOLUA_MGET_MSET(negativeInput, GetNegativeInput, SetNegativeInput)                 ///<负输入分压
    OOLUA_MGET_MSET(referenceVoltage, GetReferenceVoltage, SetReferenceVoltage)        ///<参考电压
    OOLUA_MGET_MSET(calibrationVoltage, GetCalibrationVoltage, SetCalibrationVoltage)  ///<校准电压
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(TempCalibrateFactor, SetNegativeInput, SetReferenceVoltage, SetCalibrationVoltage)
OOLUA_EXPORT_FUNCTIONS_CONST(TempCalibrateFactor, GetNegativeInput, GetReferenceVoltage, GetCalibrationVoltage)

/**
 * @brief 恒温模式。
 */
OOLUA_PROXY(,ThermostatMode)
    OOLUA_TAGS(
        Register_class_enums  ///< 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
    OOLUA_ENUM(Auto)            ///< 自动模式，根据需要及硬件能力综合使用加热器和制冷器。
    OOLUA_ENUM(Heater)          ///< 纯加热模式，不使用制冷器。
    OOLUA_ENUM(Refrigerate)     ///< 纯制冷模式，不使用加热器。
    OOLUA_ENUM(Natural)         ///< 自然模式，加热器和制冷器都不参与，靠环境传递热量，自然升温或冷却。
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(ThermostatMode)  ///< 导出函数声明（没有函数也需要声明）

/**
 * @brief 恒温操作结果。
 */
OOLUA_PROXY(,ThermostatOperateResult)
    OOLUA_TAGS(
        Register_class_enums  ///< 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
    OOLUA_ENUM(Reached)     ///< 恒温目标达成，目标温度在规定时间内达成，后续将继续保持恒温，直到用户停止。
    OOLUA_ENUM(Failed)      ///< 恒温中途出现故障，未能完成。
    OOLUA_ENUM(Stopped)     ///< 恒温被停止。
    OOLUA_ENUM(Timeout)     ///< 恒温超时，指定时间内仍未达到目标温度。
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(ThermostatOperateResult)  ///< 导出函数声明（没有函数也需要声明）

/**
 * @brief 恒温结果。
 * @details
 */
OOLUA_PROXY(,ThermostatResult)
    OOLUA_MGET_MSET(result, GetResult, SetResult)     ///< 恒温操作结果。
    OOLUA_MGET_MSET(temp, GetTemp, SetTemp)           ///< 当前温度。
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ThermostatResult, SetResult, SetTemp)
OOLUA_EXPORT_FUNCTIONS_CONST(ThermostatResult, GetResult, GetTemp)

/**
 * 温度控制器接口
 */
OOLUA_PROXY(,TemperatureControlInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC_CONST(GetCalibrateFactor)       ///< 查询温度传感器的校准系数
    OOLUA_MFUNC(SetCalibrateFactor)             ///< 设置温度传感器的校准系数
    OOLUA_MFUNC_CONST(GetTemperature)           ///< 查询当前温度
    OOLUA_MFUNC_CONST(GetThermostatParam)       ///< 查询恒温控制参数
    OOLUA_MFUNC(SetThermostatParam)             ///<  设置恒温控制参数
    OOLUA_MFUNC(StartThermostat)                ///< 开始恒温
    OOLUA_MFUNC(StopThermostat)                 ///< 停止恒温控制
    OOLUA_MFUNC(SetTemperatureNotifyPeriod)     ///< 设置温度上报周期
    OOLUA_MFUNC(ExpectThermostat)               ///< 恒温结果事件
    OOLUA_MFUNC(BoxFanSetOutput)                ///< 机箱风扇输出
    OOLUA_MFUNC(DigestionFanSetOutput)          ///<消解冷却风扇输出
    OOLUA_MFUNC_CONST(GetHeaterMaxDutyCycle)        ///<查询加热丝最大占空比
    OOLUA_MFUNC(SetHeaterMaxDutyCycle)              ///<设置加热丝最大占空比
    OOLUA_MFUNC_CONST(GetThermostatStatus)          ///< 查询恒温器的工作状态
    OOLUA_MFUNC_CONST(GetCurrentThermostatParam)    ///< 查询当前恒温控制参数
    OOLUA_MFUNC(SetCurrentThermostatParam)      ///< 设置当前恒温控制参数。
    OOLUA_MFUNC(RegisterTemperatureNotice)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(TemperatureControlInterface, SetCalibrateFactor, SetThermostatParam,StartThermostat,
                       StopThermostat, SetTemperatureNotifyPeriod, ExpectThermostat,BoxFanSetOutput,
                       DigestionFanSetOutput,SetHeaterMaxDutyCycle,SetCurrentThermostatParam, RegisterTemperatureNotice)
OOLUA_EXPORT_FUNCTIONS_CONST(TemperatureControlInterface, GetCalibrateFactor, GetTemperature, GetThermostatParam,
                             GetHeaterMaxDutyCycle, GetThermostatStatus, GetCurrentThermostatParam)

/**
 * 温度控制器
 */
OOLUA_PROXY(,TemperatureController, BaseController)
    OOLUA_TAGS(
        No_default_constructor
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )

OOLUA_MFUNC(GetCurrentTemperature)
    OOLUA_MFUNC(Init)
    OOLUA_MFUNC(Uninit)
//    OOLUA_MEM_FUNC(maybe_null<lua_return<TemperatureControlInterface*> >, GetITemperatureControl)
    OOLUA_MFUNC(GetITemperatureControl)
//    OOLUA_MEM_FUNC(void, Register, cpp_in_p<ISignalNotifiable*>)
    OOLUA_MFUNC(Register)
    OOLUA_MFUNC_CONST(GetDigestTemperature)
    OOLUA_MFUNC_CONST(GetEnvironmentTemperature)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(TemperatureController,
                       GetCurrentTemperature,
                       Init,
                       Uninit,
                       GetITemperatureControl,
                       Register)
OOLUA_EXPORT_FUNCTIONS_CONST(TemperatureController,
                             GetDigestTemperature,
                             GetEnvironmentTemperature)

TemperatureControllerPluginProxy::TemperatureControllerPluginProxy()
{

}

void TemperatureControllerPluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    // 注册导出类，lua 中将以全局 table 的形式存在。创建的对象是 userdata 类型。
    lua->register_class<Temperature>();
    lua->register_class<ThermostatParam>();
    lua->register_class<TempCalibrateFactor>();
    lua->register_class<ThermostatMode>();
    lua->register_class<ThermostatOperateResult>();
    lua->register_class<ThermostatResult>();
    lua->register_class<TemperatureControlInterface>();
    lua->register_class<TemperatureController>();
}
