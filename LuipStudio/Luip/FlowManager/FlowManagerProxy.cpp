#include "FlowManager.h"
#include "FlowManagerProxy.h"
#include "Communication/CommunicationProxy.h"

using namespace Flow;

/**
 * @brief 流程管理器。
 * @details
 */
OOLUA_PROXY(,FlowManager)
    OOLUA_TAGS(
        //No_default_constructor,
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)  // 单便入口函数
    OOLUA_MFUNC(StartFlow)
    OOLUA_MFUNC(StopFlow)
    OOLUA_MFUNC(IsFlowEnable)
    OOLUA_MFUNC(IsComValid)
    OOLUA_MFUNC(IsAuthorize)
    OOLUA_MFUNC(IsReagentAuthorize)
    OOLUA_MFUNC(UpdateFlowTime)
    OOLUA_MFUNC(UpdateFlowResult)
    OOLUA_MFUNC(UpdateFlowMessage)
    OOLUA_MFUNC(UpdateAllFlowFinish)
    OOLUA_MFUNC(ClearAllRemainEvent)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(FlowManager, StartFlow, StopFlow, IsFlowEnable, IsComValid,IsAuthorize,
                       IsReagentAuthorize,UpdateFlowTime, UpdateFlowResult,UpdateFlowMessage,UpdateAllFlowFinish,ClearAllRemainEvent)
OOLUA_EXPORT_FUNCTIONS_CONST(FlowManager)

FlowManagerProxy::FlowManagerProxy()
{

}

void FlowManagerProxy::Proxy(OOLUA::Script *lua)
{
    lua->register_class<FlowManager>();
    lua->register_class_static<FlowManager>("Instance",
             &OOLUA::Proxy_class<FlowManager>::Instance); // 静态函数要单独注册

}
