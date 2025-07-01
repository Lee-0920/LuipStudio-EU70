#ifndef GP80PRINTERPLUGINPROXY_H
#define GP80PRINTERPLUGINPROXY_H


#include "LuipShare.h"
#include "oolua.h"
//#include "PrinterPlugin/Printer.h"
//#include "GP80Printer.h"

//using PrinterSpace::Printer;
//using PrinterSpace::GP80Printer;

//OOLUA_PROXY(LUIP_SHARE, GP80Printer, Printer)
//OOLUA_PROXY_END

namespace PrinterSpace
{
class GP80PrinterPluginProxy
{
public:
    GP80PrinterPluginProxy();
    static void Proxy();
};
}
#endif // GP80PRINTERPLUGINPROXY_H
