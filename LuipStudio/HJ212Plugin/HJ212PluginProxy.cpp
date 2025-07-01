#include "HJ212PluginProxy.h"
#include "HJ212Plugin/HJ212Data.h"

using namespace OOLUA;
using namespace HJ212;
using namespace MonitoringSpace;


/**
 * @brief 监测信息类型
 * @details
 */
OOLUA_PROXY(,InfoType)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(None)  // 枚举值
        OOLUA_ENUM(Logger)    // 枚举值
        OOLUA_ENUM(Status)     // 枚举值
        OOLUA_ENUM(Parameter)  // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(InfoType)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 监测信息索引
 * @details
 */
OOLUA_PROXY(,MonitoringInfoIndex)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(none)  // 枚举值
        OOLUA_ENUM(iEquipmentLogCode)    // 枚举值
        OOLUA_ENUM(iEquipmentStauts)     // 枚举值
        OOLUA_ENUM(iEquipmentConnectStatus)  // 枚举值
        OOLUA_ENUM(iReagentMargin)  // 枚举值
        OOLUA_ENUM(iAlarmCode)  // 枚举值
        OOLUA_ENUM(iRangeValue)  // 枚举值
        OOLUA_ENUM(iMeaAccuracy)  // 枚举值
        OOLUA_ENUM(iMeaInterval)  // 枚举值
        OOLUA_ENUM(iDigestionTemp)  // 枚举值
        OOLUA_ENUM(iDigestionTime)  // 枚举值
        OOLUA_ENUM(iBlankCalibrationDateTime)  // 枚举值
        OOLUA_ENUM(iCurveB)  // 枚举值
        OOLUA_ENUM(iCurveK)  // 枚举值
        OOLUA_ENUM(iDetectionLimit)  // 枚举值
        OOLUA_ENUM(iMeaProcessValue)  // 枚举值
        OOLUA_ENUM(iLinear)  // 枚举值
        OOLUA_ENUM(iBinomialFactor)  // 枚举值
        OOLUA_ENUM(iStdCalibrationDateTime)  // 枚举值
        OOLUA_ENUM(iSysLogCode)  // 枚举值
        OOLUA_ENUM(iSysStauts)  // 枚举值
        OOLUA_ENUM(iSysUser)  // 枚举值
        OOLUA_ENUM(iSysConnectStatus)  // 枚举值
        OOLUA_ENUM(iChunnelConnectStauts)  // 枚举值


        OOLUA_ENUM(iAlarmInfo)  // 枚举值
        OOLUA_ENUM(iCorrectionFactor)  // 枚举值
        OOLUA_ENUM(iCorrectionValue)  // 枚举值
        OOLUA_ENUM(iSystemTime)  // 枚举值
        OOLUA_ENUM(iSamplingLastTime)  // 枚举值
        OOLUA_ENUM(iStdCheckLastTime)  // 枚举值
        OOLUA_ENUM(iStdCheckResult)  // 枚举值
        OOLUA_ENUM(iZeroCheckLastTime)  // 枚举值

        OOLUA_ENUM(iMeaAbsorbanceValue)  // 枚举值
        OOLUA_ENUM(iZeroCheckDensity)  // 枚举值
        OOLUA_ENUM(iZeroCheckOriginalValue)  // 枚举值
        OOLUA_ENUM(iZeroCheckDrift)  // 枚举值
        OOLUA_ENUM(iRangeCheckTime)  // 枚举值

        OOLUA_ENUM(iRangeCheckDensity)  // 枚举值
        OOLUA_ENUM(iRangeCheckOriginalValue)  // 枚举值
        OOLUA_ENUM(iRangeCheckDrift)  // 枚举值
        OOLUA_ENUM(iMeaOriginalValue)  // 枚举值
        OOLUA_ENUM(iAutoCalibrationInterval)  // 枚举值

        OOLUA_ENUM(iStd1AbsorbanceValue)  // 枚举值
        OOLUA_ENUM(iStd2AbsorbanceValue)  // 枚举值
        OOLUA_ENUM(iZeroAbsorbanceValue)  // 枚举值

        OOLUA_ENUM(iCorrectionK)  // 枚举值
        OOLUA_ENUM(iCorrectionB)  // 枚举值
        OOLUA_ENUM(iStdCheckTime)  // 枚举值
        OOLUA_ENUM(iStdCheckDensity)  // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(MonitoringInfoIndex)  // 导出函数声明（没有函数也需要声明）

namespace HJ212
{


HJ212PluginProxy::HJ212PluginProxy()
{

}

void HJ212PluginProxy::Proxy(OOLUA::Script *lua)
{
    lua->register_class<InfoType>();
    lua->register_class<MonitoringInfoIndex>();
}

}
