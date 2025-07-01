#ifndef COMMUNICATION_COMMUNICATION_EXCEPTION_H_
#define COMMUNICATION_COMMUNICATION_EXCEPTION_H_

#include "Dscp/DscpAddress.h"
#include "LuipShare.h"
#include "oolua.h"
#include "LuaException.h"

using Communication::Dscp::DscpAddress;
using System::Uint16;

namespace Communication
{

/**
 * @brief DncpStack初始异常的异常。
 */
class LUIP_SHARE DncpStackInitException
{
public:
    DncpStackInitException() : m_message("DncpStackInitException") {}
    DncpStackInitException(const char* message) : m_message(message) {}
    virtual ~DncpStackInitException() {}

    /**
     * @brief 查询异常的信息。
     */
    virtual const String& What()
    {
        return m_message;
    }

protected:
    String m_message;
};

/**
 * @brief 通信异常。
 */
class LUIP_SHARE CommunicationException
{
public:
    CommunicationException(const char* message,const char*  type) : m_message(message),m_type(type) {  }
    /**
     * @brief LuaException C++向LUA抛异常时，必须使用此构造函数,
     *  此构造函数会将异常类压入栈顶，由lua_error函数以栈顶的值作为错误对象，抛出一个 Lua 错误
     * @param vm
     * @param message
     */
    CommunicationException(DscpAddress addr, Uint16 code, const char* message = "通信异常");
    CommunicationException(const CommunicationException &other);

    virtual ~CommunicationException()
    {
    }

    virtual const String& What()
    {
        return m_message;
    }

    Byte GetAddrA1(void)
    {
        return m_addr.a1;
    }
    Byte GetAddrA2(void)
    {
        return m_addr.a2;
    }
    Byte GetAddrA3(void)
    {
        return m_addr.a3;
    }
    Byte GetAddrA4(void)
    {
        return m_addr.a4;
    }

protected:
    std::string m_message;//异常信息

public:
    DscpAddress m_addr;
    Uint16 m_code;
    std::string m_type;
    std::string m_name;
};


/**
 * @brief 命令应答超时异常。
 */
class LUIP_SHARE CommandTimeoutException : public CommunicationException
{
public:
    CommandTimeoutException(DscpAddress addr, Uint16 code, const char* message = "命令应答超时");
    CommandTimeoutException(const CommandTimeoutException &other):
        CommunicationException(other)
    {
    }
    ~CommandTimeoutException()
    {
    }
};

/**
 * @brief 事件等待超时异常。
 */
class LUIP_SHARE ExpectEventTimeoutException : public CommunicationException
{
public:
    ExpectEventTimeoutException(DscpAddress addr, Uint16 code, const char* message = "事件超时");
    ExpectEventTimeoutException(const ExpectEventTimeoutException &other):
            CommunicationException(other)
    {
    }
    ~ExpectEventTimeoutException()
    {
    }
};

class CommunicationExceptionProxy
{
public:
    CommunicationExceptionProxy() {}
    ~CommunicationExceptionProxy() {}

public:
    static void Init();

};
}



#endif  // CONTROLNET_COMMUNICATION_COMMUNICATION_EXCEPTION_H_
