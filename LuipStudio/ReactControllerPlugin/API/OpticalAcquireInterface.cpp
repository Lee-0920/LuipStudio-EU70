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
    Byte on = 1;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_TURN_ON_LED, &on, 1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 关闭LED灯。
 * @return 返回状态，操作是否成功。
 */
bool OpticalAcquireInterface:: TurnOffLED() const
{
    Uint16 status = DscpStatus::Error;
    Byte off = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_TURN_ON_LED, &off, 1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 打开氙灯。
 * @return 返回状态，操作是否成功。
 */
bool OpticalAcquireInterface:: TurnOnXonen() const
{
    Uint16 status = DscpStatus::Error;
    Byte on = 1;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_TURN_ON_XONEN, &on, 1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    return (status == DscpStatus::OK);
}

/**
 * @brief 关闭氙灯。
 * @return 返回状态，操作是否成功。
 */
bool OpticalAcquireInterface:: TurnOffXonen() const
{
    Uint16 status = DscpStatus::Error;
    Byte off = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_TURN_ON_XONEN, &off, 1));
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

    Uint8 cmdData[5];

    memcpy(cmdData, &acquireTime, sizeof(acquireTime));
    cmdData[4] = MEASURE_TYPE_TN;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_START_ACQUIRER, cmdData, sizeof(cmdData)));
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
 * @brief 闪烁氙灯。
 * @return 返回状态，操作是否成功。
 */
bool OpticalAcquireInterface:: GlitterXonen(bool isOpen) const
{
    Uint16 status = DscpStatus::Error;
    Byte data = 1;
    if(isOpen == false)
    {
        data = 0;
    }

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_GLITTER_XONEN, &data, sizeof(Byte)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    return (status == DscpStatus::OK);
}

/**
 * @brief 启动静态AD调节至目标值操作。
 * @details 开启LED，调整电位器输出值，优化目标AD。
 * @param index Uint8，目标通道索引号。(0参考 1测量)
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
 * @brief 信号采集完成事件。
 * @details 启动采集后，采完数据时将产生该事件。
 * @param[in] timeout Uint32，时间等待超时，单位ms。
 * @param[out] reference Uint32，参考端AD。
 * @param[out] measure Uint32，测量端AD。
 * @see DSCP_EVENT_OAI_TN_AD_ACQUIRED
 */
AcquiredResult OpticalAcquireInterface:: ExpectADAcquirer(long timeout)
{
    AcquiredResult acquiredResult;
    memset(&acquiredResult, 0, sizeof(acquiredResult));

    DscpEventPtr event = this->Expect(DSCP_EVENT_OAI_TN_AD_ACQUIRED,timeout);

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
 * @brief 查询静态AD控制功能是否有效
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  功能有效，操作成功；
 *  - @ref DSCP_ERROR 功能无效，操作失败；
 */
bool OpticalAcquireInterface::IsStaticADControlValid()
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
 * @brief 注册信号AD定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报信号AD。
 *  上报周期可通过命令 @ref DSCP_CMD_OAI_SET_SIGNAL_AD_NOTIFY_PERIOD 设定。
 * @param[in] handle 定时接收信号AD上报事件的对象。
 */
void OpticalAcquireInterface::RegisterAcquireADNotice(IEventReceivable *handle)
{
    EventHandler::Instance()->Register(m_addr, DSCP_EVENT_OAI_SIGNAL_AD_NOTICE, handle);

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
