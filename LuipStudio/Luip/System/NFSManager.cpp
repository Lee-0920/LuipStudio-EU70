#include "NFSManager.h"
#include "LuaEngine/LuaEngine.h"
#include "UdpPlugin/UdpManager.h"

using namespace OOLUA;
using namespace Lua;
using namespace System;

#define LOCAL_MOUNT_PATH "/mnt/udisk"
#define REMOTE_MOUNT_PATH "/nfs"


NFSManager::NFSManager()
{
    m_remoteIP = "";
    m_isEnable = false;
}

bool NFSManager::IsNFSEnable()
{
    return m_isEnable;
}

bool NFSManager::IsConnected()
{
    return m_client.IsConnected();
}

void NFSManager::Init()
{
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        std::string addr;
        luaEngine->GetLuaValue(state, "config.system.nfsEnable", m_isEnable);
        luaEngine->GetLuaValue(state, "config.system.nfsAddr", addr);
        m_remoteIP = QString::fromStdString(addr);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("NFSManager::NFSManager() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("NFSManager::NFSManager() => %s", e.what());
    }

    trLogger->debug("NFSManager::Init enable[%s] remoteIp[%s]!", m_isEnable ? "true" : "false", m_remoteIP.toStdString().c_str());

    if (m_remoteIP.isEmpty())
        trLogger->warn("NFSManager::Init remoteIp is empty!");

    m_client.Init();

    auto onRefreshParams = [=](QString& remoteIP, QString& remotePath, QString& localPath, bool& connect)
    {
        bool open = true;
        auto server = UdpSpace::UdpManager::Instance()->GetNFSServer(open);
        if (!server.isEmpty() &&
            (server != m_remoteIP || open != m_isEnable))
        {
            m_remoteIP = server;
            m_isEnable = open;

            trLogger->debug("NFSManager::refresh enable[%s] remoteIp[%s]!", m_isEnable ? "true" : "false", m_remoteIP.toStdString().c_str());

            try
            {
                LuaEngine* luaEngine = LuaEngine::Instance();
                std::string addr = m_remoteIP.toStdString();
                luaEngine->SetLuaValue("config.system.nfsEnable", m_isEnable);
                luaEngine->SetLuaValue("config.system.nfsAddr", addr);
            }
            catch(OOLUA::Exception e)
            {
                logger->warn("NFSManager::NFSManager() => %s", e.what());
            }
            catch(std::exception e)
            {
                logger->warn("NFSManager::NFSManager() => %s", e.what());
            }
        }

        connect    = m_isEnable;
        remoteIP   = m_remoteIP;
        remotePath = REMOTE_MOUNT_PATH;
        localPath  = LOCAL_MOUNT_PATH;
    };

    m_client.SetRefreshParamsFunc(onRefreshParams);
    if (m_isEnable)
        m_client.Connect(m_remoteIP, REMOTE_MOUNT_PATH, LOCAL_MOUNT_PATH);
}

void NFSManager::Uninit()
{
    m_client.Uninit();
}

