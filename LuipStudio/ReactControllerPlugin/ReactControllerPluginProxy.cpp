#include "ReactController.h"
#include "Communication/CommunicationProxy.h"
#include "ControllerPlugin/ControllerPluginProxy.h"
#include "LuaEngine/LuaEngine.h"
#include "ReactControllerPluginProxy.h"
#include "API/TemperatureControlInterface.h"
#include "API/OpticalAcquireInterface.h"
#include "API/RCSolenoidValveInterface.h"

using Communication::Dscp::DscpAddress;
using namespace Controller;
using namespace Controller::API;
using namespace OOLUA;
using namespace Lua;


/**
 * @brief 阀映射图。
 * @details
 */
OOLUA_PROXY(,RCValveMap)

    OOLUA_CTORS(
        OOLUA_CTOR(unsigned long)
    )
    OOLUA_MFUNC(SetData)
    OOLUA_MFUNC(GetData)
    OOLUA_MFUNC(SetOn)
    OOLUA_MFUNC(SetOff)
    OOLUA_MFUNC(IsOn)
    OOLUA_MFUNC(clear)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RCValveMap, SetData, GetData, SetOn, SetOff, IsOn, clear)
OOLUA_EXPORT_FUNCTIONS_CONST(RCValveMap)

/**
 * @brief 光学信号采集的AD值。
 * @details
 */
OOLUA_PROXY(,OpticalAD)
    OOLUA_MGET_MSET(reference, GetReference, SetReference)
    OOLUA_MGET_MSET(measure, GetMeasure, SetMeasure)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(OpticalAD, SetReference, SetMeasure)
OOLUA_EXPORT_FUNCTIONS_CONST(OpticalAD, GetReference, GetMeasure)

/**
 * @brief 光学信号采集操作结果码。
 * @details
 */
OOLUA_PROXY(,AcquiredOperateResult)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Finished)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
        OOLUA_ENUM(Stopped)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(AcquiredOperateResult)  // 导出函数声明（没有函数也需要声明）


/**
 * @brief 光学信号采集的结果。
 * @details
 */
OOLUA_PROXY(,AcquiredResult)
    OOLUA_MGET_MSET(ad, GetAD, SetAD)
    OOLUA_MGET_MSET(result, GetResult, SetResult)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(AcquiredResult, SetAD, SetResult)
OOLUA_EXPORT_FUNCTIONS_CONST(AcquiredResult, GetAD, GetResult)


/**
 * @brief 静态AD调节控制结果码
 * @details
 */
OOLUA_PROXY(,StaticADControlResult)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Unfinished)   // 枚举值
        OOLUA_ENUM(Finished)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(StaticADControlResult)  // 导出函数声明（没有函数也需要声明）


/**
 * @brief LED控制参数。
 * @details
 */
OOLUA_PROXY(,LedParam)
    OOLUA_MGET_MSET(proportion, GetProportion, SetProportion)
    OOLUA_MGET_MSET(integration, GetIntegration, SetIntegration)
    OOLUA_MGET_MSET(differential, GetDifferential, SetDifferential)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LedParam, SetProportion, SetIntegration, SetDifferential)
OOLUA_EXPORT_FUNCTIONS_CONST(LedParam, GetProportion, GetIntegration, GetDifferential)


/**
 * @brief 光学信号采集接口。
 * @details 定义了一序列光学信号采集相关的操作。
 */
OOLUA_PROXY(,OpticalAcquireInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC_CONST(TurnOnLED)
    OOLUA_MFUNC_CONST(TurnOffLED)
    OOLUA_MFUNC_CONST(TurnOnXonen)
    OOLUA_MFUNC_CONST(TurnOffXonen)
    OOLUA_MFUNC_CONST(GlitterXonen)
    OOLUA_MFUNC(SetAcquireADNotifyPeriod)
    OOLUA_MFUNC(StartAcquirer)
    OOLUA_MFUNC(StopAcquirer)
    OOLUA_MFUNC(ExpectADAcquirer)
    OOLUA_MFUNC_CONST(GetStaticADControlParam)
    OOLUA_MFUNC(StartStaticADControl)
    OOLUA_MFUNC(StopStaticADControl)
    OOLUA_MFUNC(SetStaticADControlParam)
    OOLUA_MFUNC(IsStaticADControlValid)
    OOLUA_MFUNC(ExpectStaticADControlResult)
    OOLUA_MFUNC_CONST(GetLEDDefaultValue)
    OOLUA_MFUNC(SetLEDDefaultValue)
//    OOLUA_MEM_FUNC(void, RegisterAcquireADNotice, cpp_in_p<IEventReceivable*>)
    OOLUA_MFUNC(RegisterAcquireADNotice)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(OpticalAcquireInterface, SetAcquireADNotifyPeriod, StartAcquirer, StopAcquirer,
                       ExpectADAcquirer,StartStaticADControl, StopStaticADControl, SetStaticADControlParam,
                       IsStaticADControlValid, ExpectStaticADControlResult, RegisterAcquireADNotice,
                       SetLEDDefaultValue)
OOLUA_EXPORT_FUNCTIONS_CONST(OpticalAcquireInterface, TurnOnLED, TurnOffLED, TurnOnXonen,
                             TurnOffXonen, GlitterXonen, GetStaticADControlParam, GetLEDDefaultValue)

/**
 * @brief 温度。
 * @details
 */
OOLUA_PROXY(,RCTemperature)
    OOLUA_MGET_MSET(thermostatTemp, GetThermostatTemp, SetThermostatTemp)      ///<  恒温室温度，单位为摄氏度
    OOLUA_MGET_MSET(environmentTemp, GetEnvironmentTemp, SetEnvironmentTemp)   ///<  环境温度，单位为摄氏度
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RCTemperature, SetThermostatTemp, SetEnvironmentTemp)
OOLUA_EXPORT_FUNCTIONS_CONST(RCTemperature, GetThermostatTemp, GetEnvironmentTemp)

/**
 * @brief 温度控制参数。
 * @details
 */
OOLUA_PROXY(,RCThermostatParam)
    OOLUA_MGET_MSET(proportion, GetProportion, SetProportion)          ///< PID的比例系数
    OOLUA_MGET_MSET(integration, GetIntegration, SetIntegration)       ///< PID的积分系数
    OOLUA_MGET_MSET(differential, GetDifferential, SetDifferential)    ///< PID的微分系数
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RCThermostatParam, SetProportion, SetIntegration, SetDifferential)
OOLUA_EXPORT_FUNCTIONS_CONST(RCThermostatParam, GetProportion, GetIntegration, GetDifferential)

/**
 * @brief 温度校准参数。
 * @details
 */
OOLUA_PROXY(,RCTempCalibrateFactor)
    OOLUA_MGET_MSET(negativeInput, GetNegativeInput, SetNegativeInput)                 ///<负输入分压
    OOLUA_MGET_MSET(referenceVoltage, GetReferenceVoltage, SetReferenceVoltage)        ///<参考电压
    OOLUA_MGET_MSET(calibrationVoltage, GetCalibrationVoltage, SetCalibrationVoltage)  ///<校准电压
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RCTempCalibrateFactor, SetNegativeInput, SetReferenceVoltage, SetCalibrationVoltage)
OOLUA_EXPORT_FUNCTIONS_CONST(RCTempCalibrateFactor, GetNegativeInput, GetReferenceVoltage, GetCalibrationVoltage)

/**
 * @brief 恒温模式。
 */
OOLUA_PROXY(,RCThermostatMode)
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
OOLUA_EXPORT_NO_FUNCTIONS(RCThermostatMode)  ///< 导出函数声明（没有函数也需要声明）

/**
 * @brief 恒温操作结果。
 */
OOLUA_PROXY(,RCThermostatOperateResult)
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
OOLUA_EXPORT_NO_FUNCTIONS(RCThermostatOperateResult)  ///< 导出函数声明（没有函数也需要声明）

/**
 * @brief 恒温结果。
 * @details
 */
OOLUA_PROXY(,RCThermostatResult)
    OOLUA_MGET_MSET(result, GetResult, SetResult)     ///< 恒温操作结果。
    OOLUA_MGET_MSET(temp, GetTemp, SetTemp)           ///< 当前温度。
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RCThermostatResult, SetResult, SetTemp)
OOLUA_EXPORT_FUNCTIONS_CONST(RCThermostatResult, GetResult, GetTemp)

/**
 * 温度控制器接口
 */
OOLUA_PROXY(,RCTemperatureControlInterface, DeviceInterface)
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
    OOLUA_MFUNC(TurnOnRays)                     ///< 紫外灯开
    OOLUA_MFUNC(TurnOffRays)                    ///< 紫外灯开关
    OOLUA_MFUNC(RegisterTemperatureNotice)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RCTemperatureControlInterface, SetCalibrateFactor, SetThermostatParam,
                       StartThermostat, StopThermostat, SetTemperatureNotifyPeriod, ExpectThermostat,
                       BoxFanSetOutput, DigestionFanSetOutput,SetHeaterMaxDutyCycle,
                       SetCurrentThermostatParam, RegisterTemperatureNotice, TurnOnRays, TurnOffRays)
OOLUA_EXPORT_FUNCTIONS_CONST(RCTemperatureControlInterface, GetCalibrateFactor, GetTemperature, GetThermostatParam,
                             GetHeaterMaxDutyCycle, GetThermostatStatus, GetCurrentThermostatParam)



/**
 * @brief 电磁阀控制接口。
 * @details 定义了一序列电磁阀控制相关的操作。
 */
OOLUA_PROXY(,RCSolenoidValveInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(GetTotalValves)
    OOLUA_MFUNC(GetValveMap)
    OOLUA_MFUNC(SetValveMap)
    OOLUA_MFUNC(SetValveMapNormalOpen)
    OOLUA_MFUNC(SetGainMap)
    OOLUA_MFUNC(GetGainMap)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RCSolenoidValveInterface, GetTotalValves, GetValveMap, SetValveMap, SetValveMapNormalOpen,
                       SetGainMap, GetGainMap)
OOLUA_EXPORT_FUNCTIONS_CONST(RCSolenoidValveInterface)

/**
 * @brief 反应堆控制器。
 * @details
 */
OOLUA_PROXY(,ReactController, BaseController)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(Init)
    OOLUA_MFUNC(Uninit)
//    OOLUA_MEM_FUNC(maybe_null<lua_return<OpticalAcquireInterface*> >, GetIOpticalAcquire)
    OOLUA_MFUNC(GetIOpticalAcquire)
    OOLUA_MFUNC(GetITemperatureControl)
    OOLUA_MFUNC(GetISolenoidValve)
    OOLUA_MFUNC(GetCurrentTemperature)    
    OOLUA_MFUNC_CONST(GetDigestTemperature)
    OOLUA_MFUNC_CONST(GetEnvironmentTemperature)
    OOLUA_MFUNC_CONST(GetScanData)
    OOLUA_MFUNC_CONST(GetScanLen)
    OOLUA_MFUNC_CONST(NDIRResultHandle)
    OOLUA_MFUNC(ClearBuf)
    OOLUA_MFUNC_CONST(GetScanDataRef)
    OOLUA_MFUNC_CONST(GetRefTemp)
    OOLUA_MFUNC_CONST(GetMeaTemp)
    OOLUA_MFUNC(GetPeakTemperature)

//    OOLUA_MEM_FUNC(void, Register, cpp_in_p<ISignalNotifiable*>)
    OOLUA_MFUNC(Register)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ReactController, Init, Uninit, GetIOpticalAcquire, GetITemperatureControl,
                       GetCurrentTemperature, GetISolenoidValve, Register, ClearBuf, GetPeakTemperature)
OOLUA_EXPORT_FUNCTIONS_CONST(ReactController, GetDigestTemperature, GetEnvironmentTemperature,
                              GetScanData, GetScanLen, NDIRResultHandle, GetScanDataRef,
                             GetRefTemp,GetMeaTemp)

ReactControllerPluginProxy::ReactControllerPluginProxy()
{

}

void ReactControllerPluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    lua->register_class<OpticalAD>();
    lua->register_class<AcquiredOperateResult>();
    lua->register_class<AcquiredResult>();
    lua->register_class<LedParam>();
    lua->register_class<StaticADControlResult>();
    lua->register_class<OpticalAcquireInterface>();
    lua->register_class<RCValveMap>();
    lua->register_class<RCTemperature>();
    lua->register_class<RCThermostatParam>();
    lua->register_class<RCTempCalibrateFactor>();
    lua->register_class<RCThermostatMode>();
    lua->register_class<RCThermostatOperateResult>();
    lua->register_class<RCThermostatResult>();
    lua->register_class<RCTemperatureControlInterface>();
    lua->register_class<RCSolenoidValveInterface>();
    lua->register_class<ReactController>();
}
