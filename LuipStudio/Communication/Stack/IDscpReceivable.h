/**
 * @file
 * @brief 
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#ifndef STACK_DSCP_RECEIVABLE_H_
#define STACK_DSCP_RECEIVABLE_H_

#include "../LuipShare.h"
#include "../Dscp/DscpPacket.h"

using Communication::Dscp::DscpPackPtr;

namespace Communication
{
namespace Stack
{

/**
 * @brief DSCP数据包接收接口。
 * @details 观察者。
 */
class LUIP_SHARE IDscpReceivable
{
public:
    IDscpReceivable() {}
    virtual ~IDscpReceivable() {}

public:
    virtual void OnReceive(DscpPackPtr pack) = 0;
};

}
}

#endif  //CONTROLNET_STACK_DSCP_RECEIVABLE_H_
