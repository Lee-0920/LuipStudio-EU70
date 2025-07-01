#ifndef HJ212SerialConnection_H
#define HJ212SerialConnection_H

#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <QObject>
#include <QMutex>
#include <QSerialPort>
#include <QReadWriteLock>

#include "System/Types.h"
#include "API/HJ212Packet.h"
#include "HJ212Connection.h"
#include "HJ212Data.h"

namespace HJ212
{

#define MAX_BUFFER_LENGTH   1036

class HJ212SerialConnection : public QObject, public HJ212Connection
{
    Q_OBJECT

public:
    HJ212SerialConnection(SerialPortConfig com, OnReceiveCallBack onReceiveCallBack, void* onReceiveParent);
    ~HJ212SerialConnection();

public:
    bool Connect();
    void CloseConnection();

    int Send(HJ212PackPtr pack, int& errNum);   //发送数据包
    int ReceiveData();

signals:
    void SignalSendData(std::string buffer, int len);
    void SignalSendDataBlocking(std::string buffer, int len);
    void SignalReOpen();
    void SignalReOpenBlocking();

private slots:
    void SlotReceiveData();
    void SlotSendData(std::string buffer, int len);
    void SlotReOpen();

private:
    QSerialPort* m_serialPort;
    QString m_threadStr;

    QReadWriteLock m_wrlock;
    QMutex m_recvMute;

    int m_recvBuffersCapacity; //接收缓存的最大容量
    int m_recvBytes;           //接收缓存当前大小
    std::string m_recvBuffers; //接收缓存

};

}
#endif // HJ212SerialConnection_H
