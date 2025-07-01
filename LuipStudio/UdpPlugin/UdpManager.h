#ifndef Udp_MANAGER_H
#define Udp_MANAGER_H

#include "UdpClient.h"
#include "UdpServer.h"
#include <QObject>

namespace UdpSpace
{

class LUIP_SHARE UdpManager : public QObject
{
    Q_OBJECT
    TREASURE_SINGELTON(UdpManager)

public:
    UdpManager();
    ~UdpManager();

    void InitByClient(Treasure::Logger* logger, DeviceInfo info);
    void InitByServer(Treasure::Logger* logger);
    void Uninit();

public:
    bool SendSearch();
    bool SendNFSServerNotify(bool open = true);
    void SetOnRecvDeviceFunc(OnRecvDevice func);
    void SetOnRecvNFSServerFunc(OnRecvNFSServer func);
    void ClearBeforeSendSerach();
    QString GetNFSServer(bool& open);
    QMap<QString, DeviceInfo> GetDevices(); //关键字IP:Port

private:
    bool ContainServerIp(QString ip);
    bool ContainClientIp(QString ip);

    void RefreshIp();

private:
    bool m_initServer;
    bool m_initClient;
    QList<UdpServerPtr> m_servers;
    QList<UdpClientPtr> m_clients;
    QMutex m_mutex;

    DeviceInfo      m_deviceInfo;
    OnRecvDevice    m_recvDeviceFunc;
    OnRecvNFSServer m_recvNFSServer;

    QTimer m_timer;
};

}

#endif
