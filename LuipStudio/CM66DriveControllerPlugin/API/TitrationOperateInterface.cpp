#include "Log.h"
#include "TitrationOperateInterface.h"
#include "Code/TitrationOperateInterface.h"
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Communication/EventHandler.h"
#include "ControllerPlugin/API/Code/DeviceStatusInterface.h"
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
 * @brief 滴定控制接口构造。
 * @param[in] addr 设备地址。
 */
TitrationOperateInterface::TitrationOperateInterface(DscpAddress addr)
    : DeviceInterface(addr)
{

}

/**
 * @brief 设置信号AD上报周期。
 * @details 系统将根据设定的周期，定时向上发出信号AD上报事件。
 * @param period Float32，信号AD上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_TOI_ELECTRODE_SIGNAL_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。而且通信失败时不上报。
 */
bool TitrationOperateInterface::SetAcquireElectrodeADNotifyPeriod(float period)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_TOI_SET_ELECTRODE_SIGNAL_NOTIFY_PERIOD, &period, sizeof(period)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 启动泵进行滴定。
 * @details 滴定过程中电极电位值及已滴定步数将以事件形式上报至主控器 @ref DSCP_EVENT_TOI_TITRATE_AD_ACQUIRED
 * @param speed Float32，滴定速度，单位为 ml/秒。
 * @param samplePeriod Float32，滴定过程中信号值及已滴定步数的采样周期，单位：秒。
 * @param notifyPeriod Float32，滴定过程中数据上报周期，单位：秒。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败，如泵正在工作，无法启动泵，需要先停止；
 * @note 该命令将立即返回，需要调用DSCP_CMD_TOI_STOP_TITRATION终止滴定过程。
 */
bool TitrationOperateInterface::StartTitration(float speed, float samplePeriod, float notifyPeriod)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[12] = {0};

    memcpy(cmdData,   &speed, sizeof(float));
    memcpy(cmdData+4, &samplePeriod, sizeof(float));
    memcpy(cmdData+8, &notifyPeriod, sizeof(float));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_TOI_START_TITRATION, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 停止采集过程。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool TitrationOperateInterface::StopTitration(void)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_TOI_STOP_TITRATION));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询滴定的工作状态。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_IDLE 空闲；
 *  - @ref DSCP_BUSY 忙碌，需要停止后才能做下一个动作；
 */
Uint16 TitrationOperateInterface::GetTitrationStatus()
{
    Uint16 status = DscpStatus::Busy;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_TOI_GET_TITRATION_STATUS));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return status;
}

/**
 * @brief 启动滴定后，采样到的当前信号值及滴定步数。
 * @details 启动滴定后，采集数据时将产生该事件。
 * @param number Uint32，采样到的电极AD值及滴定步数对数目。
 * @param {Uint32 ad1, Uint32 step1}、{Uint32 ad2, Uint32 step2}....。
 * @see DSCP_CMD_EAI_START_ACQUIRER。
 */
Uint32 TitrationOperateInterface::ExpectTitrationData(long timeout, TitrationData* titrationData)
{
    Uint32 number = 0;
    DscpEventPtr event = this->Expect(DSCP_EVENT_TOI_TITRATE_AD_ACQUIRED, timeout);

    if (event != nullptr)
    {
        number = *((Uint32*)event->data);
        for(Uint32 i = 0; i < number; i++)
        {
            titrationData[i].electrodeAD   = *((Uint32*)(event->data+(8*(i+1)-4)));
            titrationData[i].titrationStep = *((Uint32*)(event->data+8*(i+1)));
        }
    }

    return number;
}

//Uint32 TitrationOperateInterface::ExpectTitrationData(long timeout, TitrationData* titrationData)
//{
//    Uint32 number = 30;

//    for(Uint32 i = 0; i < number; i++)
//    {
//        titrationData[i].electrodeAD   = i+number;
//        titrationData[i].titrationStep = 2*i+number;
//        qDebug()<<"titrationData i = "<<i<<" AD = "<<titrationData[i].electrodeAD<<" Step = "<<titrationData[i].titrationStep;
//    }

//    return number;
//}

/**
* @brief 电极信号定时上报事件。
* @details 系统将根据设置的上报周期，定时向上汇报电极信号AD值。
*  上报周期可通过命令 @ref DSCP_CMD_TOI_SET_ELECTRODE_SIGNAL_NOTIFY_PERIOD 设定。
* @param electrodeValue Uint32，电极信号值。
*/
void TitrationOperateInterface::RegisterAcquireElectrodeADNotice(IEventReceivable *handle)
{
    EventHandler::Instance()->Register(m_addr, DSCP_EVENT_TOI_ELECTRODE_SIGNAL_NOTICE, handle);
}

/**
 * @brief 获取当前电极信号值。
 * @param value Uint32，电极信号AD值
 */
Uint32 TitrationOperateInterface::GetElectrodeSignal(void)
{
    Uint32 value = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_TOI_GET_ELECTRODE_SIGNAL));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        value = *((Uint32*)resp->data);
    }

    return value;
}

/**
 * @brief 设置滴定速度
 * @details 该接口主要用于滴定过程中在电极信号值达到设置的切换阈值时，调用该接口改变滴定速度
 * @param titrationSpeed Float32，滴定速度，单位ml/s
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败，如泵正在工作，无法启动泵，需要先停止；
 */
bool TitrationOperateInterface::SetTitrationSpeed(float titrationSpeed)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[4] = {0};

    memcpy(cmdData, &titrationSpeed, sizeof(float));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_TOI_SET_TITRATION_SPEED, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 滴定电极输出
 * @param isOpen Bool 滴定电极输出。TRUE 开启，FALSE 关闭
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool TitrationOperateInterface::TitrationElectrodeOutput(bool isOpen)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_TOI_ELECTRODE_OUTPUT, &isOpen, sizeof(isOpen)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

}

}
