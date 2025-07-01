#include "ControllerManager.h"
#include "ControllerPlugin/ControllerPluginProxy.h"
#include "ControllerManagerProxy.h"
#include "Communication/CommunicationProxy.h"

using namespace OOLUA;
/**
 * @brief 控制器管理器。
 * @details
 */
OOLUA_PROXY(,ControllerManager)
    OOLUA_TAGS(
        //No_default_constructor,
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)  // 单便入口函数
    OOLUA_MFUNC(IsAllConnected)
    OOLUA_MFUNC(SetUpdateFlag)
    OOLUA_MFUNC(GetUpdateFlag)
    OOLUA_MFUNC(SetParamSettingFlag)
    OOLUA_MFUNC(GetParamSettingFlag)
//    OOLUA_MEM_FUNC(maybe_null<lua_return<BaseController*> >, GetController, String)  // 指定可以为NULL，且由lua支配返回的对象
//    OOLUA_MEM_FUNC(void, AddController, String, cpp_in_p<BaseController*>)
    OOLUA_MFUNC(AddController)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ControllerManager, IsAllConnected, SetUpdateFlag, GetUpdateFlag,
                       SetParamSettingFlag, GetParamSettingFlag, AddController)
OOLUA_EXPORT_FUNCTIONS_CONST(ControllerManager)

ControllerManagerProxy::ControllerManagerProxy()
{

}

void ControllerManagerProxy::Proxy(Script *lua)
{
    lua->register_class<ControllerManager>();
    lua->register_class_static<ControllerManager>("Instance",
             &OOLUA::Proxy_class<ControllerManager>::Instance); // 静态函数要单独注册

}
