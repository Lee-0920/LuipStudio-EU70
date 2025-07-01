#ifndef MAINTENANCEWIDGET_H
#define MAINTENANCEWIDGET_H

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

using Flow::IFlowNotifiable;

namespace UI
{

struct MaintenanceItem
{
    System::String text;
    RoleType writePrivilege;
    RoleType readPrivilege;
    OOLUA::Lua_func_ref maintenanceFunc;
};

struct CurRunFlow
{
    bool isStart;
    QString flowName;
    int flowTimes;                          //当前运行次数
    int flowMaxTimes;                   //最大运行次数
};

class MaintenanceWidget: public QWidget , public IUserChangeNotifiable ,public IFlowNotifiable
{
    Q_OBJECT
public:
    explicit MaintenanceWidget(QWidget *parent = 0);
    ~MaintenanceWidget();
    void ConfigInit();
    void OnUserChange();
    void showEvent(QShowEvent *event);

    void OnFlowActivated(String name, long time);
    void OnFlowFinished(String name, String result);
    void OnFlowProcessMessage(String name, String mag);
    void OnFlowThreadExit();
private:
    MQtableWidget *maintenanceTableWidget;
    QList<QPushButton *> m_buttonList;
    QPushButton *stopButton;
    std::map<System::Uint8, MaintenanceItem> m_items;
    TableWidgetInfo m_tableInfo;
    int m_isOEM;
    CurRunFlow m_curRunFlow;
    QLabel *m_flowMessage;
    void SpaceInit();
    void ConnectInit(System::Uint8 row);
signals:
    void MaintenanceUpdateFlowMessageSignal();
    void MaintenanceClearFlowMessageSignal();
private slots:
    void SlotUpdateFlowMessage();
    void SlotClearFlowMessage();
    void StopMeasure();
    void measureStart(int);
    void ShowKeyboard();
    void CheckValue(QTableWidgetItem* item);

};
}




#endif // MAINTENANCEWIDGET_H
