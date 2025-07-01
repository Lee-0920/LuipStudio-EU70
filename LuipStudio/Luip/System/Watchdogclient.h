/**
 * @file
 * @brief 后台监控软件进程间通信。
 * @details
 * @version 1.0.0
 * @author ludijun
 * @date 2016/10/14
 */

#include <memory>
#include <QThread>
#include <QLocalSocket>

#if !defined(SYSTEM_WATCHDOGCLIENT_H_)
#define SYSTEM_WATCHDOGCLIENT_H_

namespace System
{
class WatchdogClient:public QThread
{
    Q_OBJECT

public:
    static WatchdogClient *Instance();
    void Init();

protected :
    void run(void);

private:
   static std::unique_ptr<WatchdogClient> m_instance;
};
}
#endif // SYSTEM_WATCHDOGCLIENT_H_
