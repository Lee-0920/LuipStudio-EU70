/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 链路适配接口Socket-UDP的实现。
 * @details 在Socket上，通过UDP实现一个测试用的Lai层实体，作为上层协议的测试桩。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2012-12-20
 */

#ifndef DNCP_LAI_UDP_H_
#define DNCP_LAI_UDP_H_

#include <Winsock2.h>
#include <WinBase.h>
#include "Lai.h"

#ifdef __cplusplus
extern "C" {
#endif

//*******************************************************************
// LaiUdp 定义

/**
 * @brief UDP的Lai层实体对象。
 */
typedef struct LaiUdpStruct
{
    Lai base;                           ///< 实现Lai实体的基本数据结构

    // LaiUdp实现相关
    SOCKET socketLocal;
    struct sockaddr_in sockaddrLocal;
    Uint16 linkLabel;                   ///< 链路标识
    Uint8 localPort;                    ///< 本地端口号

    // 线程状态控制
    HANDLE threadSending;
    HANDLE threadReceiving;
    Bool isThreadTxRunning;
    Bool isThreadRxRunning;

    // 接收逻辑
    Uint8 recvStatus;                   ///< 接收状态（帧解析状态机）
    Uint8 recvEscStatus;                ///< 接收转义状态
    UINT16 recvBytes;                   ///< 接收到的字节长度
    Byte recvBuffer[FRAME_MAX_SIZE];    ///< 接收缓冲
    Byte recvBackup[FRAME_MAX_SIZE];    ///< 接收备用缓冲

    // 发送逻辑，只能缓冲一帧数据
    CRITICAL_SECTION csSending;         ///< 用于保护发送缓冲的临界区
    Bool isSending;                     ///< 帧发送请求，有帧需要发送时，值为TRUE
    Uint8 sendTo;                       ///< 要发往的地址
    UINT16 sendBytes;                   ///< 要发送的字节长度
    Byte sendBuffer[FRAME_MAX_SIZE];    ///< 接收缓冲
    HANDLE hSendEvent;                   ///< 发送事件，要发送一帧数据时触发该事件
}
LaiUdp;


//*******************************************************************
// UDP 实现相关

void LaiUdp_Init(LaiUdp* lai, Uint16 linkLabel, Uint8 localPort);

void LaiUdp_Start(LaiUdp* lai);
void LaiUdp_Stop(LaiUdp* lai);

void LaiUdp_Uninit(LaiUdp* lai);


#ifdef __cplusplus
}
#endif

#endif  // DNCP_LAI_UDP_H_

/** @} */
