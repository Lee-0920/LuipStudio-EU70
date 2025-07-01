/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief DNCP 主控协议栈。
 * @details 为设备构建一个静态全局的协议栈栈体。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2012-12-21
 */

#ifndef DNCP_STACK_DNCP_MC_STACK_H_
#define DNCP_STACK_DNCP_MC_STACK_H_

#include "../eCek/Common/Types.h"
#include "../eCek/DNCP/App/Dscp.h"
#include "../eCek/DNCP/Lai/IConnectStatusNotifiable.h"

#ifdef __cplusplus
extern "C" {
#endif   

Bool DncpMcStack_Init(char* serialPort, IConnectStatusNotifiable *handle);
void DncpMcStack_Uninit(void);
void DncpMcStack_AddSlaveNode(int addr);
Bool DncpMcStack_Reconnect(char* serialPort, IConnectStatusNotifiable *handle);
void DncpMcStack_HandleDscpCmd(void);
void DncpMcStack_SetDscpCmdTable(DscpCmdTable cmdTable, DscpVersion version);

#ifdef __cplusplus
}
#endif

#endif  // DNCP_STACK_DNCP_MC_STACK_H_

/** @} */
