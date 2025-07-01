/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 设备命令表（示例）。
 * @details 本命令表仅为示例程序，具体应用应该实现不同的命令表。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2013-2-27
 */

#ifndef DNCP_STACK_DEVICE_CMD_TABLE_H_
#define DNCP_STACK_DEVICE_CMD_TABLE_H_

#include "Communication/eCek/DNCP/App/DscpDevice.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 获取设备的DNCP命令表。
 */
extern DscpCmdTable DevCmdTable_GetTable(void);

/**
 * @brief 获取设备命令表的版本信息。
 */
extern DscpVersion DevCmdTable_GetVersion(void);

#ifdef __cplusplus
}
#endif

#endif  // DNCP_STACK_DEVICE_CMD_TABLE_H_

/** @} */
