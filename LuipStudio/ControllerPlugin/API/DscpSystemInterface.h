/**
 * @file
 * @brief Dscp 系统接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/10
 */


#if !defined(CONTROLLER_API_DSCPSYSTEMINTERFACE_H)
#define CONTROLLER_API_DSCPSYSTEMINTERFACE_H

#include "../LuipShare.h"
#include "Version.h"
#include "DeviceInterface.h"

using System::Byte;

namespace Controller
{
namespace API
{

/**
 * @brief Dscp 系统接口。
 * @details 提供 DSCP 系统命令的调用操作。
 */
class LUIP_SHARE DscpSystemInterface : public DeviceInterface
{
public:
    DscpSystemInterface(DscpAddress addr);

    bool Echo(int len = 0, Byte* data = nullptr);
    Version GetVersion();
    bool AcquireExpectEvent();
};

}
}

#endif  //CONTROLLER_API_DSCPSYSTEMINTERFACE_H
