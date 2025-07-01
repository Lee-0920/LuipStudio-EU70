#include "NT66CurvePluginProxy.h"
#include "LuaEngine/LuaEngine.h"
#include "NT66MeasureCurveWidget.h"
#include "NT66CalibrateCurveWidget.h"
#include "LuipShare.h"
#include "oolua.h"
#include "ResultDetailPlugin/ResultDetailPluginProxy.h"
#include "DriveControllerPlugin/DriveControllerPluginProxy.h"
#include "CurveManager.h"

using namespace OOLUA;
using namespace Lua;
using namespace  ResultDetail;

OOLUA_PROXY(, NT66MeasureCurveWidget, MeasureResultDetailWidget)
    OOLUA_MFUNC(SetCurves)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NT66MeasureCurveWidget, SetCurves)
OOLUA_EXPORT_FUNCTIONS_CONST(NT66MeasureCurveWidget)

OOLUA_PROXY(, NT66CalibrateCurveWidget, CalibrateResultDetailWidget)
    OOLUA_MFUNC(SetCurves)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(NT66CalibrateCurveWidget, SetCurves)
OOLUA_EXPORT_FUNCTIONS_CONST(NT66CalibrateCurveWidget)

OOLUA_PROXY(, CurveFile)
    OOLUA_TAGS(
        No_public_constructors  // 不能在 lua 中创建对象
    )
    OOLUA_MFUNC(ClearCurve)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(CurveFile, ClearCurve)
OOLUA_EXPORT_FUNCTIONS_CONST(CurveFile)

OOLUA_PROXY(, CurveManager)
    OOLUA_TAGS(
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)   //Lua 中访问代码：ResultManager.Instance()
    OOLUA_MFUNC(Addcurve)
    OOLUA_MFUNC(Getcurve)
    OOLUA_MFUNC(GetCurveFile)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(CurveManager, Addcurve, Getcurve, GetCurveFile)
OOLUA_EXPORT_FUNCTIONS_CONST(CurveManager)

namespace  ResultDetail
{
NT66CurvePluginProxy::NT66CurvePluginProxy()
{

}

void NT66CurvePluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    OOLUA::register_class<NT66MeasureCurveWidget>(*lua);
    OOLUA::register_class<NT66CalibrateCurveWidget>(*lua);
    OOLUA::register_class<CurveFile>(*lua);
    OOLUA::register_class<CurveManager>(*lua);
    OOLUA::register_class_static<CurveManager>(*lua, "Instance",
                 &OOLUA::Proxy_class<CurveManager>::Instance);
}
}
