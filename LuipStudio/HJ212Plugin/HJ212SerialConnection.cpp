#include "HJ212SerialConnection.h"
#include "Log.h"
#include <QThread>



using namespace HJ212;
using namespace System;

static QString GetThreadString()
{
    return QStringLiteral("%1").arg(quintptr(QThread::currentThreadId()));
}

HJ212SerialConnection::HJ212SerialConnection(SerialPortConfig com, OnReceiveCallBack onReceiveCallBack, void* onReceiveParent)
    : HJ212Connection(onReceiveCallBack, onReceiveParent)
{
    m_threadStr = GetThreadString();

    QMap<QString,QSerialPort::Parity> parityConverter;
    parityConverter.insert("N", QSerialPort::NoParity);
    parityConverter.insert("O", QSerialPort::OddParity);
    parityConverter.insert("E", QSerialPort::EvenParity);

    m_serialPort = new QSerialPort();
    m_serialPort->setPortName(com.name);
    m_serialPort->setBaudRate(com.baud);
    m_serialPort->setDataBits((QSerialPort::DataBits)com.dataBit);
    m_serialPort->setStopBits((QSerialPort::StopBits)com.stopBit);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->setParity(parityConverter[com.parity]);

    QObject::connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(SlotReceiveData()));
    QObject::connect(this, SIGNAL(SignalSendData(std::string,int)), this, SLOT(SlotSendData(std::string,int)));
    QObject::connect(this, SIGNAL(SignalSendDataBlocking(std::string,int)), this, SLOT(SlotSendData(std::string,int)), Qt::BlockingQueuedConnection);
    QObject::connect(this, SIGNAL(SignalReOpen()), this, SLOT(SlotReOpen()));
    QObject::connect(this, SIGNAL(SignalReOpenBlocking()), this, SLOT(SlotReOpen()), Qt::BlockingQueuedConnection);

    m_isConnected = false;
    m_recvBytes = 0;
    m_recvBuffers = "";
    m_recvBuffersCapacity = HJ212_FINAL_PACKET_MAXSIZE * 2;
}

HJ212SerialConnection::~HJ212SerialConnection()
{
    m_serialPort->close();
    delete m_serialPort;
}


bool HJ212SerialConnection::Connect()
{
    if(GetThreadString() != m_threadStr)
        emit SignalReOpenBlocking();
    else
        emit SignalReOpen();

    int i = 500;
    while (i--)
    {
        if (m_isConnected)
            break;
        QThread::msleep(10);
    }
    return m_isConnected;
}

void HJ212SerialConnection::CloseConnection()
{

}


void HJ212SerialConnection::SlotReOpen()
{
    QWriteLocker locker(&m_wrlock);
    m_isConnected = false;

    if (m_isConnected)
        m_serialPort->close();

    try
    {
        if(m_serialPort->open(QIODevice::ReadWrite))
        {
            m_serialPort->clear();
            m_isConnected = true;
            return;
        }

        logger->warn("Platform serialport connected to server failed! com errno = %d", m_serialPort->error());
    }
    catch (std::exception e)
    {
        logger->warn("HJ212Client::ConnectToServer() failed=> %s", e.what());
    }
    catch (...)
    {
        logger->warn("HJ212Client::ConnectToServer() failed");
    }
}


void HJ212SerialConnection::SlotReceiveData()
{
    QReadLocker locker(&m_wrlock);

    char tmpBuffer[(HJ212_FINAL_PACKET_MAXSIZE + 1) / 2] = {0};
    int len = m_serialPort->read(tmpBuffer, sizeof (tmpBuffer));

    // 合并缓存包
    QMutexLocker recvLock(&m_recvMute);
    m_recvBuffers += std::string(tmpBuffer);
    m_recvBytes = m_recvBytes + len;

    // 防止服务乱发数据
    if (m_recvBytes >= m_recvBuffersCapacity)
    {
        logger->warn("HJ212SerialConnection recv finalpacket too long(maybe data err), length=%d data=%s", m_recvBytes, m_recvBuffers.c_str());
        m_recvBytes = 0;
        m_recvBuffers = "";
    }
}

int HJ212SerialConnection::ReceiveData()
{
//    QReadLocker locker(&m_wrlock);

    if (!m_isConnected)
    {
        logger->warn("HJ212SerialConnection unconnect!(receiveData)");
        return -1;
    }

    int ret = 0;
    while(true)
    {
        // 从缓存包中查找完整的通讯包（##开头 \r\n结尾）
        int finalPacketSize = 0;
        std::string finalPack = "";

        {
            QMutexLocker recvLock(&m_recvMute);
            bool check = HJ212Packet::FindFinalPacket(m_recvBuffers, m_recvBytes, finalPack, finalPacketSize);
            if (!check)
                break;
        }

        ret += finalPacketSize;

        // 检查通讯包中的crc和数据段长度
        int dataSize = 0;
        std::string data = "";
        if (!HJ212Packet::CheckFinalPacket(finalPack, finalPacketSize, data, dataSize))
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

    return ret;
}

int HJ212SerialConnection::Send(HJ212PackPtr pack, int& error)
{
    if (!m_isConnected)
    {
        logger->warn("HJ212SerialConnection unconnect!(send)");
        error = -1;
        return 0;
    }

    // 装载通讯包
    unsigned int crc16 = 0;
    int bufferSize = 0;
    std::string buffer = "";
    if (!pack->LoadFinalPacket(buffer, bufferSize, crc16))
    {
        logger->warn("HJ212SerialConnection final packet too long! length = %d > %d, dataLen = %d", bufferSize, HJ212_FINAL_PACKET_MAXSIZE, pack->GetDataLength());
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

    if(GetThreadString() != m_threadStr)
        emit SignalSendDataBlocking(buffer, bufferSize);
    else
        emit SignalSendData(buffer, bufferSize);

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    return bufferSize;
}

void HJ212SerialConnection::SlotSendData(std::string buffer, int len)
{
    QReadLocker locker(&m_wrlock);
    m_serialPort->write(buffer.c_str(), len);
    if(!m_serialPort->waitForBytesWritten(1000))
    {
        logger->warn("HJ212串口发送数据超时!");
    }
}


