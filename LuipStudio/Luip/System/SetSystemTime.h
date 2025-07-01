#ifndef SYSTEM_SYSTEMTIME_H
#define SYSTEM_SYSTEMTIME_H

#include <QDateTime>
#include "System/Types.h"

namespace System
{
struct DataTime
{
    Uint16 year;
    Uint8 month;
    Uint8 day;
    Uint8 hour;
    Uint8 min;
    Uint8 sec;
};
class SetSystemTime
{
public:
    static void SetTime(QDateTime datetime);
    static void SetTime(int year, int month, int day, int hour, int min, int sec);
    static void SetDataTime(DataTime datetime);
};

}
#endif // SYSTEM_SYSTEMTIME_H
