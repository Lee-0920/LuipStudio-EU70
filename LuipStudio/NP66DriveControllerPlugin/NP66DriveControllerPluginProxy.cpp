#include "API/NP66MeterPoints.h"
#include "API/NP66OpticalMeterInterface.h"
#include "API/NP66PeristalticPumpInterface.h"
#include "API/NP66SolenoidValveInterface.h"
#include "NP66DriveController.h"
#include "Communication/CommunicationProxy.h"
#include "ControllerPlugin/ControllerPluginProxy.h"
#include "LuaEngine/LuaEngine.h"
#include "NP66DriveControllerPluginProxy.h"
#include "API/NP66TemperatureControlInterface.h"
#include "API/NP66OpticalAcquireInterface.h"

using Communication::Dscp::DscpAddress;
using namespace Controller;
using namespace Controller::API;
using namespace OOLUA;
using namespace Lua;

/**
 * @brief 光学定量点体积。
 * @details
 */
OOLUA_PROXY(,NP66MeterPoint)
    OOLUA_MGET_MSET(setVolume, GetSetVolume, SetSetVolume)
    OOLUA_MGET_MSET(realVolume, GetRealVolume, SetRealVolume)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66MeterPoint, SetSetVolume, SetRealVolume)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66MeterPoint, GetSetVolume, GetRealVolume)

/**
 * @brief 光学定量点。
 * @details
 */
OOLUA_PROXY(,NP66MeterPoints)
    OOLUA_CTORS(
        OOLUA_CTOR(int)
    )
    OOLUA_TAGS(
        Equal_op
    )
    OOLUA_MFUNC(SetNum)
    OOLUA_MFUNC(GetNum)
    OOLUA_MFUNC(SetPoint)
    OOLUA_MFUNC(GetPoint)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66MeterPoints, SetNum, GetNum, SetPoint, GetPoint)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66MeterPoints)

/**
 * @brief 蠕动泵运动参数。
 * @details
 */
OOLUA_PROXY(,NP66MotionParam)
    OOLUA_MGET_MSET(acceleration, GetAcceleration, SetAcceleration)
    OOLUA_MGET_MSET(speed, GetSpeed, SetSpeed)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66MotionParam, SetAcceleration, SetSpeed)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66MotionParam, GetAcceleration, GetSpeed)

/**
 * @brief 蠕动泵状态。
 * @details
 */
OOLUA_PROXY(,NP66PumpStatus)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Idle)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
        OOLUA_ENUM(Busy)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(NP66PumpStatus)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 蠕动泵旋转方向。
 * @details
 */
OOLUA_PROXY(,NP66RollDirection)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Suck)   // 枚举值
        OOLUA_ENUM(Drain)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(NP66RollDirection)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 蠕动泵操作结果码。
 * @details
 */
OOLUA_PROXY(,NP66PumpOperateResult)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Finished)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
        OOLUA_ENUM(Stopped)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(NP66PumpOperateResult)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 蠕动泵结果。
 * @details
 */
OOLUA_PROXY(,NP66PumpResult)
    OOLUA_MGET_MSET(index, GetIndex, SetIndex)
    OOLUA_MGET_MSET(result, GetResult, SetResult)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66PumpResult, SetIndex, SetResult)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66PumpResult, GetIndex, GetResult)

/**
 * @brief 阀映射图。
 * @details
 */
OOLUA_PROXY(,NP66ValveMap)

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
OOLUA_EXPORT_FUNCTIONS(NP66ValveMap, SetData, GetData, SetOn, SetOff, IsOn, clear)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66ValveMap)

/**
 * @brief 光学定量模式。
 * @details
 */
OOLUA_PROXY(,NP66MeterMode)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Accurate)  // 枚举值
        OOLUA_ENUM(Direct)    // 枚举值
        OOLUA_ENUM(Smart)     // 枚举值
        OOLUA_ENUM(Ropiness)  // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(NP66MeterMode)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 定量操作结果。
 * @details
 */
OOLUA_PROXY(,NP66MeterResult)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Finished)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
        OOLUA_ENUM(Stopped)   // 枚举值
        OOLUA_ENUM(Overflow)   // 枚举值
        OOLUA_ENUM(Unfinished)   // 枚举值
        OOLUA_ENUM(AirBubble)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(NP66MeterResult)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 温度。
 * @details
 */
OOLUA_PROXY(,NP66Temperature)
    OOLUA_MGET_MSET(thermostatTemp, GetThermostatTemp, SetThermostatTemp)      ///<  恒温室温度，单位为摄氏度
    OOLUA_MGET_MSET(environmentTemp, GetEnvironmentTemp, SetEnvironmentTemp)   ///<  环境温度，单位为摄氏度
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66Temperature, SetThermostatTemp, SetEnvironmentTemp)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66Temperature, GetThermostatTemp, GetEnvironmentTemp)

/**
 * @brief 温度控制参数。
 * @details
 */
OOLUA_PROXY(,NP66ThermostatParam)
    OOLUA_MGET_MSET(proportion, GetProportion, SetProportion)          ///< PID的比例系数
    OOLUA_MGET_MSET(integration, GetIntegration, SetIntegration)       ///< PID的积分系数
    OOLUA_MGET_MSET(differential, GetDifferential, SetDifferential)    ///< PID的微分系数
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66ThermostatParam, SetProportion, SetIntegration, SetDifferential)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66ThermostatParam, GetProportion, GetIntegration, GetDifferential)

/**
 * @brief 温度校准参数。
 * @details
 */
OOLUA_PROXY(,NP66TempCalibrateFactor)
    OOLUA_MGET_MSET(negativeInput, GetNegativeInput, SetNegativeInput)                 ///<负输入分压
    OOLUA_MGET_MSET(referenceVoltage, GetReferenceVoltage, SetReferenceVoltage)        ///<参考电压
    OOLUA_MGET_MSET(calibrationVoltage, GetCalibrationVoltage, SetCalibrationVoltage)  ///<校准电压
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66TempCalibrateFactor, SetNegativeInput, SetReferenceVoltage, SetCalibrationVoltage)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66TempCalibrateFactor, GetNegativeInput, GetReferenceVoltage, GetCalibrationVoltage)

/**
 * @brief 恒温模式。
 */
OOLUA_PROXY(,NP66ThermostatMode)
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
OOLUA_EXPORT_NO_FUNCTIONS(NP66ThermostatMode)  ///< 导出函数声明（没有函数也需要声明）

/**
 * @brief 恒温操作结果。
 */
OOLUA_PROXY(,NP66ThermostatOperateResult)
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
OOLUA_EXPORT_NO_FUNCTIONS(NP66ThermostatOperateResult)  ///< 导出函数声明（没有函数也需要声明）

/**
 * @brief 恒温结果。
 * @details
 */
OOLUA_PROXY(,NP66ThermostatResult)
    OOLUA_MGET_MSET(result, GetResult, SetResult)     ///< 恒温操作结果。
    OOLUA_MGET_MSET(temp, GetTemp, SetTemp)           ///< 当前温度。
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66ThermostatResult, SetResult, SetTemp)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66ThermostatResult, GetResult, GetTemp)

/**
 * @brief 光学定量接口。
 * @details 定义了一序列光学定量相关的操作。
 */
OOLUA_PROXY(,NP66OpticalMeterInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(TurnOnLED)
    OOLUA_MFUNC(TurnOffLED)
    OOLUA_MFUNC_CONST(GetPumpFactor)
    OOLUA_MFUNC(SetPumpFactor)
    OOLUA_MFUNC_CONST(GetMeterPoints)
    OOLUA_MFUNC(SetMeterPoints)
    OOLUA_MFUNC_CONST(GetMeterStatus)
    OOLUA_MFUNC(StartMeter)
    OOLUA_MFUNC(StopMeter)
    OOLUA_MFUNC(IsAutoCloseValve)
    OOLUA_MFUNC(SetOpticalADNotifyPeriod)
    OOLUA_MFUNC(ExpectMeterResult)
    OOLUA_MFUNC(SetMeteSpeed)
    OOLUA_MFUNC_CONST(GetMeteSpeed)
    OOLUA_MFUNC(SetMeterFinishValveMap)
    OOLUA_MFUNC_CONST(GetSingleOpticalAD)
//    OOLUA_MEM_FUNC(void, RegisterOpticalADNotice, cpp_in_p<IEventReceivable*>)
    OOLUA_MFUNC(RegisterOpticalADNotice)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66OpticalMeterInterface, TurnOnLED, TurnOffLED,
                       SetPumpFactor, SetMeterPoints, StartMeter, StopMeter,
                       IsAutoCloseValve, SetOpticalADNotifyPeriod,ExpectMeterResult,
                       SetMeteSpeed, SetMeterFinishValveMap, RegisterOpticalADNotice)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66OpticalMeterInterface, GetPumpFactor, GetMeterPoints,
                             GetMeterStatus, GetMeteSpeed, GetSingleOpticalAD)

/**
 * @brief 蠕动泵控制接口。
 * @details 定义了一序列蠕动泵控制相关的操作。
 */
OOLUA_PROXY(,NP66PeristalticPumpInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(GetTotalPumps)
    OOLUA_MFUNC(GetPumpFactor)
    OOLUA_MFUNC(SetPumpFactor)
    OOLUA_MFUNC(GetMotionParam)
    OOLUA_MFUNC(SetMotionParam)
    OOLUA_MFUNC(GetPumpStatus)
    OOLUA_MFUNC(StartPump)
    OOLUA_MFUNC(StopPump)
    OOLUA_MFUNC(GetPumpVolume)
    OOLUA_MFUNC(ExpectPumpResult)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66PeristalticPumpInterface, GetTotalPumps, GetPumpFactor, SetPumpFactor,
                       GetMotionParam, SetMotionParam, GetPumpStatus, StartPump,
                       StopPump, GetPumpVolume, ExpectPumpResult)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66PeristalticPumpInterface)

/**
 * @brief 电磁阀控制接口。
 * @details 定义了一序列电磁阀控制相关的操作。
 */
OOLUA_PROXY(,NP66SolenoidValveInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(GetTotalValves)
    OOLUA_MFUNC(GetValveMap)
    OOLUA_MFUNC(SetValveMap)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66SolenoidValveInterface, GetTotalValves, GetValveMap, SetValveMap)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66SolenoidValveInterface)

/**
 * 温度控制器接口
 */
OOLUA_PROXY(,NP66TemperatureControlInterface, DeviceInterface)
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
OOLUA_EXPORT_FUNCTIONS(NP66TemperatureControlInterface, SetCalibrateFactor, SetThermostatParam,StartThermostat,
                   StopThermostat, SetTemperatureNotifyPeriod, ExpectThermostat,BoxFanSetOutput,
                   DigestionFanSetOutput,SetHeaterMaxDutyCycle,SetCurrentThermostatParam, RegisterTemperatureNotice)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66TemperatureControlInterface, GetCalibrateFactor, GetTemperature, GetThermostatParam,
                             GetHeaterMaxDutyCycle, GetThermostatStatus, GetCurrentThermostatParam)

/**
 * @brief 光学信号采集的AD值。
 * @details
 */
OOLUA_PROXY(,NP66OpticalAD)
    OOLUA_MGET_MSET(reference, GetReference, SetReference)
    OOLUA_MGET_MSET(measure, GetMeasure, SetMeasure)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66OpticalAD, SetReference, SetMeasure)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66OpticalAD, GetReference, GetMeasure)

/**
 * @brief LED控制参数。
 * @details
 */
OOLUA_PROXY(,NP66LedParam)
    OOLUA_MGET_MSET(proportion, GetProportion, SetProportion)
    OOLUA_MGET_MSET(integration, GetIntegration, SetIntegration)
    OOLUA_MGET_MSET(differential, GetDifferential, SetDifferential)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66LedParam, SetProportion, SetIntegration, SetDifferential)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66LedParam, GetProportion, GetIntegration, GetDifferential)


/**
 * @brief 静态AD调节控制结果码
 * @details
 */
OOLUA_PROXY(,NP66StaticADControlResult)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Unfinished)   // 枚举值
        OOLUA_ENUM(Finished)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(NP66StaticADControlResult)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 光学信号采集接口。
 * @details 定义了一序列光学信号采集相关的操作。
 */
OOLUA_PROXY(,NP66OpticalAcquireInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC_CONST(TurnOnLED)
    OOLUA_MFUNC(SetAcquireADNotifyPeriod)
    OOLUA_MFUNC(StartAcquirer)
    OOLUA_MFUNC(StopAcquirer)
    OOLUA_MFUNC(StartLEDController)
    OOLUA_MFUNC_CONST(TurnOffLED)
    OOLUA_MFUNC_CONST(GetLEDControllerTartet)
    OOLUA_MFUNC(SetLEDControllerTarget)
    OOLUA_MFUNC(SetLEDControllerStableTarget)
    OOLUA_MFUNC_CONST(GetLEDControllerParam)
    OOLUA_MFUNC(SetLEDControllerParam)
    OOLUA_MFUNC(ExpectADAcquirer)
    OOLUA_MFUNC(ExpectADStableAdjust)
    OOLUA_MFUNC_CONST(GetStaticADControlParam)
    OOLUA_MFUNC(StartStaticADControl)
    OOLUA_MFUNC(StopStaticADControl)
    OOLUA_MFUNC(SetStaticADControlParam)
    OOLUA_MFUNC_CONST(IsStaticADControlValid)
    OOLUA_MFUNC(ExpectStaticADControlResult)

//    OOLUA_MEM_FUNC(void, RegisterAcquireADNotice, cpp_in_p<IEventReceivable*>)
    OOLUA_MFUNC(RegisterAcquireADNotice)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66OpticalAcquireInterface, SetAcquireADNotifyPeriod, StartAcquirer, StopAcquirer,
                       StartLEDController, SetLEDControllerTarget, SetLEDControllerStableTarget,
                       SetLEDControllerParam, ExpectADAcquirer,  ExpectADStableAdjust, RegisterAcquireADNotice,
                       StartStaticADControl, StopStaticADControl, SetStaticADControlParam, ExpectStaticADControlResult)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66OpticalAcquireInterface, GetLEDControllerTartet, GetLEDControllerParam,
                             IsStaticADControlValid, TurnOnLED, TurnOffLED, GetStaticADControlParam)

/**
 * @brief 驱动控制器。
 * @details
 */
OOLUA_PROXY(,NP66DriveController, BaseController)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(Init)
    OOLUA_MFUNC(Uninit)
    OOLUA_MFUNC(SetBoxFanEnable)
    OOLUA_MFUNC(GetIPeristalticPump)
    OOLUA_MFUNC(GetISolenoidValve)
    OOLUA_MFUNC(GetIOpticalMeter)
    OOLUA_MFUNC(GetITemperatureControl)
    OOLUA_MFUNC(GetIOpticalAcquire)
    OOLUA_MFUNC(Register)
    OOLUA_MFUNC(GetCurrentTemperature)
    OOLUA_MFUNC_CONST(GetDigestTemperature)
    OOLUA_MFUNC_CONST(GetEnvironmentTemperature)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NP66DriveController, Init, Uninit, SetBoxFanEnable, GetIPeristalticPump, GetISolenoidValve,
                       GetIOpticalMeter,GetCurrentTemperature,GetIOpticalAcquire, GetITemperatureControl, Register)
OOLUA_EXPORT_FUNCTIONS_CONST(NP66DriveController,
                             GetDigestTemperature,
                             GetEnvironmentTemperature)

NP66DriveControllerPluginProxy::NP66DriveControllerPluginProxy()
{

}

void NP66DriveControllerPluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    lua->register_class<NP66MeterPoint>();
    lua->register_class<NP66MeterPoints>();
    lua->register_class<NP66MotionParam>();
    lua->register_class<NP66PumpStatus>();
    lua->register_class<NP66RollDirection>();
    lua->register_class<NP66PumpOperateResult>();
    lua->register_class<NP66PumpResult>();
    lua->register_class<NP66ValveMap>();
    lua->register_class<NP66MeterMode>();
    lua->register_class<NP66MeterResult>();
    lua->register_class<NP66Temperature>();
    lua->register_class<NP66ThermostatParam>();
    lua->register_class<NP66TempCalibrateFactor>();
    lua->register_class<NP66ThermostatMode>();
    lua->register_class<NP66ThermostatOperateResult>();
    lua->register_class<NP66ThermostatResult>();
    lua->register_class<NP66OpticalAD>();
    lua->register_class<NP66LedParam>();
    lua->register_class<NP66StaticADControlResult>();
    lua->register_class<NP66OpticalMeterInterface>();
    lua->register_class<NP66PeristalticPumpInterface>();
    lua->register_class<NP66SolenoidValveInterface>();
    lua->register_class<NP66TemperatureControlInterface>();
    lua->register_class<NP66OpticalAcquireInterface>();
    lua->register_class<NP66DriveController>();
}

