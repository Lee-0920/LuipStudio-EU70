/**
 * @file
 * @brief 异步调度器。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(COMMUNICATION_ASYNCSCHEDULER_H)
#define COMMUNICATION_ASYNCSCHEDULER_H

#include <condition_variable>
#include <mutex>
#include "Stack/IDscpReceivable.h"
#include "Stack/DncpStack.h"
#include "CallingQueue.h"
#include "AsyncTask.h"
#include "LuipShare.h"

using Communication::Stack::IDscpReceivable;
using Communication::Stack::DncpStack;
using Communication::CallingQueue;

namespace Communication
{

/**
 * @brief 异步调度器。Singleton 类。
 * @details 需要实现如下机制。
 *  - 流量控制：在把异步任务解析成一条条命令下发时，需要做简单的流量控制，确保下发的一批命令不会因为下位机内存有限造成丢失。可以通过定时器，每10毫秒发N条命令。命令从调用队列中依次取出。
 *  - 超时检测与处理：开启一个定时器，每10毫秒查询一次所有已发出但未回应的异步调用，如果发现某一调用超时，则根据该调用的超时尝试次数进行再次重发尝试，如不需要重发则直接抛出超时异常。
 */
class LUIP_SHARE AsyncScheduler : public IDscpReceivable
{
public:
    AsyncScheduler();
    ~AsyncScheduler();

public:
    static AsyncScheduler* Instance();

    void Execute(AsyncTaskPtr task);
    void Cancel(AsyncTaskPtr task);
    void Clear();
    long GetTimeout();
    void SetTimeout(int ms);
    int GetFlowRate();
    void SetFlowRate(int pps);
    void Start();
    void Stop();
    void OnReceive(DscpPackPtr pack);
    void CheckRetry();

private:
    static  std::unique_ptr<AsyncScheduler> m_instance;
    static void SendingThreadRun(AsyncScheduler* asyncScheduler);
    static void ReceivingThreadRun(AsyncScheduler *asyncScheduler);

private:

    // 发送线程执行的标识
    bool m_isSendingThreadRunning;
    // 接收线程执行的标识
    bool m_isReceivingThreadRunning;
    // 检测超时线程执行的标识
    bool m_isCheckTimeoutThreadRunning;
    // 异步任务列表。
    list<AsyncTaskPtr> m_tasks;
    // 底层DNCP协议栈列表。
    list<DscpRespPtr> m_receivingQueue;
    // 唤醒发送线程的标识
    std::mutex m_lockSend;
    // 唤醒接收线程的标识
    std::mutex m_lockReceiveQueue;
    // 唤醒检测超时线程得标识
    std::mutex m_lockCheckTimeout;
    // 保护tasks在并发状态的标识
    std::mutex m_lockTasks;
    std::mutex m_lockCallingQueue;
    std::condition_variable m_sendCondition;
    std::condition_variable m_receivingCondition;
    CallingQueue m_callingQueue;

    long m_timeOut;
    int m_flowRate;
};

}

#endif  //COMMUNICATION_ASYNCSCHEDULER_H
