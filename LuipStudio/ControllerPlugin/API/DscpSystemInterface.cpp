/**
 * @file
 * @brief Dscp 系统接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/10
 */

#include <stdlib.h>
#include "Communication/SyncCaller.h"
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/eCek/DNCP/App/DscpSysDefine.h"
#include "DscpSystemInterface.h"

using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief Dscp 系统接口构造。
 * @details 提供 DSCP 系统命令的调用操作。
 * @param[in] addr 设备地址。
 *
 */
DscpSystemInterface::DscpSystemInterface(DscpAddress addr /*= 0*/)
    : DeviceInterface(addr)
{

}

/**
 * @brief Echo 测试。
 * @details 向设备发送一段数据，测试是否原样返回。
 * @param[in] len Echo 时附带的数据长度。0 表示不附带任务数据，只是单纯的 Echo。
 * @param[in] data 指定的附带数据缓冲， nullptr 表示不指定，由系统随机生成。
 */
bool DscpSystemInterface::Echo(int len /*= 0*/, Byte* data /*= nullptr*/)
{
    if (len > 240)
        len = 240;
    if (len < 0)
        len = 0;

    if (len == 0)
    {
        data = nullptr;
    }
    else
    {
        if (data == nullptr)
        {
            data = new Byte[len];
            for (int i = 0; i < len; i++)
            {
                data[i] = static_cast<Byte>(rand() & 0xFF);
            }
        }
    }

    bool ret = false;
    DscpRespPtr resp = nullptr;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_SYSCMD_ECHO, data, len));
    SyncCaller  syncCaller(m_retries);

    resp = syncCaller.Send(cmd);

    if (resp)
    {
        if (resp->code == DSCP_SYSCMD_ECHO &&
            resp->len == len && resp->addr == m_addr)
        {
            if (0 == memcmp(resp->data, data, len))
            {
                ret = true;
            }
        }
    }

    if (data)
    {
        delete[] data;
    }

    return ret;
}

/**
 * @brief 系统命令：获取DSCP接口版本号。
 * @details DSCP接口版本号表征了应用命令的版本变更，一般情况下，接口命令、回应或状态
 *  有增或删或改时，DSCP应用会变更该版本号。
 * @return 版本号，Byte[4]：
 *  - @b major Uint8，主版本号；
 *  - @b minor Uint8，次版本号；
 *  - @b revision Uint8，修订版本号；
 *  - @b build Uint8，编译版本号；
 */
Version DscpSystemInterface::GetVersion()
{   
    Version ver;
    DscpRespPtr resp = nullptr;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_SYSCMD_IFVER_GET));
    SyncCaller  syncCaller(m_retries);

    resp = syncCaller.Send(cmd);

    if (resp)
    {
        if (resp->code == DSCP_SYSCMD_IFVER_GET && resp->addr == m_addr)
        {
            ver.SetData(resp->data, 4);
        }
    }
    return ver;
}

bool DscpSystemInterface::AcquireExpectEvent()
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_SYSCMD_ACQUIRE_EVENT));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

}
}
