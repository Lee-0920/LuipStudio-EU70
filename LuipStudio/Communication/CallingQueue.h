/**
 * @file
 * @brief 命令调用队列。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(COMMUNICATION_CALLINGQUEUE_H)
#define COMMUNICATION_CALLINGQUEUE_H

#include <list>
#include "AsyncCall.h"

using std::list;


namespace Communication
{

/**
 * @brief 命令调用队列，优先队列，高优先级的先发送。
 * @details 本队列仅保存待发送的调用。已发送的调用将被清除。
 */
class CallingQueue
{
public:

    void Push(AsyncCallPtr call);
    AsyncCallPtr Peek();
    AsyncCallPtr Pop();
    bool IsEmpty();
    int size();
    bool Cancel(AsyncCallPtr call);
    void Clear();
    int compare(AsyncCallPtr call1, AsyncCallPtr call2);

private:
    list<AsyncCallPtr> m_calls;
};

}

#endif  //CONTROLNET_COMMUNICATION_CALLINGQUEUE_H
