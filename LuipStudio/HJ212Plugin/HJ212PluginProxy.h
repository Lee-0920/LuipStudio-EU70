#ifndef HJ212PLUGINPROXY_H
#define HJ212PLUGINPROXY_H

#include "oolua.h"

namespace HJ212
{

class HJ212PluginProxy
{
public:
    HJ212PluginProxy();
    static void Proxy(OOLUA::Script *lua);
};

}

#endif // HJ212PLUGINPROXY_H
