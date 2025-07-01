/**
 * @file
 * @brief 异步调用（命令）。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#include "AsyncCall.h"
#include <QDateTime>
#include <QDebug>

namespace Communication
{

/**
 * @brief 使用指定的DSCP命令构建一个异步调用对象，优先级为默认100。
 * @param[in] DSCP 命令。
 *
 */
AsyncCall::AsyncCall(Dscp::DscpCmdPtr cmd)
{
    this->m_cmd = cmd;
    this->m_isResponded =false;
    this->m_isSendout = false;
    this->m_priority = 0;
    this->m_retries = 0;
    this->m_initRetries = 0;
    this->m_timestamp = 0;
}

/**
 * @brief 使用指定的DSCP命令和优先级构建一个异步调用对象。
 * @param[in] cmd DSCP 命令。
 * @param[in] priority DSCP 命令发送优先级；
 *
 */
AsyncCall::AsyncCall(Dscp::DscpCmdPtr cmd, int priority)
{
    this->m_cmd = cmd;
    this->m_priority =  priority;
    this->m_isResponded =false;
    this->m_isSendout = false;
    this->m_retries = 0;
    this->m_initRetries = 0;
    this->m_timestamp = 0;
}

DscpCmdPtr AsyncCall::GetDscpCmd()
{
    return m_cmd;
}

/**
 * @brief 查询优先级。
 * @details 数值越小优先级越高，0为最高优先级。
 * @return 返回优先级数值。
 *
 */
int AsyncCall::GetPriority()
{
    return m_priority;
}

/**
 * @brief 设置优先级。
 * @details 数值越小优先级越高，0为最高优先级。
 * @param[in] prior 设置的优先级数值。
 *
 */
void AsyncCall::SetPriority(int prior)
{
    this->m_priority = prior;
}

/**
 * @brief 查询异步调用命令发送状态。
 * @details 查询异步调用的DSCP命令是否发出。
 * @return 命令是否发送成功。
 *
 */
bool AsyncCall::IsSendout()
{
    return m_isSendout;
}

/**
 * @brief 标记异步调用是否发出。
 * @details 仅供异步调度器调用。
 * @param[in] value 设置的标记值。
 *
 */
void AsyncCall::SetSendout(bool value)
{
    this->m_isSendout = value;
}

/**
 * @brief 查询异步调用是否完成。
 * @details 正确匹配了回应包即为完成。
 * @return 异步调用是否完成。
 *
 */
bool AsyncCall::IsResponded()
{
    return m_isResponded;
}

/**
 * @brief 标记异步调用是否完成。
 * @details 仅供异步调度器调用。
 * @param[in] value 设置的标记值。
 *
 */
void AsyncCall::SetResponded(bool value)
{
    this->m_isResponded = value;
}

/**
 * @brief 获取异步调用的回应包对象。
 * @return 回应包。
 *
 */
DscpRespPtr AsyncCall::GetRespond()
{
    return m_respond;
}

/**
 * @brief 设置异步调用的回应包对象。
 * @param[in] resp 回应包。
 *
 */
void AsyncCall::SetRespond(Dscp::DscpRespPtr resp)
{
    this->m_respond = resp;
}

/**
 * @brief 查询异步调用已经发出的时戳。
 * @details 毫秒为单位
 * @return 返回毫秒数。
 *
 */
long AsyncCall::GetTimestamp()
{
    return m_timestamp;
}

/**
 * @brief 查询异步调用已经发出的时戳。
 * @details 仅供异步调度器在往DNCP发送命令时调用。
 *
 */
void AsyncCall::UpdateTimestamp()
{
    this->m_timestamp =QDateTime::currentDateTime().toMSecsSinceEpoch();
}

/**
 * @brief 查询当前的超时/错误重传的初始次数。
 * @details 仅供异步调度器调用
 * @return 返回重传初始次数。
 *
 */
int AsyncCall::GetInitRetries()
{
    return m_initRetries;
}

/**
 * @brief 查询当前的超时/错误重传次数。
 * @details 仅供异步调度器调用
 * @return 返回重传次数。
 *
 */
int AsyncCall::GetRetries()
{
    return m_retries;
}

/**
 * @brief 设置当前的超时/错误重传次数。
 * @details value 重传次数
 *
 */
void AsyncCall::SetRetries(int value)
{
    this->m_retries = value;
    this->m_initRetries = value;
}

/**
 * @brief 当发生重传时，更新重传的次数。
 *
 */
void AsyncCall::UpdateRetries()
{
    this->m_retries -= 1;
}

/**
 * @brief 检查是否匹配指定的回应包。
 * @details 仅供异步调度器调用
 * @return 返回回应包是否匹配。
 *
 */
bool AsyncCall::IsMatch(DscpRespPtr resp)
{
    bool status = false;

    if (m_cmd->addr.a1 == resp->addr.a1 && m_cmd->addr.a2 == resp->addr.a2 &&
        m_cmd->addr.a3 == resp->addr.a3 && m_cmd->addr.a4 == resp->addr.a4 &&
        m_cmd->code == resp->code)
    {
        status = true;
    }
    return status;
}
        
}
