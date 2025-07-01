/**
 * @file
 * @brief 流程管理器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#include <QDebug>
#include "Log.h"
#include "oolua.h"
#include "LuaEngine/LuaEngine.h"
#include "Communication/EventHandler.h"
#include "Controller/ControllerManager.h"
#include "StatusManager/StatusManager.h"
#include "FlowManager.h"
#include "AlarmManager/AlarmManager.h"
#include "System/Exception.h"
#include "System/AuthorizationManager.h"
#include "UI/Frame/UpdateWidgetManager.h"

using namespace std;
using namespace OOLUA;
using namespace Communication;
using namespace Controller;
using namespace Measure;
using namespace Lua;
using namespace System;
using namespace UI;

namespace Flow
{

unique_ptr<FlowManager> FlowManager::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 FlowManager 对象（单件）。
 */
FlowManager* FlowManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new FlowManager);
    }

    return m_instance.get();
}

void FlowManager::Init()
{
    m_isFlowRunning = false;

    m_cFlow = nullptr;
    m_isCFlowRunning = false;
    m_mainState = LuaEngine::Instance()->GetMainState();
    m_newState = LuaEngine::Instance()->CreateThread();
    m_flowThread = new QThread();
    this->moveToThread(m_flowThread);
    m_flowThread->start();

    connect(this, SIGNAL(StartFlowSignal()), this, SLOT(StartFlowSlot()));
}

void FlowManager::RegisterActivatedNotifier(IFlowNotifiable *notifier)
{
    m_flowActivatedNotifier.push_back(notifier);
}

void FlowManager::RegisterFinishedNotifier(IFlowNotifiable *notifier)
{
    m_flowFinishedNotifier.push_back(notifier);
}

void FlowManager::RegisterProcessMessageNotifier(IFlowNotifiable *notifier)
{
    m_flowMessageNotifier.push_back(notifier);
}

bool FlowManager::StartFlow()
{
    QMutexLocker locker(&m_startMutex);

    if (!IsFlowEnable())
    {
        return false;
    }
    EventHandler::Instance()->EmptyEvents();    //清空事件池

    emit StartFlowSignal();

    return true;
}

bool FlowManager::StartCFlow(CFlow *cFlow)
{
    if (!IsFlowEnable())
    {
        return false;
    }

    if (m_cFlow != nullptr)
    {
        delete m_cFlow;
        m_cFlow = nullptr;
    }
    m_cFlow = cFlow;
    m_isCFlowRunning = true;
    return StartFlow();
}

bool FlowManager::StopFlow()
{
//    ControllerManager::Instance()->StopControllers();

    if (!m_isCFlowRunning)
    {
        try
        {
            LuaEngine* luaEngine = LuaEngine::Instance();
            lua_State * state = luaEngine->GetThreadState();

            OOLUA::Lua_function call(state);
            OOLUA::Lua_func_ref stopAllFlow;
            luaEngine->GetLuaValue(state, "FlowList.StopAllFlow", stopAllFlow);
            call(stopAllFlow);
        }
        catch(OOLUA::Exception e)
        {
            logger->warn("FlowManager::StopFlow() => %s", e.what());
        }
        catch(std::exception e)
        {
            logger->warn("FlowManager::StopFlow() => %s", e.what());
        }
    }
    return true;
}

bool FlowManager::RemoveFlow(String name)
{
    if (!m_isCFlowRunning)
    {
        try
        {
            LuaEngine* luaEngine = LuaEngine::Instance();
            lua_State * state = luaEngine->GetThreadState();

            OOLUA::Lua_function call(state);
            OOLUA::Lua_func_ref removeFlow;
            luaEngine->GetLuaValue(state, "FlowList.RemoveFlow", removeFlow);
            call(removeFlow, name);
        }
        catch(OOLUA::Exception e)
        {
            logger->warn("FlowManager::RemoveFlow() => %s", e.what());
        }
        catch(std::exception e)
        {
            logger->warn("FlowManager::RemoveFlow() => %s", e.what());
        }
    }
    return true;
}

bool FlowManager::IsFlowRunning()
{
    return m_isFlowRunning;
}

void FlowManager::UpdateFlowTime(String name, long time)
{
    for (std::list<IFlowNotifiable *>::iterator it = m_flowActivatedNotifier.begin();
         it != m_flowActivatedNotifier.end(); it++)
    {
        (*it)->OnFlowActivated(name, time);
    }
}

void FlowManager::UpdateFlowResult(String name, String result)
{
    for (std::list<IFlowNotifiable *>::iterator it = m_flowFinishedNotifier.begin();
         it != m_flowFinishedNotifier.end(); it++)
    {
        if (*it)
        {
            (*it)->OnFlowFinished(name, result);
        }
    }
}

void FlowManager::UpdateFlowMessage(String name, String mag)
{
    for (std::list<IFlowNotifiable *>::iterator it = m_flowMessageNotifier.begin();
         it != m_flowMessageNotifier.end(); it++)
    {
        if (*it)
        {
            (*it)->OnFlowProcessMessage(name, mag);
        }
    }
}

void FlowManager::UpdateAllFlowFinish()
{

}

/**
 * @file
 * @brief 执行lua业务流程。
 * @details 该函数在流管理器的线程中调用，当在多线程系统中，与lua交互时，容易出现交互栈的混乱，
 * @details 为此问题，在C++个线程中通过lua_newthread函数创建一条新协程，并将其压栈，并返回维护
 * @details 这个线程的 lua_State 指针，这个函数返回的协线程共享原线程的全局环境。
 * @details 启动一个协程的话，需要把主函数（lua函数）以及它需要的参数压入线程栈； 然后调用 lua_resume，
 * @details 执行该主函数的程序。
 */
void FlowManager::RunLuaFlow()
{
    try
    {
        lua_settop(m_newState ,0);//清空栈
        lua_pushcfunction(m_newState, &CallLuaFlowFunc);  /* to call 'pmain' in protected mode */

        int ret = lua_resume(m_newState, m_mainState, 0);
        if (ret != 0)
        {
            logger->warn("FlowManager::RunLuaFlow() => lua_resume call 'RunAllFlow' function failed, error code:%d", ret);
        }
    }
    catch(std::exception e)
    {
        logger->warn("FlowManager::RunLuaFlow() => %s", e.what());
    }
}

int FlowManager::CallLuaFlowFunc(lua_State *L)
{
    lua_getglobal(L, "FlowList");

    int status = lua_istable(L ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("FlowManager::CallLuaFlowFunc() => get 'FlowList' table failed");
    }

    lua_pushstring(L , "RunAllFlow");
    lua_gettable(L ,-2); //取栈顶下一个元素
    status = lua_isfunction(L ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("FlowManager::CallLuaFlowFunc() => get 'RunAllFlow' function failed");
    }

    status = lua_pcall(L, 0, 0, 0);  /* do the call */

    if (status != LUA_OK)
    {
      const char *msg = lua_tostring(L, -1);
      logger->warn("FlowManager::CallLuaFlowFunc() =>pcall error(%s)", msg);
      lua_pop(L, 1);  /* remove message */
    }

    return 0;
}

void FlowManager::RunCFlow()
{
    if (m_cFlow!= nullptr)
    {
        AlarmManager *alarmManager = AlarmManager::Instance();
        try
        {
            m_cFlow->OnStarted();
            m_cFlow->Run();
        }
        catch (ExpectEventTimeoutException e)  // 期望事件等待超时异常。
        {
            alarmManager->AddCommunicationExceptionAlarm(e);
        }
        catch (CommandTimeoutException e)  // 命令应答超时异常。
        {
            alarmManager->AddCommunicationExceptionAlarm(e);
        }
        catch (UpdateException e)  // 升级异常。
        {
            alarmManager->AddExceptionAlarm(e);
        }
        catch (System::Exception e)       // 系统异常。
        {
            alarmManager->AddExceptionAlarm(e);
        }
        catch (std::exception e)
        {
            alarmManager->AddExceptionAlarm(e);
        }
        try
        {
            m_cFlow->OnStopped();
        }
        catch (ExpectEventTimeoutException e)  // 期望事件等待超时异常。
        {
            alarmManager->AddCommunicationExceptionAlarm(e);
        }
        catch (CommandTimeoutException e)  // 命令应答超时异常。
        {
            alarmManager->AddCommunicationExceptionAlarm(e);
        }
        catch (UpdateException e)  // 升级异常。
        {
            alarmManager->AddExceptionAlarm(e);
        }
        catch (System::Exception e)       // 系统异常。
        {
            alarmManager->AddExceptionAlarm(e);
        }
        catch (std::exception e)
        {
            alarmManager->AddExceptionAlarm(e);
        }
        delete m_cFlow;
        m_cFlow = nullptr;
        StatusManager::Instance()->ResetDeviceStatus();
    }
    m_isCFlowRunning = false;
}

void FlowManager::StartFlowSlot()
{
    logger->debug("Flow Thread ID = %d", QThread::currentThreadId());
    m_isFlowRunning = true;     // 假如没有流程运行，先将标志流程运行，反正其他业务启动

    LuaEngine::Instance()->AttachCurrentThread(m_newState);

    try
    {
        if (m_isCFlowRunning)
        {
            this->UpdateFlowTime("CFlow", 0);

            this->RunCFlow();    //运行C++的业务流程
        }
        else
        {
            this->RunLuaFlow(); //运行lua的业务流程
        }
    }
    catch(std::exception e)
    {
        logger->warn("FlowManager::run() => %s", e.what());
    }

    m_isFlowRunning = false;

    if (!m_flowActivatedNotifier.empty())
    {
        for (std::list<IFlowNotifiable *>::iterator it = m_flowActivatedNotifier.begin();
          it != m_flowActivatedNotifier.end(); it++)
        {
            (*it)->OnFlowThreadExit();
        }
    }
}

bool FlowManager::IsFlowEnable()
{
    if (m_isFlowRunning)
    {
        logger->debug("FlowManager::IsFlowEnable() => 尚有流程执行");
        return false;
    }

    if (!StatusManager::Instance()->IsIdleStatus())  // 空闲状态
    {
        logger->debug("FlowManager::IsFlowEnable() => 不是空闲状态");
        return false;
    }

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    bool isFatalError = false;
    bool isExistFatalError = false;
    Table measureTable;
    luaEngine->GetLuaValue(state, "status.measure", measureTable);
    isExistFatalError = measureTable.safe_at("isFatalError", isFatalError);

    if(isExistFatalError && isFatalError)
    {
        logger->debug("FlowManager::IsFlowEnable() => 仪器处于严重故障状态");
        return false;
    }

    bool isFaultBlocking = false;
    luaEngine->GetLuaValue(state, "config.system.faultBlocking", isFaultBlocking);

    if(StatusManager::Instance()->IsFaultStatus() && isFaultBlocking)
    {
        logger->debug("FlowManager::IsFlowEnable() => 仪器处于故障停机状态");
        return false;
    }

    return true;
}

bool FlowManager::IsComValid()
{
#ifdef _CS_ARM_LINUX
    if (!ControllerManager::Instance()->IsAllConnected())
    {
        logger->debug("FlowManager::IsComValid() => 通信未连接");
        return false;
    }
#endif

    return true;
}

bool FlowManager::IsAuthorize()
{
    return AuthorizationManager::Instance()->GetAuthorization();  // 运行权限
}

bool FlowManager::IsReagentAuthorize()
{
    return AuthorizationManager::Instance()->GetReagentAuthorization();  // 试剂授权
}

lua_State *FlowManager::GetNewState()
{
    return m_newState;
}

void FlowManager::ClearAllRemainEvent()
{
    EventHandler::Instance()->EmptyEvents();    //清空事件池
}


}

