#include <stdio.h>
#include <thread>
#include <QTextCodec>
#include <QDebug>

#include "HJ212TcpConnection.h"
#include "Log.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <errno.h>
    #define MSG_NOSIGNAL 0
    #define GET_ERROR WSAGetLastError()
    void CloseSocket(int& socket)
    {
        if (socket > 0)
            closesocket(socket);
        socket = 0;
    }
#else
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define GET_ERROR errno
    void CloseSocket(int& socket)
    {
        if (socket > 0)
            close(socket);
        socket = 0;
    }
#endif


namespace HJ212
{

HJ212TcpConnection::HJ212TcpConnection(std::string ip, int port, OnReceiveCallBack onReceiveCallBack, void* onReceiveParent)
    :HJ212Connection(onReceiveCallBack, onReceiveParent)
{
    //服务地址和端口
    m_serverIP = inet_addr(ip.c_str());
    m_serverPort = port;

    //网络参数
    m_socket = 0;

    //接收缓存
    m_recvBytes = 0;
    m_recvBuffers = "";
    m_recvBuffersCapacity = HJ212_FINAL_PACKET_MAXSIZE * 2;

    //上一次接收异常的参数
    m_lastAbnormalRecvError = 0;
    m_lastAbnormalRecvLen = 0;
    m_lastAbnormalRecvCount = 0;
}

HJ212TcpConnection::~HJ212TcpConnection()
{
    CloseConnection();
}

bool HJ212TcpConnection::Connect()
{
    QWriteLocker locker(&m_wrlock);

#ifdef _WIN32
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2,2), &ws))
    {
        logger->warn("HJ212TcpConnection WSAStartup() Fail!");
        return false;
    }
#endif
    // 关闭套接字，如果有的话
    CloseSocket(m_socket);
    // 缓存包清空 | 状态修改
    m_recvBuffers = "";
    m_recvBytes = 0;
    m_isConnected = false;

    // 申请套接字
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket <= 0)
    {
        logger->warn("HJ212TcpConnection socket creat failed! errno = %d", GET_ERROR);
        return false;
    }

    // 连接
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = m_serverIP;
    serverAddress.sin_port = htons(m_serverPort);
    int check = connect(m_socket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    if (check == 0)
    {
        m_isConnected = true;
        logger->debug("HJ212TcpConnection socket connect success.");
        return true;
    }
    else
    {
        // 关闭套接字
        CloseSocket(m_socket);
        logger->warn("HJ212TcpConnection socket connect failed! errno = %d", GET_ERROR);
        return false;
    }
}


void HJ212TcpConnection::CloseConnection()
{
    QWriteLocker locker(&m_wrlock);
    if (!m_isConnected)
        return;

    CloseSocket(m_socket);
    m_isConnected = false;
}

int HJ212TcpConnection::Send(HJ212PackPtr pack, int& error)
{
    QReadLocker locker(&m_wrlock);
    if (!m_isConnected)
    {
        logger->warn("HJ212TcpConnection unconnect!(send)");
        error = -1;
        return 0;
    }

    // 装载通讯包
    unsigned int crc16 = 0;
    int bufferSize = 0;
    std::string buffer = "";
    if (!pack->LoadFinalPacket(buffer, bufferSize, crc16))
    {
        logger->warn("HJ212TcpConnection final packet too long! length = %d > %d, dataLen = %d", bufferSize, HJ212_FINAL_PACKET_MAXSIZE, pack->GetDataLength());
        error = -1;
        return 0;
    }

    // 打印日志：HJ212报文记录
    if (hj212Logger)
    {
        std::string cmd = pack->GetCN();
        std::string data = pack->GetDataFormatChinese();
        hj212Logger->debug("发 %s %s%04X", cmd.c_str(), data.c_str(), crc16);
    }
    // 发送
    int result = send(m_socket, (const char *)buffer.c_str(), bufferSize, MSG_NOSIGNAL);
    if (result <= 0)
    {
        error = GET_ERROR;
    }
    return result;
}


int HJ212TcpConnection::ReceiveData()
{
    QReadLocker locker(&m_wrlock);
    int msecond = 10;
	
    if (!m_isConnected)
    {
        logger->warn("HJ212TcpConnection unconnect!(receiveData)");
        return -1;
    }

    // 设置接收超时
    int check = 0;
    if (msecond > 0)
    {
        fd_set sdset;
        struct timeval tv;

        tv.tv_sec = 0;
        tv.tv_usec = msecond * 1000;
        FD_ZERO(&sdset); //套节字集合清空
        FD_SET((unsigned int)m_socket, &sdset); //套节字加到集合
        check = select(m_socket + 1, &sdset, NULL, NULL, &tv);
        if (check == 0) //超时
        {
            return 0;
        }
        else if (check < 0)
        {
            logger->warn("HJ212TcpConnection select failed, ret = %d, errno = %d, socket = %d", check, GET_ERROR, m_socket);
            return -1;
        }

        if(!FD_ISSET((unsigned int)m_socket, &sdset))
        {
            logger->warn("HJ212TcpConnection select failed(socket not in fd_set), ret = %d, errno = %d, socket = %d", check, GET_ERROR, m_socket);
            return -1;
        }
    }

    // 读取数据
    char tmpBuffer[(HJ212_FINAL_PACKET_MAXSIZE + 1) / 2] = {0};
    int len = recv(m_socket, (char *)tmpBuffer, sizeof(tmpBuffer), 0);
    if (len <= 0)
    {
        int errnum = GET_ERROR;
        if (m_lastAbnormalRecvError != errnum || m_lastAbnormalRecvLen != len)
        {
            m_lastAbnormalRecvError = errnum;
            m_lastAbnormalRecvLen = len;
            m_lastAbnormalRecvCount = 0;
        }
        else
            m_lastAbnormalRecvCount++;

        // 异常次数<=10即输出日志 + (异常次数<=100 && 异常次数可以被10整除)即输出日志
        //if (m_lastAbnormalRecvCount <= 10 || (m_lastAbnormalRecvCount <= 100 && m_lastAbnormalRecvCount % 10 == 0))
        // 日志太多了，这里只打印一条日志
        if (m_lastAbnormalRecvCount == 0)
             logger->debug("HJ212TcpConnection recv err(count:%d). len = %d, errno = %d, socket = %d, check=%d", m_lastAbnormalRecvCount, len, errnum, m_socket, check);
        return -1;
    }

    //接收异常清空
    m_lastAbnormalRecvCount = 0;
    m_lastAbnormalRecvLen = 0;
    m_lastAbnormalRecvError = 0;

    // 合并缓存包
    m_recvBuffers += std::string(tmpBuffer);
    m_recvBytes = m_recvBytes + len;

    while(true)
    {
        // 从缓存包中查找完整的通讯包（##开头 \r\n结尾）
        int finalPacketSize = 0;
        std::string finalPack = "";
        bool check = HJ212Packet::FindFinalPacket(m_recvBuffers, m_recvBytes, finalPack, finalPacketSize);
        if (!check)
            break;

        // 检查通讯包中的crc和数据段长度
        int dataSize = 0;
        std::string data = "";
        check = HJ212Packet::CheckFinalPacket(finalPack, finalPacketSize, data, dataSize);
        if (!check)
        {
            logger->warn("HJ212TcpConnection CheckFinalPacket err. packet=%s",finalPack.c_str());
            continue;
        }

        // 接收响应回调
        HJ212PackPtr pack(new HJ212Packet(data.c_str(), dataSize));
        if (hj212Logger)
            hj212Logger->debug("收 %s %s", pack->GetCN().c_str(), data.c_str());
        if (m_onReceive)
            m_onReceive(m_onReceiveParent, pack);
    }

    // 防止服务乱发数据
    if (m_recvBytes >= m_recvBuffersCapacity)
    {
        logger->warn("HJ212TcpConnection recv finalpacket too long(maybe data err), length=%d data=%s", m_recvBytes, m_recvBuffers.c_str());
        m_recvBytes = 0;
        m_recvBuffers = "";
    }

    return len;
}


}
