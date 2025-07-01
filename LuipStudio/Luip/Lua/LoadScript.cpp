
#include "Setting/Environment.h"
#include "LuaEngine/LuaEngine.h"
#include "Log.h"
#include <thread>
#include "LoadScript.h"

using namespace std;
using namespace OOLUA;
using namespace Configuration;
namespace Lua
{
unique_ptr<LoadScript> LoadScript::m_instance(nullptr);

LoadScript::LoadScript()
{

}

LoadScript::~LoadScript()
{

}

LoadScript* LoadScript::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new LoadScript);
    }

    return m_instance.get();
}


bool LoadScript::LoadInitScript()
{
    // 运行初始化脚本
    Script *lua = LuaEngine::Instance()->GetEngine();

    String file = Environment::Instance()->GetAppPath() + "/SP/Init.ls";
    if (false == lua->run_file(file))
    {
        logger->warn("加载 Init.ls失败");
        return false;
    }
    String defaultPath = Environment::Instance()->GetAppPath() + "/SP";
    Table configLists;
    configLists.bind_script(*lua);
    configLists.set_table("ConfigLists");
    Lua_func_ref configListsFunc;
    configLists.at("Init",configListsFunc);
    return lua->call(configListsFunc, defaultPath, Environment::Instance()->GetDeviceDataPath());
}

bool LoadScript::LoadPluginScript()
{
    // 运行初始化脚本
    String file = Environment::Instance()->GetAppPath() + "/SP/PluginLoader.ls";
    if (false == LuaEngine::Instance()->GetEngine()->run_file(file))
    {
        logger->warn("加载 PluginLoader.ls失败");
        return false;
    }
    return true;
}

bool LoadScript::LoadInstrumentScript()
{
    // 运行初始化脚本
    String file = Environment::Instance()->GetAppPath() + "/SP/InstrumentLoader.ls";
    if (false == LuaEngine::Instance()->GetEngine()->run_file(file))
    {
        logger->warn("加载 InstrumentLoader.ls失败");
        return false;
    }
    return true;
}

bool LoadScript::InitWidgetUserConfig()
{
    // 初始化界面中用户自定义的配置

    bool ret = false;

    Script *lua = LuaEngine::Instance()->GetEngine();

    OOLUA::Lua_func_ref initWidgetUserConfig;
    Table _G;

    try
    {
        _G.bind_script(*lua);
        _G.set_table("_G");

        ret = _G.safe_at("InitWidgetUserConfig", initWidgetUserConfig);

        if (ret)
        {
            lua->call(initWidgetUserConfig);
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("LoadScript::InitInstrument => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("LoadScript::InitInstrument() => %s", e.what());
    }

    return ret;
}

bool LoadScript::InitInstrument()
{
    // 初始化脚本业务

    bool ret = false;

    Script *lua = LuaEngine::Instance()->GetEngine();

    OOLUA::Lua_func_ref initInstrument;
    Table _G;

    try
    {
        _G.bind_script(*lua);
        _G.set_table("_G");

        ret = _G.safe_at("InitInstrument", initInstrument);

        if (ret)
        {
            lua->call(initInstrument);
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("LoadScript::InitInstrument => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("LoadScript::InitInstrument() => %s", e.what());
    }

    return ret;
}

}
