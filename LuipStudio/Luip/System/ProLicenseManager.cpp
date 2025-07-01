#include <QDateTime>
#include "Log.h"
#include "oolua.h"
#include "LuaEngine/LuaEngine.h"
#include "Setting/Environment.h"
#include "AlarmManager/AlarmManager.h"
#include "FlowManager/MeasureScheduler.h"
#include "ProLicenseManager.h"

using namespace std;
using namespace OOLUA;
using namespace Lua;
using namespace Measure;
using namespace Flow;

namespace System
{
std::unique_ptr<ProLicenseManager> ProLicenseManager::m_instance(nullptr);

ProLicenseManager::~ProLicenseManager()
{

}

ProLicenseManager *ProLicenseManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new ProLicenseManager);
    }

    return m_instance.get();

}

bool ProLicenseManager::GetProLicense()
{
    bool isValid = false;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State *lua = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(lua, "config.info.instrument.proLicense", isValid);
     }
     catch(OOLUA::Exception e)
     {
         logger->warn("ProLicenseManager::GetProLicense() => %s", e.what());
     }
     catch(std::exception e)
     {
         logger->warn("ProLicenseManager::GetProLicense() => %s", e.what());
     }

     return isValid;
}

void ProLicenseManager::SetProLicense(bool license)
{
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State *lua = luaEngine->GetThreadState();
        Lua_function call(lua);

        Lua_func_ref SaveEncryptionFile;
        luaEngine->GetLuaValue(lua, "Serialization.SaveEncryptionFile", SaveEncryptionFile);

        Table instrument;
        luaEngine->GetLuaValue(lua, "config.info.instrument", instrument);

        instrument.set("proLicense", license);

        String path = Configuration::Environment::Instance()->GetDeviceDataPath() + "/InstrumentInfomation.ls";
        call(SaveEncryptionFile, instrument, path, "config.info.instrument");
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ProLicenseManager::SetProLicense() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ProLicenseManager::SetProLicense() => %s", e.what());
    }
}

/**
 * @brief 是否启用试剂授权。
 * @param
 * @return 试剂授权使能，int，支持的状态有：
 *  - @ref ReagentAuthorizationEnable::Unsupported  不支持试剂授权；
 *  - @ref ReagentAuthorizationEnable::Invalid      试剂授权关闭；
 *  - @ref ReagentAuthorizationEnable::Effective    试剂授权打开；
 */
int ProLicenseManager::IsProLicenseEnable()
{
    int isValid = (int)ProLicenseEnable::Unsupported;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State *lua = luaEngine->GetThreadState();

        Table instrument;
        luaEngine->GetLuaValue(lua, "config.info.instrument", instrument);

        bool bRet = false;
        bool proLicense;
        bRet = instrument.safe_at("proLicense", proLicense);

        if (bRet)
        {
            bool bEnable = false;
            instrument.at("proLicense", bEnable);
            if (bEnable)
            {
                isValid = (int)ProLicenseEnable::Effective;
            }
            else
            {
                isValid = (int)ProLicenseEnable::Invalid;
            }
        }
     }
     catch(OOLUA::Exception e)
     {
         logger->warn("ProLicenseManager::GetReagentAuthorization() => %s", e.what());
     }
     catch(std::exception e)
     {
         logger->warn("ProLicenseManager::GetReagentAuthorization() => %s", e.what());
     }

    return isValid;
}

bool ProLicenseManager::Check()
{

}

}
