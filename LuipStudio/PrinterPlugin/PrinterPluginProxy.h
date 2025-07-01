#ifndef PRINTERPLUGINPROXY_H
#define PRINTERPLUGINPROXY_H

#include "Printer.h"
#include "LuipShare.h"
#include "oolua.h"

using PrinterSpace::Printer;

OOLUA_PROXY(LUIP_SHARE, Printer)
OOLUA_PROXY_END

namespace PrinterSpace
{
class PrinterPluginProxy
{
public:
    PrinterPluginProxy();
    static void Proxy();
};
}
#endif // PRINTERPLUGINPROXY_H
