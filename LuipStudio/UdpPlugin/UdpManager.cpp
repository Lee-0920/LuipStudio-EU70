#include "UdpManager.h"
#include "UdpDef.h"
#include "Treasure/Tool/Socket.h"
#include "Treasure/Tool/NetTool.h"

using namespace UdpSpace;

UdpManager::UdpManager()
{
    Treasure::Socket::Init();
    m_recvDeviceFunc = NULL;
    m_recvNFSServer = NULL;
    m_initServer = false;
    m_initClient = false;
    qDebug("UdpManager version 6.0");

#ifdef MORE_DEBUG_MSG
    trLogger->ShowQDebug(true);
#endif

    connect(&m_timer, &QTimer::timeout, this, [this]{
        if (!m_mutex.tryLock())
            return;
        this->RefreshIp();
        m_mutex.unlock();
    });
}

UdpManager::~UdpManager()
{
    Uninit();
}

void UdpManager::InitByServer(Treasure::Logger* logger)
{
    QMutexLocker lock(&m_mutex);
    Treasure::Logger::Instance(logger);

    m_initServer = true;
    RefreshIp();

    if (!m_timer.isActive())
        m_timer.start(5000);
}

void UdpManager::InitByClient(Treasure::Logger* logger, DeviceInfo info)
{
    QMutexLocker lock(&m_mutex);
    Treasure::Logger::Instance(logger);

    m_deviceInfo = info;
    m_initClient = true;
    RefreshIp();

    if (!m_timer.isActive())
        m_timer.start(5000);
}

void UdpManager::Uninit()
{
    QMutexLocker lock(&m_mutex);

    if (m_timer.isActive())
        m_timer.stop();

    foreach (auto item, m_servers)
        item->StopThread();
    foreach (auto item, m_clients)
        item->StopThread();

    m_servers.clear();
    m_clients.clear();

    m_initServer = false;
    m_initClient = false;
}

bool UdpManager::SendSearch()
{
    QMutexLocker lock(&m_mutex);
    if (m_servers.isEmpty())
        return false;

    foreach(auto item, m_servers)
    {
        item->SendSearch();
    }
    return true;
}

bool UdpManager::SendNFSServerNotify(bool open)
{
    QMutexLocker lock(&m_mutex);
    if (m_servers.isEmpty())
        return false;

    foreach(auto item, m_servers)
    {
        item->SendNFSServerNotify(open);
    }
    return true;
}

void UdpManager::SetOnRecvDeviceFunc(OnRecvDevice func)
{
    m_recvDeviceFunc = func;
    foreach(auto item, m_servers)
    {
        item->SetOnRecvDeviceFunc(m_recvDeviceFunc);
    }
}

void UdpManager::SetOnRecvNFSServerFunc(OnRecvNFSServer func)
{
    m_recvNFSServer = func;
    foreach(auto item, m_clients)
    {
        item->SetOnRecvNFSServerFunc(func);
    }
}

void UdpManager::ClearBeforeSendSerach()
{
    QMutexLocker lock(&m_mutex);
    foreach(auto item, m_servers)
    {
        item->ClearDevices();
    }
}

QString UdpManager::GetNFSServer(bool& open)
{
    QMutexLocker lock(&m_mutex);

    QString retServer = "";
    foreach(auto item, m_clients)
    {
        bool tmp = true;
        auto server = item->GetNFSServer(tmp);
        if (server.isEmpty())
            continue;

        retServer = server;
        open = tmp;
    }
    return retServer;
}

QMap<QString, DeviceInfo> UdpManager::GetDevices()
{
    QMutexLocker lock(&m_mutex);

    QMap<QString, DeviceInfo> rets;
    foreach(auto item, m_servers)
    {
        auto arrays = item->GetDevices();
        foreach(auto key, arrays.keys())
        {
            if (rets.contains(key))
                continue;
            rets.insert(key, arrays[key]);
        }
    }
    return rets;
}

bool UdpManager::ContainServerIp(QString ip)
{
    foreach(auto item, m_servers)
    {
        if (item->GetBindIp() == ip)
            return true;
    }
    return false;
}

bool UdpManager::ContainClientIp(QString ip)
{
    foreach(auto item, m_clients)
    {
        if (item->GetBindIp() == ip)
            return true;
    }
    return false;
}

void UdpManager::RefreshIp()
{
    auto entrys = Treasure::NetTool::GetNetEntrys();

    //server refresh
    if (m_initServer)
    {
        foreach(auto item, entrys)
        {
            if (ContainServerIp(item.ip))
                return;

            auto ptr = UdpServerPtr(new UdpServer(item.ip));
            ptr->Init();

            if (m_recvDeviceFunc)
                ptr->SetOnRecvDeviceFunc(m_recvDeviceFunc);

            m_servers.append(ptr);

            trLogger->Debug("UdpManager server ip/sum:%s/%d", item.ip.toStdString().c_str(), m_servers.count());
        }
    }

    //client refresh
    if (m_initClient)
    {
        foreach(auto item, entrys)
        {
            if (ContainClientIp(item.ip))
                return;

            auto ptr = UdpClientPtr(new UdpClient(item.ip));
            ptr->SetDeviceInfo(m_deviceInfo);
            ptr->Init();

            if (m_recvNFSServer)
                ptr->SetOnRecvNFSServerFunc(m_recvNFSServer);

            m_clients.append(ptr);

            trLogger->Debug("UdpManager client ip/sum:%s/%d", item.ip.toStdString().c_str(), m_clients.count());
        }
    }
}
