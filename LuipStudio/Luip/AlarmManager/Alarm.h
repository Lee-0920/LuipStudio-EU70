#if !defined(ALARMMANAGER_ALARM_H_)
#define ALARMMANAGER_ALARM_H_

#include <QDateTime>
#include "System/Types.h"

using namespace System;

namespace Measure
{

/**
 * @brief 告警。
 * @details 告警包含级别、类型、名称、内容等信息。
 *  @p 告警级别有：
 *  - Fatal 致命的，严重错误
 *  - Error 错误，该级别以上的告警将显示到状态栏,输出Info等级日志,详细信息输出warn等级日志
 *  - Warning 一般警示,只输出Info等级日志
 *  - Info 无关紧要的提示,只输出warn等级日志
 *  @p 告警类型有：
 *  - DeviceFault 设备故障
 */
class Alarm
{
public:
    Alarm(): level("Warning"), type("DeviceFault")
    {
        this->time = QDateTime::currentDateTime();
    }

    Alarm(String type, String name, String cause, String level = "Warning")
    {
        this->cause = cause;
        this->level = level;
        this->type = type;
        this->name = name;
        this->time = QDateTime::currentDateTime();
    }

    Alarm& operator =(const Alarm& other)
    {

        this->time = other.time;
        this->level = other.level;
        this->type = other.type;
        this->name = other.name;
        this->cause = other.cause;

        return *this;
    }

    void SetAlarm(String type, String name, String cause, String level = "Warning")
    {
        this->type = type;
        this->name = name;
        this->cause = cause;
        this->level = level;
        this->time = QDateTime::currentDateTime();
    }

    unsigned int GetTime()
    {
        return time.toTime_t();
    }

public:
    QDateTime time;//产生告警的时间
    String level;//告警等级
    String type;//告警类型
    String name;//告警名称
    String cause;//告警详细信息
};

}
#endif // ALARMMANAGER_ALARM_H_
