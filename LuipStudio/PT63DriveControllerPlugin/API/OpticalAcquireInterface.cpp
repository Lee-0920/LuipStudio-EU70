/**

 * @file
 * @brief 光学信号采集接口。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "Code/OpticalAcquireInterface.h"
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Communication/EventHandler.h"
#include "OpticalAcquireInterface.h"
#include "ControllerPlugin/API/Code/DeviceStatusInterface.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief 光学信号采集接口构造。
 * @param[in] addr 设备地址。
 */
OpticalAcquireInterface::OpticalAcquireInterface(DscpAddress addr)
    : DeviceInterface(addr)
{

}

/**
 * @brief 打开LED灯。
 * @return 返回状态，操作是否成功。
 */
bool OpticalAcquireInterface:: TurnOnLED() const
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_TURN_ON_LED));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 设置信号AD上报周期。
 * @details 系统将根据设定的周期，定时向上发出信号AD上报事件。
 * @param period Float32，信号AD上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_OAI_AD_ACQUIRED_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。
 */
bool OpticalAcquireInterface::SetAcquireADNotifyPeriod(float period)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_SET_SIGNAL_AD_NOTIFY_PERIOD, &period, sizeof(period)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 启动采集过程。
 * @details 启动后，将自动打开LED并采样，采集到的信号数据将以事件 @ref DSCP_EVENT_OAI_AD_ACQUIRED
 *  的形式上传给上位机。
 * @param acquireTime float，采样时间，单位为秒，0表示只采1个样本。
 * @param needTurnOn Bool，是否打开LED，0为不打开，1为打开。
 * @return 返回状态，操作是否成功。
 */
bool OpticalAcquireInterface:: StartAcquirer(float acquireTime)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_START_ACQUIRER, &acquireTime, sizeof(acquireTime)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 停止采集过程。
 * @return 状态回应，Uint16，支持的状态有：
 * @return 返回状态，操作是否成功。
 */
bool OpticalAcquireInterface:: StopAcquirer()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_STOP_ACQUIRER));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 启动LED控制器,并打开LED灯。
 * @details LED控制器启动后，系统将根据设定的LED控制器参数进行自动控制，尝试达到指定参考端AD值。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::StartLEDController()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_START_LEDCONTROLLER));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}


/**
 * @brief  停止LED控制器和关闭LED。
 * @return 返回状态，操作是否成功。
 */
bool OpticalAcquireInterface:: TurnOffLED() const
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_STOP_LEDCONTROLLER));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询LED控制器设定的目标值。
 * @return target Uint32 ，目标值。
 * @see DSCP_CMD_OAI_SET_LEDCONTROLLER_TARGET
 */
Uint32 OpticalAcquireInterface::GetLEDControllerTartet() const
{
    Uint32 target = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_GET_LEDCONTROLLER_TARGET));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        target = *((Uint32*)resp->data);
    }

    return target;
}

/**
 * @brief 设置LED控制器设定的目标值。该参数永久保存在FLASH。
 * @param target Uint32 ，目标值。目标为参考端的AD值。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::SetLEDControllerTarget(Uint32 target)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_SET_LEDCONTROLLER_TARGET, &target, sizeof(target)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询LED控制器参数。
 * @return LED控制器参数，格式如下：
 *  - proportion Float32，PID的比例系数。
 *  - integration Float32，PID的积分系数。
 *  - differential Float32，PID的微分系数。
 * @see DSCP_CMD_OAI_SET_LEDCONTROLLER_PARAM
 */
LedParam OpticalAcquireInterface::GetLEDControllerParam() const
{
    LedParam param;
    memset(&param, 0, sizeof(param));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_GET_LEDCONTROLLER_PARAM));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        param = *((LedParam*)resp->data);
    }

    return param;
}

/**
 * @brief 设置LED控制器参数。
 * @details LED控制器将根据设置的参数进行PID调节。该参数永久保存在FLASH。
 * @param proportion Float32，PID的比例系数。
 * @param integration Float32，PID的积分系数。
 * @param differential Float32，PID的微分系数。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::SetLEDControllerParam(LedParam param)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_SET_LEDCONTROLLER_PARAM, &param, sizeof(param)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}


/**
 * @brief 打开LED灯,启动AD调节至目标值操作，到目标值后不再调节。
 * @details LED控制器启动后，系统将根据设定的LED控制器参数进行控制，尝试达到指定参考端AD值。
 * @param targetAD Uint32，目标AD。
 * @param tolerance Uint32，容差AD。
 * @param timeout Uint32，超时时间，单位ms。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::StartLEDOnceAdjust(Uint32 targetAD, Uint32 tolerance, Uint32 timeout)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[12] = {0};
    memcpy(cmdData, &targetAD, sizeof(Uint32));
    memcpy(cmdData+4, &tolerance, sizeof(Uint32));
    memcpy(cmdData+8, &timeout, sizeof(Uint32));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_START_LEDADJUST, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    if (status != DscpStatus::OK)
    {
        logger->debug("启动LED调节命令应答状态：%d", (int)status);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 停止测量模块LED单次定向调节和关闭LED。
 * @details 如果LED控制器没有打开则只关闭LED灯
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::StopLEDOnceAdjust() const
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_STOP_LEDADJUST));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 启动静态AD调节至目标值操作。
 * @details 开启LED，调整电位器输出值，优化目标AD。
 * @param index Uint8，目标LED索引号。
 * @param targetAD Uint32，目标AD。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::StartStaticADControl(Uint8 index, Uint32 targetAD)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[5] = {0};
    memcpy(cmdData, &index, sizeof(Uint8));
    memcpy(cmdData+1, &targetAD, sizeof(Uint32));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_START_STATIC_AD_CONTROL, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    if (status != DscpStatus::OK)
    {
        logger->debug("启动LED调节命令应答状态：%d", (int)status);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 停止静态AD调节并关闭LED。
 * @details 停止静态AD调节过程
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::StopStaticADControl()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_STOP_STATIC_AD_CONTROL));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询默认静态AD控制参数。
 * @return value Uint16 ，静态AD调节参数。
 * @see DSCP_CMD_OAI_SET_STATIC_AD_CONTROL_PARAM
 */
Uint16 OpticalAcquireInterface::GetStaticADControlParam(Uint8 index) const
{
    Uint16 value = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_GET_STATIC_AD_CONTROL_PARAM, &index, sizeof(index)));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        value = *((Uint32*)resp->data);
    }

    return value;
}

/**
 * @brief 设置默认静态AD控制参数。
 * @details 静态AD控制器将根据设置的参数设置器件，并将参数永久保存在FLASH。
 * @details 静态AD调节状态中禁用。
 * @param index Uint8，目标LED索引号。
 * @param value Uint16 ，静态AD调节参数。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::SetStaticADControlParam(Uint8 index, Uint16 value)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[3] = {0};
    memcpy(cmdData, &index, sizeof(Uint8));
    memcpy(cmdData+1, &value, sizeof(Uint16));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_SET_STATIC_AD_CONTROL_PARAM, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    if (status != DscpStatus::OK)
    {
        logger->debug("启动LED调节命令应答状态：%d", (int)status);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询静态AD控制功能是否有效
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  功能有效，操作成功；
 *  - @ref DSCP_ERROR 功能无效，操作失败；
 */
bool OpticalAcquireInterface::IsStaticADControlValid() const
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_IS_STATIC_AD_CONTROL_VALID));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询LED默认输出。
 * @return Float32，电压V：
 */
float OpticalAcquireInterface::GetLEDDefaultValue() const
{
    float value = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_GET_LED_DEFAULT_VALUE));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        value = *((float*)resp->data);
    }

    return value;
}

/**
 * @brief 设置LED默认输出。该参数永久保存在FLASH。
 * @param value Float32，电压V。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::SetLEDDefaultValue(float value)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_SET_LED_DEFAULT_VALUE, &value, sizeof(value)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 信号AD采集完成事件。
 * @details 启动采集后，采完数据时将产生该事件。
 * @param[in] timeout Uint32，时间等待超时，单位ms。
 * @param[out] reference Uint32，参考端AD。
 * @param[out] measure Uint32，测量端AD。
 * @see DSCP_CMD_OAI_START_ACQUIRER
 */
AcquiredResult OpticalAcquireInterface:: ExpectADAcquirer(long timeout)
{
    AcquiredResult acquiredResult;
    memset(&acquiredResult, 0, sizeof(acquiredResult));

    DscpEventPtr event = this->Expect(DSCP_EVENT_OAI_AD_ACQUIRED,timeout);

    if (event != nullptr)
    {
        acquiredResult.ad.reference = *((Uint32*)event->data);
        acquiredResult.ad.measure = *((Uint32*)(event->data+4));
        if (event->len == 9)
        {
            acquiredResult.result = (int)(*(event->data+8));
        }
        else
        {
            acquiredResult.result = (int)AcquiredOperateResult::Finished;
        }
    }

    return acquiredResult;
}

/**
 * @brief 注册信号AD定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报信号AD。
 *  上报周期可通过命令 @ref DSCP_CMD_OAI_SET_SIGNAL_AD_NOTIFY_PERIOD 设定。
 * @param[in] handle 定时接收信号AD上报事件的对象。
 */
void OpticalAcquireInterface::RegisterAcquireADNotice(IEventReceivable *handle)
{
    EventHandler::Instance()->Register(m_addr,DSCP_EVENT_OAI_SIGNAL_AD_NOTICE,handle);
}

/**
 * @brief 注册漏液检测定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报漏液检测AD。
 *  上报周期可通过命令 @ref DSCP_EVENT_DSI_CHECK_LEAKING_NOTICE 设定。
 * @param[in] handle 定时接收信号漏液检测上报事件的对象。
 */
void OpticalAcquireInterface::RegisterAcquireCheckLeaking(IEventReceivable *handle)
{
    EventHandler::Instance()->Register(m_addr,DSCP_EVENT_DSI_CHECK_LEAKING_NOTICE,handle);
}

/**
 * @brief 信号AD调节完成事件。
 * @details 启动LED自适应调节到指定范围后，产生该事件。
 * @see DSCP_CMD_OAI_START_LEDADJUST
 */
AdjustResult OpticalAcquireInterface::ExpectLEDOnceAdjust(long timeout)
{
    AdjustResult adjustResult;
    memset(&adjustResult, 0, sizeof(adjustResult));
    DscpEventPtr event = this->Expect(DSCP_EVENT_OAI_AD_ADJUST_RESULT,timeout);

    if (event != nullptr)
    {
        adjustResult = (AdjustResult)(*(event->data));
    }

    return adjustResult;

}

/**
 * @brief 静态AD电位控制完成事件。
 * @details 启动LED自适应调节到指定范围后，产生该事件。
 * @see DSCP_CMD_OAI_START_LEDADJUST
 */
StaticADControlResult OpticalAcquireInterface::ExpectStaticADControlResult(long timeout)
{
    StaticADControlResult result = StaticADControlResult::Unfinished;

    DscpEventPtr event = this->Expect(DSCP_EVENT_OAI_STATIC_AD_CONTROL_RESULT, timeout);

    if (event != nullptr)
    {
        result = (StaticADControlResult)(*(event->data));
    }

    return result;

}


}
}
