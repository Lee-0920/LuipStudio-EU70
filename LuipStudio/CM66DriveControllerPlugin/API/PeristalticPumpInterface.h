/**
 * @file
 * @brief 蠕动泵控制接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_PERISTALTICPUMPINTERFACE_H)
#define CONTROLLER_API_PERISTALTICPUMPINTERFACE_H

#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"
#include "../LuipShare.h"

using System::Uint8;

namespace Controller
{
namespace API
{    

/**
 * @brief 蠕动泵运动参数。
 * @details
 */
struct MotionParam
{
    float acceleration;           ///加速度，单位为 ml/平方秒
    float speed;                  ///最大速度，单位为 ml/秒
};


/**
 * @brief 蠕动泵状态。
 * @details
 */
enum class PumpStatus
{
    Idle,                   ///泵空闲
    Failed,                 ///泵异常
    Busy                    ///泵忙碌
};

/**
 * @brief 蠕动泵抽排动作。
 * @details
 */
enum class RollDirection
{
    Suck,                      ///泵抽操作
    Drain,                     ///泵排操作
};

/**
 * @brief 泵安装类型。
 * @details 决定泵抽排操作与步进电机顺时针/逆时针转动的对应关系
 */
enum class PumpFixedType
{
    ClockwiseDrainAnticlockwiseSuck,  ///顺排逆抽
    ClockwiseSuckAnticlockwiseDrain,  ///顺抽逆排
};

/**
 * @brief 蠕动泵操作结果码。
 * @details
 */
enum class PumpOperateResult
{
    Finished,                   ///泵操作正常完成
    Failed,                     ///泵操作中途出现故障，未能完成
    Stopped                     ///泵操作被停止
};

/**
 * @brief 蠕动泵结果。
 * @details
 */
struct PumpResult
{
    Uint8 index;        // 产生事件的泵索引，0号泵为光学定量泵。
    int result;         // 泵操作结果码,赋值为PumpOperateResult。
};

/**
 * @brief 蠕动泵控制接口。
 * @details 定义了一序列蠕动泵控制相关的操作。
 */
class LUIP_SHARE PeristalticPumpInterface : public DeviceInterface
{
public:
    PeristalticPumpInterface(DscpAddress addr);
    // 查询系统支持的总泵数目。
    int GetTotalPumps();
    //查询指定泵的校准系数。
    float GetPumpFactor(int index);
    // 设置指定泵的校准系数。
    bool SetPumpFactor(int index, float factor);
    // 查询指定泵的运动参数。
    MotionParam GetMotionParam(int index);
    // 设置指定泵的运动参数。
    bool SetMotionParam(int index, MotionParam param);
    // 查询指定泵的工作状态。
    PumpStatus GetPumpStatus(int index);
    // 启动泵。
    bool StartPump(int index, RollDirection dir, float volume, float seep);
    // 停止泵。
    bool StopPump(int index);
    // 查询泵出的体积。
    float GetPumpVolume(int index);
    //查询液位传感器状态
    Uint8 GetSenseStatus(int index);
    // 查询指定泵的安装类型。
    PumpFixedType GetPumpFixedType(int index);
    // 设置指定泵的安装类型。
    bool SetPumpFixedType(int index, PumpFixedType type);

    //泵操作结果事件。
    //
    //启动泵转动操作结束时将产生该事件。
    PumpResult ExpectPumpResult(long timeout);
};

}
}

#endif  //CONTROLLER_API_PERISTALTICPUMPINTERFACE_H
