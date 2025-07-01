#include "API/MeterPoints.h"
#include "API/OpticalMeterInterface.h"
#include "API/PeristalticPumpInterface.h"
#include "API/LCPeristalticPumpInterface.h"
#include "API/SolenoidValveInterface.h"
#include "API/LCSolenoidValveInterface.h"
#include "LiquidController.h"
#include "Communication/CommunicationProxy.h"
#include "ControllerPlugin/ControllerPluginProxy.h"
#include "LuaEngine/LuaEngine.h"
#include "LiquidControllerPluginProxy.h"
#include "API/LCTemperatureControlInterface.h"

using Communication::Dscp::DscpAddress;
using namespace Controller;
using namespace Controller::API;
using namespace OOLUA;
using namespace Lua;

///**
// * @brief 光学定量点体积。
// * @details
// */
//OOLUA_PROXY(,MeterPoint)
//    OOLUA_MGET_MSET(setVolume, GetSetVolume, SetSetVolume)
//    OOLUA_MGET_MSET(realVolume, GetRealVolume, SetRealVolume)
//OOLUA_PROXY_END
//OOLUA_EXPORT_FUNCTIONS(MeterPoint, SetSetVolume, SetRealVolume)
//OOLUA_EXPORT_FUNCTIONS_CONST(MeterPoint, GetSetVolume, GetRealVolume)

///**
// * @brief 光学定量点。
// * @details
// */
//OOLUA_PROXY(,MeterPoints)
//    OOLUA_CTORS(
//        OOLUA_CTOR(int)
//    )
//    OOLUA_TAGS(
//        Equal_op
//    )
//    OOLUA_MFUNC(SetNum)
//    OOLUA_MFUNC(GetNum)
//    OOLUA_MFUNC(SetPoint)
//    OOLUA_MFUNC(GetPoint)

//OOLUA_PROXY_END
//OOLUA_EXPORT_FUNCTIONS(MeterPoints, SetNum, GetNum, SetPoint, GetPoint)
//OOLUA_EXPORT_FUNCTIONS_CONST(MeterPoints)

/**
 * @brief 蠕动泵运动参数。
 * @details
 */
OOLUA_PROXY(,LCMotionParam)
    OOLUA_MGET_MSET(acceleration, GetAcceleration, SetAcceleration)
    OOLUA_MGET_MSET(speed, GetSpeed, SetSpeed)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LCMotionParam, SetAcceleration, SetSpeed)
OOLUA_EXPORT_FUNCTIONS_CONST(LCMotionParam, GetAcceleration, GetSpeed)

/**
 * @brief 蠕动泵状态。
 * @details
 */
OOLUA_PROXY(,LCPumpStatus)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Idle)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
        OOLUA_ENUM(Busy)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(LCPumpStatus)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 蠕动泵旋转方向。
 * @details
 */
OOLUA_PROXY(,LCRollDirection)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Suck)   // 枚举值
        OOLUA_ENUM(Drain)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(LCRollDirection)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 蠕动泵操作结果码。
 * @details
 */
OOLUA_PROXY(,LCPumpOperateResult)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Finished)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
        OOLUA_ENUM(Stopped)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(LCPumpOperateResult)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 蠕动泵结果。
 * @details
 */
OOLUA_PROXY(,LCPumpResult)
    OOLUA_MGET_MSET(index, GetIndex, SetIndex)
    OOLUA_MGET_MSET(result, GetResult, SetResult)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LCPumpResult, SetIndex, SetResult)
OOLUA_EXPORT_FUNCTIONS_CONST(LCPumpResult, GetIndex, GetResult)

/**
 * @brief 阀映射图。
 * @details
 */
OOLUA_PROXY(,LCValveMap)

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
OOLUA_EXPORT_FUNCTIONS(LCValveMap, SetData, GetData, SetOn, SetOff, IsOn, clear)
OOLUA_EXPORT_FUNCTIONS_CONST(LCValveMap)


/**
 * @brief 温度。
 * @details
 */
OOLUA_PROXY(,LCTemperature)
    OOLUA_MGET_MSET(thermostatTemp, GetThermostatTemp, SetThermostatTemp)      ///<  恒温室温度，单位为摄氏度
    OOLUA_MGET_MSET(environmentTemp, GetEnvironmentTemp, SetEnvironmentTemp)   ///<  环境温度，单位为摄氏度
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LCTemperature, SetThermostatTemp, SetEnvironmentTemp)
OOLUA_EXPORT_FUNCTIONS_CONST(LCTemperature, GetThermostatTemp, GetEnvironmentTemp)

/**
 * @brief 温度控制参数。
 * @details
 */
OOLUA_PROXY(,LCThermostatParam)
    OOLUA_MGET_MSET(proportion, GetProportion, SetProportion)          ///< PID的比例系数
    OOLUA_MGET_MSET(integration, GetIntegration, SetIntegration)       ///< PID的积分系数
    OOLUA_MGET_MSET(differential, GetDifferential, SetDifferential)    ///< PID的微分系数
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LCThermostatParam, SetProportion, SetIntegration, SetDifferential)
OOLUA_EXPORT_FUNCTIONS_CONST(LCThermostatParam, GetProportion, GetIntegration, GetDifferential)

/**
 * @brief 温度校准参数。
 * @details
 */
OOLUA_PROXY(,LCTempCalibrateFactor)
    OOLUA_MGET_MSET(negativeInput, GetNegativeInput, SetNegativeInput)                 ///<负输入分压
    OOLUA_MGET_MSET(referenceVoltage, GetReferenceVoltage, SetReferenceVoltage)        ///<参考电压
    OOLUA_MGET_MSET(calibrationVoltage, GetCalibrationVoltage, SetCalibrationVoltage)  ///<校准电压
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LCTempCalibrateFactor, SetNegativeInput, SetReferenceVoltage, SetCalibrationVoltage)
OOLUA_EXPORT_FUNCTIONS_CONST(LCTempCalibrateFactor, GetNegativeInput, GetReferenceVoltage, GetCalibrationVoltage)

/**
 * @brief 恒温模式。
 */
OOLUA_PROXY(,LCThermostatMode)
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
OOLUA_EXPORT_NO_FUNCTIONS(LCThermostatMode)  ///< 导出函数声明（没有函数也需要声明）

/**
 * @brief 恒温操作结果。
 */
OOLUA_PROXY(,LCThermostatOperateResult)
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
OOLUA_EXPORT_NO_FUNCTIONS(LCThermostatOperateResult)  ///< 导出函数声明（没有函数也需要声明）

/**
 * @brief 恒温结果。
 * @details
 */
OOLUA_PROXY(,LCThermostatResult)
    OOLUA_MGET_MSET(result, GetResult, SetResult)     ///< 恒温操作结果。
    OOLUA_MGET_MSET(temp, GetTemp, SetTemp)           ///< 当前温度。
    OOLUA_MGET_MSET(index, GetIndex, SetIndex)           ///< 当前恒温器索引
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LCThermostatResult, SetResult, SetTemp, SetIndex)
OOLUA_EXPORT_FUNCTIONS_CONST(LCThermostatResult, GetResult, GetTemp, GetIndex)


/**
 * 温度控制器接口
 */
OOLUA_PROXY(,LCTemperatureControlInterface, DeviceInterface)
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
OOLUA_MFUNC(DigestionFanSetOutputForTOC)          ///<消解冷却风扇输出
OOLUA_MFUNC_CONST(GetHeaterMaxDutyCycle)        ///<查询加热丝最大占空比
OOLUA_MFUNC(SetHeaterMaxDutyCycle)              ///<设置加热丝最大占空比
OOLUA_MFUNC_CONST(GetThermostatStatus)          ///< 查询恒温器的工作状态
OOLUA_MFUNC_CONST(GetCurrentThermostatParam)    ///< 查询当前恒温控制参数
OOLUA_MFUNC(SetCurrentThermostatParam)      ///< 设置当前恒温控制参数。
OOLUA_MFUNC(ReviseThermostatTemp)                ///< 修改恒温参数
OOLUA_MFUNC(RegisterTemperatureNotice)
OOLUA_MFUNC(SetCalibrateFactorForTOC)             ///< 设置温度传感器的校准系数
OOLUA_MFUNC_CONST(GetCalibrateFactorForTOC)       ///< 查询温度传感器的校准系数
OOLUA_MFUNC(BoxFanSetOutputForTOC)                ///< TOC机箱上下风扇输出
OOLUA_MFUNC(RelayControlForTOC)                   ///< TOC燃烧炉电源继电器控制
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LCTemperatureControlInterface, SetCalibrateFactor, SetThermostatParam,StartThermostat,
                   StopThermostat, SetTemperatureNotifyPeriod, ExpectThermostat,BoxFanSetOutput,
                   DigestionFanSetOutputForTOC,SetHeaterMaxDutyCycle,SetCurrentThermostatParam, RegisterTemperatureNotice,
                   ReviseThermostatTemp, SetCalibrateFactorForTOC, BoxFanSetOutputForTOC, RelayControlForTOC)
OOLUA_EXPORT_FUNCTIONS_CONST(LCTemperatureControlInterface, GetCalibrateFactor, GetTemperature, GetThermostatParam,
                             GetHeaterMaxDutyCycle, GetThermostatStatus, GetCurrentThermostatParam, GetCalibrateFactorForTOC)


///**
// * @brief 光学定量模式。
// * @details
// */
//OOLUA_PROXY(,MeterMode)
//    OOLUA_TAGS(
//        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
//    )
//    OOLUA_ENUMS(
//        OOLUA_ENUM(Accurate)   // 枚举值
//        OOLUA_ENUM(Direct)   // 枚举值
//        OOLUA_ENUM(Smart)   // 枚举值
//        OOLUA_ENUM(Ropiness)   // 枚举值
//    )
//OOLUA_PROXY_END
//OOLUA_EXPORT_NO_FUNCTIONS(MeterMode)  // 导出函数声明（没有函数也需要声明）

///**
// * @brief 静态定量AD调节控制结果码
// * @details
// */
//OOLUA_PROXY(,StaticMeterADControlResult)
//    OOLUA_TAGS(
//        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
//    )
//    OOLUA_ENUMS(
//        OOLUA_ENUM(Unfinished)   // 枚举值
//        OOLUA_ENUM(Finished)   // 枚举值
//    )
//OOLUA_PROXY_END
//OOLUA_EXPORT_NO_FUNCTIONS(StaticMeterADControlResult)  // 导出函数声明（没有函数也需要声明）


///**
// * @brief 定量操作结果。
// * @details
// */
//OOLUA_PROXY(,MeterResult)
//    OOLUA_TAGS(
//        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
//    )
//    OOLUA_ENUMS(
//        OOLUA_ENUM(Finished)   // 枚举值
//        OOLUA_ENUM(Failed)   // 枚举值
//        OOLUA_ENUM(Stopped)   // 枚举值
//        OOLUA_ENUM(Overflow)   // 枚举值
//        OOLUA_ENUM(Unfinished)   // 枚举值
//        OOLUA_ENUM(AirBubble)   // 枚举值
//    )
//OOLUA_PROXY_END
//OOLUA_EXPORT_NO_FUNCTIONS(MeterResult)  // 导出函数声明（没有函数也需要声明）

///**
// * @brief 光学定量接口。
// * @details 定义了一序列光学定量相关的操作。
// */
//OOLUA_PROXY(,OpticalMeterInterface, DeviceInterface)
//    OOLUA_TAGS(
//        No_default_constructor //没有默认构造函数时可以使用此标签
//    )
//    OOLUA_CTORS(
//        OOLUA_CTOR(DscpAddress)
//    )
//    OOLUA_MFUNC_CONST(TurnOnLED)//
//    OOLUA_MFUNC_CONST(TurnOffLED)//
//    OOLUA_MFUNC_CONST(GetPumpFactor)
//    OOLUA_MFUNC(SetPumpFactor)
//    OOLUA_MFUNC_CONST(GetMeterPoints)
//    OOLUA_MFUNC(SetMeterPoints)
//    OOLUA_MFUNC_CONST(GetMeterStatus)
//    OOLUA_MFUNC(StartMeter)
//    OOLUA_MFUNC(StopMeter)
//    OOLUA_MFUNC_CONST(IsAutoCloseValve)//
//    OOLUA_MFUNC(SetOpticalADNotifyPeriod)
//    OOLUA_MFUNC(ExpectMeterResult)
//    OOLUA_MFUNC(SetMeteSpeed)
//    OOLUA_MFUNC_CONST(GetMeteSpeed)
//    OOLUA_MFUNC(SetMeterFinishValveMap)
//    OOLUA_MFUNC_CONST(GetSingleOpticalAD)
//    OOLUA_MFUNC_CONST(GetStaticADControlParam)//
//    OOLUA_MFUNC(StartStaticADControl)
//    OOLUA_MFUNC(StopStaticADControl)
//    OOLUA_MFUNC(SetStaticADControlParam)
//    OOLUA_MFUNC_CONST(IsStaticADControlValid)//
//    OOLUA_MFUNC(ExpectStaticADControlResult)
////    OOLUA_MEM_FUNC(void, RegisterOpticalADNotice, cpp_in_p<IEventReceivable*>)
//    OOLUA_MFUNC(RegisterOpticalADNotice)

//OOLUA_PROXY_END
//OOLUA_EXPORT_FUNCTIONS(OpticalMeterInterface, StartStaticADControl, StopStaticADControl, SetStaticADControlParam,
//                       SetPumpFactor, SetMeterPoints, StartMeter, StopMeter,
//                       SetOpticalADNotifyPeriod, ExpectMeterResult, ExpectStaticADControlResult,
//                       SetMeteSpeed, SetMeterFinishValveMap, RegisterOpticalADNotice)
//OOLUA_EXPORT_FUNCTIONS_CONST(OpticalMeterInterface, GetPumpFactor, GetMeterPoints,TurnOnLED, TurnOffLED, IsStaticADControlValid,
//                             GetMeterStatus, GetMeteSpeed, GetSingleOpticalAD, IsAutoCloseValve, GetStaticADControlParam)

/**
 * @brief 蠕动泵控制接口。
 * @details 定义了一序列蠕动泵控制相关的操作。
 */
OOLUA_PROXY(,LCPeristalticPumpInterface, DeviceInterface)
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
OOLUA_EXPORT_FUNCTIONS(LCPeristalticPumpInterface, GetTotalPumps, GetPumpFactor, SetPumpFactor,
                       GetMotionParam, SetMotionParam, GetPumpStatus, StartPump,
                       StopPump, GetPumpVolume, ExpectPumpResult)
OOLUA_EXPORT_FUNCTIONS_CONST(LCPeristalticPumpInterface)

///**
// * @brief 电磁阀控制接口。
// * @details 定义了一序列电磁阀控制相关的操作。
// */
//OOLUA_PROXY(,SolenoidValveInterface, DeviceInterface)
//    OOLUA_TAGS(
//        No_default_constructor //没有默认构造函数时可以使用此标签
//    )
//    OOLUA_CTORS(
//        OOLUA_CTOR(DscpAddress)
//    )
//    OOLUA_MFUNC(GetTotalValves)
//    OOLUA_MFUNC(GetValveMap)
//    OOLUA_MFUNC(SetValveMap)

//OOLUA_PROXY_END
//OOLUA_EXPORT_FUNCTIONS(SolenoidValveInterface, GetTotalValves, GetValveMap, SetValveMap)
//OOLUA_EXPORT_FUNCTIONS_CONST(SolenoidValveInterface)


/**
 * @brief 电磁阀控制接口。
 * @details 定义了一序列电磁阀控制相关的操作。
 */
OOLUA_PROXY(,LCSolenoidValveInterface, DeviceInterface)
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
    OOLUA_MFUNC(GetSensorsMap)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LCSolenoidValveInterface, GetTotalValves, GetValveMap, SetValveMap, SetValveMapNormalOpen, GetSensorsMap)
OOLUA_EXPORT_FUNCTIONS_CONST(LCSolenoidValveInterface)

/**
 * @brief 液路控制器。
 * @details
 */
OOLUA_PROXY(,LiquidController, BaseController)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(Init)
    OOLUA_MFUNC(Uninit)
//    OOLUA_MEM_FUNC(maybe_null<lua_return<PeristalticPumpInterface*> >, GetIPeristalticPump)
    OOLUA_MFUNC(GetIPeristalticPump)
//    OOLUA_MEM_FUNC(maybe_null<lua_return<SolenoidValveInterface*> >, GetISolenoidValve)
    OOLUA_MFUNC(GetISolenoidValve)
    OOLUA_MFUNC(GetITemperatureControl)
//    OOLUA_MEM_FUNC(maybe_null<lua_return<OpticalMeterInterface*> >, GetIOpticalMeter)
//    OOLUA_MFUNC(GetIOpticalMeter)
//    OOLUA_MEM_FUNC(void, Register, cpp_in_p<ISignalNotifiable*>)
    OOLUA_MFUNC(Register)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LiquidController, Init, Uninit, GetIPeristalticPump,
                        GetISolenoidValve, GetITemperatureControl, Register)
OOLUA_EXPORT_FUNCTIONS_CONST(LiquidController)

LiquidControllerPluginProxy::LiquidControllerPluginProxy()
{

}

void LiquidControllerPluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();
//    lua->register_class<MeterPoint>();
//    lua->register_class<MeterPoints>();
    lua->register_class<LCMotionParam>();
    lua->register_class<LCPumpStatus>();
    lua->register_class<LCRollDirection>();
    lua->register_class<LCPumpOperateResult>();
    lua->register_class<LCPumpResult>();
    lua->register_class<LCValveMap>();
    lua->register_class<LCTemperature>();
    lua->register_class<LCThermostatParam>();
    lua->register_class<LCTempCalibrateFactor>();
    lua->register_class<LCThermostatMode>();
    lua->register_class<LCThermostatOperateResult>();
    lua->register_class<LCThermostatResult>();
//    lua->register_class<MeterMode>();
//    lua->register_class<MeterResult>();
//    lua->register_class<StaticMeterADControlResult>();
//    lua->register_class<OpticalMeterInterface>();
//    lua->register_class<SolenoidValveInterface>();
    lua->register_class<LCSolenoidValveInterface>();
    lua->register_class<LCPeristalticPumpInterface>();
    lua->register_class<LCTemperatureControlInterface>();
    lua->register_class<LiquidController>();
}

