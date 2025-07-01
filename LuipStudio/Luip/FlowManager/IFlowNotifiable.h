#ifndef MEASURE_FLOW_IFLOWNOTIFIABLE_H_
#define MEASURE_FLOW_IFLOWNOTIFIABLE_H_

#include "System/Types.h"

using System::String;

namespace Flow
{

/**
 * @brief 流程管理器启动流程上报接口。
 * @details 观察者。
 */
class IFlowNotifiable
{

public:
    IFlowNotifiable() {}
    virtual ~IFlowNotifiable() {}

public:
    virtual void OnFlowActivated(String name, long time) = 0;
    virtual void OnFlowFinished(String name, String result) = 0;
    virtual void OnFlowProcessMessage(String name, String mag)
    {
        (void) name;
        (void) mag;
    }
    virtual void OnFlowThreadExit() = 0;
};

}
#endif /* MEASURE_FLOW_IFLOWNOTIFIABLE_H_ */
