#include "ADCManager.h"
#include "Log.h"
#include "ADC.h"
#include "System/Types.h"
#include "LuaEngine/LuaEngine.h"
#include "Setting/SettingManager.h"
#include "checkscreen.h"

using namespace std;
using namespace System;
using namespace OOLUA;
using namespace Lua;

namespace System
{

std::unique_ptr<ADCManager> ADCManager::m_instance(nullptr);

ADCManager::~ADCManager()
{
    if (true == CheckScreen::Instance()->isVNC())
    {
        return;
    }
    for(auto it = m_adcDetecters.begin(); it != m_adcDetecters.end();)
    {
        auto second = it->second;
        if(second != nullptr)
        {
            delete second;
            second = nullptr;

            m_adcDetecters.erase(it++);
        }
        delete it->second;
    }
    m_adcDetecters.clear();
}

/**
 * @brief 获取全局唯一的 WeepingDetector 对象（单间）。
 */
ADCManager* ADCManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new ADCManager);
    }
    return m_instance.get();
}

bool ADCManager::Init()
{
    if (true == CheckScreen::Instance()->isVNC())
    {
        return true;
    }
    #ifdef _DEV_USING_CONTROL

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();
        OOLUA::Script *lua = luaEngine->GetEngine();

        Table configTable;
        luaEngine->GetLuaValue(state, "config.system.adcDetect", configTable);

        int index = 0;
        oolua_ipairs(configTable)
        {
            Table itemTable;

            if(lua->pull(itemTable))
            {
                index = index + 1;
                bool enable;
                itemTable.at("enable", enable);

                if(enable)
                {
                    ADCDetecter* adcDetecter = new ADCDetecter(index);

                    m_adcDetecters.insert(make_pair(index, adcDetecter));
                }
            }
        }
        oolua_ipairs_end()
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ADCManager::Init => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ADCManager::Init => %s", e.what());
    }

    if (m_adcDetecters.size() > 0)
    {
        this->Start();
    }
    else
    {
        logger->warn("Not open analog detect function !!!");
        return false;
    }
    #endif
    return true;
}

bool ADCManager::Start()
{
    if (true == CheckScreen::Instance()->isVNC())
    {
        return true;
    }
    for(std::map<int, ADCDetecter*>::iterator it = m_adcDetecters.begin(); it != m_adcDetecters.end(); it++)
    {
        if(it->second->Init())
        {
            it->second->Start();
        }
    }
    return true;
}

void ADCManager::Stop()
{
    if (true == CheckScreen::Instance()->isVNC())
    {
        return;
    }
    for(std::map<int, ADCDetecter*>::iterator it = m_adcDetecters.begin(); it != m_adcDetecters.end(); it++)
    {
        it->second->Stop();
    }
}


}


