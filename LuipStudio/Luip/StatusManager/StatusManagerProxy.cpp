#include "StatusManagerProxy.h"
#include "StatusManager.h"

using namespace Measure;
using namespace OOLUA;
using namespace std;

OOLUA_PROXY(,RunAction)
    OOLUA_CTORS(
        OOLUA_CTOR(string, string, int)
    )
    OOLUA_MGET_MSET(name, GetName, SetName)
    OOLUA_MGET_MSET(content, GetContent, SetContent)
    OOLUA_MGET_MSET(time, GetTime, SetTime)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RunAction, SetName, SetContent, SetTime)
OOLUA_EXPORT_FUNCTIONS_CONST(RunAction, GetName, GetContent, GetTime)

OOLUA_PROXY(,RunStatus)
    OOLUA_CTORS(
        OOLUA_CTOR(string, string, int)
    )
    OOLUA_MGET_MSET(name, GetName, SetName)
    OOLUA_MGET_MSET(content, GetContent, SetContent)
    OOLUA_MGET_MSET(time, GetTime, SetTime)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RunStatus, SetName, SetContent, SetTime)
OOLUA_EXPORT_FUNCTIONS_CONST(RunStatus, GetName, GetContent, GetTime)

OOLUA_PROXY(,StatusManager)
    OOLUA_TAGS(
        No_public_constructors
    )
    OOLUA_SFUNC(Instance)
    OOLUA_MFUNC(ResetDeviceStatus)
    OOLUA_MFUNC(GetStatus)
    OOLUA_MFUNC(SetStatus)
    OOLUA_MFUNC(SetStatusDelayTime)
    OOLUA_MFUNC(GetAction)
    OOLUA_MFUNC(SetAction)
    OOLUA_MFUNC(SetActionDelayTime)
    OOLUA_MFUNC(IsIdleStatus)
    OOLUA_MFUNC(IsIdleAction)
    OOLUA_MFUNC(IsFaultStatus)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(StatusManager, GetStatus, SetStatus, SetStatusDelayTime, GetAction,
                       SetAction, SetActionDelayTime,IsIdleStatus, IsIdleAction, ResetDeviceStatus,
                       IsFaultStatus)
OOLUA_EXPORT_FUNCTIONS_CONST(StatusManager)

OOLUA_PROXY(,BaseLineStatus)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Ok)
        OOLUA_ENUM(Failed)
        OOLUA_ENUM(Stop)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(BaseLineStatus)

namespace Measure
{



Script* StatusManagerProxy::m_lua;
void StatusManagerProxy::Proxy(Script * lua)
{
    m_lua = lua;
    m_lua->register_class<RunAction>();
    m_lua->register_class<RunStatus>();
    m_lua->register_class<StatusManager>();
    m_lua->register_class_static<StatusManager>("Instance",
             &OOLUA::Proxy_class<StatusManager>::Instance);
}

void StatusManagerProxy::ProxyEnum(Script * lua)
{
    m_lua = lua;

    m_lua->register_class<BaseLineStatus>();
}
}
