/**
 * @file
 * @brief 扩展的光学信号采集接口。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#ifndef EXTOPTICALACQUIREINTERFACE_H
#define EXTOPTICALACQUIREINTERFACE_H


#include "../LuipShare.h"
#include "Communication/IEventReceivable.h"
#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"
#include "Code/ExtOpticalAcquireInterface.h"
#include "OpticalAcquireInterface.h"

using System::Uint16;
using System::Uint32;
using Communication::IEventReceivable;

namespace Controller
{
namespace API
{

/**
 * @brief 扩展的光学信号采集接口。
 * @details 定义了一序列光学信号采集相关的操作。
 * @note 驱动板程序V1.5.0.0以上支持
 */
class LUIP_SHARE ExtOpticalAcquireInterface : public DeviceInterface
{
public:
    ExtOpticalAcquireInterface(DscpAddress addr);
    // 打开LED灯。
    bool TurnOnLED(Uint8 index) const;
    // 设置采集信号AD上报周期。
    bool SetAcquireADNotifyPeriod(float period);
    // 启动采集过程。
    //
    // 启动后，将自动打开LED并采样，采集到的信号数据将以事件。
    bool StartAcquirer( float acquireTime);
    // 停止采集过程。
    bool StopAcquirer();
    // 启动LED控制器,并打开LED灯。
    bool StartLEDController(Uint8 index);
    // 关闭LED灯。
    bool TurnOffLED(Uint8 index) const;
    // 查询LED控制器设定的目标值。
    Uint32 GetLEDControllerTartet(Uint8 index) const;
    // 设置LED控制器设定的目标值。
    bool SetLEDControllerTarget(Uint8 index, Uint32 target);
    // 启动LED单次定向调节至目标值（到达目标值后不再调节）。
    bool StartLEDOnceAdjust(Uint8 index, Uint32 targetAD, Uint32 tolorance, Uint32 timeout);
    // 停止LED单次定向调节至目标值操作
    bool StopLEDOnceAdjust(Uint8 index) const;
    // 查询LED控制器参数。
    LedParam GetLEDControllerParam(Uint8 index) const;
    // 设置LED控制器设定的目标值。
    bool SetLEDControllerParam(Uint8 index, LedParam param);

    // 启动静态AD电位控制
    bool StartStaticADControl(Uint8 index, Uint32 targetAD, bool isExtLED);
    // 停止静态AD电位控制
    bool StopStaticADControl();
    // 获取目标器件电位默认参数
    Uint16 GetStaticADControlParam(Uint8 index, bool isExtLED) const;
    // 设置目标器件电位默认参数
    bool SetStaticADControlParam(Uint8 index, Uint16 value, bool isExtLED);
    // 静态AD调节功能是否有效
    bool IsStaticADControlValid() const;

    // 查询LED DAC默认输出电压
    float GetLEDDefaultValue(Uint8 index) const;
    // 设置LED DAC默认输出电压
    bool SetLEDDefaultValue(Uint8 index, float value);

    //信号AD采集完成事件。
    //
    //启动采集后，采完数据时将产生该事件。
    AcquiredResult ExpectADAcquirer(long timeout);

    // 注册信号AD定时上报事件。
    ///void RegisterAcquireADNotice(IEventReceivable *handle);

    //参考AD调节完成事件。
    //
    //启动调节后，调节完成将产生该事件。
    AdjustResult ExpectLEDOnceAdjust(long timeout);

    //启动调节后，调节完成将产生该事件。
    StaticADControlResult ExpectStaticADControlResult(long timeout);
};

}
}

#endif // EXTOPTICALACQUIREINTERFACE_H
