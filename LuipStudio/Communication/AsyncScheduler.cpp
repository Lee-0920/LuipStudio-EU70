/**
 * @file
 * @brief 异步调度器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include <thread>
#include <QDebug>
#include <QDateTime>
#include "Log.h"
#include "AsyncScheduler.h"
#include "AsyncTask.h"


using namespace std;
using namespace Communication::Stack;

//static long SC = 0;
//static long RC = 0;
//static long t1 = 0;
//static long t2 = 0;
//static long t3 = 0;
//static long t4 = 0;
//static long t5 = 0;
//static long t6 = 0;

namespace Communication
{

unique_ptr<AsyncScheduler> AsyncScheduler::m_instance(nullptr);

AsyncScheduler::AsyncScheduler()
{
    m_timeOut = 500;
    m_isSendingThreadRunning  = false;
    m_isReceivingThreadRunning  = false;
    m_isCheckTimeoutThreadRunning  = false;
    m_flowRate = 0;
}

AsyncScheduler::~AsyncScheduler()
{

}

/**
 * @brief @brief 获取全局唯一的 AsyncScheduler 对象（单件）。
 * @details
 * @param[in]
 * @return  AsyncScheduler 对象。
 *
 */
AsyncScheduler* AsyncScheduler::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new AsyncScheduler);
    }

    return m_instance.get();
}

/**
 * @brief @brief 发送异步任务线程回调函数。
 * @details
 * @param[in]
 * @return
 *
 */
void AsyncScheduler::SendingThreadRun(AsyncScheduler* asyncScheduler)
{
    while (asyncScheduler->m_isSendingThreadRunning)
    {
        std::unique_lock<std::mutex> lockSend(asyncScheduler->m_lockSend);
        asyncScheduler->m_sendCondition.wait_for(lockSend, std::chrono::milliseconds(10));

        if (!asyncScheduler->m_callingQueue.IsEmpty())
        {
            std::unique_lock<std::mutex> lockCallingQueue(asyncScheduler->m_lockCallingQueue);

            unsigned long size = asyncScheduler->m_callingQueue.size();
            for (unsigned long i = 0; i < size; i++)
            {
                AsyncCallPtr call = asyncScheduler->m_callingQueue.Peek();
                DscpCmdPtr dscpCommand = call->GetDscpCmd();

                bool status = DncpStack::Instance()->Send(dscpCommand->addr, dscpCommand->code,
                                                          dscpCommand->data, dscpCommand->len);
                if (status)
                {
                    call->SetSendout(true);// 标记发送命令成功
                    call->UpdateTimestamp();
                    asyncScheduler->m_callingQueue.Pop();// 从命令调用队列中去除此call

//                    SC++;
//                    logger->debug("send：addr = %s, code = %X, SC = %d, RC = %d, d = %d, m_tasks = %d",
//                                 dscpCommand->addr.ToString().c_str(),
//                                 dscpCommand->code,
//                                 SC,
//                                 RC,
//                                 SC-RC,
//                                 asyncScheduler->m_tasks.size());
                }
            }

            lockCallingQueue.unlock();
        }
    }

   asyncScheduler->m_isSendingThreadRunning = TRUE;
}

/**
 * @brief @brief 接收回应包线程回调函数。
 * @details
 * @param[in]
 * @return
 *
 */
void AsyncScheduler::ReceivingThreadRun(AsyncScheduler* asyncScheduler)
{
    bool isMatch = false;

    while (asyncScheduler->m_isReceivingThreadRunning)
    {
//        logger->debug("------------ lock Receive Queue ------------");

//        t1 = QDateTime::currentDateTime().toMSecsSinceEpoch();

        // 等待回应包接收唤醒，否则10毫秒超时自动唤醒
        std::unique_lock<std::mutex> lockRQ(asyncScheduler->m_lockReceiveQueue);

        if (asyncScheduler->m_receivingQueue.empty())
        {
            asyncScheduler->m_receivingCondition.wait_for(lockRQ,std::chrono::milliseconds(10));
        }

//        t2 = QDateTime::currentDateTime().toMSecsSinceEpoch();
//        logger->debug("T1 -> T2 = %d ms", t2 - t1);

        std::unique_lock<std::mutex> lockTasks(asyncScheduler->m_lockTasks);

        // 检查接收队列是否接收到回应包，有回应包则进行处理
        if (!asyncScheduler->m_receivingQueue.empty())
        {
//            t3 = QDateTime::currentDateTime().toMSecsSinceEpoch();

//            logger->debug("m_receivingQueue =  %d, m_tasks = %d",
//                          asyncScheduler->m_receivingQueue.size(),
//                          asyncScheduler->m_tasks.size());

            if(!asyncScheduler->m_tasks.empty())
            {
                unsigned int size = asyncScheduler->m_receivingQueue.size();

                for (unsigned int i =0; i < size; i++)
                {
                    isMatch = false;

                    DscpRespPtr dscpPack = asyncScheduler->m_receivingQueue.front();

//                    logger->debug("Process：addr = %s, code = %X",
//                                   dscpPack->addr.ToString().c_str())
//                                   dscpPack->code);

                    for (std::list<AsyncTaskPtr>::iterator it = asyncScheduler->m_tasks.begin();
                         it != asyncScheduler->m_tasks.end(); it++)
                    {
                        if ((*it)->Match(dscpPack))
                        {
                            isMatch = true;
                            asyncScheduler->m_receivingQueue.pop_front();

//                          logger->debug("Match：addr = %s, code = %X",
//                                          dscpPack->addr.ToString().c_str())
//                                          dscpPack->code);

                            if ((*it)->IsFinished())
                            {
                                asyncScheduler->m_tasks.erase(it);
                            }
                            break;
                        }
                    }

                    if(isMatch == false)    // 超时重传后，原命令已经应答同时删除task后，重传命令再应答找不到匹配task，删除应答包
                    {
                        asyncScheduler->m_receivingQueue.pop_front();
                        logger->debug("not match task");
                    }
                }
            }
            else
            {
               asyncScheduler->m_receivingQueue.clear();
            }

            lockRQ.unlock();

//            t4 = QDateTime::currentDateTime().toMSecsSinceEpoch();
//            logger->debug("T3 -> T4 = %d ms", t4 - t3);
        }

        // 应答超时重传检查，如有超时将命令重新加入发送队列
//        t5 = QDateTime::currentDateTime().toMSecsSinceEpoch();

        asyncScheduler->CheckRetry();

        lockTasks.unlock();

//        t6 = QDateTime::currentDateTime().toMSecsSinceEpoch();
//        logger->debug("************** T5 -> T6 = %d ms **************", t6 - t5);
    }

    asyncScheduler->m_isReceivingThreadRunning = TRUE;
}

/**
 * @brief @brief 执行指定的异步任务。
 * @details 实现提示：
 * @details  1. 需要刷新任务中所有调用的重试次数，非0忽略。
 * @details  2. 需要把任务中所有调用移到调用队列中。
 * @details  3. 唤醒发送线程，开始发送工作。
 * @param[in] AsyncTask 对象。
 * @return
 *
 */
void AsyncScheduler::Execute(AsyncTaskPtr task)
{
//    logger->debug("m_tasks lock");

    std::unique_lock<std::mutex> lockTasks(m_lockTasks);
    m_tasks.push_back(task);
    lockTasks.unlock();

//    logger->debug("m_tasks unlock");

    list<AsyncCallPtr> calls = task->GetCalls();

//    logger->debug("m_callingQueue lock");

    std::unique_lock<std::mutex> lockCallingQueue(m_lockCallingQueue);

    for(std::list<AsyncCallPtr>::iterator it = calls.begin(); it != calls.end(); it++)
    {
       (*it)->SetRetries(task->GetRetries());
        m_callingQueue.Push(*it);
    }

    lockCallingQueue.unlock();

// logger->debug("m_callingQueue unlock");

    m_sendCondition.notify_one();
}

/**
 * @brief 取消指定的异步任务。
 * @details
 * @param[in] AsyncTask 对象
 * @return
 *
 */
void AsyncScheduler::Cancel(AsyncTaskPtr task)
{
    std::unique_lock<std::mutex> lock(m_lockTasks);

    std::list<AsyncTaskPtr>::iterator it;
    for(it = m_tasks.begin(); it != m_tasks.end(); it++)
    {
        if(*it == task)
        {
            m_tasks.erase(it);
            break;
        }
    }

    lock.unlock();
}

/**
 * @brief 清空所有异步任务。
 * @details
 * @param[in]
 * @return
 *
 */
void AsyncScheduler::Clear()
{
    std::unique_lock<std::mutex> lock(m_lockTasks);

    m_tasks.clear();

    lock.unlock();
}

/**
 * @brief 查询自动超时检测机制的超时时间。
 * @details
 * @param[in]
 * @return 超时时间
 *
 */
long AsyncScheduler::GetTimeout()
{
    return m_timeOut;
}

/**
 * @brief 设置自动超时检测机制的超时时间。
 * @details 不同的设备，有不同的通信效率，超时时间也不一样。
 * @param[in] ms 超时时间
 * @return
 *
 */
void AsyncScheduler::SetTimeout(int ms)
{
    this->m_timeOut = ms;
}

/**
 * @brief  查询系统允许的最大流速。
 * @details
 * @param[in]
 * @return 最大流速值。
 *
 */
int AsyncScheduler::GetFlowRate()
{
    return m_flowRate;
}

/**
 * @brief  设置系统允许的最大流速。
 * @details 不同的设备，有不同的通信效率，最大发送速率也不一样。
 * @param[in] pps最大流速值
 * @return
 *
 */
void AsyncScheduler::SetFlowRate(int pps)
{
    this->m_flowRate = pps;
}

/**
 * @brief  启动异步调度器。
 * @details 实现提示：启动各常规线程、例行任务等。
 * @param[in]
 * @return
 *
 */
void AsyncScheduler::Start()
{
    this->m_isSendingThreadRunning = true;
    this->m_isReceivingThreadRunning = true;
    std::thread sendingThread(SendingThreadRun,this);
    std::thread receivingThread(ReceivingThreadRun,this);
    DncpStack::Instance()->SetRespondHandler(this);

    sendingThread.detach();
    receivingThread.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

/**
 * @brief  停止异步调度器。
 * @details
 * @param[in]
 * @return
 *
 */
void AsyncScheduler::Stop()
{
    int sleepTime = 10;
    int timeOut = 1000 / sleepTime;

    // 安全地终止通信线程
    m_isSendingThreadRunning = FALSE;
    m_isReceivingThreadRunning = FALSE;

    while (timeOut--)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

        // 线程退出后，会设置这个标志位
        if ((m_isSendingThreadRunning) && (m_isReceivingThreadRunning))
        {
            DncpStack::Instance()->SetRespondHandler(nullptr);
            m_callingQueue.Clear();
            m_receivingQueue.clear();

            break;
        }
    }
}

/**
 * @brief  停止异步调度器。
 * @details
 * @param[in]
 * @return
 *
 */
void AsyncScheduler::OnReceive(DscpPackPtr pack)
{
    std::unique_lock<std::mutex> lock(m_lockReceiveQueue);

    DscpRespPtr dscpResp = dynamic_pointer_cast<DscpRespond>(pack);
    m_receivingQueue.push_back(dscpResp);

//    RC++;
//      logger->debug("Recv：addr = %s, code = %X, SC = %d, RC = %d, d = %d",
//                      dscpResp->addr.ToString().c_str(),
//                      dscpResp->code,
//                      SC,
//                      RC,
//                      SC-RC);

    lock.unlock();

    m_receivingCondition.notify_one();
}

void AsyncScheduler::CheckRetry()
{
    bool isErase = false;

    if(!m_tasks.empty())
    {
//        std::unique_lock<std::mutex> tasksLock(m_lockTasks);
        for(std::list<AsyncTaskPtr>::iterator itTasks = m_tasks.begin(); itTasks != m_tasks.end();)
        {
            list<AsyncCallPtr> retryCalls = (*itTasks)->CheckTimeoutAndRetry(m_timeOut);

            if (!retryCalls.empty())
            {
                std::unique_lock<std::mutex> lockCallingQueue(m_lockCallingQueue);

                for(std::list<AsyncCallPtr>::iterator itRetry = retryCalls.begin();
                    itRetry != retryCalls.end(); itRetry++)
                {
                    m_callingQueue.Push(*itRetry);
                }

                lockCallingQueue.unlock();

                m_sendCondition.notify_one();
            }
            else
            {
                if ((*itTasks)->IsFinished())
                {
                    itTasks = m_tasks.erase(itTasks);
                    isErase = true;
                }
            }
            //判断是否有删除task，有删除则在删除时修改迭代器指针
            if (isErase == true)
            {
              isErase = false;
            }
            else
            {
                itTasks++;
            }
        }

//        tasksLock.unlock();
    }
}

}
