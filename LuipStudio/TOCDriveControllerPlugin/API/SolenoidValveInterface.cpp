/**

 * @file
 * @brief 电磁阀控制接口。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "Code/SolenoidValveInterface.h"
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Communication/EventHandler.h"
#include "SolenoidValveInterface.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief 电磁阀控制接口构造。
 * @param[in] addr 设备地址。
 */
SolenoidValveInterface::SolenoidValveInterface(DscpAddress addr)
    : DeviceInterface(addr)
{

}

/**
 * @brief 查询系统支持的总电磁阀数目。
 * @return 总电磁阀数目， Uint16。
 */
int SolenoidValveInterface::GetTotalValves()
{
    int totalValves = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_SVI_GET_TOTAL_VALVES));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        totalValves = *((Uint16*)resp->data);
    }

    return totalValves;
}

/**
 * @brief 查询当前开启的阀门映射图。
 * @return 通道映射图，Uint32，每位表示一个通道的开关状态，1为打开，0为闭合，低位开始。
 */
ValveMap SolenoidValveInterface::GetValveMap()
{
    ValveMap map(0);

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_SVI_GET_VALVE_MAP));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        map.SetData(*((Uint32*)resp->data));
    }

    return map;
}

/**
 * @brief 设置要开启的阀门映射图。
 * @param map Uint32，通道映射图，每位表示一个通道的开关状态，1为打开，0为闭合，低位开始。
* @return 返回状态，操作是否成功。
 */
bool SolenoidValveInterface::SetValveMap(ValveMap map)
{
    Uint16 status = DscpStatus::Error;
    Uint32 mapVal = map.GetData();

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_SVI_SET_VALVE_MAP, &mapVal, sizeof(mapVal)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 设置要常开阀门映射图。
 * @param map Uint32，通道映射图，每位表示一个通道的开关状态，1为打开，0为闭合，低位开始。
* @return 返回状态，操作是否成功。
 */
bool SolenoidValveInterface::SetValveMapNormalOpen(ValveMap map)
{
    Uint16 status = DscpStatus::Error;
    Uint32 mapVal = map.GetData();

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_SVI_SET_VALVE_MAP_NORMAL_OPEN, &mapVal, sizeof(mapVal)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 注册压力定时上报事件。
 * @details 定时2s向上汇报压力。
 *  上报周期暂不更改。
 * @param[in] handle 定时接收压力上报事件的对象。
 */
void SolenoidValveInterface::RegisterPressureNotice(IEventReceivable *handle)
{
    EventHandler::Instance()->Register(m_addr,DSCP_EVENT_SVI_PREESURE_NOTICE,handle);
}


}
}
