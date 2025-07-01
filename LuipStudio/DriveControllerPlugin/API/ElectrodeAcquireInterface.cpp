/**

 * @file
 * @brief 电极信号采集接口。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "Code/ElectrodeAcquireInterface.h"
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Communication/EventHandler.h"
#include "ElectrodeAcquireInterface.h"
#include "ControllerPlugin/API/Code/DeviceStatusInterface.h"
#include "Log.h"
#include <QDebug>

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief 电极信号采集接口构造。
 * @param[in] addr 设备地址。
 */
ElectrodeAcquireInterface::ElectrodeAcquireInterface(DscpAddress addr)
    : DeviceInterface(addr)
{

}


/**
 * @brief 设置信号AD上报周期。
 * @details 系统将根据设定的周期，定时向上发出信号AD上报事件。
 * @param period Float32，信号AD上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_EAI_SIGNAL_AD_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。而且通信失败时不上报。
 */
bool ElectrodeAcquireInterface::SetAcquireElectrodeADNotifyPeriod(float period)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_SET_SIGNAL_AD_NOTIFY_PERIOD, &period, sizeof(period)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 启动电极扫描采集过程
 * @details 启动后，DAC根据设置的起始电位、终止电位和扫描速率，依次改变输出电位值，
 *  同时启动ADC采集电极AD值，将采集数据存入缓冲，扫描完成后上报完成事件。
 * @param startPotential Float32，扫描起始电位，单位：mV；
 * @param stopPotential Float32，扫描终止电位，单位：mV；
 * @param scanRate Float32，扫描速率，单位：mV/s；
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @note 该命令将立即返回，电极扫描完成情况将以事件的形式上报。
 */
bool ElectrodeAcquireInterface::StartElectrodeScan(float startPotential, float stopPotential, float scanRate)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[12] = {0};
    memcpy(cmdData, &startPotential, sizeof(float));
    memcpy(cmdData+4, &stopPotential, sizeof(float));
    memcpy(cmdData+8, &scanRate, sizeof(float));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_START_SCAN, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    if (status != DscpStatus::OK)
    {
        logger->debug("启动电极扫描命令应答状态：%d", (int)status);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 停止采集过程。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool ElectrodeAcquireInterface::StopElectrodeADAcquirer()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_STOP_ACQUIRER));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 设置DAC输出电位
 * @param potential Float32，DAC输出电位，单位：mV，范围-2000~2000。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool ElectrodeAcquireInterface::SetDACPotential(float potential)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_SET_POTENTIAL , &potential, sizeof(potential)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 获取DAC输出电位
 * @return potential Float32，DAC输出电位，单位：mV，范围-2000~2000。
 */
float ElectrodeAcquireInterface::GetDACPotential()
{
    float potential = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_GET_POTENTIAL));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        potential = *((float*)resp->data);
    }

    return potential;
}

/**
 * @brief 设置ADC灵敏度等级
 * @param sensitivity Uint8，ADC采集灵敏度等级，范围0~2。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool ElectrodeAcquireInterface::SetADCSensitivity(Uint8 sensitivity)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_SET_SENSITIVITY , &sensitivity, sizeof(sensitivity)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 获取ADC灵敏度等级
 * @return sensitivity Uint8，ADC采集灵敏度等级，范围0~2。
 */
Uint8 ElectrodeAcquireInterface::GetADCSensitivity()
{
    Uint8 potential = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_GET_SENSITIVITY));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        potential = *((Uint8*)resp->data);
    }

    return potential;
}

/**
 * @brief 获取电极扫描分辨率
 * @return scanResolution Float32，扫描分辨率,单位：mV。
 */
float ElectrodeAcquireInterface::GetElectrodeScanResolution()
{
    float scanResolution = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_GET_SCAN_RESOLUTION));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        scanResolution = *((float*)resp->data);
    }

    return scanResolution;
}

/**
 * @brief 获取ADC采集的电极扫描数据
 * @param[out] current Float32*，电流值缓存区指针，单位：uA
 * @param pos Uint16，起始位置索引
 * @param length Uint16，数据个数
 * @return 状态回应，Uint16，支持的状态有：
 * @ref DSCP_OK  操作成功；
 * @ref DSCP_ERROR 操作失败；
 */
bool ElectrodeAcquireInterface::GetElectrodeScanData(float* current, Uint16 pos, Uint16 length)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[4] = {0};
    memcpy(cmdData, &pos, sizeof(Uint16));
    memcpy(cmdData+2, &length, sizeof(Uint16));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_GET_SCAN_DATA, cmdData, sizeof(cmdData)));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        for(Uint32 i = 0; i < (resp->len)/4; i++)
        {
            current[i] = *((float*)(resp->data+4*i));
//            logger->debug("current data = %f",current[i]);
        }
        status = DscpStatus::OK;
    }

    return (status == DscpStatus::OK);
}

/**
 * @brief 关闭DAC输出
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool ElectrodeAcquireInterface::TurnOffDAC()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_DAC_OFF));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询电极扫描状态。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_IDLE 空闲；
 *  - @ref DSCP_BUSY 忙碌，需要停止后才能做下一个动作；
 */

Uint16 ElectrodeAcquireInterface::GetScanStatus()
{
    Uint16 status = DscpStatus::Busy;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_EAI_GET_SCAN_STATUS));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return status;
}


/**
 * @brief 信号AD定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报信号AD。
 *  上报周期可通过命令 @ref DSCP_CMD_EAI_SET_SIGNAL_AD_NOTIFY_PERIOD 设定。
 * @param electrodeValue Uint32，电极信号值
 */
void ElectrodeAcquireInterface::RegisterAcquireElectrodeADNotice(IEventReceivable *handle)
{
    EventHandler::Instance()->Register(m_addr,DSCP_EVENT_EAI_SIGNAL_AD_NOTICE,handle);
}

/**
 * @brief 电极扫描完成事件。
 * @details 电极扫描完成或终止会产生该事件。
 * @param result Uint8，电极扫描操作结果码（ @ref ElectrodeScanResult ），定义如下：
 *  - @ref ELCTRODE_SCAN_RESULT_FINISHED  电极扫描操作正常完成；
 *  - @ref ELCTRODE_SCAN_RESULT_FAILED  电极扫描操作失败。
 */
ElectrodeScanResult ElectrodeAcquireInterface::ExpectElectrodeScan(long timeout)
{
    ElectrodeScanResult result = ElectrodeScanResult::Failed;

    DscpEventPtr event = this->Expect(DSCP_EVENT_EAI_AD_ACQUIRED, timeout);

    if (event != nullptr)
    {
        result = (ElectrodeScanResult)(int)(*event->data);
    }

    return result;
}


}
}
