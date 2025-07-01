#include "LuaEngine.h"
#include "luacmd.h"
#include "Log.h"
#include <thread>

using namespace std;
using namespace OOLUA;

namespace Lua
{
unique_ptr<LuaEngine> LuaEngine::m_instance(nullptr);

LuaEngine::LuaEngine()
{
    m_engine = nullptr;
    m_lua = nullptr;
}

int LuaEngine::Panic(lua_State *L)
{
    logger->error("PANIC: (%X)unprotected error in call to Lua API (%s)\n",L,
                                       lua_tostring(L, -1));
    return 0;
}

LuaEngine::~LuaEngine()
{
    if (m_engine)
    {
        delete m_engine;
        m_engine = nullptr;
        m_lua = nullptr;
    }
}

LuaEngine* LuaEngine::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new LuaEngine);
    }

    return m_instance.get();
}

bool LuaEngine::Init(log4cpp::Category* log)
{
    logger = log;

    m_engine = new Script;
    m_lua = m_engine->state();
    lua_atpanic(m_lua, &Panic);
    this->AttachCurrentThread(m_lua);

    return true;
}

void LuaEngine::Uninit()
{

}

int LuaEngine::RunParser(int argc, char **argv)
{
#ifdef _CS_X86_WINDOWS
    thread luaThread(luacmd_main, m_lua, argc, argv);
    luaThread.detach();
#endif

    return 0;
}

lua_State *LuaEngine::GetThreadState()
{
    lua_State * state = nullptr;

    int id = (int)QThread::currentThreadId();

    std::map<int, lua_State* >::iterator it=m_threaLuaMap.find(id);
    if (it != m_threaLuaMap.end())
    {
        state = it->second;
    }

    if (state == nullptr)
    {
        state = CreateThread();
        this->AttachCurrentThread(state);

        logger->debug("Thread[%d] not state, new Thread State[%X] ", id, state);
    }

    return state;
}

void LuaEngine::AttachCurrentThread(lua_State *state)
{
    int id = (int)QThread::currentThreadId();

    if (m_threaLuaMap.find(id) == m_threaLuaMap.end())
    {
        m_threaLuaMap.insert(make_pair(id, state));
//        logger->debug("thread[%d] attach lua_State[%X]", id, state);
    }
}

void LuaEngine::DetachCurrentThread()
{
    int id = (int)QThread::currentThreadId();

    std::map<int, lua_State* >::iterator it=m_threaLuaMap.find(id);
    if (it != m_threaLuaMap.end())
    {
//        logger->debug("Thread[%d] detach lua_State[%X]", id, it->second);
        m_threaLuaMap.erase(it);
    }
}

lua_State *LuaEngine::CreateThread()
{
    lua_State *L = lua_newthread(LuaEngine::Instance()->GetMainState());

    if (L)
    {
        logger->debug("Thread[%d] New lua_State[%X]", (int)QThread::currentThreadId(), L);
    }
    else
    {
        logger->warn("LuaEngine::CreateThread() => new thread file");
    }

    return L;
}

void LuaEngine::FieldsAnalysis(string strPath, list<LuaEngine::Fields> &lists)
{
    QString path(strPath.c_str());
    QStringList strlist = path.split(QRegExp("[.]"));

    for (int i = 0; i < strlist.size(); i++)
    {
        Fields fields;

        string name = strlist.at(i).toStdString();

        std::size_t sPos = name.find("[");
        std::size_t ePos = name.find("]");

        if (sPos != string::npos && ePos != string::npos)
        {
            fields.name = name.substr(0, sPos);
            fields.index.push_back(name.substr(sPos + 1, ePos - sPos - 1));

            while(true)
            {
                sPos = name.find("[", ePos);

                if (sPos != string::npos)
                {
                    ePos = name.find("]", sPos);

                    if (ePos != string::npos)
                    {
                        fields.index.push_back(name.substr(sPos + 1, ePos - sPos - 1));
                    }
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            fields.name = name;
        }

        lists.push_back(fields);
    }
}

std::list<std::string> LuaEngine::KeyAnalysis(std::string const &key)
{
    std::list<std::string> lists;

    QString qkey(key.c_str());
    QStringList strlist = qkey.split(QRegExp("[.]"));

    for (int i = 0; i < strlist.size(); i++)
    {
        string name = strlist.at(i).toStdString();
        std::size_t sPos = name.find("[");
        std::size_t ePos = name.find("]");
        if (sPos != string::npos && ePos != string::npos)
        {
            lists.push_back(name.substr(0, sPos));
            lists.push_back(name.substr(sPos + 1, ePos - sPos - 1));
            while(true)
            {
                sPos = name.find("[", ePos);
                if (sPos != string::npos)
                {
                    ePos = name.find("]", sPos);
                    if (ePos != string::npos)
                    {
                        lists.push_back(name.substr(sPos + 1, ePos - sPos - 1));
                    }
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            lists.push_back(name);
        }
    }
    return lists;
}

void LuaEngine::LoadKeyPreviousTable(std::list<std::string> lists, Table &table)
{
    if (lists.size() > 1)
    {
        OOLUA::Table pTable, nTable;
        lua_State* state = GetThreadState();
        string name = lists.front();
        QRegExp rx("^[0-9]*$");

        OOLUA::get_global(state, name.c_str(), pTable);
        lists.pop_front();

        while(lists.size() > 1)
        {
            name = lists.front();
            lists.pop_front();

            if (rx.exactMatch(QString(name.c_str())))
            {
                int num = std::atoi(name.c_str());
                pTable.at(num, nTable);
            }
            else
            {
                pTable.at(name, nTable);
            }
            pTable = nTable;
        }
        table = pTable;
    }
}

template<typename T>bool LuaEngine::Set(std::string const &key, T const& value)
{
    QMutexLocker locker(&m_writeLuaMutex);

    bool ret = false;
    try
    {
        lua_State* state = GetThreadState();

        std::list<std::string> lists = KeyAnalysis(key);
        string name = lists.back();
        Table table;
        QRegExp rx("^[0-9]*$");

        if (lists.size() > 1)
        {
            LoadKeyPreviousTable(lists, table);
            if (rx.exactMatch(QString(name.c_str())))
            {
                int num = std::atoi(name.c_str());
                table.set(num, value);
            }
            else
            {
                table.set(name, value);
            }
            ret = true;
        }
        else
        {
            ret = OOLUA::set_global(state, name.c_str(), value);
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("LuaEngine::Set() key %s => %s", key.c_str(), e.what());
    }
    catch(std::exception e)
    {
        logger->warn("LuaEngine::Set() key %s => %s", key.c_str(), e.what());
    }
    return ret;
}

bool LuaEngine::SetLuaValue(std::string const &key, std::string const& value)
{
    return Set(key, value);
}

bool LuaEngine::SetLuaValue(std::string const &key, bool const& value)
{
    return Set(key, value);
}

bool LuaEngine::SetLuaValue(std::string const &key, double const& value)
{
    return Set(key, value);
}

bool LuaEngine::SetLuaValue(std::string const &key, float const& value)
{
    return Set(key, value);
}

bool LuaEngine::SetLuaValue(std::string const &key, Table const& value)
{
    return Set(key, value);
}

bool LuaEngine::SetLuaValue(std::string const &key, unsigned int const& value)
{
    return Set(key, value);
}

bool LuaEngine::SetLuaValue(std::string const &key, int const& value)
{
    return Set(key, value);
}

bool LuaEngine::SetLuaValue(std::string const &key, unsigned char const& value)
{
    return Set(key, value);
}

bool LuaEngine::RunChunk(std::string const& chunk)
{
    bool ret = false;
    QString qchunk(chunk.c_str());
    QStringList strlist = qchunk.split(QRegExp("[=]"));
    if (strlist.size() == 2)
    {
        std::string key = strlist.at(0).trimmed().toStdString();
        QString value = strlist.at(1).trimmed();

        QRegExp integerRx("^-?[0-9]*$");
        QRegExp decimalRx("^-?[0-9]*\\.[0-9]*$");

        if (integerRx.exactMatch(value))
        {
            int result = value.toInt();
            ret = SetLuaValue(key, result);
        }
        else if (decimalRx.exactMatch(value))
        {
            double result = value.toDouble();
            ret = SetLuaValue(key, result);
        }
        else if (value == "true")
        {
            ret = SetLuaValue(key, true);
        }
        else if (value == "false")
        {
            ret = SetLuaValue(key, false);
        }
        else if (value.startsWith("\"") && value.endsWith("\""))
        {
            string str = value.remove("\"").toStdString();
            ret = SetLuaValue(key, str);
        }
    }
    return ret;
}

}
