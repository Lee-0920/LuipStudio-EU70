/**
 * @file
 * @brief 命令调用队列。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#include "CallingQueue.h"
#include "AsyncCall.h"

namespace Communication
{

/**
 * @brief 压入一个新的调用请求到队列中。
 * @details 新的调用对象将根据其优先级自动排序。
 * @param[in] call 异步调用对象
 *
 */
void CallingQueue::Push(AsyncCallPtr call)
{
    if(!m_calls.empty())
    {
        for(std::list<AsyncCallPtr>::iterator it = m_calls.begin(); it != m_calls.end(); it++)
        {
            if((*it)->GetPriority() > call->GetPriority())
            {
                m_calls.insert(it, 1, call);
                break;
            }
            else if((*it)->GetPriority() == call->GetPriority())
            {
                it++;
                if(it == m_calls.end())
                {
                   m_calls.push_back(call);
                   break;
                }

                it--;
            }
        }
    }
    else
    {
        m_calls.push_back(call);
    }
}

/**
 * @brief 取出队列中的第一个异步调用对象。
 * @details 取出第一个对象，但并不弹出，该对象依旧在队列中。
 * @param[in]
 * @return 异步调用对象指针
 *
 */
AsyncCallPtr CallingQueue::Peek()
{  
    return m_calls.front();
}

/**
 * @brief 从队列中弹出一个异步调用对象。
 * @details 该调用拥有最高优先级。
 * @param[in]
 * @return 异步调用对象指针
 *
 */
AsyncCallPtr CallingQueue::Pop()
{
    AsyncCallPtr call = m_calls.front();
    m_calls.pop_front();
    return call;
}

/**
 * @brief 查询调用队列是否为空。
 * @details
 * @param[in]
 * @return
 *
 */
bool CallingQueue::IsEmpty()
{
    return m_calls.empty();
}

/**
 * @brief 查询调用队列元素数量。
 * @details
 * @param[in]
 * @return 元素数量
 *
 */
int CallingQueue::size()
{
    return m_calls.size();
}

/**
 * @brief 取消指定的调用对象。
 * @details 该对象将从队列中移除。
 * @param[in]
 * @return 如果取消成功，不存在将返回假。
 *
 */
bool CallingQueue::Cancel(AsyncCallPtr call)
{
    for(std::list<AsyncCallPtr>::iterator it = m_calls.begin(); it != m_calls.end(); it++)
    {
        if(*it == call)
        {
            m_calls.erase(it);

            return true;
        }
    }
    return false;
}

/**
 * @brief 清除队列中所有对象。
 * @details
 * @param[in]
 * @return
 *
 */
void CallingQueue::Clear()
{
    m_calls.clear();
}

int CallingQueue::compare(AsyncCallPtr call1, AsyncCallPtr call2)
{
        return call1->GetPriority() - call2->GetPriority();
}

}
