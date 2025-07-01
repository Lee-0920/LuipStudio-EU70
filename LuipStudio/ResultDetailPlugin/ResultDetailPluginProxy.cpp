#include "ResultDetailPluginProxy.h"
#include "LuaEngine/LuaEngine.h"

using namespace OOLUA;
using namespace Lua;

OOLUA_EXPORT_FUNCTIONS(ResultDetailWidget)
OOLUA_EXPORT_FUNCTIONS_CONST(ResultDetailWidget)

OOLUA_EXPORT_FUNCTIONS(MeasureResultDetailWidget)
OOLUA_EXPORT_FUNCTIONS_CONST(MeasureResultDetailWidget)

OOLUA_EXPORT_FUNCTIONS(CalibrateResultDetailWidget)
OOLUA_EXPORT_FUNCTIONS_CONST(CalibrateResultDetailWidget)

namespace ResultDetail
{
ResultDetailPluginProxy::ResultDetailPluginProxy()
{

}
void ResultDetailPluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    OOLUA::register_class<ResultDetailWidget>(*lua);
    OOLUA::register_class<MeasureResultDetailWidget>(*lua);
    OOLUA::register_class<CalibrateResultDetailWidget>(*lua);
}
}
