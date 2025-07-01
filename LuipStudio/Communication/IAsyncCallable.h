/**
 * @file
 * @brief 异步调用回调接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#if !defined(COMMUNICATION_IASYNCCALLABLE_H)
#define COMMUNICATION_IASYNCCALLABLE_H

#include "LuipShare.h"

namespace Communication
{

enum class CallStatus;
class AsyncTask;
class AsyncCall;

/**
 * @brief 异步调用回调接口。
 * @details 
 */
class LUIP_SHARE IAsyncCallable
{
public:
    IAsyncCallable() {}
    ~IAsyncCallable() {}
public:
    // 任务开始时响应。
    virtual void OnStart(AsyncTask* task) = 0;
    // 任务中有回应到达时响应。
    virtual bool OnRespond(AsyncTask* task, AsyncCall* call, CallStatus status) = 0;
    // 任务结束（所有异步调用均已完成）时响应。
    virtual void OnStop(AsyncTask* task, CallStatus status) = 0;
};

/**
 * @brief 调用状态。
 * @details 
 */
enum class CallStatus
{
    OK,
    Error,
    Timeout,
    Cancelled
};

}

#endif  //CONTROLNET_COMMUNICATION_IASYNCCALLABLE_H
