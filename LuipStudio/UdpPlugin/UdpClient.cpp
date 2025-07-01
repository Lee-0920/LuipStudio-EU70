#include "UdpClient.h"

using namespace UdpSpace;

UdpClient::UdpClient(QString bindIp) : UdpBase(bindIp)
{
    onRecvFunc = NULL;
    nfsServer = "";
    nfsEnable = true;
}

/**
 * @brief 处理数据
 */
void UdpClient::DealRecvData(char* buffer, int len, QString ip)
{
    static int cmpSearchLen = sizeof(UDP_SEARCH_BUFF) - 1;
    static int cmpNFSLen = sizeof(UDP_NFSSERVER_NOTIFY) - 1;

    //接收搜索信号
    if (len >= cmpSearchLen && memcmp(buffer, UDP_SEARCH_BUFF, cmpSearchLen) == 0)
    {
        //组播设备信息
        QString data = UDP_DEVICE_INFO + DeviceInfoToString(this->device);
        //指定发送的IP，而不是发到组播中，这样可以减少其他设备的垃圾数据
        //而且单播的方式比较稳，有些机子总是接收不到组播的数据
        SendData(this->m_socket, data);
        SendData(this->m_socket, data, ip);
    }

    //接收NFS服务信号
    if (len >= cmpNFSLen && memcmp(buffer, UDP_NFSSERVER_NOTIFY, cmpNFSLen) == 0)
    {
        QString data = QString::fromUtf8(buffer, len).replace(UDP_NFSSERVER_NOTIFY, "").replace("OPEN=", "");
        bool open = true;
        if (data.toLower() == "false")
            open = false;

        QMutexLocker lock(&mutex);
        nfsServer = ip;
        nfsEnable = open;
        if (this->onRecvFunc != NULL)
            this->onRecvFunc(nfsServer,open);

#ifdef MORE_DEBUG_MSG
        trLogger->debug("UdpManager recv nfs server ip: %s", nfsServer.toStdString().c_str());
#endif
    }
}

QString UdpClient::GetNFSServer(bool& open)
{
    QMutexLocker lock(&mutex);
    open = nfsEnable;
    return nfsServer;
}

void UdpClient::SetOnRecvNFSServerFunc(OnRecvNFSServer func)
{
    QMutexLocker lock(&mutex);
    this->onRecvFunc = func;
}
