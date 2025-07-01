#include "PrinterPluginProxy.h"
#include "LuaEngine/LuaEngine.h"
#include <QDebug>

using namespace OOLUA;
using namespace Lua;

OOLUA_EXPORT_FUNCTIONS(Printer)
OOLUA_EXPORT_FUNCTIONS_CONST(Printer)


namespace PrinterSpace
{

PrinterPluginProxy::PrinterPluginProxy()
{

}

void PrinterPluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    OOLUA::register_class<Printer>(*lua);
}

}
