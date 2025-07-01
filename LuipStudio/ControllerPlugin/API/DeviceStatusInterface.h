/**
 * @file
 * @brief 设备状态接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_DEVICESTATUSINTERFACE_H)
#define CONTROLLER_API_DEVICESTATUSINTERFACE_H

#include "../LuipShare.h"
#include "System/Types.h"
#include "Communication/IEventReceivable.h"
#include "DeviceInterface.h"

using System::Uint32;
using Communication::IEventReceivable;

namespace Controller
{
namespace API
{
    
/**
 * @brief 设备运行模式。
 * @details 
 */
enum class RunMode
{
    Application,
    Updater,
    Upgrader
};

/**
 * @brief 电源供给类型。
 * @details 
 */
enum class PowerSupplyType
{
    Unknown,
    OnGoingAC,
    Bus,
    Box,
    Battery,
    Charger
};

/**
 * @brief 设备状态接口。
 * @details 设备状态接口提供了设备实时状态的查询操作。
 */
class LUIP_SHARE DeviceStatusInterface : public DeviceInterface
{
public:
    DeviceStatusInterface(DscpAddress addr);
    // 查询程序当前的运行模式。
    // 
    // 本操作通常用于升级管理程序，以确保设备处于期望的运行模式。
    RunMode GetRunMode();
    // 闪烁设备指示灯。
    //
    // 本命令只是抽象定义了设备的指示操作，而未指定是哪一盏灯，具体的指示灯（一盏或多盏）由设备实现程序决定。
    bool Blink(Uint32 duration, int onTime, int offTime);
    // 查询设备的电源供应类型。
    // 
    // 这里的电源供给类型是指设备实际的电源类型。如中控是外部交流电源（市电）、单主控是外部电源BOX、单控是和熔配是总线供电。
    PowerSupplyType GetPowerSupplyType();
    // 设备初始化。
    bool Initialize();
    // 出厂恢复设置
    bool FactoryDefault();
    // 设备复位。
    void Reset();

    void RegisterResetNotice(IEventReceivable *handle);
};

}
}

#endif  //CONTROLNET_API_DEVICESTATUSINTERFACE_H
