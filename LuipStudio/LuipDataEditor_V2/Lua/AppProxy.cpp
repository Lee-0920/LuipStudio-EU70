
#include "AppProxy.h"
#include "App.h"

using namespace Lua;
using namespace System;

OOLUA_PROXY(,App)

    OOLUA_CTORS(

    )
    OOLUA_SFUNC(CreateUUID)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(App)
OOLUA_EXPORT_FUNCTIONS_CONST(App)

AppProxy::AppProxy()
{

}

void AppProxy::Proxy(OOLUA::Script *lua)
{
    lua->register_class<App>();
    lua->register_class_static<App>("CreateUUID",
             &OOLUA::Proxy_class<App>::CreateUUID); // 静态函数要单独注册
}
