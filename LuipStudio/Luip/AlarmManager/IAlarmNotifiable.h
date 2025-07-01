#if !defined(ALARMMANAGER_IALARMNOTIFIABLE_H_)
#define ALARMMANAGER_IALARMNOTIFIABLE_H_

#include "Alarm.h"

namespace Measure
{

/**
 * @brief 告警上报接口。
 * @details 观察者。
 */
class IAlarmNotifiable
{
public:
    IAlarmNotifiable() {}
    virtual ~IAlarmNotifiable() {}

public:
    virtual void OnAlamChanged(Alarm alarmAll) = 0;
};


}

#endif  //ALARMMANAGER_IALARMNOTIFIABLE_H_
