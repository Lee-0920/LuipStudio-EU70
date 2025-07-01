/**

 * @file
 * @brief 设备状态接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Communication/EventHandler.h"
#include "Code/DeviceStatusInterface.h"
#include "DeviceStatusInterface.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief 设备状态接口构造。
 * @param[in] addr 设备地址。
 */
DeviceStatusInterface::DeviceStatusInterface(DscpAddress addr)
    : DeviceInterface(addr)
{

}

/**
 * @brief 查询程序当前的运行模式。
 * @details 本操作通常用于升级管理程序，以确保设备处于期望的运行模式。
 * @return 运行模式，Uint8。请参考 @ref DeviceRunMode 。
 * @note App 模式和 Updater 模式都支持本命令。见： @ref DSCP_CMD_DUI_GET_RUN_MODE ，
 *  注意这两条命令的码值是一致，只是名称不同而已。
 */
RunMode DeviceStatusInterface::GetRunMode()
{
    RunMode mode = RunMode::Application;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DSI_GET_RUN_MODE));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        mode = (RunMode)(*resp->data);
    }

    return mode;
}

/**
 * @brief 闪烁设备指示灯。
 * @details 本命令只是抽象定义了设备的指示操作，而未指定是哪一盏灯，
 *  具体的指示灯（一盏或多盏）由设备实现程序决定。
 *  <p>闪烁方式由参数指定。通过调节参数，可设置LED灯为常亮或常灭：
 *  - onTime 为 0 表示灭灯
 *  - offTime 为 0 表示亮灯
 *  - onTime 和 offTime 都为0 时，不动作
 *  <p> 当持续时间结束之后，灯的状态将返回系统状态，受系统控制。
 * @param duration Uint32，持续时间，单位为毫秒。0 表示不起作用，-1 表示一直持续。
 * @param onTime Uint16，亮灯的时间，单位为毫秒。
 * @param offTime Uint16，灭灯的时间，单位为毫秒。
 * @return 返回状态，操作是否成功。
 *
 */
bool DeviceStatusInterface::Blink(Uint32 duration, int onTime, int offTime)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[8] = {0};
    memcpy(cmdData, &duration, sizeof(Uint32));
    memcpy(cmdData+4, &onTime, sizeof(Uint16));
    memcpy(cmdData+6, &offTime, sizeof(Uint16));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DSI_BLINK_DEVICE, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询设备的电源供应类型。
 * @details 这里的电源供给类型是指设备实际的电源类型。
 * @return 电源供应类型，Uint8。请参考 @ref DevicePowerSupplyType 。
 */
PowerSupplyType DeviceStatusInterface::GetPowerSupplyType()
{
    PowerSupplyType type = PowerSupplyType::Battery;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DSI_GET_POWER_SUPPLY_TYPE));
    SyncCaller syncCaller(m_retries);

    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        type = (PowerSupplyType)(*resp->data);
    }

    return type;
}

/**
 * @brief 设备初始化。
 * @details 恢复业务功能到初始状态。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 *
 */
bool DeviceStatusInterface::Initialize()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DSI_INITIALIZE));
    SyncCaller  syncCaller(m_retries);

    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 出厂恢复设置。
 * @details 恢复出厂数据。
 *
 */
bool DeviceStatusInterface::FactoryDefault()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DSI_FACTORY_DEFAULT));
    SyncCaller  syncCaller(m_retries);

    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 设备单片机复位。
 * @details 恢复单片机的初始状态。
 *
 */
void DeviceStatusInterface::Reset()
{
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DSI_RESET));
    SyncCaller syncCaller(m_retries);
    syncCaller.SendWithoutRespond(cmd);
}

void DeviceStatusInterface::RegisterResetNotice(IEventReceivable *handle)
{
    EventHandler::Instance()->Register(m_addr, DSCP_EVENT_DSI_RESET, handle);
}

}
}
