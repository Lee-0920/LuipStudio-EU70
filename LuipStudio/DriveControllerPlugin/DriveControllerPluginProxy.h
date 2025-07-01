#ifndef DRIVECONTROLLERPLUGINPROXY_H
#define DRIVECONTROLLERPLUGINPROXY_H

#include "oolua.h"
#include "LuipShare.h"
#include "CurveData.h"

using Controller::CurveData;
/**
 * @brief 曲线数据类。
 * @details 定义了电极采集数据及结果计算的操作。
 */
OOLUA_PROXY(LUIP_SHARE,CurveData)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(float, float, int)
    )
    OOLUA_MFUNC(SetData)
    OOLUA_MFUNC(GetWholeData)
    OOLUA_MFUNC_CONST(GetData)
    OOLUA_MFUNC_CONST(GetLength)
    OOLUA_MFUNC_CONST(GetBegin)
    OOLUA_MFUNC_CONST(GetEnd)
    OOLUA_MFUNC(CalculateArea)
    OOLUA_MFUNC(CalculatePeak)
    OOLUA_MFUNC(GetPeakVolt)
    OOLUA_MFUNC(AutoCalculateArea)
OOLUA_PROXY_END

class DriveControllerPluginProxy
{
public:
    DriveControllerPluginProxy();

    static void Proxy();
};


#endif // DRIVECONTROLLERPLUGINPROXY_H
