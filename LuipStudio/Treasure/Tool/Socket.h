
#ifndef __TREASURE_TOOL_SOCKET__
#define __TREASURE_TOOL_SOCKET__

#include <atomic>

#include <QString>
#include <QReadWriteLock>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <errno.h>
    #define MSG_NOSIGNAL 0
    #define GET_ERROR WSAGetLastError()
#else
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define GET_ERROR errno
#endif



namespace Treasure
{
    class Socket
    {
    public:

        /**
         * @brief 初始化网络
         */
        static bool Init()
        {
            #ifdef _WIN32
            WSADATA ws;
            if (WSAStartup(MAKEWORD(2,2), &ws))
                return false;
            #endif

            return true;
        }

        /**
         * @brief 释放网络缓存
         */
        static void Uninit()
        {
            #ifdef _WIN32
            WSACleanup();
            #endif
        }

    public:

#ifdef _WIN32
        /**
         * @brief 关闭套接字
         */
        static void CloseSocket(int& socket)
        {
            if (socket > 0)
                closesocket(socket);
            socket = 0;
        }
#else
        static void CloseSocket(int& socket)
        {
            if (socket > 0)
                close(socket);
            socket = 0;
        }
#endif

    public:
        /**
         * @brief 监听
         */
        static bool Select(int socket, int msTimeout, QString& errmsg)
        {
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = msTimeout * 1000;

            fd_set sdset;
            FD_ZERO(&sdset);
            FD_SET((unsigned int)socket, &sdset);

            int check = select(socket + 1, &sdset, NULL, NULL, &tv);
            if (check == 0)
                return false;
            else if (check < 0)
            {
                errmsg = QString("Select failed(timeout). ret = %1, errno = %2, socket = %3")
                        .arg(check)
                        .arg(GET_ERROR)
                        .arg(socket);
                return false;
            }

            if(!FD_ISSET((unsigned int)socket, &sdset))
            {
                errmsg = QString("Select failed(socket not in fd_set). ret = %1, errno = %2, socket = %3")
                        .arg(check)
                        .arg(GET_ERROR)
                        .arg(socket);
                return false;
            }
            return true;
        }

        /**
         * @brief 连接
         */
        static bool Connect(int socket, QString ip, int port, QString& errmsg)
        {
            struct sockaddr_in serverAddress;
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = inet_addr(ip.toStdString().c_str());
            serverAddress.sin_port = htons(port);
            if (connect(socket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) != 0)
            {
                errmsg = QString("Socket connect failed. errno = %1").arg(GET_ERROR);
                return false;
            }
            return true;
        }

        /**
         * @brief 绑定
         * @param socket 套接字
         * @param port 端口号
         * @param ip 绑定的IP,如果为空,则用任意IP
         */
        static bool Bind(int socket, int port, QString ip = "")
        {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = htonl(INADDR_ANY);

            if (!ip.isEmpty())
                addr.sin_addr.s_addr = inet_addr(ip.toStdString().c_str());

            if (bind(socket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
                return false;
            return true;
        }

    public:
        /**
         * @brief 端口复用
         */
        static bool SetOptReuseAddr(int socket, bool check = true)
        {
            int opt = check ? 1 : 0;
            if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR ,(char*)&opt, sizeof(opt)) < 0)
                return false;
            return true;
        }

        /**
         * @brief 加入组
         * @param socket 套接字
         * @param remoteAddr 组IP
         * @param localIP 本地IP,如果为空,则用任意IP
         */
        static bool SetOptAddMemberShip(int socket, QString remoteIP, QString localIP = "")
        {
            struct ip_mreq mreq;
            mreq.imr_multiaddr.s_addr = inet_addr(remoteIP.toStdString().c_str());
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);

            //指定接口接收组播信息
            if (!localIP.isEmpty())
                mreq.imr_interface.s_addr = inet_addr(localIP.toStdString().c_str());

            if (setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
                return false;
            return true;
        }

        static bool SetOptAddSourceMemberShip(int socket, QString remoteIP, QString srcIP, QString localIP = "")
        {
            struct ip_mreq_source mreq;
            mreq.imr_multiaddr.s_addr = inet_addr(remoteIP.toStdString().c_str());
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);
            mreq.imr_sourceaddr.s_addr = inet_addr(srcIP.toStdString().c_str());

            //指定接口接收组播信息
            if (!localIP.isEmpty())
                mreq.imr_interface.s_addr = inet_addr(localIP.toStdString().c_str());

            if (setsockopt(socket, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
                return false;
            return true;
        }

        /**
         * @brief 设置默认接口
         * @param name desc
         * @return
         */
        static bool SetOptIPMulticastIF(int socket, QString ip)
        {
            struct in_addr addr;
            memset(&addr, 0, sizeof(struct in_addr));

            addr.s_addr=inet_addr(ip.toStdString().c_str());
            if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_IF, (char*)&addr, sizeof(addr)) < 0)
                return false;
            return true;
        }

    public:
        /**
         * @brief 发送数据到指定地址
         */
        static bool SendTo(int socket, QString ip, int port,const char* data, int dataLen)
        {
            struct sockaddr_in addrSrv;
            addrSrv.sin_family=AF_INET;
            addrSrv.sin_addr.s_addr = inet_addr(ip.toStdString().c_str());
            addrSrv.sin_port=htons(port);

        #ifdef _WIN32
            if (sendto(socket, data, dataLen, 0, (struct sockaddr *)&addrSrv,sizeof(addrSrv)) > 0)
                return true;
        #else
            if (sendto(socket, data, dataLen, 0, (__CONST_SOCKADDR_ARG)&addrSrv,sizeof(addrSrv)) > 0)
                return true;
        #endif
            return false;
        }

        /**
         * @brief 接收数据
         * @param socket 套接字
         * @param buff 缓存
         * @param buffLen 缓存大小
         * @param recvIP 接收的IP
         * @param recvPort 接收的端口号
         * @return 接收的数据长度
         */
        static int RecvFrom(int socket, char* buff, int buffLen, QString& recvIP, int& recvPort)
        {
            struct sockaddr_in group_addr;
            socklen_t addr_len = sizeof(group_addr);

    #ifdef _WIN32
            int len = recvfrom(socket, buff, buffLen, 0, (struct sockaddr *) &group_addr, (int *)&addr_len);
    #else
            int len = recvfrom(socket, buff, buffLen, 0, (struct sockaddr *) &group_addr, (socklen_t *)&addr_len);
    #endif
            if (len <= 0)
                return len;

            recvIP = inet_ntoa(group_addr.sin_addr);
            recvPort = group_addr.sin_port;
            return len;
        }
    };


}

#endif
