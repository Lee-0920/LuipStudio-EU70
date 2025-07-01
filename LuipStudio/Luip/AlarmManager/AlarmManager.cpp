/**
 * @file
 * @brief 告警管理器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */
#include <QDateTime>

#include "AlarmManager.h"
#include "Setting/Environment.h"
#include <QString>
#include "Log.h"
#include "System/Exception.h"
#include "Controller/ControllerManager.h"
#include "Interface/Wqimc/WqimcManager.h"
#include "DataBasePlugin/DataBaseManager.h"

using namespace std;
using namespace System;
using namespace Configuration;
using namespace Communication;
using namespace Controller;
using namespace DataBaseSpace;

namespace Measure
{

unique_ptr<AlarmManager> AlarmManager::m_instance(nullptr);

AlarmManager::AlarmManager():
    m_currentAlarm("空闲", "空闲", "空闲"),m_upLoadStatus(false)
{
    alarmInfoFile = new AlarmInfoFile(Environment::Instance()->GetAppDataPath() +"/AlarmLog.txt");
}

AlarmManager::~AlarmManager()
{
    if (alarmInfoFile)
    {
        delete alarmInfoFile;
    }
}


/**
 * @brief 获取全局唯一的 AlarmManager 对象（单件）。
 */
AlarmManager* AlarmManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new AlarmManager);
    }

    return m_instance.get();
}

/**
 * @brief 注册AlarmManager 对象。
 */
void AlarmManager::Register(IAlarmNotifiable *handle)
{
    m_notifise.push_back(handle);
}

bool AlarmManager::UpdateAlarm(Alarm alarm)
{
    bool ret = alarmInfoFile->AddRecord(alarm);

    m_currentAlarm = alarm;

    for(std::list<IAlarmNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnAlamChanged(alarm);
        }
    }

    return ret;
}

bool AlarmManager::AddCommunicationExceptionAlarm(const CommunicationException &e)
{
    String cause = "";

    if (e.m_name != "")
    {
        cause += e.m_name;
    }
    else
    {
        cause += ControllerManager::Instance()->GetControllerText(e.m_addr);
    }

    if (e.m_type == "CommandTimeoutException")
    {
        QString str = QString("-命令-[0x%1]超时").arg(e.m_code, 0 , 16);
        cause += str.toStdString();
    }
    else  if (e.m_type == "ExpectEventTimeoutException")
    {
        QString str = QString("-事件-[0x%1]超时").arg(e.m_code, 0 , 16);
        cause += str.toStdString();
    }
    Alarm alarm("通信异常", "通信异常", cause, "Error");
    return AddAlarm(alarm);
}

bool AlarmManager::AddExceptionAlarm(std::exception& e)
{
    Alarm alarm;
    if (UpdateException* ex = dynamic_cast<UpdateException* >(&e))
    {
        alarm.type = "升级异常";
        alarm.level = "Error";
        alarm.name = "升级失败";
        alarm.cause = ex->what();
    }
    else if (std::exception* ex = dynamic_cast<std::exception* >(&e))
    {
        alarm.type = "系统异常";
        alarm.level = "Info";

        if (dynamic_cast<Exception* > (ex))
        {
            alarm.name = "系统异常";
        }
        else
        {
            alarm.name = "std异常";
        }
        alarm.cause = ex->what();
    }

    return AddAlarm(alarm);
}

void AlarmManager::ClearAlarm()
{
    Alarm alarm("空闲", "空闲", "空闲");
    m_currentAlarm = alarm;
}

Alarm AlarmManager::GetCurrentAlarm() const
{
    return m_currentAlarm;
}

bool AlarmManager::AddAlarm(Alarm alarm)
{  
/**
*  - Error 错误，该级别以上的告警将显示到状态栏,输出告警文件，输出Info等级日志,详细信息输出warn等级日志,
*   首先详细信息输出warn等级日志, 格式:alarm.name(alarm.cause);cause 为空不显示。
*   然后状态栏显示，输出告警文件，info 等级日志，格式:alarm.name(alarm.cause(第一个分隔符"-"前的字符)),
*   如果第一个分隔符"-"前的字符为空则格式为：alarm.name
*
*  - Warning 一般警示,只详细信息输出Info等级日志
*   详细信息输出info等级日志, 格式:alarm.name(alarm.cause);cause 为空不显示。
*
*  - Info 无关紧要的提示,只输出warn等级日志
*  详细信息输出warn等级日志, 格式:alarm.name(alarm.cause);cause 为空不显示。
*/

    //告警信息插入数据库
        WarningRecord record;
        record.dataTime = QDateTime::currentMSecsSinceEpoch() / 1000;
        record.level = QString::fromStdString(alarm.level);
        if(!alarm.cause.empty()){
            record.cause = QString::fromStdString(alarm.name + "(" +alarm.cause + ")");
        }else{
            record.cause = QString::fromStdString(alarm.name);
        }
        record.name = QString::fromStdString(alarm.name);
        record.type = QString::fromStdString(alarm.type);
        DataBaseManager::Instance()->GetWarningTable()->InsertData({record});



    bool ret = false;
    if (alarm.level == "Error" || alarm.level == "Fatal")
    {
        String strLog = alarm.name;
        String strAlarm = alarm.name;
        if  (!alarm.cause.empty())
        {

            QStringList strlist = QString::fromUtf8(alarm.cause.c_str()).split(QRegExp("[-]"));//分割此种详细"-自动恒温-超时-目标20度"

            if (strlist.size() > 1)
            {
                strLog += "(" + alarm.cause + ")";
                logger->warn(strLog);
            }

            alarm.cause = strlist.at(0).toStdString();
            if (!alarm.cause.empty())
            {
                strAlarm += "(" + alarm.cause + ")";
            }
        }
        logger->info(strAlarm);
        ret = UpdateAlarm(alarm);
        m_upLoadStatus = true;
        Interface::Wqimc::WqimcManager::Instance()->alarm(alarm);
    }
    else if (alarm.level == "ErrorNotRedLight")
    {
        String strLog = alarm.name;
        if  (!alarm.cause.empty())
        {
            strLog += "(" + alarm.cause + ")";
        }
        logger->info(strLog);
        ret = UpdateAlarm(alarm);
    }
    else if (alarm.level == "Warning")
    {
        String strLog = alarm.name;
        if  (!alarm.cause.empty())
        {
            strLog += "(" + alarm.cause + ")";
        }
        logger->info(strLog);
        ret = true;
    }
    else if (alarm.level == "Info")
    {
        String strLog = alarm.name;
        if  (!alarm.cause.empty())
        {
            strLog += "(" + alarm.cause + ")";
        }
        logger->warn(strLog);
        ret = true;
    }
    return ret;

}

void AlarmManager::SetUpLoadStatus(bool status)
{
    m_upLoadStatus = status;
}

bool AlarmManager::GetUpLoadStatus()
{
    return m_upLoadStatus;
}

}
