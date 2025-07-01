#include <QVBoxLayout>
#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include "TCPModbus.h"
#include <modbus.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>

#ifdef _WIN32
# include <winsock2.h>
#define closeSocket(s) closesocket(s)
#else
# include <sys/socket.h>
# include <sys/time.h>

#define closeSocket(s) close(s)
#endif

#include "Log.h"

/* For MinGW */
#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0
#endif

using namespace std;

namespace Interface
{
namespace Modbus
{

TCPModbus::TCPModbus(IFunCodeNotifiable *rw, modbus_mapping_t *mapping)
    :m_ctx(NULL),
     m_mapping(mapping),
     m_headerLength(0),
     m_rw(rw),
     m_isRuning(false)
{

}


bool TCPModbus::Start()
{
    m_isRuning = true;
    start();
    return true;
}

void TCPModbus::Stop()
{
    m_isRuning = false;
}

void TCPModbus::run()
{
    logger->debug("TCPModbus Thread ID = %d", QThread::currentThreadId());

    int maxSocket = -1;
    int listenSocket = -1;
    int clientSocket = -1;
    fd_set readSet, tempSet;
    struct timeval tm = {1, 0};

    int result = 0;
    int rc = 0;

    uint8_t *query = (uint8_t *)malloc(MODBUS_RTU_MAX_ADU_LENGTH);
    if (!query)
    {
        logger->warn("TCPModbus query is NULL");
        return;
    }
    memset(query, 0, sizeof(query[0])*MODBUS_RTU_MAX_ADU_LENGTH);

    m_ctx = modbus_new_tcp(NULL, 502);

    modbus_set_debug(m_ctx, false);

    listenSocket = modbus_tcp_listen(m_ctx, 1);

    maxSocket = listenSocket;

    FD_ZERO(&readSet);
    FD_SET(listenSocket, &readSet);

    while(m_isRuning)
    {
        FD_ZERO(&tempSet);
        tempSet = readSet;

        tm.tv_sec = 1;
        tm.tv_usec = 0;
        result = select(maxSocket+1, &tempSet, NULL, NULL, &tm);

        if (result == 0)
        {
            // 等待超时，没有可读写或错误的文件
        }
        else if (result < 0)
        {
            // select 错误
            logger->warn("{TCP select} %s", modbus_strerror(errno));
            break;
        }
        else
        {
            if (FD_ISSET(listenSocket, &tempSet))
            {
                // 关闭老连接
                if (clientSocket > 0)
                {
                    closeSocket(clientSocket);
                    FD_CLR(clientSocket, &readSet);
                }

                // 使用新的连接取代老的连接，断开老的连接，关联 clientSocket 到 libmodbus 的 contex 中
                clientSocket = modbus_tcp_accept(m_ctx, &listenSocket);

                if (clientSocket > 0)
                {
                    FD_SET(clientSocket, &readSet);
                    if (maxSocket < clientSocket)
                        maxSocket = clientSocket;

                    logger->debug("{TCP accept} %s", modbus_strerror(errno));
                }
            }
            else if (FD_ISSET(clientSocket, &tempSet))
            {
                // 读取modbus数据
                rc = modbus_receive(m_ctx, query);

                if (rc > 0)
                {
                    // 功能码 、地址、寄存器数量解析
                    int funCode = query[7];
                    int regAddr = query[8];
                    regAddr = (regAddr << 8) + query[9];

                    int regNum = 0;
                    if (funCode == 0x05 || funCode == 0x06)
                    {
                        regNum = 1;
                    }
                    else
                    {
                        regNum = query[10];
                        regNum = (regNum << 8) + query[11];
                    }

                    // 只读寄存器操作
                    m_rw->OnRegisterRead(funCode, regAddr, regNum);

                    // 应答
                    rc = modbus_reply(m_ctx, query, rc, m_mapping);

                    // 保持寄存器操作
                    m_rw->OnRegisterWrited(funCode, regAddr, regNum);

                    if (rc == -1)
                    {
                        logger->debug("{TCP modbus_reply} %s", modbus_strerror(errno));
                    }

                }
                else
                {
                    // 连接被对端断开
                    closeSocket(clientSocket);
                    FD_CLR(clientSocket, &readSet);
                    clientSocket = 0;

                    logger->debug("{TCP modbus_receive} %s", modbus_strerror(errno));
                }
            }
            else
            {
            }
        }
    }

    logger->debug("{TCPModbus Thread Quit the loop} %s", modbus_strerror(errno));

    if (listenSocket != -1)
    {
        closeSocket(listenSocket);
    }

    if (clientSocket != -1)
    {
        closeSocket(clientSocket);
    }

    free(query);
    modbus_close(m_ctx);
    modbus_free(m_ctx);
}

}
}



