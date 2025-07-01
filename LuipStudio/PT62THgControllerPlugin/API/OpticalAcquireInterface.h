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
#include <vector>
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
 * @brief 静态AD调节控制结果。
 */
enum class StaticADControlResult
{
    Unfinished = 0,       ///<静态AD调节未完成。
    Finished = 1,     ///<  静态AD调节完成。
};

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
 * @brief 光学信号采集接口。
 * @details 定义了一序列光学信号采集相关的操作。
 */
class LUIP_SHARE OpticalAcquireInterface : public DeviceInterface
{
public:
    OpticalAcquireInterface(DscpAddress addr);
    // 设置采集信号AD上报周期。
    bool SetAcquireADNotifyPeriod(float period);
    // 启动采集过程。
    bool StartAcquirer(float samplePeriod);
    // 停止采集过程。
    bool StopAcquirer();
    // 打开汞灯。
    bool TurnOnHgLamp();
    // 关闭汞灯。
    bool TurnOffHgLamp();
    // 打开LED。
    bool TurnOnLED();
    // 关闭LED。
    bool TurnOffLED();
    // 启动静态AD电位控制
    bool StartStaticADControl(Uint8 index, Uint32 targetAD);
    // 停止静态AD电位控制
    bool StopStaticADControl();
    // 获取目标器件电位默认参数
    Uint16 GetStaticADControlParam(Uint8 index) const;
    // 设置目标器件电位默认参数
    bool SetStaticADControlParam(Uint8 index, Uint16 value);
    // 静态AD调节功能是否有效
    bool IsStaticADControlValid() const;
    // 获取AD放大电路开关状态
    Uint16 GetStaticADLevelParam(Uint8 index) const;
    // 设置默认AD放大电路开关状态
    bool SetStaticADLevelParam(Uint8 index, Uint16 value);
    // 信号AD采集完成事件。
    Uint32 ExpectADAcquirer(long timeout, std::vector<OpticalAD> &ad);
    // 注册信号AD定时上报事件。
    void RegisterAcquireADNotice(IEventReceivable *handle);
    //启动调节后，调节完成将产生该事件。
    StaticADControlResult ExpectStaticADControlResult(long timeout);
};

}
}

#endif  //CONTROLLER_API_OPTICALACQUIREINTERFACE_H
