/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 设备简单控制协议（设备服务端）。
 * @details 
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2013-2-26
 */

#ifndef DNCP_APP_DSCP_DEVICE_H_
#define DNCP_APP_DSCP_DEVICE_H_

#include "Container/Queue.h"
#include "DNCP/Base/Frame.h"
#include "DNCP/Net/Trp.h"
#include "AppDefine.h"
#include "Dscp.h"
#include "LuipShare.h"

#ifdef __cplusplus
extern "C" {
#endif

//*******************************************************************
// DscpDevice 定义

/**
 * @brief DscpDevice 实体错误统计。
 * @details 统计 DscpDevice 协议实体的运行情况。
 */
LUIP_SHARE typedef struct DscpDeviceStatisticsStruct
{
    unsigned int recvCmds;              ///< 接收的命令包统计
    unsigned char cmdQueueOverflows;    ///< 命令队列溢出造成命令被丢的次数
    unsigned char transQueueOverflows;  ///< 发送队列溢出造成命令被丢的次数
    unsigned char outOfRamError;        ///< 内存分配失败（不足）的错误数
    unsigned short sendFailures;        ///< 发送失败次数
}
DscpDeviceStatistics;


/**
 * @brief 应用层 DscpDevice 实体对象。
 */
LUIP_SHARE typedef struct DscpDeviceStruct
{
    INetHandle netHandle;           ///< 下层处理接口，必须在结构初始化处定义。具体实现时必须初始化它
    Net* net;                       ///< 底层通信实体
    DscpCmdEntry* cmdTable;         ///< 应用命令表，以0结束尾表项
    DscpVersion version;            ///< DSCP接口的版本号
    Queue transQueue;               ///< 发送报文队列，包括命令、回应、事件
    Frame* transFrames[32];         ///< 发送报文队列的缓冲
    Queue cmdQueue;                 ///< 接收的命令队列
    Frame* cmdFrames[16];           ///< 接收的命令队列的缓冲

    Uint16 curCmdCode;              ///< 当前处理中的命令码
    Uint8 sourceAddr;               ///< 当前处理中的命令的源地址，上行地址
    Uint8 packOffset;               ///< 当前处理中的命令的包偏移

    DscpDeviceStatistics dscpStat;  ///< DSCP 统计信息
}
DscpDevice;

//*******************************************************************
// DSCP 实现相关

extern void DscpDevice_Init(DscpDevice* self);
extern void DscpDevice_Uninit(DscpDevice* self);
extern void DscpDevice_Setup(DscpDevice* self, Net* netToUse, DscpCmdTable cmdTable, DscpVersion version);

extern void DscpDevice_Handle(DscpDevice* self);

extern LUIP_SHARE Bool DscpDevice_SendResp(DscpDevice* self, void* data, Uint16 len);
extern LUIP_SHARE Bool DscpDevice_SendRespEx(DscpDevice* self, RespCode resp, void* data, Uint16 len);
extern LUIP_SHARE Bool DscpDevice_SendStatus(DscpDevice* self, StatusCode status);
extern LUIP_SHARE Bool DscpDevice_SendStatusEx(DscpDevice* self, RespCode resp, StatusCode status);
extern LUIP_SHARE Bool DscpDevice_SendEvent(DscpDevice* self, EventCode event, void* data, Uint16 len);
extern LUIP_SHARE Bool DscpDevice_Send(DscpDevice* self, Uint8 dscpType, Uint16 code, void* data, Uint16 len, Bool needFollow);

extern NetAddress DscpDevice_GetSourceAddr(DscpDevice* self);
extern void DscpDevice_SetDestAddr(DscpDevice* self, NetAddress destAddr);


#ifdef __cplusplus
}
#endif

#endif  // DNCP_APP_DSCP_DEVICE_H_

/** @} */
