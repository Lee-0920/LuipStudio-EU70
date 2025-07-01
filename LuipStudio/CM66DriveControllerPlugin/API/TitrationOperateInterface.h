/**
 * @file
 * @brief 滴定控制接口。
 * @details
 * @version 1.0.0
 * @author weizhe
 * @date 2017/5/9
 */

#if !defined(CONTROLLER_API_TITRATIONOPERATEINTERFACE_H)
#define CONTROLLER_API_TITRATIONOPERATEINTERFACE_H

#include "Communication/IEventReceivable.h"
#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"
#include "../LuipShare.h"

using System::Uint16;
using System::Uint32;
using Communication::IEventReceivable;

namespace Controller
{
namespace API
{

/**
 * @brief 滴定数据。
 * @details
 */
struct TitrationData
{
    Uint32 electrodeAD;         /// 滴定时电极AD值
    Uint32 titrationStep;       /// 滴定时已经滴定的步数
};

class LUIP_SHARE TitrationOperateInterface : public DeviceInterface
{
public:
    TitrationOperateInterface(DscpAddress addr);

    // 设置采集电极信号AD上报周期。
    bool SetAcquireElectrodeADNotifyPeriod(float period);

    // 启动泵进行滴定。
    bool StartTitration(float speed, float samplePeriod, float notifyPeriod);

    // 停止滴定。
    bool StopTitration(void);

    // 查询滴定的工作状态。
    Uint16 GetTitrationStatus();

    // 滴定过程数据采集完成事件。
    Uint32 ExpectTitrationData(long timeout, TitrationData* titrationData);

    // 注册电极信号AD定时上报事件。
    void RegisterAcquireElectrodeADNotice(IEventReceivable *handle);

    // 获取电极信号值
    Uint32 GetElectrodeSignal(void);

    // 设置滴定速度
    bool SetTitrationSpeed(float titrationSpeed);

    bool TitrationElectrodeOutput(bool isOpen);
};

}

}
#endif // CONTROLLER_API_TITRATIONOPERATEINTERFACE_H
