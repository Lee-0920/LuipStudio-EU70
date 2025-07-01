/**
 * @file
 * @brief 电磁阀控制接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_IOCONTROLINTERFACT_H)
#define CONTROLLER_API_IOCONTROLINTERFACT_H

#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"
#include "../LuipShare.h"
#include "Communication/IEventReceivable.h"
using Communication::IEventReceivable;
namespace Controller
{
namespace API
{

/**
 * @brief 电磁阀控制接口。
 * @details 定义了一序列电磁阀控制相关的操作。
 */
class LUIP_SHARE IOControlInterface : public DeviceInterface
{
public:
    IOControlInterface(DscpAddress addr);
    // 设置电流输出值
    bool SetOutputCurrent(Uint8 index, float value);
    // 查询当前电流输出值
    float GetOutputCurrent(Uint8 index) const;
    //继电器控制
    bool RelayOn(Uint8 index);
    bool RelayOff(Uint8 index);
    // 注册干接点触发事件。
    void RegisterTriggerNotice(IEventReceivable *handle);
};

}
}

#endif  //CONTROLNET_API_SOLENOIDVALVEINTERFACE_H
