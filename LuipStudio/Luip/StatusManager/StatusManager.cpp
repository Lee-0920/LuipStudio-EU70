/**
 * @file
 * @brief 状态管理器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */
#include<QObject>
#include"Log.h"
#include "StatusManager.h"
#include "LuaEngine/LuaEngine.h"

using namespace std;
using namespace OOLUA;
using namespace Lua;

namespace Measure
{

unique_ptr<StatusManager> StatusManager::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 StatusManager 对象（单件）。
 */
StatusManager* StatusManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new StatusManager);
    }

    return m_instance.get();
}

void StatusManager::Init()
{
    this->m_deviceFault = false;
    this->ResetDeviceStatus();
}

void StatusManager::ResetDeviceStatus()
{
    bool isDeviceFault = false;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(state, "status.measure.isDeviceFault", isDeviceFault);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("StatusManager::Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("StatusManager::Init() => %s", e.what());
    }

    if(isDeviceFault == true)
    {
        RunStatus status;
        status.name = QObject::tr("Fault").toStdString();
        status.content = QObject::tr("故障").toStdString();
        status.time = 0;
        status.createTime = QDateTime::currentDateTime();
        SetStatus(status);
    }
    else
    {
        RunStatus status;
        status.name = QObject::tr("Idle").toStdString();
        status.content = QObject::tr("空闲").toStdString();
        status.time = 0;
        status.createTime = QDateTime::currentDateTime();
        SetStatus(status);
    }

    RunAction action;
    action.name = QObject::tr("Idle").toStdString();
    action.content = QObject::tr("空闲").toStdString();
    action.time = 0;
    action.createTime = QDateTime::currentDateTime();
    SetAction(action);
}

/**
 * @brief IStatusNotifiable 对象。
 */
void StatusManager::Register(IStatusNotifiable *handle)
{
    m_notifise.push_back(handle);
}

/**
 * @brief 获取测量状态名。
 */
RunStatus StatusManager::GetStatus()
{
    QReadLocker locker(&m_statusLock);
    return m_status;
}

/**
 * @brief 设置测量状态名。
 */
void StatusManager::SetStatus(RunStatus status)
{
    QWriteLocker locker(&m_statusLock);

    m_status = status ;

 //   InterfaceManager::Instance()->UpdateStatus();

    if (status.name != "Idle" && status.name != "Fault")
    {
        logger->info(status.content);
    }

//    qDebug("m_status :name: %s, text %s, time %d", m_status.name.c_str(), m_status.content.c_str(),
//            m_status.time);
    for(std::list<IStatusNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnStattusChanged(status);
        }
    }
}

void StatusManager::SetStatusDelayTime(int time)
{
    for(std::list<IStatusNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnStattusDelay(time);
        }
    }
}

/**
 * @brief 获取测量动作名。
 */
RunAction StatusManager::GetAction()
{
    QReadLocker locker(&m_actionLock);
    return m_action;
}

/**
 * @brief 设置测量动作名。
 */
void StatusManager::SetAction(RunAction action)
{
    QWriteLocker locker(&m_actionLock);

    m_action = action ;

//    InterfaceManager::Instance()->UpdateStatus();

    if (action.name != "Idle")
    {
        logger->notice(action.content);
    }

//    qDebug("m_status :name: %s, text %s, time %d", m_status.name.c_str(), m_status.content.c_str(),
//            m_status.time);

    for(std::list<IStatusNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnActionChanged(action);
        }
    }
}

void StatusManager::SetActionDelayTime(int time)
{
    for(std::list<IStatusNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnActionDelay(time);
        }
    }
}

bool StatusManager::IsIdleStatus()
{
    QReadLocker locker(&m_statusLock);

    bool status = false;
    if (m_status.name == "Idle" || m_status.name == "Fault")
    {
        status = true;
    }

    return status;
}

bool StatusManager::IsIdleAction()
{
    QReadLocker locker(&m_actionLock);

    bool status = false;

    if (m_action.name == "Idle")
    {
        status = true;
    }

    return status;
}

bool StatusManager::IsFaultStatus()
{
    QReadLocker locker(&m_statusLock);

    bool status = false;

    if (m_status.name == "Fault")
    {
        status = true;
    }

    return status;
}

}
