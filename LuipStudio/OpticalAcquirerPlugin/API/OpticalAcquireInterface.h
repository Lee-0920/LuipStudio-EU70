/**
 * @file
 * @brief 光学信号采集接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_OPTICALACQUIREINTERFACE_H)
#define CONTROLLER_API_OPTICALACQUIREINTERFACE_H

#include "../LuipShare.h"
#include "Communication/IEventReceivable.h"
#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"

using System::Uint16;
using System::Uint32;
using Communication::IEventReceivable;

namespace Controller
{
namespace API
{    

/**
 * @brief 光学信号采集的AD值。
 * @details
 */
struct OpticalAD
{
    Uint32 reference;                ///光学信号采集的参考AD值
    Uint32 measure;                  ///光学信号采集的测量AD值
};

/**
 * @brief 光学信号采集结果码。
 * @details
 */
enum class AcquiredOperateResult
{
    Finished,                   ///采集正常完成。
    Failed,                     ///采集中途出现故障，未能完成。
    Stopped                     ///采集被停止。
};

/**
 * @brief 光学信号采集的结果。
 * @details
 */
struct AcquiredResult
{
    OpticalAD ad;
    int result;         // 泵操作结果码,赋值为AcquiredOperateResult
};

/**
 * @brief LED控制参数。
 * @details
 */
struct LedParam
{
    float proportion;     // PID的比例系数
    float integration;    // PID的积分系数
    float differential;   // PID的微分系数
};

/**
 * @brief 光学信号采集接口。
 * @details 定义了一序列光学信号采集相关的操作。
 */
class LUIP_SHARE OpticalAcquireInterface : public DeviceInterface
{
public:
    OpticalAcquireInterface(DscpAddress addr);
    // 打开LED灯。
    bool TurnOnLED();
    // 设置采集信号AD上报周期。
    bool SetAcquireADNotifyPeriod(float period);
    // 启动采集过程。
    // 
    // 启动后，将自动打开LED并采样，采集到的信号数据将以事件。
    bool StartAcquirer(float acquireTime);
    // 停止采集过程。
    bool StopAcquirer();
    // 启动LED控制器,并打开LED灯。
    bool StartLEDController();
    // 关闭LED灯。
    bool TurnOffLED();
    // 查询LED控制器设定的目标值。
    Uint32 GetLEDControllerTartet();
    // 设置LED控制器设定的目标值。
    bool SetLEDControllerTarget(Uint32 target);
    // 查询LED控制器参数。
    LedParam GetLEDControllerParam();
    // 设置LED控制器设定的目标值。
    bool SetLEDControllerParam(LedParam param);
    //信号AD采集完成事件。
    //
    //启动采集后，采完数据时将产生该事件。
    AcquiredResult ExpectADAcquirer(long timeout);

    // 注册信号AD定时上报事件。
    void RegisterAcquireADNotice(IEventReceivable *handle);
};

}
}

#endif  //CONTROLLER_API_OPTICALACQUIREINTERFACE_H
