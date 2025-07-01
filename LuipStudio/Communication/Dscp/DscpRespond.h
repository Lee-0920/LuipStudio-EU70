/**
 * @file
 * @brief DSCP 回应包。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#if !defined(COMMUNICATION_DSCP_DSCPRESPOND_H)
#define COMMUNICATION_DSCP_DSCPRESPOND_H

#include <memory>
#include "../LuipShare.h"
#include "DscpPacket.h"

namespace Communication
{
namespace Dscp
{

/**
* @brief DSCP 回应包。
*/
class LUIP_SHARE DscpRespond : public DscpPacket
{
public:
    DscpRespond()
    {
    }

    DscpRespond(DscpAddress addr, Uint16 code)
        : DscpPacket(addr, code)
    {
    }

    DscpRespond(DscpAddress addr, Uint16 code, void* data, Uint16 len)
        : DscpPacket(addr, code, data, len)
    {
    }

    DscpRespond(const DscpRespond& pack)
        : DscpPacket(pack)
    {
    }

    DscpRespond(const DscpPacket& pack)
        : DscpPacket(pack)
    {
    }

    virtual String ToString()
    {
        return String("Resp") + DscpPacket::ToString();
    }
};

/**
 * @brief DSCP回应包引用对象。使用引用计数器自动管理数据包对象的内存。
 */
typedef std::shared_ptr<DscpRespond> DscpRespPtr;

}
}

#endif  //CONTROLNET_COMMUNICATION_DSCP_DSCPRESPOND_H
