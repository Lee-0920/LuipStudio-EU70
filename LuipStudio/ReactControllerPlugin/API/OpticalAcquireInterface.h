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
 * @brief 静态AD调节控制结果。
 */
enum class StaticADControlResult
{
    Unfinished = 0,       ///<静态AD调节未完成。
    Finished = 1,     ///<  静态AD调节完成。
};

/**
 * @brief 双通道测量信号类型
 * @details
 */
typedef enum
{
    MEASURE_TYPE_NONE = 0,   /// 未处于测量状态
    MEASURE_TYPE_TP,         /// 测试总磷
    MEASURE_TYPE_TN,         /// 测量总氮
}enumMeasureType;

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
    bool TurnOnLED() const;
    // 关闭LED灯。
    bool TurnOffLED() const;
    // 打开氙灯。
    bool TurnOnXonen() const;
    // 关闭氙灯。
    bool TurnOffXonen() const;
    // 闪烁氙灯。
    bool GlitterXonen(bool isOpen = true) const;
    // 设置采集信号AD上报周期。
    bool SetAcquireADNotifyPeriod(float period);
    // 启动采集过程。
    bool StartAcquirer(float acquireTime);
    // 停止采集过程。
    bool StopAcquirer();
    // 启动静态AD电位控制
    bool StartStaticADControl(Uint8 index, Uint32 targetAD);
    // 停止静态AD电位控制
    bool StopStaticADControl();
    // 获取目标器件电位默认参数
    Uint16 GetStaticADControlParam(Uint8 index) const;
    // 设置目标器件电位默认参数
    bool SetStaticADControlParam(Uint8 index, Uint16 value);
    // 静态AD调节功能是否有效
    bool IsStaticADControlValid();
    // 查询LED DAC默认输出电压
    float GetLEDDefaultValue() const;
    // 设置LED DAC默认输出电压
    bool SetLEDDefaultValue(float value);

    // 信号AD采集完成事件。
    AcquiredResult ExpectADAcquirer(long timeout);

    //启动调节后，调节完成将产生该事件。
    StaticADControlResult ExpectStaticADControlResult(long timeout);

    // 注册信号AD定时上报事件。
    void RegisterAcquireADNotice(IEventReceivable *handle);
};

}
}

#endif  //CONTROLLER_API_OPTICALACQUIREINTERFACE_H
