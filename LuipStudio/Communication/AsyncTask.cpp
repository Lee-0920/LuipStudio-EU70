/**
 * @file
 * @brief 
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include <QDateTime>
#include "Log.h"
#include "AsyncTask.h"
#include "IAsyncCallable.h"
#include "AsyncCall.h"


namespace Communication
{

/**
 * @brief 使用指定的异步通知接口构建一个 AsyncTask 对象。
 * @details
 * @param[in] notify 异步通知接口
 * @return
 *
 */
AsyncTask::AsyncTask(IAsyncCallable* notify)
{
    this->m_notify = notify;
    this->m_retries = 0;
}

/**
 * @brief 使用指定的异步通知接口和最大尝试次数构建一个 AsyncTask 对象。
 * @details
 * @param[in] notify 异步通知接口
 * @param[in] retries 尝试次数
 * @return
 *
 */
AsyncTask::AsyncTask(IAsyncCallable* notify, int retries)
{
    this->m_notify = notify;
    this->m_retries = retries;
}

/**
 * @brief 获取所有异步调用（命令）对象。
 * @details
 * @param[in]
 * @return 返回所有异步调用对象列表。
 *
 */
list<AsyncCallPtr> AsyncTask::GetCalls()
{
    return m_calls;
}

/**
 * @brief 添加一个异步调用对象到本异步任务中。
 * @details
 * @param[in] call 异步调用对象
 * @return
 *
 */
void AsyncTask::AddCall(AsyncCallPtr call)
{
    m_calls.push_back(call);
}

/**
 * @brief 从本异步任务中删除指定的异步调用对象。
 * @details
 * @param[in] call 指定删除的异步调用对象
 * @return
 *
 */
void AsyncTask::RemoveCall(AsyncCallPtr call)
{
    m_calls.remove(call);
}

/**
 * @brief 清空所有异步调用对象。
 * @details
 * @param[in]
 * @return
 *
 */
void AsyncTask::Clear()
{
    m_calls.clear();
}

/**
 * @brief 设置超时或错误重试的次数。
 * @details 默认为0，表示不重试。
 * @param[in] retries 重试的次数。
 * @return
 *
 */
void AsyncTask::SetRetries(int retries)
{
    this->m_retries = retries;
}

/**
 * @brief 获取超时或错误重试的次数。
 * @details
 * @param[in]
 * @return 返回重试的次数。
 *
 */
int AsyncTask::GetRetries()
{
    return m_retries;
}

/**
 * @brief 从异步调用列表中匹配回应包，有匹配则返回匹配的异步调用对象。
 * @details 实现时，如果给定的回应包匹配中某一调用，那么需要立即更新该调用的状态，并回调 AsyncCallable 接口，以通知上层及时处理该调用。
 * @param[in] resp 回应包。
 * @return 匹配是否成功。
 *
 */
bool AsyncTask::Match(DscpRespPtr resp)
{
    bool status = false;

    for(std::list<AsyncCallPtr>::iterator it = m_calls.begin(); it != m_calls.end(); it++)
    {
        if (!(*it)->IsResponded() && (*it)->IsMatch(resp))
        {
            status = true;
            (*it)->SetResponded(true);
            (*it)->SetRespond(resp);
            if (m_notify != NULL)
            {
                m_notify->OnRespond(this,(*it).get(),CallStatus::OK);
            }
            m_calls.erase(it);
            break;
        }
    }
    return status;
}

/**
 * @brief 异步调用（命令）应答超时重传检查。
 * @param[in] timeOut 应答超时的毫秒数
 * @return 异步调用（命令）应答超时的命令列表。
 *
 */
list<AsyncCallPtr> AsyncTask::CheckTimeoutAndRetry(long timeOut)
{
    list<AsyncCallPtr> retryCalls;
    bool isErase = false;

    if(!m_calls.empty())
    {
        for(std::list<AsyncCallPtr>::iterator it = m_calls.begin(); it != m_calls.end();)
        {
            if ((*it)->IsSendout() && !(*it)->IsResponded()) // 发送成功，而未收到回应
            {
                long ms = QDateTime::currentDateTime().toMSecsSinceEpoch();

                long TO = ((*it)->GetInitRetries() -(*it)->GetRetries() + 1) * timeOut;

                if ((ms - (*it)->GetTimestamp()) >= TO)
                {
                    (*it)->SetSendout(false);
                    if ((*it)->GetRetries() > 0) // 依据重发次数开始设置重发
                    {
                        (*it)->UpdateRetries();
                        retryCalls.push_back((*it));

                        logger->debug("AsyncTask Retry {addr = %s, code = %X, TO = %ld ms}",
                                      (*it)->GetDscpCmd()->addr.ToString().c_str(),
                                      (*it)->GetDscpCmd()->code,
                                      ms - (*it)->GetTimestamp());
                    }
                    else // 不需要重发则移除发送请求
                    {                        
                        logger->debug("AsyncTask Timeout {addr = %s, code = %X, TO = %ld ms}",
                                      (*it)->GetDscpCmd()->addr.ToString().c_str(),
                                      (*it)->GetDscpCmd()->code,
                                      ms - (*it)->GetTimestamp());

                        if (m_notify != NULL)
                        {
                            m_notify->OnRespond(this, (*it).get(), CallStatus::Timeout);
                        }
                        it = m_calls.erase(it);
                        isErase = true;
                    }
                }
            }

            //判断是否有删除AsyncCall，有删除则在删除时修改迭代器指针
            if (isErase == true)
            {
              isErase = false;
            }
            else
            {
                it++;
            }
        }
    }
    return retryCalls;
}

/**
 * @brief 从异步调用任务中异步调用命令是否为空。
 * @details 任务中异步调用命令为空，则任务完成
 * @return 任务是否完成。
 *
 */
bool AsyncTask::IsFinished()
{
    return m_calls.empty();
}

/**
 * @brief 注销掉异步调用任务中异步调用命令的应答通知。
 * @details 当调用异步调用发送命令的上层等待超时时或者取消接收回应包，则将应答通知器设置为空。
 *
 */
void AsyncTask::Deregister()
{
    m_notify = NULL;
}

}
