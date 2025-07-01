/**
 * @file
 * @brief 告警文件输出。
 * @details 
 * @version 1.0.0
 * @author ludijun
 * @date 2016/06/28
 */


#if !defined(ALARMMANAGER_ALARMINFOFILE_H_)
#define ALARMMANAGER_ALARMINFOFILE_H_

#include <list>
#include <QMutex>
#include "System/Types.h"
#include "Alarm.h"

using namespace System;

namespace Measure
{

/**
 * @brief 告警文件输出。
 * @details
 */
class AlarmInfoFile
{
public:
    AlarmInfoFile(String fileName);
    bool AddRecord(Alarm alarm);

private:
   String m_fileName;
   QMutex m_mutex;
};

}

#endif  //ALARMMANAGER_ALARMINFOFILE_H_
