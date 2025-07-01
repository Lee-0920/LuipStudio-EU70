#ifndef __TREASURE_SYSTEM_PROCESS_H__
#define __TREASURE_SYSTEM_PROCESS_H__

#include <QDir>
#include <QProcess>
#include "../SystemDef.h"

namespace Treasure
{

class Process
{
public:
    static bool IsRunning(QString name)
    {

#ifdef _WIN32
        QProcess process;
        process.start("tasklist" ,QStringList()<<"/fi"<<"imagename eq " + name);
        process.waitForFinished();
        QString outputStr = QString::fromLocal8Bit(process.readAllStandardOutput());
        return outputStr.contains(name);
#else
        QProcess process;
        process.start("pgrep" ,QStringList() << "-x" << name);
        process.waitForFinished();
        QString outputStr = QString::fromLocal8Bit(process.readAllStandardOutput());
        return !outputStr.isEmpty();
#endif

    }
};

}

#endif //
