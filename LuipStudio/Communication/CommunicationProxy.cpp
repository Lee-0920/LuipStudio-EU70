
#include "LuaEngine/LuaEngine.h"
#include "CommunicationProxy.h"

using namespace OOLUA;
using namespace Lua;

/**
 * @brief 设备网络地址。
 * @details DSCP设备地址，对应网络层地址。
 */
/*
OOLUA_PROXY(DscpAddress)
    OOLUA_CTORS(
        OOLUA_CTOR(Byte, Byte)
        OOLUA_CTOR(Byte, Byte, Byte)
        OOLUA_CTOR(Byte, Byte, Byte, Byte)
    )
    OOLUA_MGET_MSET(a1, GetA1, SetA1)
    OOLUA_MGET_MSET(a2, GetA2, SetA2)
    OOLUA_MGET_MSET(a3, GetA3, SetA3)
    OOLUA_MGET_MSET(a4, GetA4, SetA4)
    OOLUA_MFUNC(Make)
    OOLUA_MFUNC(ToString)

OOLUA_PROXY_END */
OOLUA_EXPORT_FUNCTIONS(DscpAddress, SetA1, SetA2, SetA3, SetA4, Make, ToString)
OOLUA_EXPORT_FUNCTIONS_CONST(DscpAddress, GetA1, GetA2, GetA3, GetA4)


CommunicationProxy::CommunicationProxy()
{

}



void CommunicationProxy::Proxy()
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    OOLUA::register_class<DscpAddress>(*lua);
}

