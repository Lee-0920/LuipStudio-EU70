/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 链路适配接口Windows串口的实现。
 * @details 实现Windows平台的串口通信Lai层实体。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2013-1-15
 */

#include <stdio.h>
#include <QThread>
#include <QSerialPort>
#include <QDebug>
#include "Log.h"
#include "LaiSerial.h"

//*******************************************************************

// 帧接收状态逻辑

// 帧解析状态定义
#define FPS_FRAME_TRY_SYNC      1   // 帧等待同步中（未同步上）
#define FPS_FRAME_RECEIVING     2   // 帧接收中（已同步上）

// 帧解析过程中的转义状态定义
#define RES_ESC_NORMAL          1   // 正常状态，无转义
#define RES_ESC_ACTION          2   // 转义中


//*******************************************************************
// LaiSerialThread 实现

// LaiSerial 通信线程，处理数据收发。
class LaiSerialThread : public QThread
{
public:
    LaiSerialThread(LaiSerial* lai)
    {
        this->lai = lai;
    }

protected:
    void run()
    {
        logger->debug("LaiSerial Thread ID = %d", QThread::currentThreadId());

        Byte buffer[FRAME_MAX_SIZE * 2];
        Uint32 len;
        bool isReadyRead;
        Uint32 i;
        Byte data;
        Uint16 recvBytes;
        Bool isFrameDetected = FALSE;
        Byte* recvBuffer = lai->recvBuffer;
        Bool isSending;
        QSerialPort* serial;

        // 创建串口对象，打开串口并配置，有异常则退出线程
        if (lai->serialPort)
            delete lai->serialPort;
        serial = new QSerialPort;
        if (serial == nullptr)
            return;
        lai->serialPort = serial;

        serial->setPortName(lai->portName);
        if (serial->open(QIODevice::ReadWrite))
        {
            serial->setBaudRate(QSerialPort::Baud115200);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setFlowControl(QSerialPort::NoFlowControl);
        }
        else
        {
            if (lai->serialPort)
            {
                delete lai->serialPort;
                lai->serialPort = nullptr;
            }
            printf("\n######### Error while open serial port %s.\n\n", lai->portName);
            return;
        }

        while(lai->isCommunicatingThreadRunning)
        {
            // 需要及时退出接收等待（阻塞），以定期检查是否有数据包要发送
            isReadyRead = serial->waitForReadyRead(5);

            if (isReadyRead)        // 接收逻辑
            {
                // 接到一堆数据，解析中。。。
                len = serial->read((char*)buffer, sizeof buffer);

                // 超时无数据，跳过下面的逻辑
                if (len == 0)
                    continue;

                // 使用局部变量，以提高效率，减少代码
                recvBytes = lai->recvBytes;

                for (i = 0; i < len; i++)
                {
                    data = buffer[i];


                    // 帧同步状态机
                    if (lai->recvStatus == FPS_FRAME_TRY_SYNC)
                    {
                        if (data == FRAME_CHAR_SYNC)
                        {
                            lai->recvStatus = FPS_FRAME_RECEIVING;
                        }
                        else
                        {
                            // 帧外数据统计
                            (lai->base.commStat.outOfFrameBytes)++;
                            printf("\n OOD: %02X", data);
                        }
                    }
                    else
                    {
                        // FPS_FRAME_RECEIVING  帧接收中（已同步上）

                        // 转义状态机
                        if (lai->recvEscStatus == RES_ESC_ACTION)
                        {
                            if (data == FRAME_CHAR_RAW_SYNC)      // 原始字符为同步符
                            {
                                data = FRAME_CHAR_SYNC;
                            }
                            else if (data == FRAME_CHAR_RAW_ESC)  // 原始字符为转义符
                            {
                                data = FRAME_CHAR_ESC;
                            }
                            else
                            {
                                // 未定义的转义状态，通信可能出错，终结该帧
                                data = FRAME_CHAR_SYNC;
                                lai->recvStatus = FPS_FRAME_TRY_SYNC;

                                // 认为解析到一帧
                                isFrameDetected = TRUE;
                            }

                            recvBuffer[recvBytes++] = data;
                            lai->recvEscStatus = RES_ESC_NORMAL;
                        }
                        else
                        {
                            // 未转义

                            if (data == FRAME_CHAR_ESC)
                            {
                                lai->recvEscStatus = RES_ESC_ACTION;
                            }
                            else if (data == FRAME_CHAR_SYNC)
                            {
                                // 忽略空帧，防止帧同步符异常造成之后同步不上的问题
                                if (recvBytes > 0)
                                {
                                    // 解析到一帧
                                    isFrameDetected = TRUE;
                                    lai->recvStatus = FPS_FRAME_TRY_SYNC;
                                }
                            }
                            else
                            {
                                // 正常字符，提交
                                recvBuffer[recvBytes++] = data;
                            }
                        }
                    }

                    // 检测到新的数据帧
                    if (isFrameDetected)
                    {
                        ++(lai->base.commStat.recvFrames);

                        // 拷贝到备用缓冲，防止数据被冲掉
                        memcpy(lai->recvBackup, recvBuffer, recvBytes);
                        lai->base.handle->OnReceived(lai->base.handle, lai->recvBackup, recvBytes, lai->sendTo);

                        recvBytes = 0;
                        isFrameDetected = FALSE;
                    }
                }

                // 更新到对象
                lai->recvBytes = recvBytes;
            }
            else        // 发送逻辑
            {
                //lai->mutexSending->lock();
                isSending = lai->isSending;
                //lai->mutexSending->unlock();

                if (isSending)
                {
                    //lai->mutexSending->lock();
                    serial->write((const char*)lai->sendBuffer, lai->sendBytes);
                    serial->waitForBytesWritten(1000);
                    lai->mutexSending->lock();
                    lai->isSending = FALSE;
                    lai->mutexSending->unlock();

                    if (lai->base.handle->OnSendQuery(lai->base.handle) > 0)
                    {
                        lai->base.handle->OnSendRequest(lai->base.handle);
                    }
                }
            }
        }

        if (lai->serialPort)
        {
            lai->serialPort->close();
            delete lai->serialPort;
            lai->serialPort = nullptr;
        }
    }

private:
    LaiSerial* lai;
};


//*******************************************************************
// Lai 实现

static void LaiSerial_Reset(LaiSerial* lai, Uint16 action);
static Bool LaiSerial_IsSendable(LaiSerial* lai);
static void LaiSerial_Send(LaiSerial* lai, Uint8 destAddr, Byte* data, int len);

static const ILaiComm s_kCommInterface =
{
        (IfLaiReset) LaiSerial_Reset,
        (IfLaiIsSendable) LaiSerial_IsSendable,
        (IfLaiSend) LaiSerial_Send
};

/**
 * @brief 复位Lai协议状态逻辑。
 * @details 当上层协议体因为通信异常（如等待超时）需要复位Lai实体时调用。
 *  在Lai协议体通信之前不能也没有必要调用本函数。
 * @param lai 具体的Lai层协议实体。
 * @param action 复位动作，可以是以下一个或多个动作的合并（或）：
 *        - @ref LAI_TX_CLEAR
 *        - @ref LAI_RX_CLEAR 
 *        - @ref LAI_TX_ABORT 
 *        - @ref LAI_RX_ABORT 
 */
void LaiSerial_Reset(LaiSerial* lai, Uint16 action)
{
    if (action & LAI_TX_ABORT)
    {
    }

    if (action & LAI_RX_ABORT)
    {
    }

    if (action & LAI_TX_CLEAR)
    {
        lai->serialPort->clear(QSerialPort::Output);

        lai->mutexSending->lock();
        lai->isSending = FALSE;
        lai->sendBytes = 0;
        lai->mutexSending->unlock();
    }

    if (action & LAI_RX_CLEAR)
    {
        lai->serialPort->clear(QSerialPort::Input);

        lai->recvBytes =  0;
    }

    // 清除可能的串口错误
    lai->serialPort->clearError();
}

/**
 * @brief 查询接口是否可发送数据。
 * @details 通常在 Send 之前调用。
 *  <p>可发送表示：本实体可立即发送一帧新数据，此时上层可直接调用 Send()
 *      函数把数据发送出去；
 *  <p>不可发送表示：本实体正在发送数据，或者发送缓冲中无剩余空间可供等待发送。
 *      此时上层实体应该等待 OnSendRequest 的到来进行异步发送。
 *  <p>如果正在发送，意味着发送缓冲存在数据或者硬件正在发送数据，
 *      此时上层可等待异步的发送请求。
 * @param lai 具体的Lai层协议实体，如 LaiRs485 。
 */
Bool LaiSerial_IsSendable(LaiSerial* lai)
{
    Bool status;

    lai->mutexSending->lock();
    status = lai->isSending;
    lai->mutexSending->unlock();

    return (status == FALSE);
}

/**
 * @brief 发送一帧数据。
 * @details 请先调用 IsSendable() 查询发送缓冲是否足够，无剩余空间则应该等待
 *   OnSendRequest 的到来进行异步发送。
 * @param lai 具体的Lai层协议实体，如 LaiRs485 。
 * @param destAddr 本帧要发往的目标地址（链路地址）。
 * @param data 要发送的帧数据（逻辑帧的裸数据）。
 * @param len 要发送的帧长度。
 */
void LaiSerial_Send(LaiSerial* lai, Uint8 destAddr, Byte* data, int len)
{
    Byte* sendBuffer = lai->sendBuffer;
#define FIFO_SIZE       sizeof(lai->sendBuffer)
    Uint16 idxFifo = 0;          // FIFO 写指针（有效数据长度）
#define FIFO_IS_AVAIL()     (idxFifo < FIFO_SIZE)
#define FIFO_PUSH(data)     (sendBuffer[idxFifo++] = data)
#define FIFO_RESET()        (idxFifo = 0)
#define FIFO_LENTGH()       (idxFifo)
    int idxParse;
    Byte datum;

    lai->mutexSending->lock();
    if (lai->isSending == FALSE)
    {
        // 预先转义好待发送的数据到缓冲中，真正的发送工作交给发送线程完成
        FIFO_RESET();

        // 帧起始符
        FIFO_PUSH(FRAME_CHAR_SYNC);

        // 帧业务数据
        for (idxParse = 0; idxParse < len; idxParse++)
        {
            datum = data[idxParse];
            if (datum == FRAME_CHAR_SYNC || datum == FRAME_CHAR_ESC)
            {
                FIFO_PUSH(FRAME_CHAR_ESC);
                FIFO_PUSH((datum == FRAME_CHAR_SYNC) ? FRAME_CHAR_RAW_SYNC : FRAME_CHAR_RAW_ESC);
            }
            else
            {
                FIFO_PUSH(datum);
            }
        }

        // 帧结束符
        FIFO_PUSH(FRAME_CHAR_SYNC);

        lai->sendTo = destAddr;
        lai->sendBytes = FIFO_LENTGH();
        lai->isSending = TRUE;
    }
    lai->mutexSending->unlock();
}


//*******************************************************************
// 实现相关


/**
 * @brief LaiSerial 协议实体初始化。
 * @param lai 具体的Lai层协议实体。
 * @param portName 要打开的串口名称。Windows 的名称形如："COM1"；Linux 的名称形如："ttyS0"。
 * @return 是否成功打开串口并初始化。
 */
Bool LaiSerial_Init(LaiSerial* lai, char* portName)
{
    Bool retCode = FALSE;
    QSerialPort serial;

    memset(lai, 0, sizeof(LaiSerial));
    strncpy(lai->portName, portName, sizeof lai->portName);
    lai->base.comm = (ILaiComm*) &s_kCommInterface;
    lai->serialPort = nullptr;
    lai->mutexSending = new QMutex;
    lai->communicatingThread = new LaiSerialThread(lai);

    // 测试串口是否能打开，不作通信用。QSerialPort对象创建必须放在工作线程中。
    serial.setPortName(portName);
    if (serial.open(QIODevice::ReadWrite))
    {
        serial.close();
        retCode = TRUE;
    }
    else
    {
        printf("\n######### Error while open serial port %s.\n\n", portName);
    }

    return retCode;
}


/**
 * @brief LaiSerial 协议实体结束化，释放相关资源。
 */
void LaiSerial_Uninit(LaiSerial* lai)
{
    if (lai->serialPort)
    {
        delete lai->serialPort;
        lai->serialPort = nullptr;
    }

    if (lai->mutexSending)
    {
        delete lai->mutexSending;
        lai->mutexSending = nullptr;
    }

    if (lai->communicatingThread)
    {
        delete lai->communicatingThread;
        lai->communicatingThread = nullptr;
    }
}


/**
 * @brief 开启 LaiSerial 实体服务。
 * @param lai LaiSerial 实体。
 */
void LaiSerial_Start(LaiSerial* lai)
{
    // 状态初始化
    lai->recvStatus = FPS_FRAME_TRY_SYNC;
    lai->recvEscStatus = RES_ESC_NORMAL;
    lai->recvBytes = 0;

    lai->isSending = FALSE;
    lai->sendBytes = 0;

    // 创建并启动收发线程
    lai->isCommunicatingThreadRunning = TRUE;
    lai->communicatingThread->start();
    QThread::sleep(1);  // 等待串口对象打开
}


/**
 * @brief 停止 LaiSerial 实体服务。
 * @param lai LaiSerial 实体。
 */
void LaiSerial_Stop(LaiSerial* lai)
{
    // 安全地终止通信线程
    lai->isCommunicatingThreadRunning = FALSE;
    lai->communicatingThread->wait(1000);
}


/** @} */
