#ifndef SIGNALMANAGERPROXY_H
#define SIGNALMANAGERPROXY_H

#include "oolua.h"

class SignalManagerProxy
{
public:
    SignalManagerProxy() {}
    ~SignalManagerProxy() {}
public:
    static void Proxy(OOLUA::Script * lua);

protected:
    static OOLUA::Script* m_lua;
};

#endif // SIGNALMANAGERPROXY_H
