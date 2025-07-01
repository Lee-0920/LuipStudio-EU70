#if !defined(STATUSMANAGER_STATUSMANAGERPROXY_H)
#define STATUSMANAGER_STATUSMANAGERPROXY_H


#include "oolua.h"

namespace Measure
{
class StatusManagerProxy
{
public:
    StatusManagerProxy() {}
    ~StatusManagerProxy() {}
public:
    static void Proxy(OOLUA::Script * lua);
    static void ProxyEnum(OOLUA::Script * lua);

protected:
    static OOLUA::Script* m_lua;
};
}
#endif // STATUSMANAGER_STATUSMANAGERPROXY_H
