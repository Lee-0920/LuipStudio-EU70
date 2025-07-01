#include "GP80PrinterPluginProxy.h"
#include "LuaEngine/LuaEngine.h"
#include "PrinterPlugin/Printer.h"
#include "PrinterPlugin/PrinterPluginProxy.h"
#include "GP80Printer.h"
#include <QDebug>

using namespace OOLUA;
using namespace Lua;
using namespace PrinterSpace;

OOLUA_PROXY(, GP80Printer, Printer)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(GP80Printer)
OOLUA_EXPORT_FUNCTIONS_CONST(GP80Printer)

namespace PrinterSpace
{

GP80PrinterPluginProxy::GP80PrinterPluginProxy()
{

}

void GP80PrinterPluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    OOLUA::register_class<GP80Printer>(*lua);
}

}
