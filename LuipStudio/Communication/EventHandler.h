/**
 * @file
 * @brief 事件处理器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(COMMUNICATION_EVENTHANDLER_H)
#define COMMUNICATION_EVENTHANDLER_H

#include <list>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include "Log.h"
#include "LuipShare.h"
#include "Stack/IDscpReceivable.h"
#include "IEventReceivable.h"
#include "IEventReceivable.h"
#include "Stack/DncpStack.h"


using std::list;
using Communication::Stack::IDscpReceivable;
using Communication::Stack::DncpStack;

namespace Communication
{

/**
 * @brief 事件处理器。Singleton 类。
 * @details 处理下位机主动上报的某个事件。。
 *
 */
class ExpectInfo
{

public:

    DscpAddress addr;
    int code;
    int timeout;
    QWaitCondition* waitCondition;
    DscpEventPtr event;
};

/**
 * @brief 事件注册信息。Singleton 类。
 * @details 上层需要处理下位上报的事件的注册信息。
 *
 */
class RegisterInfo
{

public:

    DscpAddress addr;
    int code;
    IEventReceivable* handle;
};


/**
 * @brief DSCP事件处理器。Singleton 类。
 * @details 为上层应用接口提供一致的事件处理方式：事件同步等待和异步通知。
 *
 */
class LUIP_SHARE EventHandler : public QThread, public IDscpReceivable
{
    Q_OBJECT

public:
    static EventHandler* Instance();

    void Init();
    void Uninit();
    DscpEventPtr GetEvent(DscpAddress addr, int code);
    void EmptyEvents();
    DscpEventPtr Expect(DscpAddress addr, int code, int timeout);
    void Register(DscpAddress addr, int code, IEventReceivable* handle);
    void Deregister(DscpAddress addr, int code, IEventReceivable* handle);
    void OnReceive(DscpPackPtr pack);

    ~EventHandler();

protected:
    void run();

private:
    bool ExpectEventMatch(DscpEventPtr event);
    bool RegisterEventMatch(DscpEventPtr event);
    void SaveRemainEvent(DscpEventPtr event);

private:
    std::list<ExpectInfo> m_expectList;
    std::list<RegisterInfo> m_registerList;
    std::list<DscpEventPtr> m_remainEventList;
    std::list<DscpEventPtr> m_DscpEventList;

    QMutex m_expectMutex;
    QMutex m_registerMutex;
    QMutex m_remainMutex;
    QMutex DscpEventMutex;

    bool m_isPrint;

    static  std::unique_ptr<EventHandler> m_instance;
};

}

#endif  //COMMUNICATION_EVENTHANDLER_H
