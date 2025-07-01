#ifndef _LUAEXCEPTION_H_
#define _LUAEXCEPTION_H_

#include "oolua.h"

#	if OOLUA_USE_EXCEPTIONS == 1
namespace OOLUA {
/**
 * @brief The MyException class C++与Lua异常交互基类
 * @note Lua向C++抛异常，或者C++向Lua抛异常，异常类需要使用此类或者此类的派生类
 */
class LuaException
{
public:
    LuaException() :
        m_message(""),type("LuaException")
    {

    }

    LuaException(const char * message) :
        m_message(message),type("LuaException")
    {

    }

    LuaException(LuaException const& rhs)
        : m_message(rhs.m_message),type(rhs.type)
    {

    }
    /**
     * @brief LuaException C++向LUA抛异常时，必须使用此构造函数,
     *  此构造函数会将异常类压入栈顶，由lua_error函数以栈顶的值作为错误对象，抛出一个 Lua 错误
     * @param vm
     * @param message
     */
    LuaException(lua_State* vm, const char* message);

    virtual const std::string& What()
    {
        return m_message;
    }

    virtual ~LuaException()
    {

    }

    /**
     * @brief ThrowScriptError 弹出栈中LUA的异常类，并抛出此异常类
     * @param vm
     */
    static void ThrowScriptError(lua_State* vm);

    /**
     * @brief GetType 在C++获取LUA抛出的异常类类型
     * @return
     */
    std::string GetType(void)
    {
        return type;
    }

protected:
    std::string m_message;//异常信息
    std::string type;//异常类类型
};

/**
 * @brief The LuaExceptionProxy class 向LUA注册C++与Lua异常交互基类的类
 */
class LuaExceptionProxy
{
public:
    LuaExceptionProxy() {}
    ~LuaExceptionProxy() {}

public:
    static void Init(Script * lua);

protected:
    static Script* m_lua;
};

}
#   endif

#endif // _LUAEXCEPTION_H_
