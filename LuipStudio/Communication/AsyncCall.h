/**
 * @file
 * @brief 异步调用（命令）。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(COMMUNICATION_ASYNCCALL_H)
#define COMMUNICATION_ASYNCCALL_H

#include "Dscp/DscpCommand.h"
#include "Dscp/DscpRespond.h"
#include "LuipShare.h"

using Communication::Dscp::DscpCommand;
using Communication::Dscp::DscpRespond;
using Communication::Dscp::DscpCmdPtr;
using Communication::Dscp::DscpRespPtr;

namespace Communication
{

/**
 * @brief 异步调用（命令）。
 * @details 封装了实现异步命令发送和回应及一些异步处理的细节，供异步调度器调用。
 */
class LUIP_SHARE AsyncCall
{
public:

    AsyncCall(DscpCmdPtr cmd);
    AsyncCall(DscpCmdPtr cmd, int priority);
    Dscp::DscpCmdPtr GetDscpCmd();
    int GetPriority();
    void SetPriority(int prior);
    bool IsSendout();
    void SetSendout(bool value);
    bool IsResponded();
    void SetResponded(bool value);
    Dscp::DscpRespPtr GetRespond();
    void SetRespond(DscpRespPtr resp);
    long GetTimestamp();
    void UpdateTimestamp();
    int GetInitRetries();
    int GetRetries();
    void SetRetries(int value);
    void UpdateRetries();
    bool IsMatch(Dscp::DscpRespPtr resp);

private:
    // 要调用的DSCP命令对象。
    DscpCmdPtr m_cmd;
    // 优先级。优先级高（数值低）的命令将优先发送。同优先级的调用将按调用先后顺序排列。
    // 
    // 优先级总体分布为：
    // 0 - 49：同步调用；
    // 50 - 199：异步调用（如异步采集、异步升级）；
    // 200 - 255：同步调用；
    int m_priority;
    // 回应包。
    DscpRespPtr m_respond;
    // 命令发送的时间戳，当前系统时间的毫秒数。
    // 调用 System.currentTimeMillis() 获取。
    long m_timestamp;
    // 命令是否已经发出。
    bool m_isSendout;
    // 命令是否已经回应过。
    bool m_isResponded;
    // 当前的超时/错误重试次数。
    // 初始重测次数。
    int m_initRetries;
    // 每次重试都将减一，0 表示不应重试直接报错。
    int m_retries;
};

typedef std::shared_ptr<AsyncCall> AsyncCallPtr;

}

#endif  //COMMUNICATION_ASYNCCALL_H
