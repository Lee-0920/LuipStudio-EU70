#ifndef EXTERNALINTERFACEPROXY_H
#define EXTERNALINTERFACEPROXY_H

#include "oolua.h"

class ExternalInterfaceProxy
{
public:
    ExternalInterfaceProxy();

    static void Proxy(OOLUA::Script * lua);
};

#endif // FLOWMANAGERPROXY_H
