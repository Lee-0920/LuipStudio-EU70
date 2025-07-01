
#include "CommunicationException.h"
#include "LuaEngine/LuaEngine.h"

using namespace Communication;
using namespace OOLUA;
using namespace Lua;

OOLUA_PROXY(,CommunicationException)
    OOLUA_TAGS(
        No_public_constructors
    )
    OOLUA_MFUNC(What)
    OOLUA_MGET(m_code, GetCode)
    OOLUA_MFUNC(GetAddrA1)
    OOLUA_MFUNC(GetAddrA2)
    OOLUA_MFUNC(GetAddrA3)
    OOLUA_MFUNC(GetAddrA4)
    OOLUA_MGET(m_type, GetType)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(CommunicationException, What, GetAddrA1, GetAddrA2, GetAddrA3, GetAddrA4)
OOLUA_EXPORT_FUNCTIONS_CONST(CommunicationException, GetCode, GetType)

OOLUA_PROXY(,CommandTimeoutException, CommunicationException)
    OOLUA_TAGS(
        No_public_constructors
    )
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(CommandTimeoutException)
OOLUA_EXPORT_FUNCTIONS_CONST(CommandTimeoutException)

OOLUA_PROXY(,ExpectEventTimeoutException, CommunicationException)
    OOLUA_TAGS(
        No_public_constructors
    )
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ExpectEventTimeoutException)
OOLUA_EXPORT_FUNCTIONS_CONST(ExpectEventTimeoutException)

namespace Communication
{

CommunicationException::CommunicationException(DscpAddress addr, Uint16 code, const char* message):
    m_message(message),m_type("CommunicationException")
{
    m_addr.Make(addr.a1, addr.a2, addr.a3, addr.a4);
    m_code = code;
    //将所有权更改为Lua。通知库Lua将控制正在使用的指针，并在适当时调用delete。
    CommunicationException *res = this;

    lua_State * state = LuaEngine::Instance()->GetThreadState();
    OOLUA::push(state, res);

//    lua->push(res);
}

CommunicationException::CommunicationException(const CommunicationException &other)
{
    this->m_addr.Make(other.m_addr.a1, other.m_addr.a2, other.m_addr.a3, other.m_addr.a4);
    this->m_code = other.m_code;
    this->m_type = other.m_type;
    this->m_name = other.m_name;
    OOLUA::cpp_acquire_ptr<CommunicationException *> res;
    lua_State * state = LuaEngine::Instance()->GetThreadState();
    OOLUA::pull(state, res);
}

CommandTimeoutException::CommandTimeoutException(DscpAddress addr, Uint16 code, const char* message):
    CommunicationException(message, "CommandTimeoutException")
{
    this->m_addr.Make(addr.a1, addr.a2, addr.a3, addr.a4);
    this->m_code = code;

    CommandTimeoutException *res = this;

    lua_State * state = LuaEngine::Instance()->GetThreadState();
    OOLUA::push(state, res);
}

ExpectEventTimeoutException::ExpectEventTimeoutException(DscpAddress addr, Uint16 code, const char* message):
        CommunicationException(message, "ExpectEventTimeoutException")
{
    this->m_addr.Make(addr.a1, addr.a2, addr.a3, addr.a4);
    this->m_code = code;

    ExpectEventTimeoutException *res = this;

    lua_State * state = LuaEngine::Instance()->GetThreadState();
    OOLUA::push(state, res);
}

void CommunicationExceptionProxy::Init()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    lua->register_class<CommunicationException>();
    lua->register_class<CommandTimeoutException>();
    lua->register_class<ExpectEventTimeoutException>();
}
}
