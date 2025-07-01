#if !defined (SETTING_SETTINGMANEGER_H_)
#define SETTING_SETTINGMANEGER_H_

#include <memory>
#include "LuaEngine/LuaEngine.h"
#include "System/Types.h"

namespace Configuration
{

/**
 * @brief 继电器功能
 */
enum  class RelayOut
{
    UpLimit = 0,             ///< 浓度上限
    LowLimit = 1,            ///< 浓度下限
    MeasureInstruct = 2,    ///< 测量指示
    CalibrateInstruct = 3,  ///< 校准指示
    CleanInstruct = 4,      ///< 清洗指示
};


/**
 * @brief 上报模式
 */
enum  class ReportMode
{
    OnLine = 0,
    OffLine = 1,
    Maintain = 2,
    Fault = 3,
    Calibrate = 4,
};

/**
 * @brief 设置IP模式
 */
enum  class SettingIPMode
{
    DHCP = 0,
    STATIC = 1,
};

/**
 * @brief 测量模式
 */
enum  class Language
{
    SimplifiedChinese = 0,
    TraditionalChinese = 1,
    English = 2,
};

/**
 * @brief 制造类型
 */
enum  class ManufactureType
{
    LS = 0,
    OEM = 1,
    DIY = 2,
};

/**
 * @brief 测量模式
 */
enum  class MeasureMode
{
    Trigger = 0,
    Periodic = 1,
    Timed = 2,
    Continous = 3,
    Manual = 4
};

/**
 * @brief 比较类型
 */
enum class CompareType
{
    EQ = 0,     ///<等于
    NE = 1,     ///<不等于
    LE = 2,     ///<小于或等于
    LT = 3,     ///<小于
    GE = 4,     ///<大于或等于
    GT = 5,     ///<大于
};

/**
 * @brief 配置文件管理
 */
class SettingManager
{
public:
    SettingManager();
    ~SettingManager();
    static SettingManager* Instance();
    bool Init(void);
    void Uninit(void);
    System::String GetValidInstrumentInfoArgus();
    System::String GetInstrumentInfoArgus(void);
    int GetParamType();
    System::String GetParamTypeName();
    bool IsProducerStatus(void);
    std::vector<bool> GetTimedPoint(void);
    bool SetTimedPoint(std::vector<bool> point);
    ManufactureType GetManufactureType();

    bool Save(Table const &table, System::String const & fileName, string tableName);
    bool LoadDefault(Table &table, System::String const & filePath, string tableName);
    bool AuthorizationSave();
    bool InstrumentInfoSave();
    bool LoadDefaultInstrumentInfo(Table  &table);
    bool MainBoardInfoSave();
    bool InterconnectionSave();
    bool SchedulerSave();
    bool RemainSave();
    bool ConsumableSave();
    bool MeasureStatusSave();
    Language GetAppLanguage();

private:
    static std::unique_ptr<SettingManager> m_instance;
    int m_instrumentType;
    ManufactureType m_manufactureType;
    Lua::LuaEngine* m_luaEngine;
};

}

#endif // SETTING_SETTINGMANEGER_H_
