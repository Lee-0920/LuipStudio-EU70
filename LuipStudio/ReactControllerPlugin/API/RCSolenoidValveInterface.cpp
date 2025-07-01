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
#include "RCSolenoidValveInterface.h"

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
RCSolenoidValveInterface::RCSolenoidValveInterface(DscpAddress addr)
    : DeviceInterface(addr)
{

}

/**
 * @brief 查询系统支持的总电磁阀数目。
 * @return 总电磁阀数目， Uint16。
 */
int RCSolenoidValveInterface::GetTotalValves()
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
RCValveMap RCSolenoidValveInterface::GetValveMap()
{
    RCValveMap map(0);
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
bool RCSolenoidValveInterface::SetValveMap(RCValveMap map)
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
bool RCSolenoidValveInterface::SetValveMapNormalOpen(RCValveMap map)
{
    Uint16 status = DscpStatus::Error;
    Uint32 mapVal = map.GetData();

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_SVI_SET_VALVE_MAP_NORMAL_OPEN, &mapVal, sizeof(mapVal)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 设置要开启的阀门映射图。
 * @param map Uint32，通道映射图，每位表示一个通道的开关状态，1为打开，0为闭合，低位开始。
* @return 返回状态，操作是否成功。
 */
bool RCSolenoidValveInterface::SetGainMap(Uint8 map)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_SVI_SET_GAIN_MAP, &map, sizeof(map)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 设置要开启的阀门映射图。
 * @param map Uint32，通道映射图，每位表示一个通道的开关状态，1为打开，0为闭合，低位开始。
* @return 返回状态，操作是否成功。
 */
Uint8 RCSolenoidValveInterface::GetGainMap(void)
{
    Uint8 map = 0;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_SVI_GET_GAIN_MAP));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
       map = *((Uint8*)resp->data);
    }

    return map;
}

}
}
