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

#ifndef DNCP_LAI_RS485_MASTER_H_
#define DNCP_LAI_RS485_MASTER_H_

#include <QMutex>
#include <list>
#include "Lai.h"
#include "IConnectStatusNotifiable.h"

class QSerialPort;
class LaiRS485MasterCommunicatingThread;
class LaiRS485MasterPullingThread;

#ifdef __cplusplus
extern "C" {
#endif

#define LAIRS485MASTER_FirstLevel         0
#define LAIRS485MASTER_SecondLevel        1

/**
 * @brief LaiRS485Master状态。
 */
typedef enum
{
    Idle,
    Pulling,
    Sending,
}LaiRS485MasterStatus;

/**
 * @brief 通信连接状态。
 */
typedef enum
{
    Init = 0,
    disconneted = 1,
    Conneted = 2,
}ConnectStatus;
/**
 * @brief 通信连接信息。
 */
typedef struct ConnectSInfoStruct
{
    Uint8 addr;
    Uint8 timeOutCount;
    ConnectStatus status;
}ConnectInfo;

//*******************************************************************
// LaiRS485Master 定义

/**
 * @brief 串口的Lai层实体对象。
 */
typedef struct LaiRS485MasterStruct
{
    Lai base;               ///< 实现Lai实体的基本数据结构

    LaiRS485MasterStatus RS485CurrentStatus;  ///< 主机RS485接口当前状态
    // LaiRS485Master实现相关
    char portName[24];                  ///< 串口名称
    QSerialPort* serialPort;            ///< 串口对象
//    int RS485Switchfd;

    // 线程状态控制
    Bool isCommunicatingThreadRunning;
    LaiRS485MasterCommunicatingThread* communicatingThread; ///< 通信线程
//    Bool isPullingThreadRunning;
//    LaiRS485MasterPullingThread* pullingThread; ///< 轮询线程

    // 接收逻辑
    Uint8 recvStatus;                   ///< 接收状态（帧解析状态机）
    Uint8 recvEscStatus;                ///< 接收转义状态
    Uint16 recvBytes;                   ///< 接收到的字节长度
    Byte recvBuffer[FRAME_MAX_SIZE];    ///< 接收缓冲
    Byte recvBackup[FRAME_MAX_SIZE];    ///< 接收备用缓冲

    // 发送逻辑，只能缓冲一帧数据
    QMutex* mutexSending;               ///< 用于保护发送缓冲的临界区
    Bool isSending;                     ///< 帧发送请求，有帧需要发送时，值为TRUE
    Uint8 sendTo;                       ///< 要发往的地址
    Uint16 sendBytes;                   ///< 要发送的字节长度
    Byte sendBuffer[(FRAME_MAX_SIZE+1)*2];  ///< 发送缓冲
    volatile Uint8 txByteCount;         ///< 用于计数已发送的字节数

    //令牌帧轮询逻辑
    std::list<Uint8> *firstLevelSlaveAddr;
    std::list<Uint8> *secondLevelSlaveAddr;
    Uint8 pollingAddr;
    Uint16 pullMaxTimeOut;
    Uint16 pullCntTimeOut;
    Bool pullTimeOutStar;
    IConnectStatusNotifiable *notify;
    std::vector<ConnectInfo> connectInfo;
}
LaiRS485Master;


//*******************************************************************
// 串口实现相关

Bool LaiRS485Master_Init(LaiRS485Master* lai, char* portName);

void LaiRS485Master_Start(LaiRS485Master* lai);
void LaiRS485Master_Stop(LaiRS485Master* lai);

void LaiRS485Master_Uninit(LaiRS485Master* lai);

void LaiRS485Master_AddSlaveNode(LaiRS485Master *Lai,Uint8 Level,Uint8 SlaveAddr);
void LaiRS485Master_SetPullMaxTimeOut(LaiRS485Master* lai,Uint16 Temp);
void LaiRS485Master_SetConnectionHandle(LaiRS485Master* lai, IConnectStatusNotifiable *handle);
#ifdef __cplusplus
}
#endif

#endif  // DNCP_LAI_SERIAL_H_

/** @} */
