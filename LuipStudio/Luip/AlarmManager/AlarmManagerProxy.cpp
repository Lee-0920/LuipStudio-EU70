#include "AlarmManagerProxy.h"
#include "AlarmManager.h"

using namespace Measure;
using namespace OOLUA;
using namespace std;

OOLUA_PROXY(,Alarm)

    OOLUA_CTORS(
        OOLUA_CTOR(string, string, string, string)
    )
    OOLUA_MFUNC(SetAlarm)
    OOLUA_MGET_MSET(level, GetLevel, SetLevel)
    OOLUA_MGET_MSET(type, GetType, SetType)
    OOLUA_MGET_MSET(name, GetName, SetName)
    OOLUA_MGET_MSET(cause, GetCause, SetCause)
    OOLUA_MFUNC(GetTime)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(Alarm, SetAlarm, SetLevel, SetType, SetName, SetCause, GetTime)
OOLUA_EXPORT_FUNCTIONS_CONST(Alarm, GetLevel, GetType, GetName, GetCause)

OOLUA_PROXY(,AlarmManager)
    OOLUA_TAGS(
        No_public_constructors
    )
    OOLUA_SFUNC(Instance)
    OOLUA_MFUNC(AddAlarm)
    OOLUA_MFUNC(GetUpLoadStatus)
    OOLUA_MFUNC_CONST(GetCurrentAlarm)
    OOLUA_MFUNC(ClearAlarm)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(AlarmManager, AddAlarm, GetUpLoadStatus, ClearAlarm)
OOLUA_EXPORT_FUNCTIONS_CONST(AlarmManager, GetCurrentAlarm)


namespace Measure
{
Script* AlarmManagerProxy::m_lua;
void AlarmManagerProxy::Proxy(Script * lua)
{
    m_lua = lua;

    m_lua->register_class<Alarm>();
    m_lua->register_class<AlarmManager>();
    m_lua->register_class_static<AlarmManager>("Instance",
             &OOLUA::Proxy_class<AlarmManager>::Instance);

}

}
