#include "PrinterManagerProxy.h"
#include "PrinterManager.h"
#include "PrinterPlugin/PrinterPluginProxy.h"

using namespace PrinterSpace;

OOLUA_PROXY(,PrinterManager)
    OOLUA_TAGS(
        //No_default_constructor,
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)  // 单便入口函数
    OOLUA_MFUNC(SetMeasureDataPrinter)
    OOLUA_MFUNC(SetCalibrateDataPrinter)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(PrinterManager, SetMeasureDataPrinter, SetCalibrateDataPrinter)
OOLUA_EXPORT_FUNCTIONS_CONST(PrinterManager)

namespace PrinterSpace
{

PrinterManagerProxy::PrinterManagerProxy()
{

}

void PrinterManagerProxy::Proxy(OOLUA::Script *lua)
{
    lua->register_class<PrinterManager>();
    lua->register_class_static<PrinterManager>("Instance",
             &OOLUA::Proxy_class<PrinterManager>::Instance); // 静态函数要单独注册

}
}
