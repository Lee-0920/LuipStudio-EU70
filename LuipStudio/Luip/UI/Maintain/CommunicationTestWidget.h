#ifndef COMMUNICATIONTEST_H
#define COMMUNICATIONTEST_H

#include <QPushButton>
#include <QFont>
#include <QTableWidget>
#include <QWidget>
#include <QDebug>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/Types.h"
#include "oolua.h"
#include "FlowManager/IFlowNotifiable.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/RowHiddenHandler.h"
#include<QTimer>

using Flow::IFlowNotifiable;

namespace UI
{

struct CommunicationCheckItem
{
    System::Uint8 row;
    System::String name;
    OOLUA::Lua_func_ref func;
    RoleType writePrivilege;
    RoleType readPrivilege;
};

class CommunicationTestWidget: public QWidget, public IUserChangeNotifiable, public IFlowNotifiable, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    explicit CommunicationTestWidget(QWidget *parent = 0);
    ~CommunicationTestWidget();
    void OnUserChange();
    void OnFlowActivated(String name, long time);
    void OnFlowFinished(System::String name, System::String result);
    void OnFlowProcessMessage(String name, String mag);
    void OnFlowThreadExit();
    void OnUpdateWidget(UpdateEvent event, System::String message);
private:
    QAbstractItemModel *standardItemModel;
    MQtableWidget *communicationTestTableWidget;
    QWidget *rightWidget;
    QAbstractItemModel *model;
    QPushButton *oneCheck;
    std::map<System::Uint8, QCheckBox *>m_checkBox;
    std::vector<CommunicationCheckItem> m_items;
    System::String m_suitName;
    TableWidgetInfo m_tableInfo;
    QTimer m_timer;

    void SpaceInit();

signals:
    void UpdateItemTestMessageSignal(QString name, QString mag);
private slots:
    void SlotOneCheck();
    void UpdateItemTestMessageSlot(QString name, QString mag);
};

}

#endif
//COMMUNICATIONTEST_H
