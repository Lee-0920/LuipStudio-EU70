/**
 * @file
 * @brief 附加温度控制接口。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_EXTRATEMPERATURECONTROLINTERFACE_H)
#define CONTROLLER_API_EXTRATEMPERATURECONTROLINTERFACE_H

#include "TemperatureControlInterface.h"
#include "Communication/IEventReceivable.h"
#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"
#include "../LuipShare.h"

using System::Uint16;
using Communication::IEventReceivable;

namespace Controller
{
namespace API
{

/**
 * @brief 附加温度控制接口。
 * @details 定义了一序列附加温度控制相关的操作。
 */
class LUIP_SHARE ExtraTemperatureControlInterface : public DeviceInterface
{
public:
    ExtraTemperatureControlInterface(DscpAddress addr);
    // 查询温度传感器的校准系数。
    TempCalibrateFactor GetCalibrateFactor();
    // 设置温度传感器的校准系数。
    bool SetCalibrateFactor(TempCalibrateFactor factor);
    // 查询当前温度。
    Temperature GetTemperature();
    // 查询恒温控制参数。
    ThermostatParam GetThermostatParam();
    // 设置恒温控制参数。
    bool SetThermostatParam(ThermostatParam param);
    // 查询恒温器的工作状态。
    Uint16 GetThermostatStatus();
    // 开始恒温。
    bool StartThermostat(ThermostatMode mode, float targetTemp, float toleranceTemp, float timeout);
    // 停止恒温控制。
    bool StopThermostat();
    // 设置温度上报周期。
    bool SetTemperatureNotifyPeriod(float period);
    // 恒温结果事件。
    ThermostatResult ExpectThermostat(long timeout);
    // 注册温度定时上报事件。
    void RegisterTemperatureNotice(IEventReceivable *handle);
    //机箱风扇输出
    //bool BoxFanSetOutput(float level);
};

}
}

#endif  //CONTROLLER_API_EXTRATEMPERATURECONTROLINTERFACE_H
