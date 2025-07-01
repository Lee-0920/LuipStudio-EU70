/**
 * @file
 * @brief 状态上报接口。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */
#if !defined(STATUSMANAGER_ISTATUSNOTIFIABLE_H_)
#define STATUSMANAGER_ISTATUSNOTIFIABLE_H_

#include "RunStatus.h"
#include "RunAction.h"

namespace Measure
{

enum class BaseLineStatus
{
    Ok = 0,
    Failed = 1,
    Stop = 2,
};

/**
 * @brief 状态上报接口。
 * @details 观察者。
 */
class IStatusNotifiable
{

public:
    IStatusNotifiable() {}
    virtual ~IStatusNotifiable() {}

public:
    virtual void OnStattusChanged(RunStatus status) = 0;
    virtual void OnStattusDelay(int time) = 0;
    virtual void OnActionChanged(RunAction action) = 0;
    virtual void OnActionDelay(int time) = 0;    
};


}

#endif  //STATUSMANAGER_ISTATUSNOTIFIABLE_H_
