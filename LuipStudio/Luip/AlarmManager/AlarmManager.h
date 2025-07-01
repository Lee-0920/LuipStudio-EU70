#if !defined(ALARMMANAGER_ALARMMANAGER_H_)
#define ALARMMANAGER_ALARMMANAGER_H_

#include <memory>
#include <list>

#include "AlarmInfoFile.h"
#include "IAlarmNotifiable.h"
#include "Alarm.h"
#include "Communication/CommunicationException.h"

namespace Measure
{


/**
 * @brief 告警管理器。
 * @details
 */
class AlarmManager
{
public:
    static AlarmManager *Instance();
    void Register(IAlarmNotifiable *handle);	
    AlarmManager();
    ~AlarmManager();
    bool AddAlarm(Alarm alarm);
    bool AddCommunicationExceptionAlarm(const Communication::CommunicationException &e);
    bool AddExceptionAlarm(std::exception& e);
    void ClearAlarm();
    Alarm GetCurrentAlarm()const;
    void SetUpLoadStatus(bool status);
    bool GetUpLoadStatus();
private:
    bool UpdateAlarm(Alarm alarm);
private:
   std::list<IAlarmNotifiable* > m_notifise;
   static std::unique_ptr<AlarmManager> m_instance;
   AlarmInfoFile *alarmInfoFile;
   Alarm m_currentAlarm;
   bool m_upLoadStatus;
};

}

#endif  //ALARMMANAGER_ALARMMANAGER_H_
