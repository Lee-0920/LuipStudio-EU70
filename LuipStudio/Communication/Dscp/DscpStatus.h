/**
 * @file
 * @brief DSCP 状态包。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(COMMUNICATION_DSCP_DSCPSTATUS_H)
#define COMMUNICATION_DSCP_DSCPSTATUS_H

#include "DscpPacket.h"
#include "../LuipShare.h"

namespace Communication
{
namespace Dscp
{

/**
* @brief DSCP 状态包。
*/
class LUIP_SHARE DscpStatus : public DscpPacket
{
public:
    static const Uint16 OK;                           ///< 正常，操作成功。
    static const Uint16 Idle;                         ///< 系统状态：空闲。
    static const Uint16 Busy;                         ///< 系统状态：忙碌。
    static const Uint16 Error;                     ///< 出错。
    static const Uint16 ErrorParam;          ///< 参数错误，传入的参数有问题。
    static const Uint16 Timeout;                 ///< 超时。
    static const Uint16 NotSupported;      ///< 该命令不被系统支持
    static const Uint16 DscpInternalError;///< DSCP 内部错误，协议栈出错。

    DscpStatus()
    {
    }

    DscpStatus(DscpAddress addr, Uint16 code)
        : DscpPacket(addr, code)
    {
    }

    DscpStatus(DscpAddress addr, Uint16 code, void* data, Uint16 len)
        : DscpPacket(addr, code, data, len)
    {
    }

    DscpStatus(DscpAddress addr, Uint16 code, Uint16 status)
    {
        this->addr = addr;
        this->code = code;
        this->len = sizeof(Uint16);
        this->data = new Byte[this->len];
        *((Uint16*)(this->data)) = status;
    }

    DscpStatus(const DscpStatus& pack)
        : DscpPacket(pack)
    {
    }

    DscpStatus(const DscpPacket& pack)
        : DscpPacket(pack)
    {
    }

    Uint16 GetStatus()
    {
        return *((Uint16*)(this->data));
    }

    void SetStatus(Uint16 statusCode)
    {
        *((Uint16*)(this->data)) = statusCode;
    }

    virtual String ToString();
};

}
}

#endif  //CONTROLNET_COMMUNICATION_DSCP_DSCPSTATUS_H
