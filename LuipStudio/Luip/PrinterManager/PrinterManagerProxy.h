#ifndef PRINTERMANAGERPROXY_H
#define PRINTERMANAGERPROXY_H

#include "oolua.h"
namespace PrinterSpace
{
class PrinterManagerProxy
{
public:
    PrinterManagerProxy();
    static void Proxy(OOLUA::Script * lua);
};
}
#endif // PRINTERMANAGERPROXY_H
