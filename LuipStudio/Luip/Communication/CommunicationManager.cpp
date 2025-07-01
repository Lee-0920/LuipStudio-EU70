/**
 * @file
 * @brief 液路控制器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#include "Log.h"
#include "oolua.h"
#include "LuaEngine/LuaEngine.h"
#include "Communication/CommunicationLibrary.h"
#include "Controller/ControllerManager.h"
#include "CommunicationManager.h"
#include "DevCmdTable.h"
#include "Setting/Environment.h"

using namespace std;
using namespace OOLUA;
using namespace Lua;
using namespace Controller;
using namespace Configuration;


namespace Communication
{

unique_ptr<CommunicationManager> CommunicationManager::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 ControlNetwork 对象（单件）。
 */
CommunicationManager::~CommunicationManager()
{

}

CommunicationManager* CommunicationManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new CommunicationManager);
    }

    return m_instance.get();
}

void CommunicationManager::Proxy()
{
    CommunicationLibrary::Instance()->Proxy();
}

bool CommunicationManager::LoadAddrScript()
{
    bool ret = false;

    // 加载驱动控制相关脚本
    String qfile = Environment::Instance()->GetAppPath() + "/SP/Setting/Controllers.ls";
    LuaEngine::Instance()->GetEngine()->run_file(qfile);

    CommunicationLibrary* cl = CommunicationLibrary::Instance();

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        Script* lua = luaEngine->GetEngine();
        lua_State * state = luaEngine->GetThreadState();

        Table controllersTable;
        luaEngine->GetLuaValue(state, "setting.plugins.controllers", controllersTable);

        oolua_ipairs(controllersTable)
        {
            Table tempTable, addrTable;
            lua->pull(tempTable);
            tempTable.at("address", addrTable);

            oolua_ipairs(addrTable)
            {
                int a = 0;
                lua->pull(a);
                if (_i_index_ == 3)//令牌只取地址的第三个数
                {
                    cl->AddSlaveNode(a);
                }
            }
            oolua_ipairs_end()
        }
        oolua_ipairs_end()
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("CommunicationManager::Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("CommunicationManager::Init() => %s", e.what());
    }
    return ret;
}

bool CommunicationManager::Init()
{
    bool ret = false;

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String bridgeIP;
    luaEngine->GetLuaValue(state, "config.system.bridgeIP", bridgeIP);
    bool bridgeMode;
    luaEngine->GetLuaValue(state, "config.system.bridgeMode", bridgeMode);

    CommunicationLibrary* cl = CommunicationLibrary::Instance();
    cl->Init(logger);
    ret = cl->InitDncpStack(nullptr, bridgeMode, bridgeIP.c_str(), this);
    cl->SetDncpMcStackCmdTable(DevCmdTable_GetTable(), DevCmdTable_GetVersion());

    return ret;
}

void CommunicationManager::Uninit()
{
    CommunicationLibrary::Instance()->Uninit();
}

void CommunicationManager::OnConnectStatusChanged(unsigned char pollingAddr, bool status)
{
    ControllerManager::Instance()->ControllerConnectStatusChanged(pollingAddr, status);
}

}
