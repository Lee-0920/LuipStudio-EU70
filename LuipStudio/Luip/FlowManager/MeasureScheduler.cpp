/**
 * @file
 * @brief 测量排期调度器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */
#include <QList>
#include <list>
#include <QDebug>
#include "Log.h"
#include "StatusManager/StatusManager.h"
#include "Controller/ControllerManager.h"
#include "FlowManager.h"
#include "MeasureScheduler.h"
#include <QDateTime>
#include <QProcess>

#define DEBUG 0

using namespace std;
using namespace OOLUA;
using namespace Measure;
using namespace Controller;
using namespace Lua;

namespace Flow
{
bool SortForNextTime( const SchedulerItem &item1, const SchedulerItem &item2);
bool SortForPriority( const SchedulerItem &item1, const SchedulerItem &item2);

unique_ptr<MeasureScheduler> MeasureScheduler::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 MeasureScheduler 对象（单件）。
 */
MeasureScheduler* MeasureScheduler::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new MeasureScheduler);
    }

    return m_instance.get();
}

MeasureScheduler::MeasureScheduler()
{
    m_isEnabled = true;
    m_timeoutFailedReTry = 0;
    m_loadFailedReTry = 0;
    m_currFlow.name = "";
    m_currFlow.startTime = 0;
    m_currFlow.runTime = 0;
    m_currFlow.endTime = 0;
    logger->debug("MeasureScheduler Creat");
    m_newState = LuaEngine::Instance()->CreateThread();
    logger->debug("MeasureScheduler %d", m_newState);
    m_scheduleThread = new QThread();
    this->moveToThread(m_scheduleThread);
    m_scheduleThread->start();

    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    m_timer->moveToThread(m_scheduleThread);

    m_checkSchedulerTimer = new QTimer();

    connect(this, SIGNAL(InitSignal()), this, SLOT(InitSlot()));
    connect(this, SIGNAL(UpdateSignal()), this, SLOT(UpdateSlot()));
    connect(this, SIGNAL(EnabledTimerSignal(bool)), this, SLOT(EnabledTimerSlot(bool)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(TimeoutSlot()));
    connect(m_checkSchedulerTimer, SIGNAL(timeout()), this, SLOT(CheckSchedulerTimeoutSlot()));
    connect(this, SIGNAL(OnFlowActivatedSignal(QString,long)), this, SLOT(OnFlowActivatedSlot(QString,long)));
    connect(this, SIGNAL(OnFlowFinishedSignal(QString,QString)), this, SLOT(OnFlowFinishedSlot(QString,QString)));
    connect(this, SIGNAL(OnFlowThreadExitSignal()), this, SLOT(OnFlowThreadExitSlot()));
    connect(this, SIGNAL(StartTimerSignal(int)), this, SLOT(StartTimerSlot(int)));
    connect(this, SIGNAL(StopTimerSignal()), this, SLOT(StopTimerSlot()));

    m_checkSchedulerTimer->start(3000);

    QTimer *syncHWClockTimer = new QTimer();
    connect( syncHWClockTimer, SIGNAL(timeout()), this, SLOT(SlotCalibrateTime()));
    syncHWClockTimer->start(24 * 3600 * 1000);           //启动时间校准
}

MeasureScheduler::~MeasureScheduler()
{
    if (m_timer)
        delete m_timer;
}

void MeasureScheduler::SlotCalibrateTime()
{
    logger->debug("SyncHardwareTime start %s !",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str());

    QProcess::execute("hwclock -s");

    logger->debug("SyncHardwareTime end %s !",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str());

    this->Update();
}


void MeasureScheduler::CheckSchedulerTimeoutSlot()
{
    if (!FlowManager::Instance()->IsFlowRunning() && !m_itemList.empty() &&false == m_timer->isActive())
    {
        if (m_loadFailedReTry < 3)
        {
            m_loadFailedReTry++;
            logger->warn("********** ReLoad MeasureScheduler **********");
            this->Update();
        }
    }
    else
    {
        m_loadFailedReTry = 0;
    }
}

bool MeasureScheduler::Init()
{
    FlowManager::Instance()->RegisterActivatedNotifier(this);
    emit InitSignal();
    return true;
}

void MeasureScheduler::InitSlot()
{
    #if DEBUG
        qDebug() << "################### Init() ###################";
    #endif

    logger->debug("MeasureScheduler Thread ID = %d", QThread::currentThreadId());

    this->Update();
}

/**
 * @brief 注册IScheduleNotifiable 对象。
 */
void MeasureScheduler::Register(IScheduleNotifiable *handle)
{
    m_notifise.push_back(handle);
}

/**
 * @brief 注销IScheduleNotifiable 对象。
 */
void MeasureScheduler::Deregister(IScheduleNotifiable *handle)
{
    std::list<IScheduleNotifiable* >::iterator it;

    for(it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if((*it) == handle)
        {
            m_notifise.erase(it);
            break;
        }
    }
}

void MeasureScheduler::Update()
{
    if (m_isEnabled)
    {
        QMutexLocker locker(&m_updateMutex);
        emit UpdateSignal();
    }
}

void MeasureScheduler::Enabled(bool status)
{
    if (status)
    {
        bool needUpdater = false;
        if (false == m_isEnabled)
        {
            needUpdater = true;
        }
        m_isEnabled = true;

        if (needUpdater)
        {
            emit EnabledTimerSignal(true);

            this->Update();
        }
    }
    else
    {
        if (false == m_checkSchedulerTimer->isActive())
        {
            emit EnabledTimerSignal(false);
        }
        m_isEnabled = false;
    }
}

void MeasureScheduler::UpdateSlot()
{
    LuaEngine::Instance()->AttachCurrentThread(m_newState);

    if (FlowManager::Instance()->IsAuthorize())  // 运行权限
    {
        if (m_timer->isActive() == true)
        {
            emit StopTimerSignal();
        }

        this->UpdateItemList();
        this->UpdateQueue();
        this->UpdateReadyItem();
    }
}

void MeasureScheduler::EnabledTimerSlot(bool status)
{
    if (status)
    {
        if (false == m_checkSchedulerTimer->isActive())
        {
            m_checkSchedulerTimer->start();
        }
        if (false == m_timer->isActive())
        {
            m_timer->start();
        }
    }
    else
    {
        if (true == m_checkSchedulerTimer->isActive())
        {
            m_checkSchedulerTimer->stop();
        }
        if (true == m_timer->isActive())
        {
            m_timer->stop();
        }
    }
}

void MeasureScheduler::UpdateExpectInfo(long nextTime , String text)
{
    QString content = "";

    if (nextTime > -1 && text != "")
    {
        QString strTime = QDateTime::fromTime_t(nextTime).toString("yyyy-MM-dd hh:mm:ss");

        if (text != "")
        {
            content = "将于 " + strTime + " " + QString(text.c_str());
        }
    }

    list<IScheduleNotifiable* >::iterator it;
    for(it = m_notifise.begin(); it != m_notifise.end(); it++)  //更新UI
    {
        (*it)->OnScheduler(content);
    }
}

void MeasureScheduler::UpdateItemList()
{
    if (!m_itemList.empty())
    {
        m_itemList.clear();
    }

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * newState = luaEngine->GetThreadState();

        OOLUA::Lua_function call(newState);

        Table schedulerTable;
        luaEngine->GetLuaValue(newState, "setting.measureScheduler", schedulerTable);

        oolua_ipairs(schedulerTable)
        {
            SchedulerItem item;

            Table itemTable;
            OOLUA::pull(newState, itemTable);

            OOLUA::Lua_func_ref func;

            bool isOpen = false;
            itemTable.at("isOpen", func);
            call(func);
            OOLUA::pull(newState, item.isTimedMeasure);
            OOLUA::pull(newState, isOpen);

            if (isOpen == true)
            {
                bool valid = false;

                itemTable.at("getNextTime", func);
                call(func);
                OOLUA::pull(newState, item.nextTime);
                OOLUA::pull(newState, valid);

                if (valid == true)
                {
                    itemTable.at("name", item.name);
                    itemTable.at("text", item.text);

                    itemTable.at("getPriority", func);
                    call(func);
                    OOLUA::pull(newState, item.priority);

                    itemTable.at("getInterval", func);
                    call(func);
                    OOLUA::pull(newState, item.interval);

                    itemTable.at("getLastTime", func);
                    call(func);
                    OOLUA::pull(newState, item.lastTime);

                    itemTable.at("isRetry", func);
                    call(func);
                    pull(newState, item.isRetry);

                    itemTable.at("calculateNextTime", item.calculateNextTime);
                    itemTable.at("createFlow", item.createFlow);

                    m_itemList.push_back(item);
                }
            }
        }
        oolua_ipairs_end()
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("MeasureScheduler::UpdateItemList() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("MeasureScheduler::UpdateItemList() => %s", e.what());
    }

    #if DEBUG
        qDebug() << "****************** UpdateItemList *********************";
        for (std::list<SchedulerItem>::iterator it = m_itemList.begin();
           it != m_itemList.end(); it++)
        {
          QString strName = QString((*it).name.c_str());
          QString strNextTime = QDateTime::fromTime_t((*it).nextTime).toString("yyyy-MM-dd hh:mm:ss");
          qDebug() << strName + " = " + strNextTime;
        }
    #endif
}

void MeasureScheduler::UpdateQueue()
{
    m_queue.clear();

    if (!m_itemList.empty())
    {
        // 将参与排期项目列表中项目加载排序队列中，进行排序
        for (std::list<SchedulerItem>::iterator itList = m_itemList.begin();
             itList != m_itemList.end(); itList++)
        {
            m_queue.push_back((*itList));
        }

        // 按下次启动时间排列
        std::sort(m_queue.begin(), m_queue.end(), SortForNextTime);

        #if DEBUG
            qDebug() << "***************** UpdateQueue **********************";
            for (std::vector<SchedulerItem>::iterator it = m_queue.begin();
                 it != m_queue.end(); it++)
            {
                QString strName = QString((*it).name.c_str());
                QString strNextTime = QDateTime::fromTime_t((*it).nextTime).toString("yyyy-MM-dd hh:mm:ss");
                qDebug() << strName + " = " + strNextTime;
            }
        #endif

        // 掉电重测检测
        if (RetyAnalysis(m_queue)== true)
        {
            return;
        }

        // 流程正在运行
        if (FlowManager::Instance()->IsFlowRunning() ||
            !StatusManager::Instance()->IsIdleStatus())
        {
            // 检测排序队列中项目是否有排期自动，或手动，或外部等方式触发流程，更新各项的排期时间
            if(m_currFlow.name != "")
            {
                if (m_currFlow.runTime > 0 )
                {
                    // 检查当前启动项是否排队列表中某一项，若是,重新更新该项的下次启动时间。
                    this->NextTimeAnalysis(m_currFlow, m_queue);

                    // 检测结束时间是否晚于排队列表中某些项，若有则按优先级排列,删除比流程结束时间晚的项。
                    this->PriorityAnalysis(m_currFlow.endTime, m_queue);
                }
                else
                {
                    m_queue.clear();
                    m_queue.push_back(SchedulerItem());
                }
            }
        }
        else    // 无流程运行
        {
            this->PriorityAnalysis(QDateTime::currentDateTime().toTime_t(), m_queue);
        }
    }
}

bool MeasureScheduler::NextTimeAnalysis(CurrFlow currFlow, vector<SchedulerItem> &queue)
{
    bool ret = false;
    if (!queue.empty())
    {
        lua_State * newState = LuaEngine::Instance()->GetThreadState();
        OOLUA::Lua_function call(newState);

        // 检查整点测量是否开启，若是，则预测、更新下次启动整点测量的整点时间
        for (std::vector<SchedulerItem>::iterator it = queue.begin();
             it != queue.end(); it++)
        {
            if ((*it).isTimedMeasure == true)
            {
                SchedulerItem item = (*it);
                queue.erase(it);

                bool valid = false;

                try
                {
                    call(item.calculateNextTime, currFlow.startTime, currFlow.runTime);
                    OOLUA::pull(newState, item.nextTime);
                    OOLUA::pull(newState, valid);
                }
                catch(OOLUA::Exception e)
                {
                    logger->warn("MeasureScheduler::NextTimeAnalysis() => %s", e.what());
                }
                catch(std::exception e)
                {
                    logger->warn("MeasureScheduler::NextTimeAnalysis() => %s", e.what());
                }

                if (valid == true)
                {
                    queue.push_back(item);
                }
                break;
            }
        }

        // 检查当前流程是否是排队列表中的某一项，若是，则预测、更新该项下次启动时间
        for (std::vector<SchedulerItem>::iterator it = queue.begin();
             it != queue.end(); it++)
        {
            if ((*it).name == currFlow.name && (*it).isTimedMeasure == false)
            {
                SchedulerItem item = (*it);
                queue.erase(it);

                bool valid = false;

                try
                {
                    call(item.calculateNextTime, currFlow.startTime, currFlow.runTime);
                    OOLUA::pull(newState, item.nextTime);
                    OOLUA::pull(newState, valid);
                }
                catch(OOLUA::Exception e)
                {
                    logger->warn("MeasureScheduler::NextTimeAnalysis() => %s", e.what());
                }
                catch(std::exception e)
                {
                    logger->warn("MeasureScheduler::NextTimeAnalysis() => %s", e.what());
                }

                if (valid == true)
                {
                    queue.push_back(item);
                }
                break;
            }
        }

        // 按下次启动时间排列
        std::sort(queue.begin(), queue.end(), SortForNextTime);

        ret = true;

        #if DEBUG
            qDebug() << "***************** NextTimeAnalysis **********************";
            for (std::vector<SchedulerItem>::iterator it22 = queue.begin();
                 it22 != queue.end(); it22++)
            {
                QString strName = QString((*it22).name.c_str());
                QString strNextTime = QDateTime::fromTime_t((*it22).nextTime).toString("yyyy-MM-dd hh:mm:ss");
                qDebug() << strName + " = " + strNextTime;
            }
        #endif
    }

    return ret;
}

bool MeasureScheduler::PriorityAnalysis(long refTime, vector<SchedulerItem> &queue)
{
    bool ret = false;

    // 按下次启动时间排列
    std::sort(queue.begin(), queue.end(), SortForNextTime);

    // 取列表头的排期项
    SchedulerItem firtItem = queue.front();

    // 检测结束时间是否晚于排队列表中某些项，若有则按优先级排列，删除比流程结束时间晚的项。
    if (refTime > firtItem.nextTime)
    {
        // 进行优先级排列
        std::sort(queue.begin(), queue.end(), SortForPriority);

        // 根据优先级高到低，一项一项与参考时间比对，删除晚于参考时间的项，保证高优先级、早时间的排在前面
        for (std::vector<SchedulerItem>::iterator it = queue.begin(); it != queue.end();)
        {
            if ((*it).nextTime > refTime)
            {
                it = queue.erase(it);
            }
            else
            {
                it++;
            }
        }

        // 按下次启动时间排列
        queue[0].nextTime = refTime;

        ret = true;

        #if DEBUG
            qDebug() << "***************** PriorityAnalysis **********************";
            for (std::vector<SchedulerItem>::iterator it22 = queue.begin();
                 it22 != queue.end(); it22++)
            {
                QString strName = QString((*it22).name.c_str());
                QString strNextTime = QDateTime::fromTime_t((*it22).nextTime).toString("yyyy-MM-dd hh:mm:ss");
                qDebug() << strName + " = " + strNextTime;
            }
        #endif
    }
    return ret;
}

bool MeasureScheduler::RetyAnalysis(vector<SchedulerItem> &queue)
{
    bool ret = false;
    if (!queue.empty() && !FlowManager::Instance()->IsFlowRunning())
    {
        bool status = false;
        SchedulerItem item;

        for (std::vector<SchedulerItem>::iterator it = queue.begin();
             it != queue.end(); it++)
        {
            if ((*it).isRetry == true)
            {
                item = (*it);
                status = true;
                break;
            }
        }

        if (status == true)
        {
            queue.clear();
            queue.push_back(item);

            ret = true;
        }

        #if DEBUG
            qDebug() << "***************** RetyAnalysis **********************";
            for (std::vector<SchedulerItem>::iterator itq = m_queue.begin();
                 itq != m_queue.end(); itq++)
            {
                QString strName = QString((*itq).name.c_str());
                QString strNextTime = QDateTime::fromTime_t((*itq).nextTime).toString("yyyy-MM-dd hh:mm:ss");
                qDebug() << strName + " = " + strNextTime;
            }
        #endif
    }

    return ret;
}

void MeasureScheduler::UpdateReadyItem()
{
    Uint32 currTime = QDateTime::currentDateTime().toTime_t();
    int times = 0;

    this->RestReadyItem();

    if (!m_queue.empty())
    {
        m_readyItem = m_queue.front(); // 取列表头的排期项
        if (m_readyItem.name != "" && m_readyItem.nextTime > -1)
        {
            times = m_readyItem.nextTime - currTime;
            if (times <= 0 || m_readyItem.isRetry)
            {
                times = 5;    // 排期项下次启动时间早于当前时间，则5ms后启动定时器，启动流程
            }
            else
            {
                times *= 1000; //将秒转为毫秒
            }

            emit StartTimerSignal(times);
       }
    }

    this->UpdateExpectInfo(m_readyItem.nextTime, m_readyItem.text);

    #if DEBUG
        qDebug() << "***************** UpdateReadyItem **********************";
        QString strReadyTime = QDateTime::fromTime_t(m_readyItem.nextTime).toString("yyyy-MM-dd hh:mm:ss");
        QString strCurrTime = QDateTime::fromTime_t(currTime).toString("yyyy-MM-dd hh:mm:ss");

        qDebug() << "ReadyName = " << QString(m_readyItem.name.c_str());
        qDebug() << "ReadyTime = " << strReadyTime;
        qDebug() << "CurrTime = " << strCurrTime;
        qDebug() << "times = " << times << endl;
    #endif
}

void MeasureScheduler::TimeoutSlot()
{
    bool isFailed = false;
    if (!FlowManager::Instance()->IsFlowRunning() &&
        FlowManager::Instance()->IsFlowEnable() &&
        ControllerManager::Instance()->IsAllConnected())
    {
        try
        {
            lua_State * newState = LuaEngine::Instance()->GetThreadState();
            OOLUA::Lua_function call(newState);

            call(m_readyItem.createFlow);
            FlowManager::Instance()->StartFlow();
        }
        catch(OOLUA::Exception e)
        {
            logger->warn("MeasureScheduler::UpdateExpectInfo() => %s", e.what());
            isFailed = true;
        }
        catch(std::exception e)
        {
            logger->warn("MeasureScheduler::UpdateExpectInfo() => %s", e.what());
            isFailed = true;
        }
    }
    if (isFailed)
    {
        if(m_timeoutFailedReTry < 3)
        {
            m_timeoutFailedReTry++;
            logger->warn("********** Reset MeasureScheduler **********");
            this->Update();
        }
    }
    else
    {
        m_timeoutFailedReTry = 0;
    }
}

void MeasureScheduler::OnFlowActivated(String name, long time)
{
    emit OnFlowActivatedSignal(QString(name.c_str()), time);
}

void MeasureScheduler::OnFlowFinished(String name, String result)
{
    emit OnFlowFinishedSignal(QString(name.c_str()), QString(result.c_str()));
}

void MeasureScheduler::OnFlowThreadExit()
{
    emit OnFlowThreadExitSignal();
}

void MeasureScheduler::OnFlowActivatedSlot(QString name, long time)
{
    m_currFlow.name = name.toStdString();
    m_currFlow.startTime = QDateTime::currentDateTime().toTime_t();
    m_currFlow.runTime = time;
    m_currFlow.endTime = m_currFlow.startTime + time;

    #if DEBUG
        qDebug() << "################### OnFlowActivated() ###################";
        qDebug() << "currFlow Name = " << QString(m_currFlow.name.c_str());
        qDebug() << "currFlow startTime = " << QDateTime::fromTime_t(m_currFlow.startTime).toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << "currFlow runTime = " << m_currFlow.runTime;
        qDebug() << "currFlow endTime = " << QDateTime::fromTime_t(m_currFlow.endTime).toString("yyyy-MM-dd hh:mm:ss") << endl;
    #endif

    this->Update();
}

void MeasureScheduler::OnFlowFinishedSlot(QString name, QString result)
{
    (void)name;
    (void)result;
}

void MeasureScheduler::OnFlowThreadExitSlot()
{
    m_currFlow.name = "";
    m_currFlow.startTime = 0;
    m_currFlow.runTime = 0;
    m_currFlow.endTime = 0;

    #if DEBUG
        qDebug() << "################### OnFlowThreadExit() ###################";
    #endif

        this->Update();
}

void MeasureScheduler::StartTimerSlot(int times)
{
    m_timer->start(times);
}

void MeasureScheduler::StopTimerSlot()
{
    m_timer->stop();
}

void MeasureScheduler::RestReadyItem()
{
    m_readyItem.name = "";
    m_readyItem.text = "";
    m_readyItem.priority = 0;
    m_readyItem.interval = 0;
    m_readyItem.lastTime = -1;
    m_readyItem.nextTime = -1;
    //    m_readyItem.createFlow = NULL;
}

void MeasureScheduler::SyncSysTimeToSchedule(QDateTime oldDateTime)
{
    /*刷新组合维护排期*/
    FlowManager::Instance()->UpdateFlowMessage("MaintainCombine","UpdateTime");

    bool ret = false;

    lua_State * newState = LuaEngine::Instance()->GetThreadState();

    OOLUA::Lua_function call(newState);
    OOLUA::Lua_func_ref updateMeasureTimeStatus;

    try
    {
        ret = OOLUA::get_global(newState, "UpdateMeasureTimeStatus", updateMeasureTimeStatus);
        if (ret)
        {
            call(updateMeasureTimeStatus, oldDateTime.toTime_t());
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("MeasureScheduler::SyncSysTimeToSchedule() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("MeasureScheduler::SyncSysTimeToSchedule() => %s", e.what());
    }

    Update();
}

bool SortForNextTime(const SchedulerItem &item1, const SchedulerItem &item2)
{
    return item1.nextTime < item2.nextTime;   //升序排列
}

bool SortForPriority(const SchedulerItem &item1, const SchedulerItem &item2)
{
    return item1.priority < item2.priority;   //升序排列
}

}
