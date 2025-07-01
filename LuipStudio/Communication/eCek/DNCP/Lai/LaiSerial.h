/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 链路适配接口Qt串口的实现。
 * @details 实现Qt平台的串口通信Lai层实体。
 * @version 1.0.0
 * @author xiejinqiang
 * @date 2015-5-16
 */

#ifndef DNCP_LAI_SERIAL_H_
#define DNCP_LAI_SERIAL_H_

#include <QMutex>
#include "Lai.h"

class QSerialPort;
class LaiSerialThread;

#ifdef __cplusplus
extern "C" {
#endif

//*******************************************************************
// LaiSerial 定义

/**
 * @brief 串口的Lai层实体对象。
 */
typedef struct LaiSerialStruct
{
    Lai base;               ///< 实现Lai实体的基本数据结构

    // LaiSerial实现相关
    char portName[24];                  ///< 串口名称
    QSerialPort* serialPort;            ///< 串口对象

    // 线程状态控制
    Bool isCommunicatingThreadRunning;
    LaiSerialThread* communicatingThread; ///< 线程对象

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
}
LaiSerial;


//*******************************************************************
// 串口实现相关

Bool LaiSerial_Init(LaiSerial* lai, char* portName);

void LaiSerial_Start(LaiSerial* lai);
void LaiSerial_Stop(LaiSerial* lai);

void LaiSerial_Uninit(LaiSerial* lai);


#ifdef __cplusplus
}
#endif

#endif  // DNCP_LAI_SERIAL_H_

/** @} */
