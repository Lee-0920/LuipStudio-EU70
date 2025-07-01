/**
 * @brief 串口通信
 * @author huangyue
 * @date 2021-02-04
 * @note 工程pro文件增加模块定义serialport
 *
 *      说明：QSerialPort的创建、打开、发送、接收都要在同一个线程中
 *           所以在此类中用了指针的方式，在Open接口中进行new和连接
 */

#ifndef __TREASURE_TOOL_SERIALPORTTOOL__
#define __TREASURE_TOOL_SERIALPORTTOOL__

#include <memory>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QReadWriteLock>

namespace Treasure
{



class SerialPortTool
{

public:
    SerialPortTool()
    {
        m_serialPort = NULL;
    }

    ~SerialPortTool()
    {
        Close();
    }

    /**
     * @brief 打开串口
     * @param com 串口号
     * @param baud 波特率
     * @param dateBit 数据位
     * @param parity
     * @param stopBit 停止位
     * @param control 流控制
     */
    bool Open(QString com,
              int baud = QSerialPort::Baud9600,
              QSerialPort::DataBits dateBit = QSerialPort::Data8,
              QSerialPort::Parity parity = QSerialPort::NoParity,
              QSerialPort::StopBits stopBit = QSerialPort::OneStop,
              QSerialPort::FlowControl control = QSerialPort::NoFlowControl)
    {
        QWriteLocker locker(&m_wrlock);

        //打开前需要清空关闭还在连接的串口
        if (m_serialPort != NULL)
        {
            if (m_serialPort->isOpen())
            {
                m_serialPort->clear();
                m_serialPort->close();
            }
            delete m_serialPort;
        }

        m_serialPort = new QSerialPort;
        m_serialPort->setPortName(com);
        m_serialPort->setBaudRate(baud, QSerialPort::AllDirections);
        m_serialPort->setDataBits(dateBit);
        m_serialPort->setParity(parity);
        m_serialPort->setStopBits(stopBit);
        m_serialPort->setFlowControl(control);

        if(!m_serialPort->open(QIODevice::ReadWrite))
            return false;

        return true;
    }


    /**
     * @brief 关闭串口
     */
    void Close()
    {
        QWriteLocker locker(&m_wrlock);
        if (m_serialPort == NULL)
            return;

        if (m_serialPort->isOpen())
        {
            m_serialPort->clear();
            m_serialPort->close();
        }

        delete m_serialPort;
        m_serialPort = NULL;
    }


    /**
     * @brief 查询串口是否打开
     */
    bool IsOpen()
    {
        QReadLocker locker(&m_wrlock);
        if (m_serialPort == NULL)
            return false;
        return m_serialPort->isOpen();
    }


    /**
     * @brief 发送
     * @param data 数据
     * @param dataLen 数据长度
     */
    int Send(const char* data, int dataLen)
    {
        if (data == NULL || dataLen <= 0)
            return -1;

        QReadLocker locker(&m_wrlock);
        if (m_serialPort == NULL || !m_serialPort->isOpen())
            return -1;

        int num = m_serialPort->write(data, dataLen);
        m_serialPort->flush();
        return num;
    }


    /**
     * @brief 接收
     * @param recieveData 数据
     * @param msTimeOut 等待超时
     */
    int Recieve(QByteArray& recieveData, int msTimeOut)
    {
        QReadLocker locker(&m_wrlock);
        if (m_serialPort == NULL || !m_serialPort->isOpen())
            return -1;

        if (msTimeOut > 0)
        {
            if (!m_serialPort->waitForReadyRead(msTimeOut))
                return 0;
        }

        recieveData.clear();
        recieveData.append(m_serialPort->readAll());
        return recieveData.length();
    }

    /**
     * @brief 直到收到结束符才停止接收
     * @param recieveData 数据
     * @param msTimeOut 等待超时
     * @param untilWord 结束符号
     * @param recieveNum 尝试接收的次数
     */
    int RecieveUntil(QByteArray& recieveData, int msTimeOut, QByteArray& untilWord, int recieveNum = 3)
    {
        recieveData.clear();

        for(int i = 0; i < recieveNum; i++)
        {
            QByteArray item;
            if (Recieve(item, msTimeOut) <= 0)
                return recieveData.length();

            recieveData.append(item);
            if (recieveData.contains(untilWord))
                return recieveData.length();
        }
        return recieveData.length();
    }

    QString Recieve(int msTimeOut)
    {
        QByteArray recieveData;
        if (Recieve(recieveData, msTimeOut) >= 0)
            return QString(recieveData);
        return "";
    }


private:
    QSerialPort* m_serialPort;
    QReadWriteLock m_wrlock;
};
}

#endif
