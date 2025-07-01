#ifndef COMMUNICATIONPROXY_H
#define COMMUNICATIONPROXY_H

#include "System/Types.h"
#include "Dscp/DscpAddress.h"
#include "oolua.h"
#include "LuipShare.h"

using System::Byte;
using namespace Communication::Dscp;

/**
 * @brief 设备网络地址。
 * @details DSCP设备地址，对应网络层地址。
 */

OOLUA_PROXY(LUIP_SHARE, DscpAddress)
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

OOLUA_PROXY_END

class CommunicationProxy
{
public:
    CommunicationProxy();

    static void Proxy();
};


#endif // COMMUNICATIONPROXY_H
