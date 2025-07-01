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
#include "Code/OpticalAcquireInterface.h"

using System::Uint16;
using System::Uint32;
using Communication::IEventReceivable;

namespace Controller
{
namespace API
{

/**
 * @brief 信号采集ADC类型
 */
enum class ADCType
{
     AD7791 = 0,       ///< AD7791
     ADS1146 = 1,      ///< ADS1146
};

/**
 * @brief 光学信号采集模块工作模式
 */
enum class ADCWorkMode
{
     OnlyAD7791 = 0,       ///< 仅AD7791工作
     OnlyADS1146 = 1,      ///< 仅ADS1146工作
     Both = 2              ///< AD7791和ADS1146切换工作
};

/**
 * @brief LED调节结果。
 */
enum class AdjustResult
{
     Finished = 0,     ///< LED调节正常完成。
     Failed = 1,       ///<LED调节中途出现故障，未能完成。
     Stopped = 2,      ///<LED调节被停止。
     Timeout = 3           ///< LED调节超时，指定时间内仍未达到目标AD。
};

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
 * @brief 参考AD固定调节参数。
 * @details
 */
struct RefADParam
{
    Uint32 target;
    Uint32 tolorance;
    Uint32 timeout;
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
    // 设置采集信号AD上报周期。
    bool SetAcquireADNotifyPeriod(float period);
    // 启动采集过程。
    // 
    // 启动后，将自动打开LED并采样，采集到的信号数据将以事件。
    bool StartAcquirer(float acquireTime, Uint8 index);
    // 停止采集过程。
    bool StopAcquirer();
    // 启动LED控制器,并打开LED灯。
    bool StartLEDController();
    // 关闭LED灯。
    bool TurnOffLED() const;
    // 查询LED控制器设定的目标值。
    Uint32 GetLEDControllerTartet() const;
    // 设置LED控制器设定的目标值。
    bool SetLEDControllerTarget(Uint32 target);
    // 设置LED控制器设定的目标值（到达目标值后不再调节）。
    bool SetLEDControllerStableTarget(Uint32 targetAD, Uint32 tolorance, Uint32 timeout);
    // 查询LED控制器参数。
    LedParam GetLEDControllerParam() const;
    // 设置LED控制器设定的目标值。
    bool SetLEDControllerParam(LedParam param);

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
    // 查询LED DAC默认输出电压
    float GetLEDDefaultValue() const;
    // 设置LED DAC默认输出电压
    bool SetLEDDefaultValue(float value);
    // 氙灯控制
    bool XonenCtrl(Uint8 code) const;
    // 获取ADC工作模式
    ADCWorkMode GetADCWorkMode() const;
    // 设置ADC工作模式
    bool SetADCWorkMode(ADCWorkMode mode) const;


    //信号AD采集完成事件。
    //
    //启动采集后，采完数据时将产生该事件。
    AcquiredResult ExpectADAcquirer(long timeout);

    // 注册信号AD定时上报事件。
    void RegisterAcquireADNotice(IEventReceivable *handle);

    // 注册漏液检测AD定时上报事件。
    void RegisterAcquireCheckLeaking(IEventReceivable *handle);

    //参考AD调节完成事件。
    //
    //启动调节后，调节完成将产生该事件。
    AdjustResult ExpectADStableAdjust(long timeout);

    //启动调节后，调节完成将产生该事件。
    StaticADControlResult ExpectStaticADControlResult(long timeout);
};

}
}

#endif  //CONTROLLER_API_OPTICALACQUIREINTERFACE_H
