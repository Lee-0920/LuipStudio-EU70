/**
 * @file
 * @brief 扩展的温度控制接口。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#ifndef EXTTEMPERATURECONTROLINTERFACE_H
#define EXTTEMPERATURECONTROLINTERFACE_H

#include "TemperatureControlInterface.h"
#include "Communication/IEventReceivable.h"
#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"
#include "../LuipShare.h"

using System::Uint16;
using System::Uint8;
using Communication::IEventReceivable;

namespace Controller
{
namespace API
{

/**
 * @brief 扩展的温度控制接口。
 * @details 定义了一序列温度控制相关的操作。
 * @note 驱动板程序V1.5.0.0以上支持
 */
class LUIP_SHARE ExtTemperatureControlInterface : public DeviceInterface
{
public:
    ExtTemperatureControlInterface(DscpAddress addr);
    // 查询温度传感器的校准系数。
    TempCalibrateFactor GetCalibrateFactor(Uint8 index) const;
    // 设置温度传感器的校准系数。
    bool SetCalibrateFactor(Uint8 index, TempCalibrateFactor factor);
    // 查询恒温器温度。
    float GetThermostatTemperature(Uint8 index) const;
    // 查询恒温控制参数。
    ThermostatParam GetThermostatParam(Uint8 index) const;
    // 设置恒温控制参数。
    bool SetThermostatParam(Uint8 index, ThermostatParam param);
    // 查询恒温器的工作状态。
    Uint16 GetThermostatStatus(Uint8 index) const;
    // 开始恒温。
    bool StartThermostat(Uint8 index, ThermostatMode mode, float targetTemp,
                         float toleranceTemp, float timeout, bool isExpectEvent = true);
    // 停止恒温控制。
    bool StopThermostat(Uint8 index, bool isExpectEvent = true);
    // 设置温度上报周期。
    bool SetTemperatureNotifyPeriod(float period);
    //机箱风扇输出
    bool BoxFanSetOutput(float level);
    //设置恒温器风扇
    bool ThermostatFanSetOutput(Uint8 thermostatIndex, Uint8 fanIndex, float level);
    //查询加热丝最大占空比
    float GetHeaterMaxDutyCycle(Uint8 index) const;
    //设置加热丝最大占空比
    bool SetHeaterMaxDutyCycle(Uint8 index, float maxDutyCycle);
    // 查询当前恒温控制参数。
    ThermostatParam GetCurrentThermostatParam(Uint8 index) const;
    // 设置当前恒温控制参数。
    bool SetCurrentThermostatParam(Uint8 index, ThermostatParam param);
    // 紫外灯开
    bool TurnOnRays();
    // 紫外灯开关
    bool TurnOffRays();
    // 查询系统支持的恒温器数目。
    Uint16 GetTotalThermostat() const;
    // 查询环境温度。
    float GetEnvironmentTemperature() const;
    // 恒温结果事件。
    ThermostatResult ExpectThermostat(long timeout);
    // 注册温度定时上报事件。
    ///void RegisterTemperatureNotice(IEventReceivable *handle);
};

}
}

#endif // EXTTEMPERATURECONTROLINTERFACE_H
