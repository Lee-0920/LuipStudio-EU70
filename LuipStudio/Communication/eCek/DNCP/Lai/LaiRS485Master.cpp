/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 链路适配接口基于Qt串口的RS485主机实现。
 * @details 实现Qt平台的串口通信Lai层RS485多主实体协议。
 * @version 1.0.0
 * @author liangliwen
 * @date 2016-5-19
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <QThread>
#include <QSerialPort>
#include <QDebug>
#include <QDateTime>
#include "Log.h"
#include "Common/MessageDigest.h"
#include "LaiRS485Master.h"
#include "Log.h"
#include <QProcess>

/*
static QDateTime  slave1TxW;
static QDateTime  slave2TxW;
static QDateTime  slave3TxW;


static QDateTime  slave1TxP;
static QDateTime  slave2TxP;
static QDateTime  slave3TxP;


static QDateTime  slave1Rx;
static QDateTime  slave2Rx;
static QDateTime  slave3Rx;


static QDateTime cur;
*/
//*******************************************************************

// 帧接收状态逻辑

// 帧解析状态定义
#define FPS_FRAME_TRY_SYNC      1   // 帧等待同步中（未同步上）
#define FPS_FRAME_RECEIVING     2   // 帧接收中（已同步上）

// 帧解析过程中的转义状态定义
#define RES_ESC_NORMAL          1   // 正常状态，无转义
#define RES_ESC_ACTION          2   // 转义中


//*******************************************************************
// 串口
static bool LaiRS485Master_SearchAddr(LaiRS485Master*Lai, Uint8 Level);
static void LaiRS485Master_SendPolling(LaiRS485Master* lai);


//*******************************************************************
// LaiRS485Master 内部线程实现

// LaiRS485Master 通信线程，处理数据收发。
class LaiRS485MasterCommunicatingThread : public QThread
{
public:
    LaiRS485MasterCommunicatingThread(LaiRS485Master* lai)
    {
        this->lai = lai;
    }

protected:
    void run()
    {
        logger->debug("LaiRS485MasterCommunicating Thread ID = %d", QThread::currentThreadId());
        Byte buffer[FRAME_MAX_SIZE * 2];
        Uint32 len;
        bool isReadyRead;
        Uint32 i;
        Byte data;
        Uint16 recvBytes;
        Bool isFrameDetected = FALSE;
        Byte* recvBuffer = lai->recvBuffer;
        QSerialPort* serial;
        FrameCtrlWord fcw;
        Uint8 iSSendPollingCnt = 0;
        Uint8 level = LAIRS485MASTER_FirstLevel;
        Bool sendPollingEnd ;
        Uint8 firstLevelCount=0;

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
            logger->warn("收发线程打开 %s 失败原因: %s", lai->portName, qPrintable(serial->errorString()));
            return;
        }

        while(lai->isCommunicatingThreadRunning)
        {
            if(lai->RS485CurrentStatus == Pulling &&
                    lai->pullTimeOutStar == TRUE)
            {
                isReadyRead = serial->waitForReadyRead(5);
                iSSendPollingCnt += 5;
                if(false ==  isReadyRead)
                {
                   //下位机接收到令牌，才会发数据，在接收到下位机发的数据之前不能切换状态，不然会出现发令牌或者发业务的时候收到数据，
                   //出现LAIRS485异常。这个时候并不是上位机为主。所以超时时间必须大于硬件正常时下位机数据回来的时间。
                    lai->pullCntTimeOut++;
                    if (lai->pullCntTimeOut >= lai->pullMaxTimeOut)
                    {
                        /*
						cur = QDateTime::currentDateTime();
                        qDebug("OOOOO Addr %d pullCntTimeOut %s", lai->pollingAddr,
                                          qPrintable(cur.toString("mm:ss.zzz")));

                        if (lai->pollingAddr == 1)
                        {
                            qDebug("time %d ms",
                                    (int)(cur.toMSecsSinceEpoch() - slave1Rx.toMSecsSinceEpoch()));
                        }
                        else if (lai->pollingAddr == 16)
                        {
                            qDebug("time %d ms",
                                    (int)(cur.toMSecsSinceEpoch() - slave2Rx.toMSecsSinceEpoch()));
                        }
                        else if (lai->pollingAddr == 32)
                        {
                            qDebug("time %d ms",
                                    (int)(cur.toMSecsSinceEpoch() - slave3Rx.toMSecsSinceEpoch()));
                        }*/

                        lai->pullTimeOutStar = FALSE;
                        lai->pullCntTimeOut = 0;

                        // 下位机通信转态，令牌应答超时，超时计数加1，等于3次上报，状态异常
                        for(std::vector<ConnectInfo> ::iterator it = lai->connectInfo.begin();
                        it != lai->connectInfo.end(); it++)
                        {
                            if ((*it).addr == lai->pollingAddr)
                            {
                                (*it).timeOutCount++;
                                if ((*it).timeOutCount >= 5)
                                {
                                    (*it).timeOutCount = 0;
                                    if ((*it).status == ConnectStatus::Init || (*it).status == ConnectStatus::Conneted)
                                    {
                                        (*it).status = ConnectStatus::disconneted;
                                        if (lai->notify)
                                            lai->notify->OnConnectStatusChanged(lai->pollingAddr, false);
                                    }
                                }
                                break;
                            }
                        }
                         lai->RS485CurrentStatus = Idle;
                       // qDebug("OOOOO Addr %d RS485CurrentStatus = Idle ", lai->pollingAddr);
                    }
               }
            }
            else
            {
               isReadyRead = false;
            }

            if (isReadyRead)        // 接收逻辑
            {
                // 接到一堆数据，解析中。。。这数据可能不足一帧，可能有很多帧,最大帧数是下位机一次性能发送的帧数(4)
                len = serial->read((char*)buffer, sizeof buffer);

                // 超时无数据，跳过下面的逻辑
                if (len == 0)
                    continue;

                // 使用局部变量，以提高效率，减少代码
                recvBytes = lai->recvBytes;//已接收到一帧数据里的字节数，启动或接收完一帧数据会清零

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
                        lai->pullCntTimeOut = 0;

                        fcw.data = recvBuffer[1];
                      //  cur = QDateTime::currentDateTime();

                        if (fcw.info.flag == 0)//如果是业务帧，先提交数据给上层，避免 lai->pollingAddr被更改
                        {
                           
						   /* if (lai->pollingAddr == 1)
                            {
                                qDebug("#####Addr  %d Receive work    time %d ms", lai->pollingAddr,
                                        (int)(cur.toMSecsSinceEpoch() - slave1Rx.toMSecsSinceEpoch()));
                                slave1Rx = cur;
                            }
                            else if (lai->pollingAddr == 16)
                            {
                                qDebug("#####Addr %d Receive work    time %d ms", lai->pollingAddr,
                                        (int)(cur.toMSecsSinceEpoch() - slave2Rx.toMSecsSinceEpoch()));
                                slave2Rx = cur;
                            }
                            else if (lai->pollingAddr == 32)
                            {
                                qDebug("#####Addr %d Receive work    time %d ms", lai->pollingAddr,
                                        (int)(cur.toMSecsSinceEpoch() - slave3Rx.toMSecsSinceEpoch()));
                                slave3Rx = cur;
                            }*/
                            // 拷贝到备用缓冲，防止数据被冲掉
                            memcpy(lai->recvBackup, recvBuffer, recvBytes);
                            lai->base.handle->OnReceived(lai->base.handle, lai->recvBackup, recvBytes, lai->pollingAddr);

                              /*if(lai->pollingAddr == 1)
                              {
                                  qDebug("#####Addr 0%d OnReceived      time %d ms", lai->pollingAddr,
                                      (int)(QDateTime::currentDateTime().toMSecsSinceEpoch() - cur.toMSecsSinceEpoch()));
                              }
                              else
                              {
                                  qDebug("#####Addr %d OnReceived      time %d ms", lai->pollingAddr,
                                      (int)(QDateTime::currentDateTime().toMSecsSinceEpoch() - cur.toMSecsSinceEpoch()));
                              }*/
                        }
                        /*else
                        {
                            if (lai->pollingAddr == 1)
                            {
                                qDebug("#####Addr 0%d Receive polling time %d ms", lai->pollingAddr,
                                        (int)(cur.toMSecsSinceEpoch() - slave1Rx.toMSecsSinceEpoch()));
                            }
                            else if (lai->pollingAddr == 16)
                            {
                                qDebug("#####Addr %d Receive polling time %d ms", lai->pollingAddr,
                                        (int)(cur.toMSecsSinceEpoch() - slave2Rx.toMSecsSinceEpoch()));
                            }
                            else if (lai->pollingAddr == 32)
                            {
                                qDebug("#####Addr %d Receive polling time %d ms", lai->pollingAddr,
                                        (int)(cur.toMSecsSinceEpoch() - slave3Rx.toMSecsSinceEpoch()));
                            }
                        }*/
                        if (fcw.info.pf == 1 || fcw.man.pf == 1)//最后一帧业务帧,可能是业务也可能是令牌
                        {
                            // 下位机通信转态，令牌接收应答，清除超时计数，状态正常
                            for(std::vector<ConnectInfo> ::iterator it = lai->connectInfo.begin();
                                it != lai->connectInfo.end(); it++)
                            {
                               if ((*it).addr == lai->pollingAddr)
                               {
                                   (*it).timeOutCount = 0;

                                   if ((*it).status == ConnectStatus::Init || (*it).status == ConnectStatus::disconneted)
                                   {
                                       (*it).status =  ConnectStatus::Conneted;
                                       if (lai->notify)
                                          lai->notify->OnConnectStatusChanged(lai->pollingAddr, true);
                                   }
                                   break;
                               }
                            }
					        lai->pullTimeOutStar = FALSE;
                            lai->RS485CurrentStatus = Idle;
//                            qDebug("#####Addr %d Receive  Last work  RS485CurrentStatus = Idle ", lai->pollingAddr);
                            lai->base.handle->OnSendRequest(lai->base.handle);
                        }
                        recvBytes = 0;
                        isFrameDetected = FALSE;
                    }
                }
                // 更新到对象
                lai->recvBytes = recvBytes;
            }

            if (lai->isSending)
            {
                serial->write((const char*)lai->sendBuffer, lai->sendBytes);

                if(serial->waitForBytesWritten(1000))
                {
                    lai->mutexSending->lock();
                  //  cur = QDateTime::currentDateTime();
                    lai->isSending = FALSE;
                    if(lai->RS485CurrentStatus == Sending)
                    {
                        /*qDebug("TTTTT Sending  %d work End and time %s",lai->sendTo ,
                                 qPrintable(cur.toString("mm:ss.zzz")));
                        if (lai->sendTo == 1)
                        {
                            qDebug("@@@@@Addr 0%d Tx work         time %d ms", lai->sendTo,
                                    (int)(cur.toMSecsSinceEpoch() - slave1TxW.toMSecsSinceEpoch()));
                        }
                        else if (lai->sendTo == 16)
                        {
                            qDebug("@@@@@Addr %d Tx work         time %d ms", lai->sendTo,
                                    (int)(cur.toMSecsSinceEpoch() - slave2TxW.toMSecsSinceEpoch()));
                        }
                        else if (lai->sendTo == 32)
                        {
                            qDebug("@@@@@Addr %d Tx work         time %d ms", lai->sendTo,
                                    (int)(cur.toMSecsSinceEpoch() - slave3TxW.toMSecsSinceEpoch()));
                        }*/
                        //统计已发送帧数，在转为Pulling状态时清零
                        lai->txByteCount++;
                        //转化为Idle
                        lai->RS485CurrentStatus = Idle;
                      //  qDebug("TTTTT Sending  %d work End  lai->RS485CurrentStatus = Idle",lai->sendTo );
                        lai->mutexSending->unlock();
                        lai->base.handle->OnSendRequest(lai->base.handle);
                    }
                    else if (lai->RS485CurrentStatus == Pulling)
                    {
                        lai->pullTimeOutStar = TRUE;
                        lai->mutexSending->unlock();
                       /* qDebug("TTTTT Sending Polling End and time %s",
                                 qPrintable(cur.toString("mm:ss.zzz")));

                        if (lai->pollingAddr == 1)
                        {
                            qDebug("@@@@@Addr 0%d Tx Pulling      time %d ms", lai->pollingAddr,
                                    (int)(cur.toMSecsSinceEpoch() - slave1TxP.toMSecsSinceEpoch()));
                            slave1Rx = cur;
                        }
                        else if (lai->pollingAddr == 16)
                        {
                            qDebug("@@@@@Addr %d Tx Pulling      time %d ms", lai->pollingAddr,
                                    (int)(cur.toMSecsSinceEpoch() - slave2TxP.toMSecsSinceEpoch()));
                            slave2Rx = cur;
                        }
                        else if (lai->pollingAddr == 32)
                        {
                            qDebug("@@@@@Addr %d Tx Pulling      time %d ms", lai->pollingAddr,
                                    (int)(cur.toMSecsSinceEpoch() - slave3TxP.toMSecsSinceEpoch()));
                            slave3Rx = cur;
                        }*/

                    }
                }
                else
                {
                    lai->mutexSending->lock();
                    lai->isSending = FALSE;
                    lai->mutexSending->unlock();
                }
            }
           else
            {
               lai->base.handle->OnSendRequest(lai->base.handle);
               msleep(1);
               iSSendPollingCnt += 1;
            }

            if(iSSendPollingCnt >= 10)//发送令牌时间到
            {
                lai->mutexSending->lock();
                if(lai->RS485CurrentStatus == Idle)
                {
                    iSSendPollingCnt = 0;
                    lai->RS485CurrentStatus = Pulling;
                    sendPollingEnd = LaiRS485Master_SearchAddr(lai,level);
                    lai->mutexSending->unlock();
                    LaiRS485Master_SendPolling(lai);
                    /*cur = QDateTime::currentDateTime();
                    qDebug("SPPPP Send Addr %d Polling And time : %s ", lai->pollingAddr,
                            qPrintable(cur.toString("mm:ss.zzz")));
                    if (lai->pollingAddr == 1)
                    {
                        slave1TxP = cur;
                    }
                    else if (lai->pollingAddr == 16)
                    {
                        slave2TxP = cur;
                    }
                    else if (lai->pollingAddr == 32)
                    {
                        slave3TxP = cur;
                    }*/
                   //同等级都请求发送令牌帧了，则会清除连续发送业务帧得计数，和清除本等级时间到达的标志
                    if(sendPollingEnd == TRUE)
                    {
                       lai->txByteCount=0;
                       switch(level)
                       {
                        case LAIRS485MASTER_FirstLevel:
                           if((!lai->secondLevelSlaveAddr->empty()) && ++firstLevelCount == 2)
                           {
                               level = LAIRS485MASTER_SecondLevel;
                               firstLevelCount = 0;
                           }
                           break;
                        case LAIRS485MASTER_SecondLevel:
                           level = LAIRS485MASTER_FirstLevel;
                           break;
                        default:
                           break;
                       }
                    }
                }
                else
                {
                    lai->mutexSending->unlock();
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
    LaiRS485Master* lai;
};


//*******************************************************************
// Lai 实现

static void LaiRS485Master_Reset(LaiRS485Master* lai, Uint16 action);
static Bool LaiRS485Master_IsSendable(LaiRS485Master* lai);
static void LaiRS485Master_Send(LaiRS485Master* lai, Uint8 destAddr, Byte* data, int len);


static const ILaiComm s_kCommInterface =
{
        (IfLaiReset) LaiRS485Master_Reset,
        (IfLaiIsSendable) LaiRS485Master_IsSendable,
        (IfLaiSend) LaiRS485Master_Send

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
void LaiRS485Master_Reset(LaiRS485Master* lai, Uint16 action)
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
 * @param lai 具体的Lai层协议实体，如 LaiRS485Master 。
 */
Bool LaiRS485Master_IsSendable(LaiRS485Master* lai)
{
    Bool status = FALSE;

    lai->mutexSending->lock();
   //不处于拉取状态
    //且不处于发送状态，
    //且已发送帧数没达到最大值
    if (lai->RS485CurrentStatus == Idle)
    {
        if (lai->isSending == FALSE && ( lai->txByteCount < lai->base.maxTransmitUnit ))
        {
            status = TRUE;
            lai->RS485CurrentStatus = Sending;
        }
    }
    lai->mutexSending->unlock();
  /*  qDebug()<<"SWWWW RS485CurrentStatus:"<<lai->RS485CurrentStatus
              <<"isSending:"<<lai->isSending
             <<"txByteCount"<<lai->txByteCount<<"IsSendable result:" << status;*/

    return (status);
}

/**
 * @brief 发送一帧数据。
 * @details 请先调用 IsSendable() 查询发送缓冲是否足够，无剩余空间则应该等待
 *   OnSendRequest 的到来进行异步发送。
 * @param lai 具体的Lai层协议实体，如 LaiRS485Master 。
 * @param destAddr 本帧要发往的目标地址（链路地址）。
 * @param data 要发送的帧数据（逻辑帧的裸数据）。
 * @param len 要发送的帧长度。
 */
void LaiRS485Master_Send(LaiRS485Master* lai, Uint8 destAddr, Byte* data, int len)
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
        /*if(lai->RS485CurrentStatus == Sending)
        {
            cur = QDateTime::currentDateTime();
            qDebug("SWWWW Send Addr %d work And time : %s ", destAddr,
                    qPrintable(cur.toString("mm:ss.zzz")));
            if (destAddr == 1)
            {
                slave1TxW = cur;
            }
            else if (destAddr == 16)
            {
                slave2TxW = cur;
            }
            else if (destAddr == 32)
            {
                slave3TxW = cur;
            }
        }*/
        // 预先转义好待发送的数据到缓冲中，真正的发送工作交给发送线程完成
        FIFO_RESET();

        // 帧起始符
        FIFO_PUSH(FRAME_CHAR_SYNC);

        // 帧数据
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
 * @brief LaiRS485Master 协议实体初始化。
 * @param lai 具体的Lai层协议实体。
 * @param portName 要打开的串口名称。Windows 的名称形如："COM1"；Linux 的名称形如："ttyS0"。
 * @return 是否成功打开串口并初始化。
 */
Bool LaiRS485Master_Init(LaiRS485Master* lai, char* portName)
{
    Bool retCode = FALSE;
    QSerialPort serial;

    memset(lai, 0, sizeof(LaiRS485Master));
    strncpy(lai->portName, portName, sizeof lai->portName);
    lai->base.comm = (ILaiComm*) &s_kCommInterface;
    lai->serialPort = nullptr;
    lai->mutexSending = new QMutex;
    lai->communicatingThread = new LaiRS485MasterCommunicatingThread(lai);
//    lai->pullingThread = new LaiRS485MasterPullingThread(lai);
    lai->firstLevelSlaveAddr = new std::list<Uint8>;
    lai->secondLevelSlaveAddr = new std::list<Uint8>;
    lai->notify = NULL;

    // 测试串口是否能打开，不作通信用。QSerialPort对象创建必须放在工作线程中。
    serial.setPortName(portName);
    if (serial.open(QIODevice::ReadWrite))
    {
        serial.close();
        retCode = TRUE;
    }
    else
    {
//        qDebug("%s open failed: %s", portName, qPrintable(serial.errorString()));
        logger->warn("%s 打开失败原因: %s", portName, qPrintable(serial.errorString()));
#ifdef _CS_ARM_LINUX
        //在Linux下打开串口失败的原因,可能是串口的锁文件被破坏了。如果是ttySAC2，那么其锁文件便是在/var/lock目录下的
        //LCK..ttySAC2文件。把破坏掉的文件删除再重新打开一般会打开成功。
        logger->debug("尝试再次打开串口.");
//        qDebug("Retry");
        QProcess::startDetached(QString("rm /var/lock/LCK..") + portName);
        QThread::sleep(1);
        if (serial.open(QIODevice::ReadWrite))
        {
            serial.close();
            retCode = TRUE;
        }
        else
        {
//            qDebug("Retry %s open failed: %s", portName, qPrintable(serial.errorString()));
            logger->warn("尝试再次打开 %s 失败原因: %s", portName, qPrintable(serial.errorString()));
        }
#endif
    }
    return retCode;
}


/**
 * @brief LaiRS485Master 协议实体结束化，释放相关资源。
 */
void LaiRS485Master_Uninit(LaiRS485Master* lai)
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

//    if (lai->pullingThread)
//    {
//        delete lai->pullingThread;
//        lai->pullingThread = nullptr;
//    }
}


/**
 * @brief 开启 LaiRS485Master 实体服务。
 * @param lai LaiRS485Master 实体。
 */
void LaiRS485Master_Start(LaiRS485Master* lai)
{
    // 状态初始化
    lai->recvStatus = FPS_FRAME_TRY_SYNC;
    lai->recvEscStatus = RES_ESC_NORMAL;
    lai->recvBytes = 0;

    lai->isSending = FALSE;
    lai->sendBytes = 0;

    lai->pullTimeOutStar = FALSE;

    // 创建并启动收发线程
    lai->isCommunicatingThreadRunning = TRUE;
    lai->communicatingThread->start();
    QThread::sleep(1);  // 等待串口对象打开

//    lai->isPullingThreadRunning = TRUE;
//    lai->pullingThread->start();
}


/**
 * @brief 停止 LaiRS485Master 实体服务。
 * @param lai LaiRS485Master 实体。
 */
void LaiRS485Master_Stop(LaiRS485Master* lai)
{
    // 安全地终止通信线程
    lai->isCommunicatingThreadRunning = FALSE;
//    lai->isPullingThreadRunning = FALSE;
    lai->communicatingThread->wait(1000);
//    lai->pullingThread->wait(1000);
}


/**
 * @brief LaiRS485Master_SetSlavePriority
 * @note 设置RS485 从机的令牌轮询等级，以及先后顺序。
 * @param lai  LaiRS485Master 实体。
 * @param SlavePriorityStruct LaiRS485Master从机令牌轮询等级，以及先后顺序初始化结构体
 */
void LaiRS485Master_AddSlaveNode(LaiRS485Master *Lai,Uint8 Level,Uint8 SlaveAddr)
{
    switch ( Level)
    {
    case LAIRS485MASTER_FirstLevel:
        Lai->firstLevelSlaveAddr->push_back(SlaveAddr);

        ConnectInfo statuas1;
        statuas1.addr = SlaveAddr;
        statuas1.timeOutCount = 0;
        statuas1.status = ConnectStatus::Init;
        Lai->connectInfo.push_back(statuas1);
        break;
    case LAIRS485MASTER_SecondLevel:
        Lai->secondLevelSlaveAddr->push_back(SlaveAddr);

        ConnectInfo statuas2;
        statuas2.addr = SlaveAddr;
        statuas2.timeOutCount = 0;
        statuas2.status = ConnectStatus::Init;
        Lai->connectInfo.push_back(statuas2);
        break;
    default:
        break;
    }
}


/**
 * @brief LaiRS485Master_SetPullMaxTimeOut
 * @param lai lai 具体的Lai层协议实体。
 * @param Temp 拉取超时最大值计时值
 */
void LaiRS485Master_SetPullMaxTimeOut(LaiRS485Master* lai,Uint16 Temp)
{
    lai->pullMaxTimeOut = Temp;
}

/**
 * @brief 设置要使用连接状态处理器。默认为空，表示上层对连接状态不感兴趣。
 * @param lai lai 具体的Lai层协议实体。
 * @param handle 连接状态通知响应接口，当各从节点的连接状态有改变时，将调用此接口。
 */
void LaiRS485Master_SetConnectionHandle(LaiRS485Master* lai, IConnectStatusNotifiable *handle)
{
    lai->notify = handle;
}


/**
 * @brief LaiRS485Master_SearchAddr
 * @note 搜索某个等级发送令牌帧的从机地址,轮流把同一等级的地址返回，到最后一个地址下次在重新开始
 * @param lai LaiRS485Master 实体。
 * @param Level 轮询等级
 * @return 得到本等级最后一个地址时返回TRUE,否则返回FASLE
 */
bool LaiRS485Master_SearchAddr(LaiRS485Master*Lai, Uint8 Level)
{
    static std::list<Uint8>::iterator FirstLevel = Lai->firstLevelSlaveAddr->begin();
    static std::list<Uint8>::iterator SecondLevel = Lai->secondLevelSlaveAddr->begin();

    switch(Level)
    {
        case LAIRS485MASTER_FirstLevel:
            Lai->pollingAddr =  *FirstLevel;
            FirstLevel++;
            if (FirstLevel == Lai->firstLevelSlaveAddr->end())
            {
                FirstLevel = Lai->firstLevelSlaveAddr->begin();
                return TRUE;
            }
            return FALSE;
        case LAIRS485MASTER_SecondLevel:
            Lai->pollingAddr =  *SecondLevel;
            SecondLevel++;
            if (SecondLevel == Lai->secondLevelSlaveAddr->end())
            {
                SecondLevel = Lai->secondLevelSlaveAddr->begin();
                return TRUE;
            }
            return FALSE;
    default:
        return FALSE;
    }
}


/**
 * @brief LaiRS485Master_SendPolling
 * @note 用于向从机发送令牌帧
 * @param lai 具体的Lai层协议实体。
 * @param destAddr destAddr 本帧要发往的目标地址（链路地址）。
 */
void LaiRS485Master_SendPolling(LaiRS485Master* lai)
{
    Byte pollingFrame[4]={0};
    FrameCtrlWord fcw;
    Uint16 chksum;

    pollingFrame[0] = lai->pollingAddr;

    //配置管理帧控制字
    fcw.data = 0;
    fcw.man.pf = 1;
    fcw.man.flag = 1;
    pollingFrame[1] = fcw.data;

    //计算赋值校验
    chksum = MessageDigest_Crc16Ccitt(0, pollingFrame, 2);
    pollingFrame[2] = (Uint8) (chksum & 0x00FF);
    pollingFrame[3] = (Uint8) ((chksum & 0xFF00) >> 8);

    //发送令牌帧
    LaiRS485Master_Send(lai, pollingFrame[0], pollingFrame, 4);


}


/** @} */
