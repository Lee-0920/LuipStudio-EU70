#ifndef FLOWMANAGERPROXY_H
#define FLOWMANAGERPROXY_H

#include "oolua.h"

class FlowManagerProxy
{
public:
    FlowManagerProxy();

    static void Proxy(OOLUA::Script * lua);
};

#endif // FLOWMANAGERPROXY_H
