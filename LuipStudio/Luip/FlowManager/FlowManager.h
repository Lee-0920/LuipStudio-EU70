/**
 * @file
 * @brief 流程管理器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#if !defined(MEASURE_FLOWMANAGER_H_)
#define MEASURE_FLOWMANAGER_H_

#include <memory>
#include <list>
#include <vector>
#include <QString>
#include <QMutex>
#include <QThread>
#include "lua.hpp"
#include "IFlowNotifiable.h"
#include "CFlow.h"

namespace Flow
{

/**
 * @brief 流程管理器。
 * @details
 */
class FlowManager : public QObject
{
    Q_OBJECT
public:
    static FlowManager *Instance();
    void Init();
    void RegisterActivatedNotifier(IFlowNotifiable *notifier);
    void RegisterFinishedNotifier(IFlowNotifiable *notifier);
    void RegisterProcessMessageNotifier(IFlowNotifiable *notifier);
    bool StartFlow();
    bool StartCFlow(CFlow *cFlow);
    bool StopFlow();
    bool RemoveFlow(String name);
    bool IsFlowRunning();
    void UpdateFlowTime(String name, long time);
    void UpdateFlowResult(String name,  String result);
    void UpdateFlowMessage(String name, String mag);
    void UpdateAllFlowFinish();
    bool IsFlowEnable();
    bool IsComValid();
    bool IsAuthorize();
    bool IsReagentAuthorize();
    lua_State * GetNewState();

    void ClearAllRemainEvent();
signals:
    void StartFlowSignal();

public slots:
    void StartFlowSlot();

private:

    void RunLuaFlow();
    void RunCFlow();
    static int CallLuaFlowFunc(lua_State* L);

private:
    static std::unique_ptr<FlowManager> m_instance;
    bool m_isFlowRunning;
    std::list<IFlowNotifiable *> m_flowActivatedNotifier;
    std::list<IFlowNotifiable *> m_flowFinishedNotifier;
    std::list<IFlowNotifiable *> m_flowMessageNotifier;

    CFlow *m_cFlow;
    bool m_isCFlowRunning;

    lua_State * m_mainState;
    lua_State * m_newState;

    QThread *m_flowThread;
    QMutex m_startMutex;

};

}

#endif  //MEASURE_FLOWMANAGER_H_
