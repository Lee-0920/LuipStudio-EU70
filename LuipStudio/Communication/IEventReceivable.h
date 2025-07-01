/**
 * @file
 * @brief 事件接收接口。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#ifndef COMMUNICATION_EVENT_RECEIVABLE_H_
#define COMMUNICATION_EVENT_RECEIVABLE_H_
#include "Log.h"
#include "LuipShare.h"
#include "Dscp/DscpEvent.h"

using Communication::Dscp::DscpEventPtr;

namespace Communication
{

/**
 * @brief 事件接收接口。
 * @details 观察者。
 */
class LUIP_SHARE IEventReceivable
{
public:
    IEventReceivable() {}
    virtual ~IEventReceivable() {}

public:
    virtual void OnReceive(DscpEventPtr event) = 0;
};

}

#endif  //CONTROLNET_COMMUNICATION_EVENT_RECEIVABLE_H_
