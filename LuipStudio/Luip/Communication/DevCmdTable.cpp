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

#include <string.h>

#include "DevCmdTable.h"
#include "Communication/eCek/Common/Utils.h"
#include "InstrumentInfomation.h"
#include "DeviceInfo.h"
#include <QDebug>
/**
 * 命令接口的版本号。
 * 每一次接口命令的调整，都应该变更这个版本号。
 */
static const DscpVersion s_kDscpInterfaceVersion =
{
        1,      // 主版本号
        0,      // 次版本号
        0,      // 修订版本号
        0       // 编译版本号
};

/*
 * 各应用相关的命令的入口函数都需要添加到这里。
 * 每一条命令对应一个命令码和处理函数。
 */
static const DscpCmdEntry s_kDscpCmdTable[] =
{
    CMD_TABLE_INSTRUMENT_INFO,
    CMD_TABLE_DEVICE_INFO,
    DSCP_CMD_ENTRY_END
};

DscpCmdTable DevCmdTable_GetTable(void)
{
    return (DscpCmdTable) s_kDscpCmdTable;
}

DscpVersion DevCmdTable_GetVersion(void)
{
    return s_kDscpInterfaceVersion;
}

//*******************************************************************
// 应用命令实现

/** @} */
