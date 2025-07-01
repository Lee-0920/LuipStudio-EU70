/**
 * @file
 * @brief 告警文件。
 * @details
 * @version 1.0.0
 * @author lidijun
 * @date 2016/06/28
 */

#include <QFile>

#include <QDataStream>
#include "AlarmInfoFile.h"
#include <QTextStream>

namespace Measure
{

AlarmInfoFile::AlarmInfoFile(String fileName):m_fileName(fileName)
{
    QFile file(m_fileName.c_str());
    if (!file.exists())
    {
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        file.close();
    }
}

bool AlarmInfoFile::AddRecord(Alarm alarm)
{
   bool status = false;

   String strDateTime = alarm.time.toString("yyyy-MM-dd hh:mm:ss").toStdString();
   String strAlarm;
   if (!alarm.cause.empty())
   {
       strAlarm = strDateTime + " " + alarm.type + " " + alarm.name + "(" + alarm.cause +")\r\n";
   }
   else
   {
       strAlarm = strDateTime + " " + alarm.type + " " + alarm.name +"\r\n";
   }


   m_mutex.lock();

   QFile file(m_fileName.c_str());
    if (file.open(QFile::WriteOnly | QFile::Append))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << QString::fromUtf8(strAlarm.c_str());

        file.close();
        status = true;
    }

    m_mutex.unlock();

    return status;
}

}
