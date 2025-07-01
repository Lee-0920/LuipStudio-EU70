/**
 * @file
 * @brief DSCP 命令包。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLNET_COMMUNICATION_DSCP_DSCPCOMMAND_H_)
#define CONTROLNET_COMMUNICATION_DSCP_DSCPCOMMAND_H_

#include <memory>
#include "../LuipShare.h"
#include "Log.h"
#include "DscpPacket.h"

namespace Communication
{
namespace Dscp
{
    
/**
 * @brief DSCP 命令包。
 */
class LUIP_SHARE DscpCommand : public DscpPacket
{
public:
    DscpCommand() {}
    DscpCommand(DscpAddress addr, Uint16 code)
        : DscpPacket(addr, code)
    {
    }

    DscpCommand(DscpAddress addr, Uint16 code, const void* data, Uint16 len)
        : DscpPacket(addr, code, data, len)
    {
    }

    DscpCommand(const DscpCommand& pack)
        : DscpPacket(pack)
    {
    }

    DscpCommand(const DscpPacket& pack)
        : DscpPacket(pack)
    {
    }

    virtual String ToString()
    {
        return String("Cmd") + DscpPacket::ToString();
    }
};

/**
 * @brief DSCP回应包引用对象。使用引用计数器自动管理数据包对象的内存。
 */
typedef std::shared_ptr<DscpCommand> DscpCmdPtr;

}
}

#endif  //CONTROLNET_COMMUNICATION_DSCP_DSCPCOMMAND_H_
