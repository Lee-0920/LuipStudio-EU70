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
 * @details 启动后，将采样时间内得到的数据将以事件 @ref DSCP_EVENT_EAI_AD_ACQUIRED
 *  的形式上传给上位机，按照设置的采样周期进行采样，采样结束即上传数据，无需对数据进行处理。
 * @param samplePeriod Float32，采样周期。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface:: StartAcquirer(float samplePeriod)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_START_ACQUIRER, &samplePeriod, sizeof(samplePeriod)));
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
 * @brief 打开汞灯
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface:: TurnOnHgLamp()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_TURN_ON_MERCURY_LAMP));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 关闭汞灯
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface:: TurnOffHgLamp()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_TURN_OFF_MERCURY_LAMP));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/** * @brief 打开LED
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::TurnOnLED()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_TURN_ON_LED));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 关闭LED
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::TurnOffLED()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_TURN_OFF_LED));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 启动静态AD调节至目标值操作。
 * @details 开启LED，调整电位器输出值，优化目标AD。
 * @param index Uint8，目标LED索引号[0-参考 1-测量 2-定量1 3-定量2 4-定量3]。
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
 * @param index Uint8，目标LED索引号[0-参考 1-测量 2-定量1 3-定量2 4-定量3]。
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
        value = *((Uint16*)resp->data);
    }

    return value;
}

/**
 * @brief 设置默认静态AD控制参数。
 * @details 静态AD控制器将根据设置的参数设置器件，并将参数永久保存在FLASH。
 * @details 静态AD调节状态中禁用。
 * @param index Uint8，目标LED索引号[0-参考 1-测量 2-定量1 3-定量2 4-定量3]。
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
 * @brief 查询默认静态AD放大倍数开关。
 * @param index Uint8，目标LED索引号[0-参考 1-测量]。
 * @return value Uint16 ，放大电路开关状态[0-关 1-开]。
 * @see DSCP_CMD_OAI_SET_STATIC_AD_CONTROL_PARAM
 */
Uint16 OpticalAcquireInterface::GetStaticADLevelParam(Uint8 index) const
{
    Uint16 value = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_GET_STATIC_AD_LEVEL, &index, sizeof(index)));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        value = *((Uint16*)resp->data);
    }

    return value;
}

/**
 * @brief 设置默认静态AD放大倍数开关。
 * @details 设置默认测量信号放大电路开关状态，并将参数永久保存在FLASH。
 * @param index Uint8，目标LED索引号[0-参考 1-测量]。
 * @param value Uint16 ，放大电路档位状态[0-关 1-开]。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalAcquireInterface::SetStaticADLevelParam(Uint8 index, Uint16 value)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[3] = {0};
    memcpy(cmdData, &index, sizeof(Uint8));
    memcpy(cmdData+1, &value, sizeof(Uint16));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OAI_SET_STATIC_AD_LEVEL, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    if (status != DscpStatus::OK)
    {
        logger->debug("启动LED调节命令应答状态：%d", (int)status);
    }
    return (status == DscpStatus::OK);
}


/**
 * @brief 信号AD采集完成事件。
 * @details 启动采集后，按照设置的采样周期采完数据时将产生该事件。
 * @param number Uint32，采样时间内得到的数据个数，决定了后面的变长参数长度。
 * @param value[number][2] Uint32，采样时间内得到的AD值。value格式如下：
 *      - value[number][0] reference Uint32，参考端AD。
 *      - value[number][1] measure Uint32，测量端AD。
 * @see DSCP_CMD_OAI_START_ACQUIRER
 */
Uint32 OpticalAcquireInterface::ExpectADAcquirer(long timeout, vector<OpticalAD> &ad)
{
    Uint32 number = 0;
    DscpEventPtr event = this->Expect(DSCP_EVENT_OAI_AD_ACQUIRED ,timeout);

    if (event != nullptr)
    {
        number = *((Uint32*)event->data);

        for(Uint32 i = 0; i < number *2; i += 2)
        {
            OpticalAD data;

            data.reference = *((Uint32*)(event->data+4*(i+1)));
            data.measure = *((Uint32*)(event->data+4*(i+2)));
            ad.push_back(data);
        }
    }

    return number;
}

/**
 * @brief 信号AD定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报信号AD。
 *  上报周期可通过命令 @ref DSCP_CMD_TCI_SET_TEMPERATURE_NOTIFY_PERIOD 设定。
 * @param reference Uint32，参考端AD。
 * @param measure Uint32，测量端AD。
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
