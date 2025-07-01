#ifndef INTELLECTCHECK_H
#define INTELLECTCHECK_H

#include <QPushButton>
#include <QFont>
#include <QTableWidget>
#include <QWidget>
#include <QDebug>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/Types.h"
#include "FlowManager/IFlowNotifiable.h"
#include "oolua.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/RowHiddenHandler.h"

using Flow::IFlowNotifiable;

namespace UI
{

struct IntellectCheckItem
{
    System::Uint8 row;
    System::String name;
    OOLUA::Lua_func_ref func;
    RoleType writePrivilege;
    RoleType readPrivilege;
};

class IntellectCheck: public QWidget , public IUserChangeNotifiable ,public IFlowNotifiable, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    explicit IntellectCheck(QWidget *parent = 0);
    ~IntellectCheck();
    void OnDetectResultChanged(QString detectItem, QString result);
    void OnUserChange();
    void OnFlowActivated(String name, long time);
    void OnFlowFinished(String name, String result);
    void OnFlowProcessMessage(String name, String mag);
    void OnFlowThreadExit();
    void OnUpdateWidget(UpdateEvent event, System::String message);

private:
    QAbstractItemModel *standardItemModel;
    MQtableWidget *intellectCheckTableWidget;
    std::map<System::Uint8, QCheckBox *>m_checkBox;
    QWidget *rightWidget;
    QAbstractItemModel *model;
    QPushButton *oneCheck;
    QPushButton *stopButton;
    QPushButton *oneCheckAll;
    QPushButton *oneKeyStop;
    std::vector<IntellectCheckItem> m_items;
    System::String m_suitName;
    TableWidgetInfo m_tableInfo;

    void SpaceInit();
signals:
    void UpdateItemCheckMessageSignal(QString name, QString mag);

private slots:
    void OneCheckConfig();
    void SlotStopDetect();
    void OneCheckAllConfig();
    void SlotOneKeyStop();
    void UpdateItemCheckMessageSlot(QString name, QString mag);
};

}

#endif // INTELLECTCHECK_H
