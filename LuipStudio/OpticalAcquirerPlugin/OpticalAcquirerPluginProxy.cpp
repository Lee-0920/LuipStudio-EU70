
#include "API/OpticalAcquireInterface.h"
#include "OpticalAcquirer.h"
#include "Communication/CommunicationProxy.h"
#include "ControllerPlugin/ControllerPluginProxy.h"
#include "LuaEngine/LuaEngine.h"
#include "OpticalAcquirerPluginProxy.h"

using Communication::Dscp::DscpAddress;
using namespace Controller;
using namespace Controller::API;
using namespace OOLUA;
using namespace Lua;

/**
 * @brief 光学信号采集的AD值。
 * @details
 */
OOLUA_PROXY(,OpticalAD)
    OOLUA_MGET_MSET(reference, GetReference, SetReference)
    OOLUA_MGET_MSET(measure, GetMeasure, SetMeasure)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(OpticalAD, SetReference, SetMeasure)
OOLUA_EXPORT_FUNCTIONS_CONST(OpticalAD, GetReference, GetMeasure)

/**
 * @brief 蠕动泵操作结果码。
 * @details
 */
OOLUA_PROXY(,AcquiredOperateResult)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Finished)   // 枚举值
        OOLUA_ENUM(Failed)   // 枚举值
        OOLUA_ENUM(Stopped)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(AcquiredOperateResult)  // 导出函数声明（没有函数也需要声明）


/**
 * @brief 光学信号采集的结果。
 * @details
 */
OOLUA_PROXY(,AcquiredResult)
    OOLUA_MGET_MSET(ad, GetAD, SetAD)
    OOLUA_MGET_MSET(result, GetResult, SetResult)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(AcquiredResult, SetAD, SetResult)
OOLUA_EXPORT_FUNCTIONS_CONST(AcquiredResult, GetAD, GetResult)

/**
 * @brief LED控制参数。
 * @details
 */
OOLUA_PROXY(,LedParam)
    OOLUA_MGET_MSET(proportion, GetProportion, SetProportion)
    OOLUA_MGET_MSET(integration, GetIntegration, SetIntegration)
    OOLUA_MGET_MSET(differential, GetDifferential, SetDifferential)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(LedParam, SetProportion, SetIntegration, SetDifferential)
OOLUA_EXPORT_FUNCTIONS_CONST(LedParam, GetProportion, GetIntegration, GetDifferential)

/**
 * @brief 光学信号采集接口。
 * @details 定义了一序列光学信号采集相关的操作。
 */
OOLUA_PROXY(,OpticalAcquireInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(TurnOnLED)
    OOLUA_MFUNC(SetAcquireADNotifyPeriod)
    OOLUA_MFUNC(StartAcquirer)
    OOLUA_MFUNC(StopAcquirer)
    OOLUA_MFUNC(StartLEDController)
    OOLUA_MFUNC(TurnOffLED)
    OOLUA_MFUNC(GetLEDControllerTartet)
    OOLUA_MFUNC(SetLEDControllerTarget)
    OOLUA_MFUNC(GetLEDControllerParam)
    OOLUA_MFUNC(SetLEDControllerParam)
    OOLUA_MFUNC(ExpectADAcquirer)
//    OOLUA_MEM_FUNC(void, RegisterAcquireADNotice, cpp_in_p<IEventReceivable*>)
    OOLUA_MFUNC(RegisterAcquireADNotice)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(OpticalAcquireInterface, TurnOnLED, SetAcquireADNotifyPeriod, StartAcquirer, StopAcquirer,
                       StartLEDController, TurnOffLED, GetLEDControllerTartet, SetLEDControllerTarget,
                       GetLEDControllerParam, SetLEDControllerParam, ExpectADAcquirer, RegisterAcquireADNotice)
OOLUA_EXPORT_FUNCTIONS_CONST(OpticalAcquireInterface)


/**
 * @brief 光学信号采集器。
 * @details
 */
OOLUA_PROXY(,OpticalAcquirer, BaseController)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(Init)
    OOLUA_MFUNC(Uninit)
//    OOLUA_MEM_FUNC(maybe_null<lua_return<OpticalAcquireInterface*> >, GetIOpticalAcquire)
    OOLUA_MFUNC(GetIOpticalAcquire)
//    OOLUA_MEM_FUNC(void, Register, cpp_in_p<ISignalNotifiable*>)
    OOLUA_MFUNC(Register)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(OpticalAcquirer, Init, Uninit, GetIOpticalAcquire, Register)
OOLUA_EXPORT_FUNCTIONS_CONST(OpticalAcquirer)

OpticalAcquirerPluginProxy::OpticalAcquirerPluginProxy()
{

}

void OpticalAcquirerPluginProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    lua->register_class<OpticalAD>();
    lua->register_class<AcquiredOperateResult>();
    lua->register_class<AcquiredResult>();
    lua->register_class<LedParam>();
    lua->register_class<OpticalAcquireInterface>();
    lua->register_class<OpticalAcquirer>();
}
