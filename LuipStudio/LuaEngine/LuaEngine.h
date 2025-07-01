#if !defined(LUA_ENGINE_H)
#define LUA_ENGINE_H

#include <memory>
#include <map>
#include <list>
#include <QRegExp>
#include <QThread>
#include <QStringList>
#include <QMutexLocker>
#include "Log.h"
#include "oolua.h"
#include "LuipShare.h"

using std::list;
using std::string;
using OOLUA::Table;

namespace Lua
{

class LUIP_SHARE LuaEngine
{
    struct Fields
    {
        string name;
        list<string> index;
    };

public:
    ~LuaEngine();
    static LuaEngine* Instance();

public:
    bool Init(log4cpp::Category* log);
    void Uninit();
    bool Start();
    bool LoadPluginScript();
    bool LoadInstrumentScript();

    int RunParser(int argc, char **argv);

    OOLUA::Script* GetEngine() { return m_engine;}
    lua_State* GetMainState() { return m_lua;}

    lua_State* GetThreadState();
    void AttachCurrentThread(lua_State* state);
    void DetachCurrentThread();
    static lua_State * CreateThread();

    bool SetLuaValue(std::string const &key, std::string const& value);
    bool SetLuaValue(std::string const &key, bool const& value);
    bool SetLuaValue(std::string const &key, double const& value);
    bool SetLuaValue(std::string const &key, float const& value);
    bool SetLuaValue(std::string const &key, Table const& value);
    bool SetLuaValue(std::string const &key, unsigned int const& value);
    bool SetLuaValue(std::string const &key, int const& value);
    bool SetLuaValue(std::string const &key, unsigned char const& value);

    bool RunChunk(std::string const& chunk);

    template<typename T>void GetLuaValue(lua_State* state, std::string path, T& targe)
    {
        QMutexLocker locker(&m_readLuaMutex);

        OOLUA::Table pTable, nTable;

        list<Fields> fieldslist;
        FieldsAnalysis(path, fieldslist);

        // 取出最后一级的键值字符，从列表中删除该字段
        Fields lastFields = fieldslist.back();
        fieldslist.pop_back();

        try
        {
            if (!fieldslist.empty())
            {
                Fields fields = fieldslist.front();
                OOLUA::get_global(state, fields.name.c_str(), pTable);
                if (!fields.index.empty())
                {
                    KeyValueAnalysis(pTable, fields, nTable);
                    pTable = nTable;
                }
                
                fieldslist.pop_front();

                if (!fieldslist.empty())
                {
                    for (list<Fields>::iterator it = fieldslist.begin(); 
                         it != fieldslist.end(); it++)
                    {
                        pTable.at((*it).name.c_str(), nTable);

                        if (!(*it).index.empty())
                        {
                            KeyValueAnalysis(nTable, (*it), pTable);
                        }
                        else
                        {
                            pTable = nTable;
                        }
                    }
                }

                //获取最后一级字段的值
                if (!lastFields.index.empty())
                {
                    pTable.at(lastFields.name.c_str(), nTable);
                    KeyValueAnalysis(nTable, lastFields, targe);
                }
                else
                {
                    pTable.at(lastFields.name.c_str(), targe);
                }
            }
            else
            {
                Fields fields = lastFields;
                OOLUA::get_global(state, fields.name.c_str(), pTable);

                if (!fields.index.empty())
                {
                    KeyValueAnalysis(pTable, fields, targe);
                }
                else
                {
                    OOLUA::get_global(state, fields.name.c_str(), targe);
                }
            }
        }
        catch(OOLUA::Exception e)
        {
            logger->warn("LuaEngine::GetLuaValue() Thread[%d] lua_State[%X] path %s => %s", (int)QThread::currentThreadId(), state, path.c_str(), e.what());
        }
        catch(std::exception e)
        {
            logger->warn("LuaEngine::GetLuaValue()Thread[%d] lua_State[%X] path %s => %s", (int)QThread::currentThreadId(), state, path.c_str(), e.what());
        }
    }

private:
    void FieldsAnalysis(string strPath, list<Fields> & lists);
    std::list<std::string> KeyAnalysis(std::string const &key);
    void LoadKeyPreviousTable(std::list<std::string> lists, Table &table);
    template<typename T>bool Set(std::string const &key, T const& value);

    template<typename T>void KeyValueAnalysis(OOLUA::Table preTable, Fields fields, T& value)
    {        
        QRegExp rx("^[0-9]*$");

        string lastIndex = fields.index.back();
        fields.index.pop_back();

        if (!fields.index.empty())
        {
            OOLUA::Table table;

            for(list<string>::iterator it = fields.index.begin();
                it != fields.index.end(); it++)
            {
                if (rx.exactMatch(QString((*it).c_str())))
                {
                    int num = std::atoi((*it).c_str());
                    preTable.at(num, table);
                }
                else
                {
                    preTable.at((*it).c_str(), table);
                }

                preTable = table;
            }
        }

        if (rx.exactMatch(QString(lastIndex.c_str())))
        {
            int num = std::atoi(lastIndex.c_str());
            preTable.at(num, value);
        }
        else
        {
            preTable.at(lastIndex.c_str(), value);
        }
    }

private:
    LuaEngine();
    static int Panic(lua_State *L);

    static std::unique_ptr<LuaEngine> m_instance;
    std::map<int, lua_State* > m_threaLuaMap;

    OOLUA::Script* m_engine;
    lua_State* m_lua;
    QMutex m_readLuaMutex;
    QMutex m_writeLuaMutex;
};

}
#endif  //LUA_ENGINE_H
