#ifndef HARDWARETEST_H
#define HARDWARETEST_H

#include <QPushButton>
#include <QFont>
#include <QList>
#include <QTableWidget>
#include <QWidget>
#include <QDebug>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "System/Types.h"
#include "oolua.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/RowHiddenHandler.h"

namespace UI
{
struct HardwareTestItem
{
    OOLUA::Lua_func_ref createFlow;
    bool isButtonPress;
    System::String name;
    RoleType writePrivilege;
    RoleType readPrivilege;
};

class HardwareTestWidget: public QWidget , public IUserChangeNotifiable, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    explicit HardwareTestWidget(QWidget *parent = 0);
    ~HardwareTestWidget();
    void OnUserChange();
    void OnUpdateWidget(UpdateEvent event, System::String message);
private:
    void ConnectInit();
    void ChangeButtonStatus(int index, bool status);
    bool IsHardwareTestStatus();
    void SpaceInit();

private:
    QAbstractItemModel *standardItemModel;
    QTableWidget *hardwareTestTableWidget;
    QWidget *rightWidget;
    QAbstractItemModel *model;
    QList<QPushButton *> m_buttonList;
    std::map<System::Uint8, HardwareTestItem> m_items;
    TableWidgetInfo m_tableInfo;
    int m_isOEM;

private slots:

    void SlotButton(int index);
};

}

#endif // HARDWARETEST_H
