#if !defined(RESULTMANAGER_RECORDFIELDS_H)
#define RESULTMANAGER_RECORDFIELDS_H

#include "System/Types.h"
#include "oolua.h"
#include "System/Version.h"
#include <map>
//#include "UI/Frame/LoginDialog.h"



namespace Result
{

/**
 * @brief 角色类型
 */
enum RoleType
{
    None = 0,
    General = 1,
    Maintain = 2,
    Administrator = 3,
    Super = 4,
};


/**
 * @brief 字段的数据类型
 */

enum class FieldType
{
    Bool = 0,                //1个字节，bool类型
    Byte = 1,                //1个字节, Byte类型
    Int = 2,                 //4个字节，int 类型
    Float = 3,               //4个字节, float 类型
    Double = 4,              //8个字节，double 类型
    IntArray = 5,            //4 * len 个字节，int 类型数组
    Time = 6,                //4个字节，int 类型，此字段保存的值为时间,需转换为字符串
    Enum = 7,                //1个字节，Byte类型，字段为枚举
};

/**
 * @brief 记录文件字段的信息
 */
struct RecordField
{
    System::String name;    //名字
    System::String text;    //中文名字
    int type; //数据类型
    int size;       //数据长度
    int start;      //位于一条记录的中位置
    System::String unit;
    std::vector<String> option;
    RoleType privilege;
};

/**
 * @brief 记录文件界面显示字段
 */
struct ShowField
{
    System::String name;    //名字
    System::String format;
};

/**
 * @brief 记录文件数据模板
 */
class RecordFields
{
public:
    RecordFields(System::String tablename);
    RecordFields(System::String tablename, System::Version version);
    RecordFields(const RecordFields &other);
    RecordFields& operator=(const RecordFields &other);
    bool GetField(System::String fieldName, RecordField& field);
    bool GetFieldType(System::String fieldName, FieldType& type);
    bool GetFieldOption(System::String fieldName, std::vector<String>& option);
    bool GetFieldUnit(System::String fieldName, System::String &unit);
    bool GetFieldText(System::String fieldName, System::String &text);
    bool GetFieldSize(System::String fieldName, System::Uint32 &size);
    System::Uint32 GetFieldsSize(void) const;
    System::Version GetVersion(void);
    bool GetNextField(RecordField& field);
    bool ResetFieldPos();

protected:
    std::map<System::String, RecordField> m_usedFields;
    //std::map<System::String, System::Uint32> m_filedPosMap;
    std::vector<RecordField>m_fields;
    System::Uint32 m_size;//一条数据的长度
    std::vector<RecordField>::iterator m_iter;
    System::Version m_version;
    OOLUA::Table LoadTable(System::String tablename);
    System::Uint32 LoadFields(OOLUA::Table table, std::vector<RecordField>& fields);
};
}
#endif // RESULTMANAGER_RECORDFIELDS_H
