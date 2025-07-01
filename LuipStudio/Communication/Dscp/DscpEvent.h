/**
 * @file
 * @brief DSCP 事件包。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(COMMUNICATION_DSCP_DSCPEVENT_H_)
#define COMMUNICATION_DSCP_DSCPEVENT_H_

#include "../LuipShare.h"
#include "DscpPacket.h"

namespace Communication
{
namespace Dscp
{
    
/**
 * @brief DSCP 事件包。
 */
class LUIP_SHARE DscpEvent : public DscpPacket
{
public:
    DscpEvent()
    {
    }

    DscpEvent(DscpAddress addr, Uint16 code)
        : DscpPacket(addr, code)
    {
    }

    DscpEvent(DscpAddress addr, Uint16 code, void* data, Uint16 len)
        : DscpPacket(addr, code, data, len)
    {
    }

    DscpEvent(const DscpEvent& pack)
        : DscpPacket(pack)
    {
    }

    DscpEvent(const DscpPacket& pack)
        : DscpPacket(pack)
    {
    }

    virtual String ToString()
    {
        return String("Event") + DscpPacket::ToString();
    }
};

/**
 * @brief DSCP事件包引用对象。使用引用计数器自动管理数据包对象的内存。
 */
typedef std::shared_ptr<DscpEvent> DscpEventPtr;

}
}

#endif  //CONTROLNET_COMMUNICATION_DSCP_DSCPEVENT_H_
