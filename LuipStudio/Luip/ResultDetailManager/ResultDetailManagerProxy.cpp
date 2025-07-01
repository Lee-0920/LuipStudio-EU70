#include "ResultDetailManagerProxy.h"
#include "ResultDetailManager.h"
#include "ResultDetailPlugin/ResultDetailPluginProxy.h"

using namespace ResultDetail;

OOLUA_PROXY(,ResultDetailManager)
    OOLUA_TAGS(
        //No_default_constructor,
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)  // 单便入口函数
    OOLUA_MFUNC(SetMeasureResultDetailWidget)
    OOLUA_MFUNC(SetCalibrateResultDetailWidget)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ResultDetailManager, SetMeasureResultDetailWidget, SetCalibrateResultDetailWidget)
OOLUA_EXPORT_FUNCTIONS_CONST(ResultDetailManager)

namespace ResultDetail
{

ResultDetailManagerProxy::ResultDetailManagerProxy()
{

}

void ResultDetailManagerProxy::Proxy(OOLUA::Script *lua)
{
    lua->register_class<ResultDetailManager>();
    lua->register_class_static<ResultDetailManager>("Instance",
             &OOLUA::Proxy_class<ResultDetailManager>::Instance); // 静态函数要单独注册

}
}
