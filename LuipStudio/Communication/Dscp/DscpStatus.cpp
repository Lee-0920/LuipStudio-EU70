/**
 * @file
 * @brief 
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "DscpStatus.h"
#include "DNCP/App/DscpSysDefine.h"

namespace Communication
{
namespace Dscp
{
    const Uint16 DscpStatus::OK = DSCP_OK;                              ///< 正常，操作成功。
    const Uint16 DscpStatus::Idle = DSCP_IDLE;                          ///< 系统状态：空闲。
    const Uint16 DscpStatus::Busy = DSCP_BUSY;                          ///< 系统状态：忙碌。
    const Uint16 DscpStatus::Error = DSCP_ERROR;                        ///< 出错。
    const Uint16 DscpStatus::ErrorParam = DSCP_ERROR_PARAM;             ///< 参数错误，传入的参数有问题。
    const Uint16 DscpStatus::Timeout = DSCP_TIMEOUT;                    ///< 超时。
    const Uint16 DscpStatus::NotSupported = DSCP_NOT_SUPPORTED;         ///< 该命令不被系统支持
    const Uint16 DscpStatus::DscpInternalError = DSCP_INTERNAL_ERROR;

    String DscpStatus::ToString()
    {
        String str;
        Uint16 status = *((Uint16*)(this->data));

        switch (status)
        {
        case OK:
            str = "Status(OK)";
            break;

        case Error:
            str = "Status(Error)";
            break;

        case ErrorParam:
            str = "Status(ErrorParam)";
            break;

        case Timeout:
            str = "Status(Timeout)";
            break;

        case NotSupported:
            str = "Status(NotSupported)";
            break;

        default:
            str= String("Status(") + std::to_string((short)status) + String(")");
            break;
        }

        return str;
    }
}
    ///< DSCP 内部错误，协议栈出错。
}
