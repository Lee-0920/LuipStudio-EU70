/**
 * @file
 * @brief DNCP 协议栈。
 * @details 为设备构建一个静态全局的协议栈栈体。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2015-3-1
 */

#ifndef STACK_DNCP_STACK_H_
#define STACK_DNCP_STACK_H_

#include <memory>
#include "IDscpReceivable.h"
#include "System/Types.h"

class IConnectStatusNotifiable;

namespace Communication {namespace Dscp {
    class DscpAddress;
}}

using namespace System;
using Communication::Dscp::DscpAddress;

namespace Communication
{
namespace Stack
{

/**
 * @brief DNCP 协议栈包装类。
 * @details 为设备构建一个静态全局的协议栈栈体，单件。
 */
class DncpStack
{
public:
    ~DncpStack();
    static DncpStack* Instance();

public:
    void SetEventHandler(IDscpReceivable* handler);
    void SetRespondHandler(IDscpReceivable* handler);

    bool Init(char* serialPort, bool bridgeMode,
              const char* bridgeIP, IConnectStatusNotifiable *handle);
    bool Reconnect(char* serialPort, IConnectStatusNotifiable *handle);
    bool IsInitialized();
    void Uninit();
    bool Send(DscpAddress addr, int cmd, void* data, int len);

private:
    DncpStack();
    static std::unique_ptr<DncpStack> m_instance;

    static void OnReceive(Uint32 addr, Uint8 type, Bool isFollowed, Uint16 code, Byte* data, Uint16 len);

private:
    IDscpReceivable* m_eventHandler;
    IDscpReceivable* m_respondHandler;

    bool m_isInitialized;       ///<  是否初始化成功（协议栈接通）
    bool m_bridgeMode;
};

}
}

#endif  // CONTROLNET_STACK_DNCP_STACK_H_
