#include "UdpDef.h"
#include "UdpBase.h"
#include "Treasure/Tool/Socket.h"

using namespace UdpSpace;

UdpBase::UdpBase(QString bindIp)
{
    m_socket = 0;
    m_recvThread = NULL;
    m_localIp = bindIp;

}

UdpBase::~UdpBase()
{
    Uninit();
}

void UdpBase::Init()
{
    //启动接收线程
    m_recvThread = Treasure::Thread::CreatThread(&ThreadRecieve, this);
}

void UdpBase::Uninit()
{
    UninitSocket();
}

void UdpBase::StopThread()
{
    if (m_recvThread)
        m_recvThread->RequestInterruption();
}

QString UdpBase::GetBindIp()
{
    return m_localIp;
}

/**
 * @brief 初始化套接字
 */
bool UdpBase::InitSocket()
{
    m_socket = socket(AF_INET,SOCK_DGRAM,0);
    if (m_socket <= 0)
    {
        trLogger->debug("UdpBase::InitSocket creat failed.");
        return false;
    }

    //设置端口复用
    if (!Treasure::Socket::SetOptReuseAddr(m_socket))
    {
        Treasure::Socket::CloseSocket(m_socket);
        trLogger->debug("UdpManager::InitSocket setsockopt SO_REUSEADDR failed.");
        return false;
    }

    //绑定组播端口（如果加指定IP似乎不行）
    if (!Treasure::Socket::Bind(m_socket, UDP_MCAST_PORT/*, m_localIp.isEmpty() ? "0.0.0.0" : m_localIp*/))
    {
        Treasure::Socket::CloseSocket(m_socket);
        trLogger->debug("UdpManager::InitSocket bind failed.");
        return false;
    }

    //加入组播
    if (!Treasure::Socket::SetOptAddMemberShip(m_socket, UDP_MCAST_ADDR, m_localIp))
    {
        Treasure::Socket::CloseSocket(m_socket);
        trLogger->debug("UdpManager::InitSocket(%s) setsockopt IP_ADD_MEMBERSHIP failed.", m_localIp.toStdString().c_str());
        return false;
    }

    //指定组播数据的出口网卡
    if (!m_localIp.isEmpty())
    {
        if (!Treasure::Socket::SetOptIPMulticastIF(m_socket, m_localIp))
        {
            Treasure::Socket::CloseSocket(m_socket);
            trLogger->debug("UdpManager::InitSocket setsockopt IP_MULTICAST_IF failed.");
            return false;
        }
    }

    return true;
}

void UdpBase::UninitSocket()
{
    if (m_recvThread)
    {
        m_recvThread->RequestInterruption();
        m_recvThread->Wait();

        delete m_recvThread;
        m_recvThread = NULL;
    }
    Treasure::Socket::CloseSocket(m_socket);
}

/**
 * @brief 接收线程
 */
void UdpBase::ThreadRecieve(UdpBase* handle)
{
    trLogger->debug("UdpManager::InitRecvSock ThreadRecieve start.");

#ifndef _WIN32
    QThread::msleep(100);
#endif

    char msgbuf[2048];
    while (!handle->m_recvThread->IsRequestInterruption())
    {
        if (handle->m_socket <= 0 && !handle->InitSocket())
        {
            int count = 5;
            while(count-- > 0)
            {
                QThread::msleep(1000);
                if (handle->m_recvThread->IsRequestInterruption())
                    break;
            }
        }

        //阻塞等待
        QString errmsg;
        if (!Treasure::Socket::Select(handle->m_socket, 1000, errmsg))
            continue;

        //接收数据
        QString recvIP;
        int recvPort;
        int len = Treasure::Socket::RecvFrom(handle->m_socket, msgbuf, 2048, recvIP, recvPort);
        if (len <= 0)
            continue;

#ifdef MORE_DEBUG_MSG
        trLogger->debug("UdpManager recv(%s):%s", recvIP.toStdString().c_str(), msgbuf);
#endif

        handle->DealRecvData(msgbuf, len, recvIP);
    }

    trLogger->debug("UdpManager::InitRecvSock ThreadRecieve end.");
}


/**
 * @brief 组播数据
 */
bool UdpBase::SendData(int socket, QString data, QString ip)
{
    if (socket <= 0)
        return false;
    if (ip.isEmpty())
        ip = UDP_MCAST_ADDR;

    bool check = Treasure::Socket::SendTo(socket, ip, UDP_MCAST_PORT,
                                    data.toStdString().c_str(),
                                    strlen(data.toStdString().c_str()));
#ifdef MORE_DEBUG_MSG
    if (!check)
        trLogger->debug("UdpManager SendData failed: %s", data.toStdString().c_str());
#endif
    return check;
}

QString UdpBase::DeviceInfoToString(DeviceInfo info)
{
    QString data = QString("PORT=%2&PASSWORD=%3&NAME=%4")
            .arg(info.port)
            .arg(info.password)
            .arg(info.name);
    return data;
}

bool UdpBase::StringToDeviceInfo(QString data, DeviceInfo& info)
{
    data = data.replace(UDP_DEVICE_INFO, "");

    auto array = data.split("&");
    if (array.size() == 3)
    {
        info.port = array[0].replace("PORT=", "").toInt();
        info.password = array[1].replace("PASSWORD=", "");
        info.name = array[2].replace("NAME=", "");
        return true;
    }
    return false;
}
