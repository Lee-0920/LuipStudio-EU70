#ifndef NFSManager_H
#define NFSManager_H

#include "Treasure/Tool/NFSClient.h"

namespace System
{

class NFSManager
{
    TREASURE_SINGELTON(NFSManager)
public:
    NFSManager();

    bool IsNFSEnable();
    bool IsConnected();

    void Init();
    void Uninit();

private:
    Treasure::NFSClient m_client;
    bool m_isEnable;
    QString m_remoteIP;
};

}
#endif // NFSManager_H
