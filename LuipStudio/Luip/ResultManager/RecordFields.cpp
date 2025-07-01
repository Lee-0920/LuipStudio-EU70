#include "RecordFields.h"
#include "LuaEngine/LuaEngine.h"
#include "Log.h"

using namespace std;
using namespace OOLUA;
using namespace Lua;
using namespace System;
using namespace Controller::API;

namespace Result
{
/**
 * @brief 返回加载到的子表,此时得到的table是包含所有的数据模板
 * @param tablename
 */
Table RecordFields::LoadTable(String tablename)
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    Table resultTable;
    luaEngine->GetLuaValue(state, tablename, resultTable);

    return resultTable;
}

/**
 * @brief 迭代表 table(此表必须是单个数据模板),加载表的所有字段信息到容器 fields, 并返回该整个字段的长度
 */
Uint32 RecordFields::LoadFields(Table table, std::vector<RecordField>& fields)
{
    lua_State * newState = LuaEngine::Instance()->GetThreadState();

    Uint32 size = 0;
    //加载数据模板的每个字段
    Table fieldTable;

    oolua_ipairs(table)
    {
        RecordField field;

        OOLUA::pull(newState, fieldTable);

        fieldTable.at("name", field.name);
        fieldTable.at("text", field.text);
        fieldTable.at("type", field.type);
        fieldTable.at("size", field.size);
        fieldTable.at("privilege", field.privilege);
        fieldTable.safe_at("unit", field.unit);
        field.start = size;
        size += field.size;
        if ((FieldType)field.type == FieldType::Enum)
        {
            Table option;
            String str;
            fieldTable.at("option", option);
            oolua_ipairs(option)
            {
                pull(newState, str);
                field.option.push_back(str);
            }
            oolua_ipairs_end()
        }
        fields.push_back(field);
    }
    oolua_ipairs_end()
    return size;
}

/**
 * @brief 获取 m_table(此表必须是单个数据模板)里的版本号
 * @return
 */
Version RecordFields::GetVersion(void)
{
    return m_version;
}

/**
 * @brief 解析表名，获取当前使用的数据模板
 * @param tablename 数据模板表名，包含其所属的表
 */
RecordFields::RecordFields(String  tablename):
    m_size(0)
{
    Table table, fileFormatTable;

    fileFormatTable = LoadTable(tablename);

    Uint16 current;
    fileFormatTable.at("current", current);//加载当前使用的数据模板的序号
    fileFormatTable.at(current, table);//加载数据模板表
    fileFormatTable = table;

    int num;
    Table versionTable;
    fileFormatTable.at("version", versionTable);
    versionTable.at(1, num);
    m_version.SetMajor(num);
    versionTable.at(2, num);
    m_version.SetMinor(num);
    versionTable.at(3, num);
    m_version.SetRevision(num);
    versionTable.at(4, num);
    m_version.SetBuild(num);

    m_size = LoadFields(fileFormatTable, m_fields);
    ResetFieldPos();
}

/**
 * @brief 解析表名，获取指定版本号的数据模板
 * @param tablename
 * @param version
 */
RecordFields::RecordFields(String tablename, Version version):
    m_size(0)
{
    lua_State * newState = LuaEngine::Instance()->GetThreadState();
    Table table, fileFormatTable;

    fileFormatTable = LoadTable(tablename);

    //查询所有数据模板，找对应版本号的数据模板
    Table fieldsTable;
    Version versiontemp;
    Table versionTable;
    int num;
    bool ret = false;
    oolua_ipairs(fileFormatTable)
    {
        OOLUA::pull(newState, fieldsTable);
        fieldsTable.at("version", versionTable);

        versionTable.at(1, num);
        versiontemp.SetMajor(num);

        versionTable.at(2, num);
        versiontemp.SetMinor(num);

        versionTable.at(3, num);
        versiontemp.SetRevision(num);

        versionTable.at(4, num);
        versiontemp.SetBuild(num);

        if (versiontemp == version)
        {
            ret = true;
            break;
        }
    }
    oolua_ipairs_end()

    if (ret)
    {
        m_version = version;
        fileFormatTable = fieldsTable;
    }
    else
    {
        Uint16 current;
        fileFormatTable.at("current", current);//加载当前使用的数据模板的序号
        fileFormatTable.at(current, table);//加载数据模板表
        fileFormatTable = table;

        int num;
        Table versionTable;
        fileFormatTable.at("version", versionTable);
        versionTable.at(1, num);
        m_version.SetMajor(num);
        versionTable.at(2, num);
        m_version.SetMinor(num);
        versionTable.at(3, num);
        m_version.SetRevision(num);
        versionTable.at(4, num);
        m_version.SetBuild(num);
    }

    m_size = LoadFields(fileFormatTable, m_fields);
    ResetFieldPos();
}

RecordFields::RecordFields(const RecordFields &other)
{
    this->m_fields = other.m_fields;
    this->m_size = other.m_size;
    this->m_version = other.m_version;
    ResetFieldPos();
}

RecordFields& RecordFields::operator=(const RecordFields &other)
{
    this->m_fields = other.m_fields;
    this->m_size = other.m_size;
    this->m_version = other.m_version;
    ResetFieldPos();
    return *this;
}

bool RecordFields::GetField(String fieldName, RecordField& field)
{ 
    bool ret = false;
    if (m_usedFields.count(fieldName))
    {
        field = m_usedFields.at(fieldName);
        ret = true;
    }
    else
    {
        for (vector<RecordField>::iterator iter = m_fields.begin(); iter != m_fields.end(); ++iter)
        {
            if (iter->name == fieldName)
            {
                field = *iter;
                m_usedFields.insert(make_pair(fieldName, field));
                ret = true;
                break;
            }
        }
    }
    return ret;
}

bool RecordFields::GetFieldType(System::String fieldName, FieldType& type)
{
    bool ret = false;
    RecordField field;
    if (GetField(fieldName, field))
    {
        type = (FieldType)field.type;
        ret = true;
    }
    return ret;
}

bool RecordFields::GetFieldOption(System::String fieldName, std::vector<String>& option)
{
    bool ret = false;
    RecordField field;
    if (GetField(fieldName, field))
    {
        option = field.option;
        ret = true;
    }
    return ret;
}

bool RecordFields::GetFieldUnit(System::String fieldName, String &unit)
{
    bool ret = false;
    RecordField field;
    if (GetField(fieldName, field))
    {
        unit = field.unit;
        ret = true;
    }
    return ret;
}

bool RecordFields::GetFieldSize(System::String fieldName, System::Uint32 &size)
{
    bool ret = false;
    RecordField field;
    if (GetField(fieldName, field))
    {
        size = field.size;
        ret = true;
    }
    return ret;
}

bool RecordFields::GetFieldText(System::String fieldName, System::String &text)
{
    bool ret = false;
    RecordField field;
    if (GetField(fieldName, field))
    {
        text = field.text;
        ret = true;
    }
    return ret;
}

/**
 * @brief 获取当前数据模板的总长度
 * @return
 */
Uint32 RecordFields::GetFieldsSize(void) const
{
    return m_size;
}

bool RecordFields::GetNextField(RecordField& field)
{
    if (m_iter != m_fields.end())
    {
        field = *m_iter;
        ++m_iter;
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordFields::ResetFieldPos()
{
    m_iter = m_fields.begin();
    return true;
}

}
