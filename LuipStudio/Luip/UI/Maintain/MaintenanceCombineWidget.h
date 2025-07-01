#ifndef MAINTENANCECOMBINEWIDGET_H
#define MAINTENANCECOMBINEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>
#include <QList>
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/Types.h"
#include "oolua.h"
#include "UI/Frame/RowHiddenHandler.h"
#include "FlowManager/IFlowNotifiable.h"

#define MAX_FLOW_ITEMS 20
using Flow::IFlowNotifiable;

namespace UI
{

struct FlowCombineItem
{
    QString text = "";
    int times = 0;
    bool isExist = false;
};

struct MaintenanceCombineItem
{
    System::String text;
    OOLUA::Lua_func_ref maintenanceFunc;
};

struct FlowTimerItem
{
    int curFlowIndex;                    //当前流程的序号
    int curFlowTimes;                   //当前流程运行次数
    int flowMaxTimes;                  //最大流程运行次数
    OOLUA::Lua_func_ref maintenanceFunc;    //当前流程的创建函数
};

struct FlowCounter
{
    int curFlowIndex;                    //当前流程的序号（计数器）
    int curFlowTimes;                   //当前流程运行次数（计数器）
};

class MaintenanceCombineWidget: public QWidget , public IUserChangeNotifiable ,public IFlowNotifiable
{
    Q_OBJECT
public:
    explicit MaintenanceCombineWidget(QWidget *parent = 0);
    ~MaintenanceCombineWidget();
    void ConfigInit();
    void OnUserChange();
    void OnFlowActivated(String name, long time);
    void OnFlowFinished(String name, String result);
    void OnFlowProcessMessage(String name, String mag);
    void OnFlowThreadExit();
private:
    MQtableWidget *maintenanceTableWidget;
    QList<QPushButton *> m_buttonList;
    std::map<System::Uint8, MaintenanceCombineItem> m_items;
    FlowCombineItem m_flowCombineItems[MAX_FLOW_ITEMS + 1];
    TableWidgetInfo m_tableInfo;
    OOLUA::Lua_func_ref m_clearFlowList;
    int m_flowCount;
    bool m_flowStart;
    FlowTimerItem flowItem;
    FlowCounter flowCounter;

    void SpaceInit();
    void ConnectInit(System::Uint8 row);
    bool CheckStatus();

    QLabel *m_flowMessage;
    QLabel *m_nextStartTimeMessage;
    QComboBox *m_listBox;
    QComboBox *m_modeBox;
    QLineEdit *m_timesEdit;
    QTimer *m_timer;
    int m_nextStartTime;
    int m_mearsureMode;

    QReadWriteLock m_lock;

signals:
    void UpdateFlowMessageSignal(bool status);
    void ClearFlowMessageSignal();
    void UpdaterTimerSignal();

private slots:
    void SlotUpdateFlowMessage(bool status);
    void SlotClearFlowMessage();
    void SlotUpdaterTimer();
    void SlotCurrentIndexChanged(int index);

    void DoubleClickedSlot(QTableWidgetItem *item);
    void CheckValueSlot(QTableWidgetItem *item);

    void StopAllMeasure();
    void StartMeasure();

    void SlotsAddItems();
    void SlotsDeleteItems(int);
    void SlotsUpItems(int);
    void SlotsDownItems(int);

    void SlotTimeout();
};
}




#endif // MAINTENANCECOMBINEWIDGET_H
