#ifndef LUAPROXY_H
#define LUAPROXY_H

#include "oolua.h"

using OOLUA::Script;

class AppProxy
{
public:
    AppProxy();
    static void Proxy(Script * lua);
};

#endif // LUAPROXY_H
