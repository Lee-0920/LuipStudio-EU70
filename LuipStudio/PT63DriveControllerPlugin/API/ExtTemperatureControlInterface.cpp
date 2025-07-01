/**

 * @file
 * @brief 扩展的温度控制接口。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */
#include "Log.h"
#include "Code/ExtTemperatureControlInterface.h"
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Communication/EventHandler.h"
#include "ControllerPlugin/API/Code/DeviceStatusInterface.h"
#include "ExtTemperatureControlInterface.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief 扩展的温度控制接口。
 * @param[in] addr 设备地址。
 */
ExtTemperatureControlInterface::ExtTemperatureControlInterface(DscpAddress addr)
    : DeviceInterface(addr)
{

}

/**
 * @brief 查询温度传感器的校准系数。
 * @return  负输入分压，Float32。
 * @return 参考电压 ，Float32。
 * @return 校准电压 Float32。
 * @param index Uint8，要查询的恒温器索引。
 * @see DSCP_CMD_TCI_SET_CALIBRATE_FACTOR
 */
TempCalibrateFactor ExtTemperatureControlInterface::GetCalibrateFactor(Uint8 index) const
{
    TempCalibrateFactor factor = {0, 0, 0};

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_GET_CALIBRATE_FACTOR, &index, sizeof(index)));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        factor.negativeInput = *((float*)resp->data);
        factor.referenceVoltage = *((float*)(resp->data + 4));
        factor.calibrationVoltage = *((float*)(resp->data + 8));
    }

    return factor;
}

/**
 * @brief 设置温度传感器的校准系数。
 * @details 因为个体温度传感器有差异，出厂或使用时需要校准。该参数将永久保存。
 *   @param negativeInput Float32，负输入分压 。
 *   @param vref Float32，参考电压。
 *   @param vcal Float32，校准电压。
 *   @param index Uint8，要设置的恒温器索引。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool ExtTemperatureControlInterface::SetCalibrateFactor(Uint8 index, TempCalibrateFactor factor)
{
    Uint16 status = DscpStatus::Error;

    Uint8 cmdData[13] = {0};
    memcpy(cmdData, &factor, sizeof(factor));
    memcpy(cmdData+12, &index, sizeof(Uint8));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_SET_CALIBRATE_FACTOR, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询当前恒温器温度。
 * @param index Uint8，要查询的恒温器索引。
 * @return 当前恒温室温度，Float32。
 */
float ExtTemperatureControlInterface::GetThermostatTemperature(Uint8 index) const
{
    float temp;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_GET_TEMPERATURE, &index, sizeof(index)));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        temp = *((float*)resp->data);
    }

    return temp;
}

/**
 * @brief 查询恒温控制参数。
 * @param index Uint8，要查询的恒温器索引。
 * @return 恒温控制参数，格式如下：
 *  - proportion Float32，PID的比例系数。
 *  - integration Float32，PID的积分系数。
 *  - differential Float32，PID的微分系数。
 * @see DSCP_CMD_TCI_SET_THERMOSTAT_PARAM
 */
ThermostatParam ExtTemperatureControlInterface::GetThermostatParam(Uint8 index) const
{
    ThermostatParam param;
    memset(&param, 0, sizeof(param));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_GET_THERMOSTAT_PARAM, &index, sizeof(index)));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        param = *((ThermostatParam*)resp->data);
    }

    return param;
}

/**
 * @brief 设置恒温控制参数。
 * @details 该参数将永久保存。系统启动时同步到当前恒温控制参数上。
 * @param proportion Float32，PID的比例系数。
 * @param integration Float32，PID的积分系数。
 * @param differential Float32，PID的微分系数。
 * @param index Uint8，要设置的恒温器索引。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool ExtTemperatureControlInterface::SetThermostatParam(Uint8 index, ThermostatParam param)
{
    Uint16 status = DscpStatus::Error;

    Uint8 cmdData[13] = {0};
    memcpy(cmdData, &param, sizeof(param));
    memcpy(cmdData+12, &index, sizeof(Uint8));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_SET_THERMOSTAT_PARAM, &cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询恒温器的工作状态。
 * @param index Uint8，要查询的恒温器索引。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_IDLE 空闲；
 *  - @ref DSCP_BUSY 忙碌，需要停止后才能做下一个动作；
 */

Uint16 ExtTemperatureControlInterface::GetThermostatStatus(Uint8 index) const
{
    Uint16 status = DscpStatus::Busy;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_GET_THERMOSTAT_STATUS, &index, sizeof(index)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return status;
}

/**
 * @brief 开始恒温。
 * @details 恒温开始后，系统将根据设定的恒温控制参数进行自动温度控制，尝试达到指定温度。
 *  不管成功与否，操作结果都将以事件的形式上传给上位机，但恒温器将继续工作，
 *  直到接收到DSCP_CMD_TCI_STOP_THERMOSTAT才停止。关联的事件有：
 *   - @ref DSCP_EVENT_TCI_THERMOSTAT_RESULT
 * @param mode Uint8，恒温模式，支持的模式见： @ref ThermostatMode 。
 * @param targetTemp Float32，恒温目标温度。
 * @param toleranceTemp Float32，容差温度，与目标温度的差值在该参数范围内即认为到达目标温度。
 * @param timeout Float32，超时时间，单位为秒。超时仍未达到目标温度，也将返回结果事件。
 * @param index Uint8，要操作的恒温器索引。
 * @param isExpectEvent bool,是否等待事件。
 * @return 状态回应，Uint16，支持的状态有：
 *   - @ref DSCP_OK  操作成功；
 *   - @ref DSCP_BUSY 操作失败，如恒温已经工作，需要先停止；
 *   - @ref DSCP_ERROR_PARAM 恒温参数错误；
 *   - @ref DSCP_ERROR 温度传感器异常；
 * @note 该命令将立即返回，恒温完成将以事件的形式上报。
 */
bool ExtTemperatureControlInterface::StartThermostat(Uint8 index, ThermostatMode mode, float targetTemp,
                                                  float toleranceTemp, float timeout, bool isExpectEvent)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[15] = {0};
    memcpy(cmdData, &mode, sizeof(Uint8));
    memcpy(cmdData+1, &targetTemp, sizeof(float));
    memcpy(cmdData+5, &toleranceTemp, sizeof(float));
    memcpy(cmdData+9, &timeout, sizeof(float));
    memcpy(cmdData+13, &index, sizeof(Uint8));
    memcpy(cmdData+14, &isExpectEvent, sizeof(bool));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_START_THERMOSTAT, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    if (status != DscpStatus::OK)
    {
        logger->debug("启动恒温命令应答状态：%d", (int)status);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 停止恒温控制。
 * @details 停止后，加热器和冷却器将不工作。
 * @param index Uint8，要操作的恒温器索引。
* @param isExpectEvent bool,是否等待事件。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @note 该命令将立即返回。
 */
bool ExtTemperatureControlInterface::StopThermostat(Uint8 index, bool isExpectEvent)
{
    Uint16 status = DscpStatus::Error;

    Uint8 cmdData[2] = {0};
    memcpy(cmdData, &index, sizeof(Uint8));
    memcpy(cmdData+1, &isExpectEvent, sizeof(bool));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_STOP_THERMOSTAT, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    if (status != DscpStatus::OK)
    {
        logger->debug("停止恒温命令应答状态：%d", (int)status);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 设置温度上报周期。
 * @details 系统将根据设定的周期，定时向上发出温度上报事件。
 * @param period Float32，温度上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_TCI_TEMPERATURE_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。
 */
bool ExtTemperatureControlInterface::SetTemperatureNotifyPeriod(float period)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_SET_TEMPERATURE_NOTIFY_PERIOD, &period, sizeof(period)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 设置机箱风扇。
 * @details 根据设定的占空比，调节风扇速率
 * @param level float ,风扇速率，对应高电平占空比。默认为0，机箱风扇关闭。
 */
bool ExtTemperatureControlInterface::BoxFanSetOutput(float level)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_TURN_BOXFAN , &level, sizeof(level)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 设置恒温器风扇。
 * @details 根据设定的占空比，调节风扇速率
 * @param level float ,风扇速率，对应高电平占空比。默认为0，恒温器风扇关闭。
 * @param thermostatIndex Uint8，要设置的恒温器索引。
 * @param fanIndex Uint8，要设置的恒温器中的风扇索引。
 */
bool ExtTemperatureControlInterface::ThermostatFanSetOutput(Uint8 thermostatIndex, Uint8 fanIndex, float level)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[6] = {0};
    memcpy(cmdData, &level, sizeof(level));
    memcpy(cmdData+4, &thermostatIndex, sizeof(Uint8));
    memcpy(cmdData+5, &fanIndex, sizeof(Uint8));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_TURN_THERMOSTAT_FAN , &cmdData, sizeof(cmdData)));;
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 获取恒温器加热丝输出的最大占空比。
 * @param maxDutyCycle float ,加热丝输出的最大占空比
 * @param index Uint8，要获取的恒温器索引。
 * @see DSCP_CMD_TCI_SET_HEATER_MAX_DUTY_CYCLE
 * @note
 */
float ExtTemperatureControlInterface::GetHeaterMaxDutyCycle(Uint8 index) const
{
    float maxDutyCycle =0 ;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_GET_HEATER_MAX_DUTY_CYCLE, &index, sizeof(index)));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        maxDutyCycle = *((float*)resp->data);
    }

    return maxDutyCycle;
}

/**
 * @brief 设置恒温器加热丝输出的最大占空比。
 * @param maxDutyCycle float，加热丝输出的最大占空比
 * @param index Uint8，要设置的恒温器索引。
 * @see DSCP_CMD_TCI_GET_HEATER_MAX_DUTY_CYCLE
 */
bool ExtTemperatureControlInterface::SetHeaterMaxDutyCycle(Uint8 index, float maxDutyCycle)
{
    Uint16 status = DscpStatus::Error;

    Uint8 cmdData[5] = {0};
    memcpy(cmdData, &maxDutyCycle, sizeof(maxDutyCycle));
    memcpy(cmdData+4, &index, sizeof(Uint8));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_SET_HEATER_MAX_DUTY_CYCLE , &cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询恒温器当前恒温控制参数。
 * @param index Uint8，要查询的恒温器索引。
 * @return 恒温控制参数，格式如下：
 *  - proportion Float32，PID的比例系数。
 *  - integration Float32，PID的积分系数。
 *  - differential Float32，PID的微分系数。
 * @see DSCP_CMD_TCI_SET_CURRENT_THERMOSTAT_PARAM
 */
ThermostatParam ExtTemperatureControlInterface::GetCurrentThermostatParam(Uint8 index) const
{
    ThermostatParam param;
    memset(&param, 0, sizeof(param));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_GET_CURRENT_THERMOSTAT_PARAM, &index, sizeof(index)));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        param = *((ThermostatParam*)resp->data);
    }

    return param;
}

/**
 * @brief 设置当前恒温控制参数。
 * @details 恒温系统将根据设置的参数进行温度调节。此参数上电时获取FLASH的PID。
 * @param proportion Float32，PID的比例系数。
 * @param integration Float32，PID的积分系数。
 * @param differential Float32，PID的微分系数。
 * @param index Uint8，要设置的恒温器索引。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool ExtTemperatureControlInterface::SetCurrentThermostatParam(Uint8 index, ThermostatParam param)
{
    Uint16 status = DscpStatus::Error;

    Uint8 cmdData[13] = {0};
    memcpy(cmdData, &param, sizeof(param));
    memcpy(cmdData+12, &index, sizeof(Uint8));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_SET_CURRENT_THERMOSTAT_PARAM, &cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 开启继电器 温控板紫外灯
 */
bool ExtTemperatureControlInterface::TurnOnRays()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_TURN_ON_RELAY ));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 关闭继电器 温控板紫外灯
 */
bool ExtTemperatureControlInterface::TurnOffRays()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_TURN_OFF_RELAY));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询系统支持的恒温器数目。
 * @return 总数目， Uint16。
 */

Uint16 ExtTemperatureControlInterface::GetTotalThermostat() const
{
    Uint16 num =0 ;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_GET_TOTAL_THERMOSTAT));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        num = *((Uint16*)resp->data);
    }

    return num;
}

/**
 * @brief 查询当前环境温度。
 * @return 当前恒温室温度，Float32。
 */
float ExtTemperatureControlInterface::GetEnvironmentTemperature() const
{
    float temp;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_ETCI_GET_ENV_TEMPERATURE));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        temp = *((float*)resp->data);
    }

    return temp;
}

/**********************************************************************************/

/**
 * @brief 恒温结果事件。
 * @details 恒温操作结束时将产生该事件。
 * @param[in] timeout 恒温操作事件等待超时时间。
 * @returen 恒温结果（ @ref Thermostat ），定义如下：
 *  - @ref result Uint8，恒温结果码（ @ref ThermostatResult ）。
 *  - @ref temp float，当前温度。
 */
ThermostatResult ExtTemperatureControlInterface::ExpectThermostat(long timeout)
{
    ThermostatResult thermostatResult;
    memset(&thermostatResult, 0, sizeof(thermostatResult));

    DscpEventPtr event = this->Expect(DSCP_EVENT_ETCI_THERMOSTAT_RESULT,timeout);

    if (event != nullptr)
    {
        thermostatResult.result = (int)(*event->data);
        thermostatResult.temp = *((float*)(event->data+1));
        thermostatResult.index = *(event->data+5);
    }

    return thermostatResult;
}

/**
 * @brief 注册温度定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报温度。
 *  上报周期可通过命令 @ref DSCP_CMD_TCI_SET_TEMPERATURE_NOTIFY_PERIOD 设定。
 * @reurn environmentTemp Float32，环境温度，单位为摄氏度。
 * @reurn thermostatTemp1 Float32，恒温室1温度，单位为摄氏度。
 * @reurn thermostatTemp2 Float32，恒温室2温度，单位为摄氏度。
 *
 */
//void TemperatureControlInterface::RegisterTemperatureNotice(IEventReceivable *handle)
//{
//    EventHandler::Instance()->Register(m_addr,DSCP_EVENT_TCI_TEMPERATURE_NOTICE,handle);
//}


}
}
