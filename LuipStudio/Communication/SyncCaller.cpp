/**
 * @file
 * @brief 同步调用器，封装了一些常用的同步命令调用操作。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "Log.h"
#include "SyncCaller.h"
#include "AsyncTask.h"
#include "AsyncCall.h"
#include "AsyncScheduler.h"
#include "CommunicationException.h"
#include "System/Exception.h"
#include "Dscp/DscpStatus.h"
#include <QDateTime>
#include <QDebug>


using namespace std;
using namespace Communication::Dscp;
using namespace System;

namespace Communication
{

/**
 * @brief  构造同步调用器。
 * @details 默认超时时间为3000毫秒。
 *
 */

SyncCaller::SyncCaller()
{
    this->m_retries = 3;
    this->m_respond = nullptr;
    this->m_respondStatus = CallStatus::OK;
}

/**
 * @brief  构造同步调用器。
 * @details 使用指定的超时时间。
 * @param[in] timeout 超时毫秒数。
 *
 */
SyncCaller::SyncCaller(int retries)
{
    this->m_retries = retries;
    this->m_respond = nullptr;
    this->m_respondStatus = CallStatus::OK;
}

SyncCaller::~SyncCaller()
{

}

/**
 * @brief 查询同步调用的重传次数。
 * @return 重传次数。
 *
 */
int SyncCaller::GetRetries()
{
    return m_retries;
}

/**
 * @brief  设置同步调用的重传次数。
 * @param[in] retries 重传次数。
 *
 */
void SyncCaller::SetRetries(int retries)
{
    this->m_retries = retries;
}

/**
 * @brief 发送命令，并等待回应包。
 * @param[in] cmd Dscp命令。
 * @return 回应包
 *
 */
DscpRespPtr SyncCaller:: Send(DscpCmdPtr cmd)
{
    DscpRespPtr resp = nullptr;

    long timeout = this->sum(m_retries + 1) * AsyncScheduler::Instance()->GetTimeout() + 500;

    AsyncTaskPtr task(new AsyncTask(this, m_retries));
    task->AddCall(AsyncCallPtr(new AsyncCall(cmd)));

    m_mutex.lock();

    AsyncScheduler::Instance()->Execute(task);

    long t1 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if(m_taskWaitCondition.wait(&m_mutex,timeout) == true)
    {
        if (m_respondStatus == CallStatus::OK)
        {
            resp = m_respond;
            m_respond.reset();
            m_mutex.unlock();
        }
        else if (m_respondStatus == CallStatus::Timeout)
        {
            logger->debug("SyncCaller Respond Timeout {addr = %s, code = %X}",
                          cmd->addr.ToString().c_str(),
                          cmd->code);

          m_mutex.unlock();
          throw CommandTimeoutException(cmd->addr,cmd->code);
        }
    }
    else
    {
        long to= QDateTime::currentDateTime().toMSecsSinceEpoch() - t1;

        logger->debug("SyncCaller Wait Timeou {addr = %s, code = %X, TO = %ld ms}",
                      cmd->addr.ToString().c_str(),
                      cmd->code,
                      to);

        task->Deregister();
        m_mutex.unlock();
        throw CommandTimeoutException(cmd->addr,cmd->code);
    }

    return resp;
}

/**
 * @brief  发送命令后立即返回。
 * @details 通常用于无回应的命令。
 * @param[in] cmd Dscp命令。
 *
 */
void SyncCaller::SendWithoutRespond(DscpCmdPtr cmd)
{
    AsyncTaskPtr task(new AsyncTask(this, 0));
    task->AddCall(AsyncCallPtr(new AsyncCall(cmd)));
    try
    {
        AsyncScheduler::Instance()->Execute(task);
        AsyncScheduler::Instance()->Cancel(task);
    }
    catch(CommunicationException e)
    {

    }
    catch (System::Exception e)
    {
    }
    catch (std::exception e)
    {
    }
}

/**
 * @brief 发送命令，并等待状态回应。
 * @return  状态回应，请参考具体命令的定义。可能的状态有：
 *  - DscpStatus::DscpInternalError  DSCP 内部错误，协议栈出错；
 *  - DscpStatus::Timeout 命令超时未返回；
 *  - DscpStatus::OK
 *  - DscpStatus::Error
 *  - DscpStatus::ErrorParam
 *  - DscpStatus::NotSupported
 *  - 其它：不同命令定义的其它返回状态。
 */
int SyncCaller::SendWithStatus(DscpCmdPtr cmd)
{
    Uint16 status = DscpStatus::Timeout;
    long timeout = this->sum(m_retries + 1) * AsyncScheduler::Instance()->GetTimeout() + 500;

    AsyncTaskPtr task(new AsyncTask(this, m_retries));
    task->AddCall(AsyncCallPtr(new AsyncCall(cmd)));

    m_mutex.lock();

    AsyncScheduler::Instance()->Execute(task);

    long t1 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if(m_taskWaitCondition.wait(&m_mutex,timeout))
    {
        if (m_respondStatus == CallStatus::OK)
        {
            status = ((DscpStatus*)m_respond.get())->GetStatus();
            m_mutex.unlock();
        }
        else if (m_respondStatus == CallStatus::Timeout)
        {
            logger->debug("SyncCaller Respond Timeout {addr = %s, code = %X}",
                          cmd->addr.ToString().c_str(),
                          cmd->code);

          m_mutex.unlock();
          throw CommandTimeoutException(cmd->addr,cmd->code);
        }
    }
    else
    {
        long to= QDateTime::currentDateTime().toMSecsSinceEpoch() - t1;

        logger->debug("SyncCaller Wait Timeou {addr = %s, code = %X, TO = %ld ms}",
                      cmd->addr.ToString().c_str(),
                      cmd->code,
                      to);

        task->Deregister();
        m_mutex.unlock();
        throw CommandTimeoutException(cmd->addr,cmd->code);
    }

    return status;
}

/**
 * @brief 任务开始时响应。
 *
 */
void SyncCaller::OnStart(AsyncTask *task)
{
    (void)task;
}


/**
 * @brief 任务中有回应到达时响应。
 * @param[in] cmd Dscp命令。
 * @return 回应包
 *
 */
bool SyncCaller::OnRespond(AsyncTask *task, AsyncCall *call, CallStatus status)
{
    (void)task;
    m_mutex.lock();

    if (status == CallStatus::OK)
    {
        m_respondStatus = CallStatus::OK;
        this->m_respond = call->GetRespond();
        m_taskWaitCondition.wakeAll();
    }
    else if (status == CallStatus::Timeout)
    {
        m_respondStatus = CallStatus::Timeout;
        m_taskWaitCondition.wakeAll();
    }
    else
    {
        logger->debug("no OnRespond Status type");
    }

    m_mutex.unlock();

    return true;
}

/**
 * @brief 任务结束（所有异步调用均已完成）时响应。
 *
 */
void SyncCaller::OnStop(AsyncTask *task, CallStatus status)
{
     (void)task;
     (void)status;
}

int SyncCaller::sum(int n)
{
    int sum = 0;

    for(int i = 0; i <= n; i++)
    {
        sum += i;
    }

    return sum;
}

}
