/**
 * @file
 * @brief 蠕动泵控制接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(LC_CONTROLLER_API_PERISTALTICPUMPINTERFACE_H)
#define LC_CONTROLLER_API_PERISTALTICPUMPINTERFACE_H

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
struct LCMotionParam
{
    float acceleration;           ///加速度，单位为 ml/平方秒
    float speed;                  ///最大速度，单位为 ml/秒
};


/**
 * @brief 蠕动泵状态。
 * @details
 */
enum class LCPumpStatus
{
    Idle,                   ///泵空闲
    Failed,                 ///泵异常
    Busy                    ///泵忙碌
};

/**
 * @brief 蠕动泵旋转方向。
 * @details
 */
enum class LCRollDirection
{
    Suck,                      ///泵抽操作
    Drain,                     ///泵排操作
};

/**
 * @brief 蠕动泵操作结果码。
 * @details
 */
enum class LCPumpOperateResult
{
    Finished,                   ///泵操作正常完成
    Failed,                     ///泵操作中途出现故障，未能完成
    Stopped                     ///泵操作被停止
};

/**
 * @brief 蠕动泵结果。
 * @details
 */
struct LCPumpResult
{
    Uint8 index;        // 产生事件的泵索引，0号泵为光学定量泵。
    int result;         // 泵操作结果码,赋值为PumpOperateResult。
};

/**
 * @brief 蠕动泵控制接口。
 * @details 定义了一序列蠕动泵控制相关的操作。
 */
class LUIP_SHARE LCPeristalticPumpInterface : public DeviceInterface
{
public:
    LCPeristalticPumpInterface(DscpAddress addr);
    // 查询系统支持的总泵数目。
    int GetTotalPumps();
    //查询指定泵的校准系数。
    float GetPumpFactor(int index);
    // 设置指定泵的校准系数。
    bool SetPumpFactor(int index, float factor);
    // 查询指定泵的运动参数。
    LCMotionParam GetMotionParam(int index);
    // 设置指定泵的运动参数。
    bool SetMotionParam(int index, LCMotionParam param);
    // 查询指定泵的工作状态。
    LCPumpStatus GetPumpStatus(int index);
    // 启动泵。
    bool StartPump(int index, LCRollDirection dir, float volume, float seep);
    // 停止泵。
    bool StopPump(int index);
    // 查询泵出的体积。
    float GetPumpVolume(int index);
    //泵操作结果事件。
    //
    //启动泵转动操作结束时将产生该事件。
    LCPumpResult ExpectPumpResult(long timeout);
};

}
}

#endif  //LC_CONTROLLER_API_PERISTALTICPUMPINTERFACE_H
