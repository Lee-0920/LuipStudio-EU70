#ifndef NFSMANAGER_H
#define NFSMANAGER_H

#include "Treasure/SystemDef.h"

class NFSManager
{
    TREASURE_SINGELTON(NFSManager)
public:
    NFSManager();

    QString GetExportPath();
    void    Check();


private:
    QString m_exportPath;
};

#endif // NFSMANAGER_H
