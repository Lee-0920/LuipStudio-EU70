#include "SignalManagerProxy.h"
#include "SignalManager.h"
#include "ControllerPlugin/ControllerPluginProxy.h"

using namespace OOLUA;
using namespace std;
using namespace Controller;

OOLUA_PROXY(,SignalManager, ISignalNotifiable)
    OOLUA_TAGS(
        No_public_constructors
    )
    OOLUA_SFUNC(Instance)
    OOLUA_MFUNC(FocusSignal)
    OOLUA_MFUNC(GetSignal)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(SignalManager, FocusSignal, GetSignal)
OOLUA_EXPORT_FUNCTIONS_CONST(SignalManager)

Script* SignalManagerProxy::m_lua;
void SignalManagerProxy::Proxy(Script * lua)
{
    m_lua = lua;
    m_lua->register_class<SignalManager>();
    m_lua->register_class_static<SignalManager>("Instance",
             &OOLUA::Proxy_class<SignalManager>::Instance);
}

