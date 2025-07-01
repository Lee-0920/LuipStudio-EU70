#ifndef HJ212CONNECTION_H
#define HJ212CONNECTION_H

#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include <QReadWriteLock>

#include "API/HJ212Packet.h"

namespace HJ212
{

class HJ212Connection
{
public:
    typedef void (*OnReceiveCallBack)(void* parent, HJ212PackPtr pack);

public:
    explicit HJ212Connection(OnReceiveCallBack onReceiveCallBack, void* onReceiveParent);
    virtual ~HJ212Connection();

public:
    virtual bool Connect();                           //连接
    virtual void CloseConnection();                   //关闭连接
    virtual bool IsConnected();                       //查询连接状态

    virtual int Send(HJ212PackPtr pack, int &error);  //发送数据包
    virtual int ReceiveData();                        //接收数据

protected:
    std::atomic_bool m_isConnected;        //是否连接
    QReadWriteLock m_wrlock;

protected:
    OnReceiveCallBack m_onReceive;
    void* m_onReceiveParent;
};

typedef std::shared_ptr<HJ212Connection> HJ212ConnectionPtr;

} // namespace HJ212
#endif // HJ212CONNECTION_H
