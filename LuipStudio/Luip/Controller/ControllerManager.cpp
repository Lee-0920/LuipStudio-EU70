/**
 * @file
 * @brief 液路控制器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#include <QLibrary>
#include <QDebug>
#include "Communication/Stack/DncpStack.h"
#include "Communication/AsyncScheduler.h"
#include "Communication/EventHandler.h"
#include "Communication/CommunicationException.h"
#include "Communication/Stack/DncpMcStack.h"
#include "Communication/CommunicationManager.h"
#include "ControllerPlugin/ControllerPlugin.h"
#include "Log.h"
#include "oolua.h"
#include "LuaEngine/LuaEngine.h"
#include "Setting/Environment.h"
#include "Controller/ControllerManager.h"
#include "AlarmManager/AlarmManager.h"
#include "StatusManager/StatusManager.h"
#include "FlowManager/MeasureScheduler.h"

using namespace std;
using namespace OOLUA;
using namespace Lua;
using namespace Configuration;

using namespace Communication;
using namespace Communication::Dscp;
using namespace Communication::Stack;
using namespace Measure;
using namespace Flow;

namespace Controller
{

unique_ptr<ControllerManager> ControllerManager::m_instance(nullptr);


ControllerManager::ControllerManager()
    : m_isUpdate(false),m_isParamSetting(false),m_isInitOver(false)
{

}

ControllerManager::~ControllerManager()
{
// 应用程序退出时，delete m_controllerMap中指针元素有问题
//    for(std::vector<Controller *>::iterator it = m_controllerMap.begin();
//        it != m_controllerMap.end(); it++)
//    {
//        if ((*it) != nullptr)
//        {
//            delete *it;
//            it = m_controllerMap.erase(it);
//        }
//    }

//    m_controllerMap.clear();

//    for(unsigned int i = 0; i < m_controllerMap.size(); i++)
//    {
//        delete m_controllerMap[i];
//    }
}

/**
 * @brief 获取全局唯一的 ControlNetwork 对象（单件）。
 */
ControllerManager* ControllerManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new ControllerManager);
    }

    return m_instance.get();
}

/**
 * @brief 初始化 ControlNetwork 环境。
 */
void ControllerManager::Init()
{
    m_controllerPlugin = new ControllerPlugin();
    m_controllerPlugin->Init(logger);
    MC = new MainController(DscpAddress(1, 1));
    MC->Init();
}

void ControllerManager::Uninit()
{

}

void ControllerManager::LoadPlugin()
{
    // 定义插件中的两个导出函数的原型
    typedef ControllerPlugin* (*CreatePlugin)();

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        Script* lua = luaEngine->GetEngine();
        lua_State * state = luaEngine->GetThreadState();

        Table controllersTable;
        luaEngine->GetLuaValue(state, "setting.plugins.controllers", controllersTable);

        oolua_ipairs(controllersTable)
        {
            Table tempTable;
            String pluginName;
            lua->pull(tempTable);
            tempTable.at("name", pluginName);

            QString libName;

            #ifdef _CS_X86_WINDOWS
                libName = QString(pluginName.c_str());
            #elif _CS_ARM_LINUX
            libName = QString((Environment::Instance()->GetAppPath() + "/Plugin/lib/" + pluginName).c_str());
            #endif

            QLibrary lib(libName);
            if (lib.load())
            {
                // 解析导出函数
                CreatePlugin createPlugin = (CreatePlugin) lib.resolve("CreatePlugin");

                if (createPlugin)
                {
                     ControllerPlugin *plugin = createPlugin();
                    if (plugin)
                    {
                        plugin->Init(logger);
//                      delete plugin;
                    }
                }

                // 卸载插件
//              lib.unload();
            }
            else
            {                
                logger->debug("%s load fail,path: ",pluginName.c_str(), libName.toStdString().c_str());
            }
        }
        oolua_ipairs_end()
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ControllerManager::LoadPlugin() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ControllerManager::LoadPlugin() => %s", e.what());
    }
}

bool ControllerManager::StopControllers()
{
    bool ret = true;

    lua_State * newState = LuaEngine::Instance()->GetThreadState();

    OOLUA::Lua_function call(newState);
    OOLUA::Lua_func_ref stopOperator;

    try
    {
        OOLUA::get_global(newState, "StopOperator", stopOperator);
        call(stopOperator);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ControllerManager::StopControllers() => %s", e.what());
        ret = false;
    }
    catch(std::exception e)
    {
        logger->warn("ControllerManager::StopControllers() => %s", e.what());
        ret = false;
    }
    return ret;
}

void ControllerManager::AddController(String name, String text, BaseController *controller)
{
    controller->Init();
    m_controllerMap.insert(make_pair(name, controller));
    m_TextMap.insert(make_pair(controller->GetAddress().ToString(), text));
}

BaseController* ControllerManager::GetController(String name)
{
    std::map<String,BaseController*>::iterator it = m_controllerMap.find(name);

    return ((*it).second);
}

void ControllerManager::DeviceReset()
{
    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
        it != m_controllerMap.end(); it++)
    {
        BaseController * controller = (*it).second;

        String strController = "<Controller："+ controller->GetAddress().ToString() + "> ";

        try
        {
            controller->IDeviceStatus->Reset();
        }
        catch (CommandTimeoutException e)
        {
            (void)e;
            logger->warn( strController + "Reset Command Timeout");
        }
        catch(ExpectEventTimeoutException e)
        {
            (void)e;
            logger->warn( strController + "Reset Event Timeout");
        }
        catch (std::exception e)
        {
            (void)e;
            logger->warn( strController + "Reset Exception");;
        }
    }
}

/**
 * @brief 下位机通信检测。
 */
void ControllerManager::DeviceConnectCheck()
{
    AlarmManager *alarmManager = AlarmManager::Instance();

    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
        it != m_controllerMap.end(); it++)
    {
        BaseController * controller = (*it).second;

        String strController = "<Controller："+ controller->GetAddress().ToString() + "> ";
        String strSV = "Version：";

        try
        {
            if (controller->IsConnected())
            {
                controller->SetConnectStatus(true);
                strSV += controller->IDeviceInfo->GetSoftwareVersion().ToString();
            }
            else
            {
                controller->SetConnectStatus(false);
                strSV += "----";
            }
        }
        catch (CommandTimeoutException e)
        {
            strSV += "----";
            alarmManager->AddCommunicationExceptionAlarm(e);
        }
        catch(ExpectEventTimeoutException e)
        {
            strSV += "----";
            alarmManager->AddCommunicationExceptionAlarm(e);
        }
        catch (std::exception e)
        {
            strSV += "----";
            alarmManager->AddExceptionAlarm(e);
        }

        logger->notice( strController + strSV);
    }
}

bool ControllerManager::IsAllConnected()
{
    bool ret = true;

#ifdef _CS_ARM_LINUX
    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
        it != m_controllerMap.end(); it++)
    {
        ret = ret && (*it).second->GetConnectStatus();
    }
#endif

    return ret;
}

void ControllerManager::SetUpdateFlag(bool isUpdate)
{
    m_isUpdate = isUpdate;
}

bool ControllerManager::GetUpdateFlag()
{
    return m_isUpdate;
}

void ControllerManager::SetParamSettingFlag(bool isSetting)
{
    m_isParamSetting = isSetting;
}

bool ControllerManager::GetParamSettingFlag()
{
    return m_isParamSetting;
}

void ControllerManager::SetInitOverFlag(bool state)
{
    m_isInitOver = state;
}

bool ControllerManager::GetInitOverFlag()
{
    return m_isInitOver;
}

/**
 * @brief 驱动通信连接情况。
 */
void ControllerManager::ControllerConnectStatusChanged(Uint8 pollingAddr, bool status)
{
    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
        it != m_controllerMap.end(); it++)
    {
        BaseController * controller = (*it).second;

        String strController = "<Controller："+ controller->GetAddress().ToString() + "> ";

        if (controller->GetAddress().a3 == pollingAddr)
        {
            controller->SetConnectStatus(status);

            if (!status && !m_isUpdate && !m_isParamSetting)
            {
                logger->warn(strController + " Disconnected");
            }
            else if (status && !m_isUpdate && !m_isParamSetting)
            {
                logger->notice(strController + " Connected");
            }

            if(m_isInitOver)
            {
                MeasureScheduler::Instance()->Update();
            }
        }
    }
}

std::map<String,BaseController*> *ControllerManager::GetControllerMap(void)
{
    return &m_controllerMap;
}

String ControllerManager::GetControllerText(DscpAddress addr)
{
    String text = "";
    for(std::map<String, String>::iterator it = m_TextMap.begin();
        it != m_TextMap.end(); it++)
    {
        if (addr.ToString() == (*it).first)
        {
            text = (*it).second;
            break;
        }
    }
    return text;
}

void ControllerManager::SetResetNotice()
{
    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
     it != m_controllerMap.end(); it++)
    {
         BaseController *controller = (*it).second;
         controller->GetIDeviceStatus()->RegisterResetNotice(this);
    }
}

void ControllerManager::StartSignalUpload(DscpEventPtr event)
{
    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
        it != m_controllerMap.end(); it++)
    {
        BaseController * controller = (*it).second;

         DscpAddress addr = controller->GetAddress();

         if (addr.a1 == event->addr.a1 && addr.a2 == event->addr.a2 &&
             addr.a3 == event->addr.a3 && addr.a4 == event->addr.a4)
        {
            controller->StartSignalUpload();
		    break;
        }
    }
}

void ControllerManager::StartAllSignalUpload()
{
    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
        it != m_controllerMap.end(); it++)
    {
        (*it).second->StartSignalUpload();
    }
}

void ControllerManager::StopSignalUpload(DscpAddress addr)
{
    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
        it != m_controllerMap.end(); it++)
    {
        BaseController * controller = (*it).second;

         DscpAddress cAddress = controller->GetAddress();

         if (cAddress.a1 == cAddress.a1 && cAddress.a2 == addr.a2 &&
             cAddress.a3 == cAddress.a3 && cAddress.a4 == addr.a4)
        {
            controller->StopSignalUpload();
            break;
        }
    }
}

void ControllerManager::StopAllSignalUpload()
{
    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
        it != m_controllerMap.end(); it++)
    {
        (*it).second->StopSignalUpload();
    }
}

void ControllerManager::RegisterSignalNotifiable(ISignalNotifiable *handle)
{
    for(std::map<String,BaseController*>::iterator it = m_controllerMap.begin();
        it != m_controllerMap.end(); it++)
    {
        (*it).second->Register(handle);
    }
}

void ControllerManager::OnReceive(DscpEventPtr event)
{
    QString strLog = QString("下位机复位，地址： %1").arg(event->addr.ToString().c_str());
    logger->warn(strLog.toStdString());

//    if(!StatusManager::Instance()->IsIdleStatus())
//    {
//        FlowManager::Instance()->StopFlow();
//    }

    QThread::msleep(60);

    if((event->code) == DSCP_EVENT_DSI_RESET)
    {
        this->StartSignalUpload(event);        
    }
}

}
