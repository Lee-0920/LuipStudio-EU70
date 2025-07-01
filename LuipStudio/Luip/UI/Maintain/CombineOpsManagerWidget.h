#ifndef COMBINEOPSMANAGERWIDGET_H
#define COMBINEOPSMANAGERWIDGET_H
#include <QPushButton>
#include <QFont>
#include <QTableWidget>
#include <QWidget>
#include <QDebug>
#include <QList>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/Types.h"
#include "oolua.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/RowHiddenHandler.h"

namespace UI
{

struct CombineOpsManngerItem
{
    RoleType writePrivilege;
    RoleType readPrivilege;
    OOLUA::Lua_func_ref createFlow;
    int mode;
    bool disableWrite;
    bool isRemind;
    System::String remindText;
};

class CombineOpsManagerWidget: public QWidget , public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit CombineOpsManagerWidget(QWidget *parent = 0);
    ~CombineOpsManagerWidget();
    void OnUserChange();

private:
    QAbstractItemModel *standardItemModel;
    MQtableWidget *combineOpsManngerTableWidget;
    QPushButton *stopButton;
    QWidget *rightWidget;
    QAbstractItemModel *model;
    QList<QPushButton *> m_buttonList;
    std::map<System::Uint8, CombineOpsManngerItem> m_items;
    OOLUA::Lua_func_ref m_checkvalueFunc;
    TableWidgetInfo m_tableInfo;

    void SpaceInit();
    void CheckItem(QTableWidgetItem* item);
    void showEvent(QShowEvent *event);

private slots:
    void MeasureStart(int index);
    void ChoseDay();
    void StopMeasure();
    void CheckValue(QTableWidgetItem* item);
    void SlotIndexChanged(int);
    void SlotDoubleClicked(QTableWidgetItem *item);

};

}

class CombineOpsManagerWidget
{
public:
    CombineOpsManagerWidget();
};

#endif // COMBINEOPSMANAGERWIDGET_H
