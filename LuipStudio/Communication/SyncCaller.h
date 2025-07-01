/**
 * @file
 * @brief 同步调用器，封装了一些常用的同步命令调用操作。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(COMMUNICATION_SYNCCALLER_H)
#define COMMUNICATION_SYNCCALLER_H

#include <QWaitCondition>
#include <QMutex>
#include "LuipShare.h"
#include "IAsyncCallable.h"
#include "AsyncTask.h"
#include "AsyncCall.h"

namespace Communication
{

/**
 * @brief 同步调用器，封装了一些常用的同步命令调用操作。
 * @details 
 */
class LUIP_SHARE SyncCaller : public IAsyncCallable
{
public:

    SyncCaller();
    SyncCaller(int retries);
    ~SyncCaller();
    int GetRetries();
    void SetRetries(int retries);
    Dscp::DscpRespPtr Send(Dscp::DscpCmdPtr cmd);
    void SendWithoutRespond(Dscp::DscpCmdPtr cmd);
    int SendWithStatus(Dscp::DscpCmdPtr cmd);
    void OnStart(AsyncTask* task);
    bool OnRespond(AsyncTask *task, AsyncCall *call, CallStatus status);
    void OnStop(AsyncTask* task, CallStatus status);
private:

    int sum(int n);

    // 同步命令的回应包。
    DscpRespPtr m_respond;
    // 同步命令的应答状态。
    CallStatus m_respondStatus;
    // 同步调用的超时重传次数，0为不重传。
    int m_retries;


    QMutex m_mutex;
    QWaitCondition m_taskWaitCondition;
};

}

#endif  //CONTROLNET_COMMUNICATION_SYNCCALLER_H
