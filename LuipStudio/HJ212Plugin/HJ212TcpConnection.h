/**
 * @brief TCP通信
 * @author huangyue
 * @date 2020/11/11
 */
#ifndef HJ212TCPCONNECTION_H
#define HJ212TCPCONNECTION_H

#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include <QReadWriteLock>

#include "API/HJ212Packet.h"
#include "HJ212Connection.h"

namespace HJ212
{

class HJ212TcpConnection : public HJ212Connection
{
public:
    explicit HJ212TcpConnection(std::string ip, int port, OnReceiveCallBack onReceiveCallBack, void* onReceiveParent);
    ~HJ212TcpConnection();

    bool Connect();                           //连接
    void CloseConnection();                   //关闭连接

    int Send(HJ212PackPtr pack, int &error);  //发送数据包
    int ReceiveData();                        //接收数据

private:
    int m_serverIP;
    int m_serverPort;
    int m_socket;        //套接字

    int m_recvBuffersCapacity; //接收缓存的最大容量
    int m_recvBytes;           //接收缓存当前大小
    std::string m_recvBuffers; //接收缓存

    int m_lastAbnormalRecvError; //上一次异常接收的错误码
    int m_lastAbnormalRecvLen;   //上一次异常接收的数据长度
    int m_lastAbnormalRecvCount; //上一次异常接收计数器(用于统计同个错误码、同个数据长度的异常次数，减少日志)
};

typedef std::shared_ptr<HJ212TcpConnection> HJ212TcpConnectionPtr;

} // namespace HJ212

#endif // HJ212CONNECTION_H
