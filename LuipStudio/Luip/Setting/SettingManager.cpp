#include "Environment.h"
#include "SettingManager.h"
#include <QDebug>

using namespace std;
using namespace Lua;
using namespace OOLUA;

namespace Configuration
{

unique_ptr<SettingManager> SettingManager::m_instance(nullptr);

SettingManager::SettingManager(): m_instrumentType(0),m_manufactureType(ManufactureType::LS)
{
    m_luaEngine = nullptr;
}

SettingManager::~SettingManager()
{
}

SettingManager* SettingManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new SettingManager);
    }

    return m_instance.get();
}

bool SettingManager::Init(void)
{
    m_luaEngine = LuaEngine::Instance();
    lua_State* state = m_luaEngine->GetThreadState();

    int manufactureType = 0;
    m_luaEngine->GetLuaValue(state, "config.info.instrument.manufactureType", manufactureType);
    m_manufactureType = (ManufactureType)manufactureType;

    Table targetMap;
    m_luaEngine->GetLuaValue(state, "config.system.targetMap", targetMap);
    m_instrumentType = 0;
    oolua_pairs(targetMap)
    {
        int num = 0;

        Table target;
        OOLUA::pull(state, target);

        bool enable = false;
        target.at("enable", enable);

        if (enable)
        {
            target.at("num", num);
            m_instrumentType += num;
        }
    }
    oolua_pairs_end()
    return true;
}

void SettingManager::Uninit(void)
{

}

String SettingManager::GetInstrumentInfoArgus(void)
{
    lua_State* state = m_luaEngine->GetThreadState();
    String argusStr;
    Table argus;
    m_luaEngine->GetLuaValue(state, "setting.instrument.argus", argus);
    oolua_ipairs(argus)
    {
        String ret;
        OOLUA::pull(state, ret);
        argusStr += ret + ",";
    }
    oolua_ipairs_end()
    return argusStr;
}

String SettingManager::GetValidInstrumentInfoArgus()
{
    String result;
    QString argus = GetInstrumentInfoArgus().c_str();
    int paramType = GetParamType();
    QStringList argusList = argus.split(QRegExp("[,]"));
    int mask = 1;
    for (int i = 0; i < argusList.size(); i++)
    {
        if (mask & paramType)
        {
            QString str = argusList.at(i);
            str.remove(",");
            result += str.toStdString();
        }
        mask = mask << 1;
    }
    return result;
}

int SettingManager::GetParamType()
{
    return m_instrumentType;
}

ManufactureType SettingManager::GetManufactureType()
{
    return m_manufactureType;
}

String SettingManager::GetParamTypeName()
{
    String result;
    switch(m_instrumentType)
    {
    case 1:     //低配
        result.clear();
        break;
    case 3:     //高配
        result += QString("Plus").toStdString().data();
        break;
    default:
        result.clear();
        break;
    }
    return result;
}

bool SettingManager::IsProducerStatus(void)
{
    lua_State* state = m_luaEngine->GetThreadState();
    bool isEnable = false;
    Table configTable;
    m_luaEngine->GetLuaValue(state, "config", configTable);
    Table producerAuthorization;
    if (configTable.safe_at("producerAuthorization", producerAuthorization))
    {
        producerAuthorization.at("enable", isEnable);
    }
    return isEnable;
}

std::vector<bool> SettingManager::GetTimedPoint(System::String name)
{
    bool result;
    vector<bool> point;

    if (name.empty())
    {
        return point;
    }

    lua_State* state = m_luaEngine->GetThreadState();
    Table timedPoinTtable;
    m_luaEngine->GetLuaValue(state, name, timedPoinTtable);

    //从下标为1开始，按顺序把表中键为数字的值压人栈顶。直到当前下标的值为空，才结束循环
    oolua_ipairs(timedPoinTtable)
    {
        OOLUA::pull(state, result);
        point.push_back(result);
    }
    oolua_ipairs_end()
    return point;
}

bool SettingManager::SetTimedPoint(System::String name, vector<bool> point)
{
    bool result = false;

    if (name.empty())
    {
        return result;
    }

    lua_State* state = m_luaEngine->GetThreadState();
    Table timedPoinTtable;
    m_luaEngine->GetLuaValue(state, name, timedPoinTtable);

    //从下标为1开始，按顺序把表中键为数字的值压人栈顶。直到当前下标的值为空，才结束循环
    oolua_ipairs(timedPoinTtable)
    {
        lua_pop(state, 1);//把键为_i_index_的值弹出栈
        result = point[_i_index_ - 1];
        OOLUA::push(state, result);//把需要给键_i_index_赋值的值压人栈
        //把栈顶的值赋给索引_oolua_array_index_指向的表（即m_timedPoinTtable）的键_i_index_
        lua_rawseti(state, _oolua_array_index_, _i_index_);

        if (_i_index_ == (int)point.size())
        {
            break;
        }
    }
    oolua_ipairs_end()
    return true;
}

bool SettingManager::Save(Table const &table, String const & fileName, string tableName)
{
    lua_State* state = m_luaEngine->GetThreadState();
    OOLUA::Lua_function call(state);
    OOLUA::Lua_func_ref SaveEncryptionFile;

    m_luaEngine->GetLuaValue(state, "Serialization.SaveEncryptionFile", SaveEncryptionFile);
    return call(SaveEncryptionFile, table, fileName, tableName);
}

bool SettingManager::LoadDefault(Table &table, System::String const & filePath, string tableName)
{
    lua_State* state = m_luaEngine->GetThreadState();
    Script* lua = m_luaEngine->GetEngine();

    OOLUA::Lua_function call(state);
    OOLUA::Lua_func_ref LoadEncryptionFile;

    m_luaEngine->GetLuaValue(state, "Serialization.LoadEncryptionFile", LoadEncryptionFile);
    call(LoadEncryptionFile, filePath, tableName);
    lua->pull(table);
}

bool SettingManager::AuthorizationSave()
{
    lua_State* state = m_luaEngine->GetThreadState();
    Table table;
    m_luaEngine->GetLuaValue(state, "config.authorization", table);
    String fileName = Environment::Instance()->GetAppPath() + "/SP/Config/Authorization.ls";
    return Save(table, fileName, "config.authorization");
}

bool SettingManager::InstrumentInfoSave()
{
    lua_State* state = m_luaEngine->GetThreadState();
    Table table;
    m_luaEngine->GetLuaValue(state, "config.info.instrument", table);
    String fileName = Environment::Instance()->GetDeviceDataPath()  + "/InstrumentInfomation.ls";
    return Save(table, fileName, "config.info.instrument");
}

bool SettingManager::LoadDefaultInstrumentInfo(Table &table)
{
    lua_State* state = m_luaEngine->GetThreadState();

    String fileName = Environment::Instance()->GetDefaultConfigPath()  + "/InstrumentInfomation.ls";

    return LoadDefault(table, fileName, "config.info.instrument");
}

bool SettingManager::MainBoardInfoSave()
{
    lua_State* state = m_luaEngine->GetThreadState();
    Table table;
    m_luaEngine->GetLuaValue(state, "config.info.mainBoard", table);
    String fileName = Environment::Instance()->GetDeviceDataPath()  + "/MainBoardInfomation.ls";
    return Save(table, fileName, "config.info.mainBoard");
}

bool SettingManager::InterconnectionSave()
{
    lua_State* state = m_luaEngine->GetThreadState();
    Table table;
    m_luaEngine->GetLuaValue(state, "config.interconnection", table);
    String fileName = Environment::Instance()->GetAppPath() + "/SP/Config/InterconnectionConfig.ls";

    OOLUA::Lua_func_ref modifyRecordFunc;
    OOLUA::Lua_function call(state);
    m_luaEngine->GetLuaValue(state, "config.modifyRecord.interconnection", modifyRecordFunc);
    call(modifyRecordFunc, true);
    return Save(table, fileName, "config.interconnection");
}

bool SettingManager::SchedulerSave()
{
    lua_State* state = m_luaEngine->GetThreadState();
    Table table;
    m_luaEngine->GetLuaValue(state, "config.scheduler", table);
    String fileName = Environment::Instance()->GetAppPath() + "/SP/Config/SchedulerConfig.ls";

    OOLUA::Lua_func_ref modifyRecordFunc;
    OOLUA::Lua_function call(state);
    m_luaEngine->GetLuaValue(state, "config.modifyRecord.scheduler", modifyRecordFunc);
    call(modifyRecordFunc, true);
    return Save(table, fileName, "config.scheduler");
}

bool SettingManager::RemainSave()
{
    lua_State* state = m_luaEngine->GetThreadState();
    Table table;
    m_luaEngine->GetLuaValue(state, "config.remain", table);
    String fileName = Environment::Instance()->GetAppPath()  + "/SP/Config/RemainConfig.ls";
    return Save(table, fileName, "config.remain");
}

bool SettingManager::ConsumableSave()
{
    lua_State* state = m_luaEngine->GetThreadState();
    Table table;
    m_luaEngine->GetLuaValue(state, "config.consumable", table);
    String fileName = Environment::Instance()->GetAppPath()  +  "/SP/Config/ConsumableConfig.ls";
    return Save(table, fileName, "config.consumable");
}

bool SettingManager::MeasureStatusSave()
{
    lua_State* state = m_luaEngine->GetThreadState();
    Table table;
    m_luaEngine->GetLuaValue(state, "status.measure", table);
    String fileName = Environment::Instance()->GetAppPath()  + "/SP/Config/MeasureStatus.ls";
    return Save(table, fileName, "status.measure");
}

Language SettingManager::GetAppLanguage()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    Table table;
    luaEngine->GetLuaValue(state, "config.system", table);
    int language = 0;
    table.safe_at("language", language);

    return (Language)language;
}

bool SettingManager::MeasureParamSave()
{
    lua_State* state = m_luaEngine->GetThreadState();
    Table table;
    m_luaEngine->GetLuaValue(state, "config.measureParam", table);
    String fileName = Environment::Instance()->GetAppPath() + "/SP/Config/MeasureParamConfig.ls";

    OOLUA::Lua_func_ref modifyRecordFunc;
    OOLUA::Lua_function call(state);
    m_luaEngine->GetLuaValue(state, "config.modifyRecord.measureParam", modifyRecordFunc);
    call(modifyRecordFunc, true);
    return Save(table, fileName, "config.measureParam");
}


}

