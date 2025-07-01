#include "API/MeterPoints.h"
#include "API/OpticalMeterInterface.h"
#include "API/PeristalticPumpInterface.h"
#include "API/SolenoidValveInterface.h"
#include "DriveController.h"
#include "Communication/CommunicationProxy.h"
#include "ControllerPlugin/ControllerPluginProxy.h"
#include "LuaEngine/LuaEngine.h"
#include "DriveControllerPluginProxy.h"
#include "API/TemperatureControlInterface.h"
//#include "API/OpticalAcquireInterface.h"

using Communication::Dscp::DscpAddress;
using namespace Controller;
using namespace Controller::API;
using namespace OOLUA;
using namespace Lua;

/**
 * @brief 光学定量点体积。
 * @details
 */
OOLUA_PROXY(,MeterPoint)
    OOLUA_MGET_MSET(setVolume, GetSetVolume, SetSetVolume)
    OOLUA_MGET_MSET(realVolume, GetRealVolume, SetRealVolume)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(MeterPoint, SetSetVolume, SetRealVolume)
OOLUA_EXPORT_FUNCTIONS_CONST(MeterPoint, GetSetVolume, GetRealVolume)

/**
 * @brief 光学定量点。
 * @details
 */
OOLUA_PROXY(,MeterPoints)
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
OOLUA_EXPORT_FUNCTIONS(MeterPoints, SetNum, GetNum, SetPoint, GetPoint)
OOLUA_EXPORT_FUNCTIONS_CONST(MeterPoints)

/**
 * @brief 蠕动泵运动参数。
 * @details
 */
OOLUA_PROXY(,MotionParam)
    OOLUA_MGET_MSET(acceleration, GetAcceleration, SetAcceleration)
    OOLUA_MGET_MSET(speed, GetSpeed, SetSpeed)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(MotionParam, SetAcceleration, SetSpeed)
OOLUA_EXPORT_FUNCTIONS_CONST(MotionParam, GetAcceleration, GetSpeed)

/**
 * @brief 蠕动泵状态。
 * @details
 */
OOLUA_PROXY(,PumpStatus)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Idle)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
        OOLUA_ENUM(Busy)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(PumpStatus)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 蠕动泵旋转方向。
 * @details
 */
OOLUA_PROXY(,RollDirection)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Suck)   // 枚举值
        OOLUA_ENUM(Drain)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(RollDirection)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 蠕动泵操作结果码。
 * @details
 */
OOLUA_PROXY(,PumpOperateResult)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Finished)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
        OOLUA_ENUM(Stopped)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(PumpOperateResult)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 蠕动泵结果。
 * @details
 */
OOLUA_PROXY(,PumpResult)
    OOLUA_MGET_MSET(index, GetIndex, SetIndex)
    OOLUA_MGET_MSET(result, GetResult, SetResult)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(PumpResult, SetIndex, SetResult)
OOLUA_EXPORT_FUNCTIONS_CONST(PumpResult, GetIndex, GetResult)

/**
 * @brief 阀映射图。
 * @details
 */
OOLUA_PROXY(,ValveMap)

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
OOLUA_EXPORT_FUNCTIONS(ValveMap, SetData, GetData, SetOn, SetOff, IsOn, clear)
OOLUA_EXPORT_FUNCTIONS_CONST(ValveMap)

/**
 * @brief 光学定量模式。
 * @details
 */
OOLUA_PROXY(,MeterMode)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Accurate)   // 枚举值
        OOLUA_ENUM(Direct)   // 枚举值
        OOLUA_ENUM(Smart)   // 枚举值
        OOLUA_ENUM(Ropiness)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(MeterMode)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 定量操作结果。
 * @details
 */
OOLUA_PROXY(,MeterResult)
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
OOLUA_EXPORT_NO_FUNCTIONS(MeterResult)  // 导出函数声明（没有函数也需要声明）

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
 * @brief 电极扫描结果。
 */
OOLUA_PROXY(,ElectrodeScanResult)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Finished)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(ElectrodeScanResult)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 光学定量接口。
 * @details 定义了一序列光学定量相关的操作。
 */
OOLUA_PROXY(,OpticalMeterInterface, DeviceInterface)
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
OOLUA_EXPORT_FUNCTIONS(OpticalMeterInterface, TurnOnLED, TurnOffLED,
                       SetPumpFactor, SetMeterPoints, StartMeter, StopMeter,
                       IsAutoCloseValve, SetOpticalADNotifyPeriod,ExpectMeterResult,
                       SetMeteSpeed, SetMeterFinishValveMap, RegisterOpticalADNotice)
OOLUA_EXPORT_FUNCTIONS_CONST(OpticalMeterInterface, GetPumpFactor, GetMeterPoints,
                             GetMeterStatus, GetMeteSpeed, GetSingleOpticalAD)

/**
 * @brief 蠕动泵控制接口。
 * @details 定义了一序列蠕动泵控制相关的操作。
 */
OOLUA_PROXY(,PeristalticPumpInterface, DeviceInterface)
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
OOLUA_EXPORT_FUNCTIONS(PeristalticPumpInterface, GetTotalPumps, GetPumpFactor, SetPumpFactor,
                       GetMotionParam, SetMotionParam, GetPumpStatus, StartPump,
                       StopPump, GetPumpVolume, ExpectPumpResult)
OOLUA_EXPORT_FUNCTIONS_CONST(PeristalticPumpInterface)

/**
 * @brief 电磁阀控制接口。
 * @details 定义了一序列电磁阀控制相关的操作。
 */
OOLUA_PROXY(,SolenoidValveInterface, DeviceInterface)
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
OOLUA_EXPORT_FUNCTIONS(SolenoidValveInterface, GetTotalValves, GetValveMap, SetValveMap)
OOLUA_EXPORT_FUNCTIONS_CONST(SolenoidValveInterface)

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
    OOLUA_MFUNC(SetCalibrateFactor)         ///< 设置温度传感器的校准系数
    OOLUA_MFUNC(StartThermostat)            ///< 开始恒温
    OOLUA_MFUNC(StopThermostat)             ///< 停止恒温控制
    OOLUA_MFUNC(SetTemperatureNotifyPeriod) ///< 设置温度上报周期
    OOLUA_MFUNC(ExpectThermostat)           ///< 恒温结果事件
    OOLUA_MFUNC(BoxFanSetOutput)            ///< 机箱风扇输出
    OOLUA_MFUNC(GetCalibrateFactor)         ///< 查询温度传感器的校准系数
    OOLUA_MFUNC(GetTemperature)             ///< 查询当前温度
    OOLUA_MFUNC(GetThermostatParam)         ///< 查询恒温控制参数
    OOLUA_MFUNC(GetThermostatStatus)        ///< 查询恒温器的工作状态
    OOLUA_MFUNC(SetThermostatParam)         ///<  设置恒温控制参数
//    OOLUA_MEM_FUNC(void, RegisterTemperatureNotice, cpp_in_p<IEventReceivable*>)
    OOLUA_MFUNC(RegisterTemperatureNotice)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(TemperatureControlInterface,
                       SetCalibrateFactor,
                       StartThermostat,
                       StopThermostat,
                       SetTemperatureNotifyPeriod,
                       ExpectThermostat,
                       BoxFanSetOutput,
                       GetCalibrateFactor,
                       GetTemperature,
                       GetThermostatParam,
                       GetThermostatStatus,
                       RegisterTemperatureNotice,
                       SetThermostatParam)
OOLUA_EXPORT_FUNCTIONS_CONST(TemperatureControlInterface)

/**
 *  附加温度控制器接口
 */
OOLUA_PROXY(,ExtraTemperatureControlInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(SetCalibrateFactor)         ///< 设置温度传感器的校准系数
    OOLUA_MFUNC(StartThermostat)            ///< 开始恒温
    OOLUA_MFUNC(StopThermostat)             ///< 停止恒温控制
    OOLUA_MFUNC(SetTemperatureNotifyPeriod) ///< 设置温度上报周期
    OOLUA_MFUNC(ExpectThermostat)           ///< 恒温结果事件
    OOLUA_MFUNC(GetCalibrateFactor)         ///< 查询温度传感器的校准系数
    OOLUA_MFUNC(GetTemperature)             ///< 查询当前温度
    OOLUA_MFUNC(GetThermostatParam)         ///< 查询恒温控制参数
    OOLUA_MFUNC(GetThermostatStatus)        ///< 查询恒温器的工作状态
    OOLUA_MFUNC(SetThermostatParam)         ///< 设置恒温控制参数
//    OOLUA_MEM_FUNC(void, RegisterTemperatureNotice, cpp_in_p<IEventReceivable*>)
    OOLUA_MFUNC(RegisterTemperatureNotice)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ExtraTemperatureControlInterface,
                       SetCalibrateFactor,
                       StartThermostat,
                       StopThermostat,
                       SetTemperatureNotifyPeriod,
                       ExpectThermostat,
                       GetCalibrateFactor,
                       GetTemperature,
                       GetThermostatParam,
                       GetThermostatStatus,
                       RegisterTemperatureNotice,
                       SetThermostatParam)
OOLUA_EXPORT_FUNCTIONS_CONST(ExtraTemperatureControlInterface)


/**
 * @brief 电极信号采集接口。
 * @details 定义了一序列电极信号采集相关的操作。
 */
OOLUA_PROXY(,ElectrodeAcquireInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(SetAcquireElectrodeADNotifyPeriod)  /// 设置采集电极信号AD上报周期
    OOLUA_MFUNC(SetDACPotential)                    /// 设置DAC输出电位
    OOLUA_MFUNC(GetDACPotential)                    /// 获取DAC输出电位
    OOLUA_MFUNC(TurnOffDAC)                         /// 关闭DAC输出
    OOLUA_MFUNC(SetADCSensitivity)                  /// 设置ADC灵敏度
    OOLUA_MFUNC(GetADCSensitivity)                  /// 获取ADC灵敏度
    OOLUA_MFUNC(GetElectrodeScanResolution)         /// 获取电极扫描分辨率
    OOLUA_MFUNC(StartElectrodeScan)                 /// 启动电极扫描
    OOLUA_MFUNC(ExpectElectrodeScan)                /// 等待电极扫描完成事件
    OOLUA_MFUNC(StopElectrodeADAcquirer)
    OOLUA_MFUNC(GetScanStatus)                      /// 查询电极扫描状态
//    OOLUA_MEM_FUNC(void, RegisterAcquireElectrodeADNotice, cpp_in_p<IEventReceivable*>)
    OOLUA_MFUNC(RegisterAcquireElectrodeADNotice)   /// 注册电极信号AD定时上报事件

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ElectrodeAcquireInterface, SetAcquireElectrodeADNotifyPeriod, SetDACPotential, GetDACPotential,
                       TurnOffDAC, SetADCSensitivity, GetADCSensitivity, GetElectrodeScanResolution, StartElectrodeScan,StopElectrodeADAcquirer,
                       ExpectElectrodeScan, GetScanStatus, RegisterAcquireElectrodeADNotice)
OOLUA_EXPORT_FUNCTIONS_CONST(ElectrodeAcquireInterface)


/**
 * @brief 曲线数据类。
 * @details 定义了电极采集数据及结果计算的操作。
 */
OOLUA_EXPORT_FUNCTIONS(CurveData, SetData, GetWholeData, AutoCalculateArea,
                       CalculateArea, CalculatePeak, GetPeakVolt)
OOLUA_EXPORT_FUNCTIONS_CONST(CurveData, GetData, GetLength, GetBegin, GetEnd)


/**
 * @brief 驱动控制器。
 * @details
 */
OOLUA_PROXY(,DriveController, BaseController)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(Init)
    OOLUA_MFUNC(Uninit)
    OOLUA_MFUNC(GetIPeristalticPump)
    OOLUA_MFUNC(GetISolenoidValve)
    OOLUA_MFUNC(GetIOpticalMeter)
    OOLUA_MFUNC(GetITemperatureControl)
    OOLUA_MFUNC(GetIExtraTemperatureControl)
    OOLUA_MFUNC(GetIElectrodeAcquire)
    //OOLUA_MFUNC(GetCurveData)
    OOLUA_MEM_FUNC(maybe_null<lua_return<CurveData*>>, GetCurveData, float, float, float)
    OOLUA_MFUNC(Register)
    OOLUA_MFUNC(GetCurrentTemperature)
    OOLUA_MFUNC(GetCurrentExtraTemperature)
    OOLUA_MFUNC_CONST(GetDigestTemperature)
    OOLUA_MFUNC_CONST(GetEnvironmentTemperature)
    OOLUA_MFUNC_CONST(GetMeasureCellTemperature)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(DriveController, Init, Uninit, GetIPeristalticPump, GetISolenoidValve, GetIOpticalMeter,GetCurrentTemperature,
                       GetCurrentExtraTemperature, GetITemperatureControl, GetIExtraTemperatureControl, GetIElectrodeAcquire, GetCurveData, Register)
OOLUA_EXPORT_FUNCTIONS_CONST(DriveController,
                             GetDigestTemperature,
                             GetEnvironmentTemperature,
                             GetMeasureCellTemperature)

DriveControllerPluginProxy::DriveControllerPluginProxy()
{

}

void DriveControllerPluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    lua->register_class<MeterPoint>();
    lua->register_class<MeterPoints>();
    lua->register_class<MotionParam>();
    lua->register_class<PumpStatus>();
    lua->register_class<RollDirection>();
    lua->register_class<PumpOperateResult>();
    lua->register_class<PumpResult>();
    lua->register_class<ValveMap>();
    lua->register_class<MeterMode>();
    lua->register_class<MeterResult>();
    lua->register_class<Temperature>();
    lua->register_class<ThermostatParam>();
    lua->register_class<TempCalibrateFactor>();
    lua->register_class<ThermostatMode>();
    lua->register_class<ThermostatOperateResult>();
    lua->register_class<ThermostatResult>();
    lua->register_class<OpticalMeterInterface>();
    lua->register_class<PeristalticPumpInterface>();
    lua->register_class<SolenoidValveInterface>();
    lua->register_class<TemperatureControlInterface>();
    lua->register_class<ExtraTemperatureControlInterface>();
    lua->register_class<ElectrodeAcquireInterface>();
    lua->register_class<CurveData>();
    lua->register_class<DriveController>();
}

