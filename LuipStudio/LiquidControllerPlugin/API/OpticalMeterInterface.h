/**
 * @file
 * @brief 光学定量接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_OPTICALMETERINTERFACE_H)
#define CONTROLLER_API_OPTICALMETERINTERFACE_H

#include <vector>
#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"
#include "Communication/IEventReceivable.h"
#include "MeterPoints.h"
#include "PeristalticPumpInterface.h"
#include "../LuipShare.h"

using std::vector;
using Communication::IEventReceivable;
using System::Uint8;
using System::Uint32;

namespace Controller
{
namespace API
{    

/**
 * @brief 静态定量AD调节控制结果。
 */
enum class StaticMeterADControlResult
{
    Unfinished = 0,       ///<静态AD调节未完成。
    Finished = 1,     ///<  静态AD调节完成。
};


/**
 * @brief 光学定量模式。
 * @details
 */
enum class MeterMode
{
    Accurate = 0,           ///精准定量模式，精准定量到指定体积的定量点。
    Direct = 1,             ///直接定量模式，泵只向一个方向启动一次便可完成定量
    Smart = 2,              ///智能定量模式，结合定量点和泵计步综合定量出任意体积。
    Ropiness = 3,           ///粘稠定量模式
};

/**
 * @brief 定量操作结果。
 * @details
 */
enum class MeterResult
{
    Finished = 0,       ///定量正常完成。
    Failed = 1,         ///定量中途出现故障，未能完成。
    Stopped = 2,        ///定量被停止。
    Overflow = 3,       ///定量溢出。
    Unfinished = 4,     ///定量目标未达成。
    AirBubble = 5,     ///有气泡。
};

/**
 * @brief 光学定量接口。
 * @details 定义了一序列光学定量相关的操作。
 */
class LUIP_SHARE OpticalMeterInterface : public DeviceInterface
{
public:
    OpticalMeterInterface(DscpAddress addr);
    // 打开LED灯。
    bool TurnOnLED(Uint8 num) const;
    // 关闭LED灯。
    bool TurnOffLED(Uint8 num) const;
    // 动校准定量泵，并永久保存计算出的校准系数。
//    bool AutoCalibratePump(Uint32 valve);
    // 查询定量泵的校准系数。
    float GetPumpFactor() const;
    // 设置定量泵的校准系数。
    bool SetPumpFactor(float factor);
    // 查询定量点体积。
    MeterPoints GetMeterPoints() const;
    // 设置定量点体积。
    bool SetMeterPoints(MeterPoints points);
    // 查询定量的工作状态。
    Uint16 GetMeterStatus() const;
    // 开始定量。
    bool StartMeter(RollDirection dir, MeterMode mode, float volume, float limitVolume);
    // 停止定量。
    bool StopMeter();
    // 定量结束自动关闭阀开关
    bool IsAutoCloseValve(bool isCloseValve) const;
    // 设置定量光学信号AD上报周期。
    bool SetOpticalADNotifyPeriod(float period);
    // 定量结果事件。
    MeterResult  ExpectMeterResult(int timeout);
    // 自动校准结果事件。
//    MeterResult ExpectAutoCalibrateResult(long timeout);
    // 设定定量速度。
    bool SetMeteSpeed(float speed);
    // 设定定量速度。
    float GetMeteSpeed() const;
    // 设置定量结束时阀要改变成的状态
    bool SetMeterFinishValveMap(Uint32 map);
    // 获取获取某个定量点光信号AD
    Uint32 GetSingleOpticalAD(Uint8 num) const;

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

    //启动调节后，调节完成将产生该事件。
    StaticMeterADControlResult ExpectStaticADControlResult(long timeout);

    // 注册定量光学信号AD定时上报事件。
    void RegisterOpticalADNotice(IEventReceivable *handle);
};

}
}

#endif  //CONTROLLER_API_OPTICALMETERINTERFACE_H
