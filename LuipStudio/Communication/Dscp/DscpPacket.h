/**
 * @file
 * @brief DSCP 数据包
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#if !defined(COMMUNICATION_DSCP_DSCPPACKET_H)
#define COMMUNICATION_DSCP_DSCPPACKET_H

#include <memory>
#include <QString>
#include "../LuipShare.h"
#include "DscpAddress.h"

using System::Byte;
using System::Uint16;
using System::String;

namespace Communication
{
namespace Dscp
{

/**
 * @brief DSCP 数据包
 */
class LUIP_SHARE DscpPacket
{
public:
    DscpAddress addr;
    Uint16 code;
    Byte* data;
    Uint16 len;

public:
    DscpPacket()
    {
        this->code = 0;
        this->data = nullptr;
        this->len = 0;
    }

    DscpPacket(DscpAddress addr, Uint16 code)
    {
        this->addr = addr;
        this->code = code;
        this->data = nullptr;
        this->len = 0;
    }

    DscpPacket(DscpAddress addr, Uint16 code, const void* data, Uint16 len)
    {
        this->addr = addr;
        this->code = code;
        if (len > 0)
        {
            this->data = new Byte[len];
            memcpy(this->data, data, len);
            this->len = len;
        }
        else
        {
            this->data = nullptr;
            this->len = 0;
        }
    }

    DscpPacket(const DscpPacket& pack)
    {
        this->addr = pack.addr;
        this->code = pack.code;
        this->len = pack.len;
        this->data = nullptr;

        if (pack.len > 0)
        {
            this->data = new Byte[pack.len];
            memcpy(this->data, pack.data, pack.len);
        }
    }

    ~DscpPacket()
    {
        if (this->data)
        {
            delete[] this->data;
            this->data = nullptr;
            this->len = 0;
        }
    }

    virtual String ToString()
    {  
       QString str = QString("{0x%1} A[%2] L(%3)").arg(this->code, 0, 16).arg(this->addr.ToString().c_str()).arg(this->len);
       return str.toStdString();
//        return System::Format("{0x%X} A[%s] L(%d)",
//                              this->code,
//                              this->addr.ToString().c_str(),
//                              this->len);
    }
};

/**
 * @brief DSCP数据包引用对象。使用引用计数器自动管理数据包对象的内存。
 */
typedef std::shared_ptr<DscpPacket> DscpPackPtr;

/**
 * @brief DSCP 数据包类型。
 */
enum class DscpType
{
    Cmd,
    RespInfo,
    RespStatus,
    Event
};

/**
 * @brief DSCP 系统命令定义。
 */
class DscpSysCmd
{
public:
    // 回声测试。
    static const Uint16 ECHO;
    // 获取接口版本。
    static const Uint16 GET_IFVER;
};


}
}

#endif  //CONTROLNET_COMMUNICATION_DSCP_DSCPPACKET_H
