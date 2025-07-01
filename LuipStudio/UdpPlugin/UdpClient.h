#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include "UdpBase.h"

namespace UdpSpace
{

#define OnRecvNFSServer std::function<void(QString,bool)>

class LUIP_SHARE UdpClient : public UdpBase
{
public:
    UdpClient(QString bindIp = "");

    void SetDeviceInfo(DeviceInfo info) {device = info;}
    void SetOnRecvNFSServerFunc(OnRecvNFSServer func);
    QString GetNFSServer(bool& open);

private:
    virtual void DealRecvData(char* buffer, int len, QString ip);

private:
    DeviceInfo device;
    QString nfsServer;
    bool    nfsEnable;
    std::function<void(QString,bool)> onRecvFunc;
};

typedef std::shared_ptr<UdpClient> UdpClientPtr;

}

#endif // UDPCLIENT_H
