/**

 * @file
 * @brief 蠕动泵控制接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "Code/PeristalticPumpInterface.h"
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Communication/EventHandler.h"
#include "PeristalticPumpInterface.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief 蠕动泵控制接口构造。
 * @param[in] addr 设备地址。
 */
PeristalticPumpInterface::PeristalticPumpInterface(DscpAddress addr)
    : DeviceInterface(addr)
{
}


/**
 * @brief 查询系统支持的总泵数目。
 * @return 总泵数目， Uint16。
 */
int PeristalticPumpInterface::GetTotalPumps()
{
    Uint16 pumps = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_GET_TOTAL_PUMPS));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        pumps = *((Uint16*)resp->data);
    }

    return pumps;
}

/**
 * @brief 查询指定泵的校准系数。
 * @param[in] index Uint8，要查询的泵索引，0号泵为光学定量泵。
 * @return 校准系数， float，每步泵出的体积，单位为 ml/度。
 * @see DSCP_CMD_PPI_SET_PUMP_FACTOR
 */
float PeristalticPumpInterface::GetPumpFactor(int index)
{
    float factor = 0;

    Uint8 num = (Uint8)index;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_GET_PUMP_FACTOR, &num, sizeof(num)));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        factor = *((float*)resp->data);
    }

    return factor;
}

/**
 * @brief 设置指定泵的校准系数。
 * @details 因为蠕动泵的生产工艺及工作时长，每个泵转一步对应体积都不同，出厂或使用时需要校准。
 *  该参数将永久保存。
 * @param[in] index Uint8，要设置的泵索引，0号泵为光学定量泵。
 * @param[in] factor float，要设置的校准系数， 每步泵出的体积，单位为 ml/度。
 * @return 返回状态，操作是否成功。
 */
bool PeristalticPumpInterface::SetPumpFactor(int index, float factor)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[5];

    cmdData[0] = index;
    memcpy(cmdData+1, &factor, sizeof(float));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_SET_PUMP_FACTOR, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);

}

/**
 * @brief 查询指定泵的运动参数。
 * @details 系统将根据设定的运动参数进行运动控制和规划。
 * @param[in] index Uint8，要查询的泵索引，0号泵为光学定量泵。
 * @return 运动参数， 数据格式为：
 *  - acceleration float，加速度，单位为 ml/平方秒。
 *  - speed float，最大速度，单位为 ml/秒。
 * @see DSCP_CMD_PPI_SET_MOTION_PARAM
 */
MotionParam PeristalticPumpInterface::GetMotionParam(int index)
{
    MotionParam param;

    Uint8 num = (Uint8)index;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_GET_MOTION_PARAM, &num, sizeof(num)));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        param = *((MotionParam*)resp->data);
    }

    return param;
}

/**
 * @brief 设置指定泵的运动参数。
 * @details 系统将根据设定的运动参数进行运动控制和规划。运动参数将永久保存。
 * @param[in] index Uint8，要设置的泵索引，0号泵为光学定量泵。
 * @param[in] acceleration float，加速度，单位为 ml/平方秒。
 * @param[in] speed float，最大速度，单位为 ml/秒。
* @return 返回状态，操作是否成功。
 */
bool PeristalticPumpInterface::SetMotionParam(int index, MotionParam param)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[9];

    cmdData[0] = index;
    memcpy(cmdData+1, &param, sizeof(MotionParam));

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_SET_MOTION_PARAM, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询指定泵的工作状态。
 * @param[in] index Uint8，要设置的泵索引，0号泵为光学定量泵。
 * @return 蠕动泵状态：
 *  - @ref PUMP_RESULT_FINISHED  泵操作正常完成；
 *  - @ref PUMP_RESULT_FAILED  泵操作中途出现故障，未能完成。
 *  - @ref PUMP_RESULT_STOPPED  泵操作被停止。
 */
PumpStatus PeristalticPumpInterface::GetPumpStatus(int index)
{
    PumpStatus status = PumpStatus::Idle;

    Uint8 num = (Uint8)index;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_GET_PUMP_STATUS, &num, sizeof(num)));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        status = (PumpStatus)*resp->data;
    }

    return status;
}

/**
 * @brief 启动泵。
 * @details 启动后，不管成功与否，操作结果都将以事件的形式上传给上位机。关联的事件有：
 *   - @ref DSCP_EVENT_PPI_PUMP_RESULT
 * @param[in] index Uint8，要操作的泵索引，0号泵为光学定量泵。
 * @param[in] dir Uint8，泵转动方向，0为正向转动（抽取），1为反向转动（排空）。
 * @param[in] volume float，泵取/排空体积，单位为 ml。
 * @return 返回状态，操作是否成功。
 * @note 该命令将立即返回，泵转动完成将以事件的形式上报。
 */
bool PeristalticPumpInterface::StartPump(int index, RollDirection dir, float volume, float seep)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[10] = {0};

    cmdData[0] = index;
    cmdData[1] = (Uint8)dir;
    memcpy(cmdData+2, &volume, sizeof(float));
    memcpy(cmdData+6, &seep, sizeof(float));
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_START_PUMP, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 停止泵。
 * @param[in] index Uint8，要操作的泵索引，0号泵为光学定量泵。
 * @return 返回状态，操作是否成功。
 */
bool PeristalticPumpInterface::StopPump(int index)
{
    Uint16 status = DscpStatus::Error;

    Uint8 num = (Uint8)index;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_STOP_PUMP, &num, sizeof(num)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询泵出的体积。
 * @details 启动泵到停止泵的过程中，泵的转动体积（步数）。
 * @param[in] index Uint8，要查询的泵索引，0号泵为光学定量泵。
 * @return 泵出的体积，单位为 ml。
 * @see DSCP_CMD_PPI_START_PUMP
 */
float PeristalticPumpInterface::GetPumpVolume(int index)
{
    float volume = 0;

    Uint8 num = (Uint8)index;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_GET_PUMP_VOLUME, &num, sizeof(num)));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        volume = *((float*)resp->data);
    }

    return volume;
}

/**
 * @brief 设置指定泵的安装方式。
 * @param index Uint8，传感器索引，0~2(0、1为液位传感器，2为复位传感器)
 * @return 传感器状态，Uint8,0和1(0为遮挡，1为未遮挡)
 */
Uint8 PeristalticPumpInterface::GetSenseStatus(int index)
{
    Uint8 statu = 0;

    Uint8 num = (Uint8)index;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_GET_SENSE_STATUS, &num, sizeof(num)));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        statu = *((Uint8*)resp->data);
    }

    return statu;
}

/**
 * @brief 查询指定泵的安装方式。
 * @param index Uint8: 泵索引，0号泵为光学定量泵。
 * @return fixedType Uint8: 泵安装方式， 0=顺排逆抽, 1=顺抽逆排。
 */
PumpFixedType PeristalticPumpInterface::GetPumpFixedType(int index)
{
    PumpFixedType type = PumpFixedType::ClockwiseDrainAnticlockwiseSuck;

    Uint8 num = (Uint8)index;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_GET_PUMP_FIXED_TYPE, &num, sizeof(num)));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        type = (PumpFixedType)*resp->data;
    }

    return type;
}

/**
 * @brief 设置指定泵的安装方式。
 * @param index Uint8，泵索引，0号泵为光学定量泵。
 * @param fixedType Uint8，泵安装方式， 0=顺排逆抽, 1=顺抽逆排。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 *  - @ref DSCP_ERROR_PARAM 参数错误，传入的参数有问题
 */
bool PeristalticPumpInterface::SetPumpFixedType(int index, PumpFixedType type)
{
    Uint16 status = DscpStatus::Error;
    Uint8 cmdData[2];

    cmdData[0] = (Uint8)index;
    cmdData[1] = (Uint8)type;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_PPI_SET_PUMP_FIXED_TYPE, cmdData, sizeof(cmdData)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}


/**
 * @brief 泵操作结果事件。
 * @details 启动泵转动操作结束时将产生该事件。
 * @param[out] index Uint8，产生事件的泵索引，0号泵为光学定量泵。
 * @param[out] result Uint8，泵操作结果码（ @ref PumpResult ），定义如下：
 *  - @ref Finished 泵操作正常完成
 *  - @ref Failed 泵操作中途出现故障，未能完成
 *  - @ref Stopped 泵操作被停止
 */
PumpResult PeristalticPumpInterface::ExpectPumpResult(long timeout)
{
    PumpResult pumpResult;
    memset(&pumpResult, 0, sizeof(pumpResult));

    DscpEventPtr event = this->Expect(DSCP_EVENT_PPI_PUMP_RESULT,timeout);

    if (event != nullptr)
    {
        pumpResult.index = *event->data;
        pumpResult.result = (int)(*(event->data+1));
    }

    return pumpResult;
}

}
}
