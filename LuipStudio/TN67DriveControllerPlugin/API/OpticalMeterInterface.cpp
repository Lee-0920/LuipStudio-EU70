/**

 * @file
 * @brief 光学定量接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "Code/OpticalMeterInterface.h"
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Communication/EventHandler.h"
#include "OpticalMeterInterface.h"
#include "ControllerPlugin/API/DeviceStatusInterface.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief 光学定量接口构造。
 * @param[in] addr 设备地址。
 */
OpticalMeterInterface::OpticalMeterInterface(DscpAddress addr)
    : DeviceInterface(addr)
{

}

/**
 * @brief 打开定量点的LED。
 * @param num Uint8，需要控制的定量点 。NUM范围 1 - POINTS_MAX
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */

bool OpticalMeterInterface::TurnOnLED(Uint8 num)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_TURN_ON_LED, &num, 1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 关闭定量点的LED。
 * @param num Uint8，需要控制的定量点 。NUM范围 1 - POINTS_MAX
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalMeterInterface::TurnOffLED(Uint8 num)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_TURN_OFF_LED, &num, 1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 自动校准定量泵，并永久保存计算出的校准系数。
 * @details 将根据设定的定量点，计算出两点间的体积，并测量泵的转动角度。
 * @return 返回状态，操作是否成功。
 * @see DSCP_CMD_OMI_SET_METER_POINTS
 * @see DSCP_EVENT_OMI_AUTO_CALIBRATE_RESULT
 */
//bool OpticalMeterInterface::AutoCalibratePump(Uint32 valve)
//{
//    Uint16 status = DscpStatus::Error;
//
//    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_AUTO_CALIBRATE_PUMP, &valve, sizeof(valve)));
//    SyncCaller syncCaller(m_retries);
//    status = syncCaller.SendWithStatus(cmd);
//    return (status == DscpStatus::OK);
//
//}

/**
 * @brief 查询定量泵的校准系数。
 * @return 校准系数， float，每步泵出的体积，单位为 ml/度。
 * @see DSCP_CMD_OMI_SET_PUMP_FACTOR
 */
float OpticalMeterInterface::GetPumpFactor() const
{
    float factor =0 ;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_GET_PUMP_FACTOR));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        factor = *((float*)resp->data);
    }

    return factor;
}

/**
 * @brief 设置定量泵的校准系数。
 * @details 因为蠕动泵的生产工艺及工作时长，每个泵转一步对应体积都不同，出厂或使用时需要校准。
 *  该参数将永久保存。
 * @param[in] factor float，要设置的校准系数， 每步泵出的体积，单位为 ml/度。
 * @return 返回状态，操作是否成功。
 * @see DSCP_CMD_OMI_GET_PUMP_FACTOR
 */
bool OpticalMeterInterface::SetPumpFactor(float factor)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_SET_PUMP_FACTOR, &factor, sizeof(factor)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询定量点体积。
 * @return 定量点参数， 数据格式为：
 *     - num Uint8，定量点数目。决定了后面的变长参数长度。
 *     - setVolume1 float，定量点1设定体积，单位为 ml。
 *     - realVolume1 float，定量点1实际体积，单位为 ml。
 *     - setVolume2 float，定量点2设定体积，单位为 ml。
 *     - realVolume2 float，定量点2实际体积，单位为 ml。
 *     - setVolume3 float，定量点3设定体积，单位为 ml。
 *     - realVolume3 float，定量点3实际体积，单位为 ml。
 *     - ...
 * @see DSCP_CMD_OMI_SET_METER_POINTS
 */
MeterPoints OpticalMeterInterface::GetMeterPoints() const
{
    MeterPoint point;
    MeterPoints points;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_GET_METER_POINTS));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        points.SetNum(*((Uint8*)resp->data));

        for(Uint8 i = 0; i < points.GetNum(); i++)
        {
            memcpy(&point, resp->data + 1 + i*sizeof(point), sizeof point);
            points.SetPoint(i+1, point);
        }
    }

    return points;
}

/**
 * @brief 设置定量点体积。
 * @details 系统将根据设定的定量点体积，进行最佳的定量分配。实际体积将被用于定量校准。
 *   定量点参数将永久保存。
 * @param[in] num Uint8，定量点数目。决定了后面变长参数的个数。
 * @param[in] setVolume1 float，定量点1设定体积，单位为 ml。
 * @param[in] realVolume1 float，定量点1实际体积，单位为 ml。
 * @param[in] setVolume2 float，定量点2设定体积，单位为 ml。
 * @param[in] realVolume2 float，定量点2实际体积，单位为 ml。
 * @param[in] setVolume3 float，定量点3设定体积，单位为 ml。
 * @param[in] realVolume3 float，定量点3实际体积，单位为 ml。
 * @return 返回状态，操作是否成功。
 * @see DSCP_CMD_OMI_GET_METER_POINTS
 */
bool OpticalMeterInterface::SetMeterPoints(MeterPoints points)
{
    Uint16 status = DscpStatus::Error;
    int count = points.GetNum() * sizeof(MeterPoint);
    Byte* cmdData = new Byte[count + 1];

    cmdData[0] = points.GetNum();
    MeterPoint point;
    for(int i = 0; i < points.GetNum(); i++)
    {
        point = points.GetPoint(i+1);
        memcpy(cmdData + 1 + i * sizeof(float) * 2, &point, sizeof(float) * 2);
    }

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_SET_METER_POINTS, cmdData, count+1));

    SyncCaller syncCaller(m_retries);

    if (cmdData)
    {
        delete[] cmdData;
    }

    status = syncCaller.SendWithStatus(cmd);

    return (status == DscpStatus::OK);
}

/**
 * @brief 查询定量的工作状态。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_IDLE 空闲；
 *  - @ref DSCP_BUSY 忙碌，需要停止后才能做下一个动作；
 */
Uint16 OpticalMeterInterface::GetMeterStatus() const
{
    Uint16 status = DscpStatus::Busy;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_GET_METER_STATUS));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return status;
}

/**
 * @brief 开始定量。
 * @details 定量开始后，不管成功与否，操作结果都将以事件的形式上传给上位机。关联的事件有：
 *   - @ref DSCP_EVENT_OMI_METER_RESULT
 * @param[in] dir Uint8，定量方向，0为正向转动（抽取），1为反向转动（排空）。
 * @param[in] mode Uint8，定量模式，支持的模式见： @ref MeterMode 。
 * @param[in] volume float，定量体积，单位为 ml。
 * @param[in] limitVolume float，限量体积，在定量过程中总的泵取体积不能超过该值，0表示不进行限量判定。单位为 ml。
 * @return 返回状态，操作是否成功。
 * @note 该命令将立即返回，定量完成将以事件的形式上报。
 */
bool OpticalMeterInterface::StartMeter(RollDirection dir, MeterMode mode, float volume, float limitVolume)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[10] = {0};
    int meterMode = (int) mode;

    memcpy(cmdData, &dir, sizeof(Uint8));
    memcpy(cmdData+1, &meterMode, sizeof(Uint8));
    memcpy(cmdData+2, &volume, sizeof(float));
    memcpy(cmdData+6, &limitVolume, sizeof(float));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_START_METER, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 停止定量。
 * @details 当前定量动作将被尽快停止，已经定量的溶液不会自动排到废液池，需要显式调用。
 * @return 返回状态，操作是否成功。
 * @note 该命令将立即返回。
 */
bool OpticalMeterInterface::StopMeter()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_STOP_METER));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 定量结束自动关闭阀开关
 * @param IsAutoCloseValve Bool 是否自动关闭。TRUE 自动关闭，FALSE 定量结束不关闭阀
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalMeterInterface::IsAutoCloseValve(bool isCloseValve)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_AUTO_CLOSE_VALVE, &isCloseValve, sizeof(isCloseValve)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 设置定量点光信号AD上报周期。
 * @details 系统将根据设定的周期，定时向上发出定量点光信号AD上报事件。
 * @param period Float32，定量点光信号AD上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_OMI_OPTICAL_AD_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。
 */
bool OpticalMeterInterface::SetOpticalADNotifyPeriod(float period)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_SET_OPTICAL_AD_NOTIFY_PERIOD, &period, sizeof(period)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}


/**
 * @brief 定量结果事件。
 * @details 定量操作结束时将产生该事件。
 * @param[in] timeout 操作超时时间。
 * @returen Uint8，定量结果码（ @ref MeterResult ），定义如下：
 *  - @ref Finished 定量正常完成。
 *  - @ref Failed 定量中途出现故障，未能完成。
 *  - @ref Stopped 定量被停止。
 *  - @ref Overflow 定量溢出。
 *  - @ref Unfinished 定量目标未达成。
 */
MeterResult OpticalMeterInterface::ExpectMeterResult(int timeout)
{
    MeterResult result = MeterResult::Failed;

    DscpEventPtr event = this->Expect(DSCP_EVENT_OMI_METER_RESULT,timeout);

    if (event != nullptr)
    {
        result = (MeterResult)(*event->data);
    }

    return result;
}

/**
 * @brief 自动校准结果事件。
 * @details 自动校准操作结束时将产生该事件。
 * @param[in] timeout 操作超时时间。
 * @returen Uint8，定量结果码（ @ref MeterResult ），定义如下：
 *  - @ref Finished 定量正常完成。
 *  - @ref Failed 定量中途出现故障，未能完成。
 *  - @ref Stopped 定量被停止。
 *  - @ref Overflow 定量溢出。
 *  - @ref Unfinished 定量目标未达成。
 */
//MeterResult OpticalMeterInterface::ExpectAutoCalibrateResult(long timeout)
//{
//    MeterResult result = MeterResult::Failed;
//
//    DscpEventPtr event = this->Expect(DSCP_EVENT_OMI_AUTO_CALIBRATE_RESULT,timeout);
//
//    if (event != nullptr)
//    {
//        result = (MeterResult)(*event->data);
//    }
//
//    return result;
//}

/**
 * @brief 设置定量速度
 * @param speed float 定量的速度
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalMeterInterface::SetMeteSpeed(float speed)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_SET_METER_SPEED, &speed, sizeof(speed)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 获取定量速度
 * @return speed float 定量的速度
 */
float OpticalMeterInterface::GetMeteSpeed() const
{
    float speed =0 ;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_GET_METER_SPEED));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        speed = *((float*)resp->data);
    }

    return speed;
}

/**
 * @brief 设置定量结束时阀要改变成的状态
 * @param map Uint32，阀通道映射图，每位表示一个通道的开关状态，1为打开，0为闭合，低位开始。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalMeterInterface::SetMeterFinishValveMap(Uint32 map)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_SET_METER_FINISH_VALVE_MAP, &map, sizeof(map)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 获取获取某个定量点光信号AD
 * @param num Uint8，需要获取的定量点。NUM范围 1 - POINTS_MAX
 * @return adValue Uint32 定量点光信号AD
 */
Uint32 OpticalMeterInterface::GetSingleOpticalAD(Uint8 num) const
{
    Uint32 adValue =0 ;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_GET_SINGLE_OPTICAL_AD, &num, sizeof(num)));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        adValue = *((Uint32*)resp->data);
    }

    return adValue;
}

/**
 * @brief 设置粘稠模式定量过冲量
 * @param value Uint16 定量过冲量
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalMeterInterface::SetRopinessMeterOverValue(Uint16 value)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_SET_ROPINESS_METER_OVER_VALUE, &value, sizeof(value)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 获取粘稠模式定量过冲量
 * @return value Uint16 定量过冲量
 */
Uint16 OpticalMeterInterface::GetRopinessMeterOverValue() const
{
    Uint16 value =0 ;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_GET_ROPINESS_METER_OVER_VALUE));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        value = *((Uint16*)resp->data);
    }

    return value;
}

/**
 * @brief 设置定量LED光强等级
 * @param index Uint8 定量LED索引, 1-2
 * @param value Uint16 定量LED光强等级, 0低/1中/2高
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalMeterInterface::SetMeterLedLevel(Uint8 index, Uint16 value)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[3] = {0};
    memcpy(cmdData, &index, sizeof(Uint8));
    memcpy(cmdData+1, &value, sizeof(Uint16));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_SET_METER_LED_LEVEL, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    if (status != DscpStatus::OK)
    {
        logger->debug("设置定量LED光强命令应答状态：%d", (int)status);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 获取定量LED光强等级
 * @return value Uint16 定量LED光强等级
 */
Uint16 OpticalMeterInterface::GetMeterLedLevel(Uint8 index) const
{
    Uint16 value = 65535;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_GET_METER_LED_LEVEL, &index, sizeof(index)));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        value = *((Uint16*)resp->data);
    }

    return value;
}

/**
 * @brief 注册定量点光信号AD定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报定量点光信号AD。
 *  上报周期可通过命令 @ref DSCP_CMD_OMI_SET_OPTICAL_AD_NOTIFY_PERIOD 设定。
 * @param[in] handle 定时接收定量点光信号AD上报事件的对象。
 */
void OpticalMeterInterface::RegisterOpticalADNotice(IEventReceivable *handle)
{
    EventHandler::Instance()->Register(m_addr,DSCP_EVENT_OMI_OPTICAL_AD_NOTICE,handle);   
}

/**
 * @brief 获取精确定量模式下需要达到判定终点的次数
 * @return cnt Uint16 精确定量模式下需要达到判定终点的次数
 */
Uint16 OpticalMeterInterface::GetMeterEndPointOverCount() const
{
    Uint16 cnt =0 ;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_GET_ACCURATE_MODE_STANDARD_CNT));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        cnt = *((Uint16*)resp->data);
    }

    return cnt;
}

/**
 * @brief 设置精确定量模式下需要达到判定终点的次数
 * @param cnt Uint16 精确定量模式下需要达到判定终点的次数
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
bool OpticalMeterInterface::SetMeterEndPointOverCount(Uint16 cnt)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_OMI_SET_ACCURATE_MODE_STANDARD_CNT, &cnt, sizeof(cnt)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

}
}
