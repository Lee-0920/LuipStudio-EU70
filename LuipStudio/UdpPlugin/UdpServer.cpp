#include "UdpServer.h"
#include "Treasure/Tool/Socket.h"

#include <QDateTime>

using namespace UdpSpace;

UdpServer::UdpServer(QString bindIp) : UdpBase(bindIp)
{
    onRecvFunc = NULL;
}

/**
 * @brief 组播搜索命令
 */
bool UdpServer::SendSearch()
{
    return SendData(this->m_socket, UDP_SEARCH_BUFF);
}

/**
 * @brief 组播发送NFS服务通知
 */
bool UdpServer::SendNFSServerNotify(bool open)
{
    return SendData(this->m_socket, QString(UDP_NFSSERVER_NOTIFY) + QString("OPEN=%1").arg(open ? "true":"false"));
}

/**
 * @brief 处理数据
 */
void UdpServer::DealRecvData(char* buffer, int len, QString ip)
{
    static int cmpLen = sizeof(UDP_DEVICE_INFO) - 1;
    if (len < cmpLen || memcmp(buffer, UDP_DEVICE_INFO, cmpLen) != 0)
        return;

    DeviceInfo item;
    item.ip = ip;
    item.cononectTime = QDateTime::currentDateTime().toTime_t();

    QString data = QString::fromUtf8(buffer, len);
    if (!StringToDeviceInfo(data, item))
        return;

    {
        QMutexLocker lock(&mutex);
        QString key = ip + ":" + QString::number(item.port);
        if (devices.contains(key))
            devices[key] = item;
        else
            devices.insert(key, item);

        if (onRecvFunc == NULL)
            return;
    }

    onRecvFunc(item);
}

/**
 * @brief 清空设备
 */
void UdpServer::ClearDevices()
{
    QMutexLocker lock(&mutex);
    devices.clear();
}

/**
 * @brief 获取设备列表
 */
QMap<QString, DeviceInfo> UdpServer::GetDevices()
{
    QMutexLocker lock(&mutex);
    return devices;
}

void UdpServer::SetOnRecvDeviceFunc(OnRecvDevice func)
{
    QMutexLocker lock(&mutex);
    this->onRecvFunc = func;
}
