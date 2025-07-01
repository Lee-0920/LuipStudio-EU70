/**
 * @file
 * @brief 状态管理器。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#if !defined(STATUSMANAGER_STATUSMANAGER_H_)
#define STATUSMANAGER_STATUSMANAGER_H_
#include <memory>
#include <list>
#include <QString>
#include <QReadWriteLock>
#include "IStatusNotifiable.h"
#include "RunStatus.h"
#include "RunAction.h"

namespace Measure
{

/**
 * @brief 状态管理器。
 * @details
 */
class StatusManager
{

public:
    static StatusManager *Instance();
    void Init();
    void ResetDeviceStatus();
    void Register(IStatusNotifiable *handle);
    RunStatus GetStatus();
    void SetStatus(RunStatus status);
    void SetStatusDelayTime(int time);
    RunAction GetAction();
    void SetAction(RunAction action);
    void SetActionDelayTime(int time);

    bool IsIdleStatus();
    bool IsIdleAction();
    bool IsFaultStatus();

private:
   std::list<IStatusNotifiable* > m_notifise;
   RunStatus m_status;
   RunAction m_action;
   bool     m_deviceFault;
   BaseLineStatus     m_baseline;
   float    m_temperature;

   static std::unique_ptr<StatusManager> m_instance;
   QReadWriteLock m_statusLock;
   QReadWriteLock m_actionLock;
   QReadWriteLock m_baselineLock;
   QReadWriteLock m_temperatureLock;

};

}

#endif  //STATUSMANAGER_STATUSMANAGER_H_
