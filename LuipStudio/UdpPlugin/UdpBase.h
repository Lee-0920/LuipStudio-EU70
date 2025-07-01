#ifndef UDPBASE_H
#define UDPBASE_H

#include <map>
#include <memory>
#include <mutex>

#include <QMap>
#include <QString>

#include "UdpDef.h"
#include "Treasure/LuipShare.h"
#include "Treasure/SystemDef.h"

namespace UdpSpace
{

class UdpBase
{
public:
    UdpBase(QString bindIp = "");
    virtual ~UdpBase();

    void Init();
    void Uninit();

    void StopThread();
    QString GetBindIp();

public:
    bool InitSocket();
    void UninitSocket();

private:
    static void ThreadRecieve(UdpBase* handle);

protected:
    static bool    SendData(int socket, QString data, QString ip = "");
    static QString DeviceInfoToString(DeviceInfo info);
    static bool    StringToDeviceInfo(QString data, DeviceInfo& info);

    virtual void   DealRecvData(char* buffer, int len, QString ip) = 0;

protected:
    QMutex  mutex;
    int     m_socket;
    QString m_localIp;
    Treasure::Thread* m_recvThread;
};

}
#endif // UDPBASE_H
