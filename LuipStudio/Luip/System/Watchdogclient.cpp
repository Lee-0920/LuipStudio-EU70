/**
 * @file
 * @brief 后台监控软件进程间通信线程。
 * @details
 * @version 1.0.0
 * @author ludijun
 * @date 2016/10/14
 */
#include "Log.h"
#include "Watchdogclient.h"

#define CLIENT_SEND_CHAR        1515//监控的客户端程序发送的字符
#define SERVERNAME              "MonitorServer" //监控服务端的名称

using namespace std;

namespace System
{

unique_ptr<WatchdogClient> WatchdogClient::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 WatchdogClient 对象（单件）。
 */
WatchdogClient* WatchdogClient::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new WatchdogClient);
    }

    return m_instance.get();
}

void WatchdogClient::Init()
{
    this->start();
}

void WatchdogClient::run(void)
{
    logger->debug("WatchdogClient Thread ID = %d", QThread::currentThreadId());

    int temp = CLIENT_SEND_CHAR;
    while(1)
    {
        sleep(10);
        QLocalSocket ls;
        ls.connectToServer(SERVERNAME);
        if(ls.waitForConnected(1000))
        {
    //        qDebug("client connect");
            QTextStream ts(&ls);
            ts << temp;
            ts.flush();
            ls.waitForBytesWritten(1000);
    //        qDebug("client Tx");
            msleep(10);
            ls.disconnectFromServer();
            ls.close();
        }
    //    else
    //    {
    //        qDebug("client connect failed");
    //    }
        }
}

}
