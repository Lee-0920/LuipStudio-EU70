#ifndef CONTROLLERMANAGERPROXY_H
#define CONTROLLERMANAGERPROXY_H

#include "oolua.h"

class ControllerManagerProxy
{
public:
    ControllerManagerProxy();

    static void Proxy(OOLUA::Script * lua);
};

#endif // CONTROLLERMANAGERPROXY_H
