#if !defined(RESULTMANAGER_RESULTMANAGERPROXY_H)
#define RESULTMANAGER_RESULTMANAGERPROXY_H

#include "oolua.h"

namespace Result
{
class ResultManagerProxy
{
public:
    ResultManagerProxy() {}
    ~ResultManagerProxy() {}

public:
    static void ProxyClass(OOLUA::Script * lua);
    static void ProxyEnum(OOLUA::Script * lua);

protected:
    static OOLUA::Script* m_lua;
};
}
#endif // RESULTMANAGER_RESULTMANAGERPROXY_H
