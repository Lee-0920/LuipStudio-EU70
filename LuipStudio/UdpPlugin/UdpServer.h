#ifndef UDPSERVER_H
#define UDPSERVER_H

#include "UdpBase.h"
#include "functional"

namespace UdpSpace
{

#define OnRecvDevice std::function<void(DeviceInfo)>

class LUIP_SHARE UdpServer : public UdpBase
{
public:
    UdpServer(QString bindIp = "");

    void SetOnRecvDeviceFunc(OnRecvDevice func);

    bool SendSearch();
    bool SendNFSServerNotify(bool open);
    void ClearDevices();
    QMap<QString, DeviceInfo> GetDevices(); //关键字IP:Port

private:
    virtual void DealRecvData(char* buffer, int len, QString ip);

private:
    QMap<QString, DeviceInfo> devices;
    std::function<void(DeviceInfo)> onRecvFunc;
};

typedef std::shared_ptr<UdpServer> UdpServerPtr;
};

#endif
