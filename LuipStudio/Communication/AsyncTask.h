/**
 * @file
 * @brief 异步任务。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(COMMUNICATION_ASYNCTASK_H)
#define COMMUNICATION_ASYNCTASK_H

#include <list>
#include "IAsyncCallable.h"
#include "AsyncCall.h"
#include "LuipShare.h"

using std::list;

namespace Communication
{

/**
 * @brief 异步任务。
 * @details 由一个或多个异步调用（命令）组成。
 */
class LUIP_SHARE AsyncTask
{
public:
    AsyncTask() {}
    ~AsyncTask() {}
public:
    AsyncTask(IAsyncCallable* notify);
    AsyncTask(IAsyncCallable* notify, int retries);
    list<AsyncCallPtr> GetCalls();
    void AddCall(AsyncCallPtr call);
    void RemoveCall(AsyncCallPtr call);
    void Clear();
    void SetRetries(int retries);
    int GetRetries(); 
    bool Match(DscpRespPtr resp);
    list<AsyncCallPtr> CheckTimeoutAndRetry(long timeOut);
    bool IsFinished();
    void Deregister();

private:
    // 异步调用列表。
    list<AsyncCallPtr> m_calls;
    // 上层异步调用通知器。
    IAsyncCallable* m_notify;
    // 超时/错误重试次数。
    int m_retries;
};

typedef std::shared_ptr<AsyncTask> AsyncTaskPtr;

}

#endif  //CONTROLNET_COMMUNICATION_ASYNCTASK_H
