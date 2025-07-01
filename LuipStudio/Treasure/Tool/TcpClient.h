
#ifndef __TREASURE_TOOL_TCP_CLIENT__
#define __TREASURE_TOOL_TCP_CLIENT__

#include "Socket.h"

namespace Treasure
{
    class TCPClient
    {
    public:
        TCPClient(QString ip, int port)
        {
            m_ip = ip;
            m_port = port;
            m_isConnected = false;
            m_socket = -1;
        }

        void SetIPPort(QString ip, int port)
        {
            m_ip = ip;
            m_port = port;
        }

        /**
         * @brief 连接
         * @param host ip
         * @param port 端口号
         */
        bool Connect()
        {
            //先关闭
            CloseConnect();

            QWriteLocker locker(&m_wrlock);

            // 申请套接字
            m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (m_socket <= 0)
            {
                m_errNo = GET_ERROR;
                m_message = QString("Socket creat failed. errno = %1").arg(m_errNo);
                return false;
            }

            // 连接
            if (!Socket::Connect(m_socket, m_ip, m_port, m_message))
            {
                m_errNo = GET_ERROR;
                Socket::CloseSocket(m_socket);
                return false;
            }

            m_isConnected = true;
            m_message = "";
            return true;
        }

        bool IsConnected()
        {
            return m_isConnected;
        }

        /**
         * @brief 关闭连接
         */
        void CloseConnect()
        {
            QWriteLocker locker(&m_wrlock);
            if (!m_isConnected)
                return;

            Socket::CloseSocket(m_socket);
            m_isConnected = false;
        }

        /**
         * @brief 发送
         */
        int Send(const char* buffer, int length)
        {
            QReadLocker locker(&m_wrlock);
            if (!m_isConnected)
            {
                m_message = QString("Socket is not connected");
                return -1;
            }

            // 发送
            int result = send(m_socket, buffer, length, MSG_NOSIGNAL);
            if (result <= 0)
            {
                m_errNo = GET_ERROR;
                m_message = QString("Socket send failed. errno = %1").arg(GET_ERROR);
            }
            return result;
        }


        bool ReadyToRecieve(int msTimeout, bool& isErr)
        {
            QReadLocker locker(&m_wrlock);
            if (!m_isConnected)
            {
                m_message = QString("Socket is not connected");
                return false;
            }

            isErr = false;

            fd_set sdset;
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = msTimeout * 1000;
            FD_ZERO(&sdset);
            FD_SET((unsigned int)m_socket, &sdset);
            int check = select(m_socket + 1, &sdset, NULL, NULL, &tv);
            if (check == 0)
                return false;
            else if (check < 0)
            {
                isErr = true;
                m_errNo = GET_ERROR;
                m_message = QString("Recieve select failed. ret = %1, errno = %2, socket = %3").arg(check).arg(GET_ERROR).arg(m_socket);
                return false;
            }

            if(!FD_ISSET((unsigned int)m_socket, &sdset))
            {
                isErr = true;
                m_errNo = GET_ERROR;
                m_message = QString("Recieve select failed(socket not in fd_set). ret = %1, errno = %2, socket = %3").arg(check).arg(GET_ERROR).arg(m_socket);
                return false;
            }

            return true;
        }

        /**
         * @brief 接收
         */
        int Recieve(char* buffer, int length)
        {
            QReadLocker locker(&m_wrlock);
            if (!m_isConnected)
            {
                m_message = QString("Socket is not connected");
                return -1;
            }

            int len = recv(m_socket, buffer, length, 0);
            if (len <= 0)
            {
                m_errNo = GET_ERROR;
                m_message = QString("Recieve err. len = %1, errno = %2, socket = %3").arg(len).arg(GET_ERROR).arg(m_socket);
            }
            return len;
        }

        QString GetErrMessage() const
        {
            return m_message;
        }

        int GetErrCode() const
        {
            return m_errNo;
        }

        int GetId() const
        {
            return m_socket;
        }

        QString GetIP() const
        {
            return m_ip;
        }

        int GetPort() const
        {
            return m_port;
        }

    private:
        int     m_errNo;
        QString m_message;
        QString m_ip;
        int     m_port;
        int     m_socket;

        std::atomic_bool m_isConnected;
        QReadWriteLock   m_wrlock;

    };


}

#endif
