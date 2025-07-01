#include "SettingManagerProxy.h"
#include "SettingManager.h"

using namespace Configuration;
using namespace OOLUA;

Script* SettingManagerProxy::m_lua;

/**
 * @brief 语言类型
 */
OOLUA_PROXY(,Language)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(SimplifiedChinese)
        OOLUA_ENUM(TraditionalChinese)
        OOLUA_ENUM(English)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(Language)

/**
 * @brief 制造类型
 */
OOLUA_PROXY(,ManufactureType)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(LS)
        OOLUA_ENUM(OEM)
        OOLUA_ENUM(DIY)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(ManufactureType)

/**
 * @brief 测量模式
 */
OOLUA_PROXY(,MeasureMode)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Trigger)
        OOLUA_ENUM(Periodic)
        OOLUA_ENUM(Timed)
        OOLUA_ENUM(Continous)
        OOLUA_ENUM(Manual)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(MeasureMode)

/**
 * @brief 继电器功能
 */
OOLUA_PROXY(,RelayOut)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(UpLimit)
        OOLUA_ENUM(LowLimit)
        OOLUA_ENUM(MeasureInstruct)
        OOLUA_ENUM(CalibrateInstruct)
        OOLUA_ENUM(CleanInstruct)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(RelayOut)

/**
 * @brief 上报模式
 */
OOLUA_PROXY(,ReportMode)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(OnLine)
        OOLUA_ENUM(OffLine)
        OOLUA_ENUM(Maintain)
        OOLUA_ENUM(Fault)
        OOLUA_ENUM(Calibrate)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(ReportMode)


/**
 * @brief 设置IP模式
 */
OOLUA_PROXY(,SettingIPMode)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(DHCP)
        OOLUA_ENUM(STATIC)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(SettingIPMode)

/**
 * @brief 比较类型
 */
OOLUA_PROXY(,CompareType)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(EQ)
        OOLUA_ENUM(NE)
        OOLUA_ENUM(LE)
        OOLUA_ENUM(LT)
        OOLUA_ENUM(GE)
        OOLUA_ENUM(GT)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(CompareType)

void SettingManagerProxy::Proxy(Script * lua)
{
    m_lua = lua;

    m_lua->register_class<Language>();
    m_lua->register_class<ManufactureType>();
    m_lua->register_class<MeasureMode>();
    m_lua->register_class<RelayOut>();
    m_lua->register_class<ReportMode>();
    m_lua->register_class<SettingIPMode>();
    m_lua->register_class<CompareType>();
}
