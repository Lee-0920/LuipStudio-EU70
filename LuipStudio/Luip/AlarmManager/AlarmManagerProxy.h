#if !defined(ALARMMANAGER_ALARMMANAGERPROXY_H)
#define ALARMMANAGER_ALARMMANAGERPROXY_H

#include "oolua.h"



namespace Measure
{
class AlarmManagerProxy
{
public:
    AlarmManagerProxy() {}
    ~AlarmManagerProxy() {}

public:
    static void Proxy(OOLUA::Script * lua);

protected:
    static OOLUA::Script* m_lua;
};
}
#endif // ALARMMANAGER_ALARMMANAGERPROXY_H
