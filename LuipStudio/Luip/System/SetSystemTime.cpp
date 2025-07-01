#include "SetSystemTime.h"
#include <time.h>
#include "Log.h"
#include <stdio.h>
#include "FlowManager/MeasureScheduler.h"

#ifndef  _CS_X86_WINDOWS
#include <sys/time.h>
#endif

using namespace Flow;

namespace System
{
void SetSystemTime::SetDataTime(DataTime datetime)
{
#ifdef    _CS_ARM_LINUX
    QDateTime oldDateTime = QDateTime::currentDateTime();

    struct tm *t_tm;
    struct timeval t_timeval;
    time_t t_timet;

    t_timet=time(NULL);
    t_tm=localtime(&t_timet);

    t_tm->tm_hour = datetime.hour;
    t_tm->tm_min = datetime.min;
    t_tm->tm_sec = datetime.sec;
    t_tm->tm_year = datetime.year- 1900;
    t_tm->tm_mon = datetime.month - 1;
    t_tm->tm_mday = datetime.day;

    t_timet=mktime(t_tm);

    t_timeval.tv_sec = t_timet;
    t_timeval.tv_usec = 0;

    settimeofday(&t_timeval,NULL);

    //强制写入到CMOS
    system("hwclock -w");

    MeasureScheduler::Instance()->SyncSysTimeToSchedule(oldDateTime);

    logger->debug("修改系统时间 %4d-%02d-%02d %02d:%02d:%02d", datetime.year, datetime.month, datetime.day, datetime.hour, datetime.min, datetime.sec);
#endif
}

void SetSystemTime::SetTime(QDateTime datetime)
{
#ifdef    _CS_ARM_LINUX
    QDateTime oldDateTime = QDateTime::currentDateTime();
    QTime times = datetime.time();
    QDate dates = datetime.date();

    struct tm *t_tm;
    struct timeval t_timeval;
    time_t t_timet;

    t_timet=time(NULL);
    t_tm=localtime(&t_timet);

    t_tm->tm_hour = times.hour();
    t_tm->tm_min = times.minute();
    t_tm->tm_sec = times.second();
    t_tm->tm_year = dates.year()- 1900;
    t_tm->tm_mon = dates.month() - 1;
    t_tm->tm_mday = dates.day();

    t_timet=mktime(t_tm);

    t_timeval.tv_sec = t_timet;
    t_timeval.tv_usec = 0;

    settimeofday(&t_timeval,NULL);

    //强制写入到CMOS
    system("hwclock -w");

    MeasureScheduler::Instance()->SyncSysTimeToSchedule(oldDateTime);

    logger->debug("修改系统时间 %s", datetime.toString("yyyy-MM-dd hh:mm:ss").toLatin1().data());
#endif
}

void SetSystemTime::SetTime(int year, int month, int day, int hour, int min, int sec)
{
#ifdef    _CS_ARM_LINUX
    QDateTime oldDateTime = QDateTime::currentDateTime();

    struct tm *t_tm;
    struct timeval t_timeval;
    time_t t_timet;

    t_timet=time(NULL);
    t_tm=localtime(&t_timet);

    t_tm->tm_hour = hour;
    t_tm->tm_min = min;
    t_tm->tm_sec = sec;
    t_tm->tm_year = year- 1900;
    t_tm->tm_mon = month - 1;
    t_tm->tm_mday = day;

    t_timet=mktime(t_tm);

    t_timeval.tv_sec = t_timet;
    t_timeval.tv_usec = 0;

    settimeofday(&t_timeval,NULL);

    //强制写入到CMOS
    system("hwclock -w");

    MeasureScheduler::Instance()->SyncSysTimeToSchedule(oldDateTime);

    logger->debug("修改系统时间 %4d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec);
#endif

}

}
