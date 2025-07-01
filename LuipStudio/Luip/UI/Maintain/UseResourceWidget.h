#ifndef USERESOURCEWIDGET_H
#define USERESOURCEWIDGET_H

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

using System::String;
using System::Uint8;

namespace UI
{

struct UseResourceItem
{
    System::String name;
    System::String text;
    RoleType writePrivilege;
    RoleType readPrivilege;
};
class UseResourceWidget: public QWidget , public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit UseResourceWidget(QWidget *parent = 0);
    ~UseResourceWidget();
    void OnUserChange();
protected:
    void showEvent(QShowEvent *event);
private:
    QAbstractItemModel *standardItemModel;
    MQtableWidget *useResoureTableWidget;

    MQtableWidget *rightWidget;
    QAbstractItemModel *model;
    QList<QPushButton *> m_buttonList;
    std::map<Uint8, UseResourceItem> m_items;
    QTimer *m_materialLifeTimer;
    OOLUA::Lua_func_ref m_checkMaterialLife;
    OOLUA::Lua_func_ref m_changeMaterialLife;
    TableWidgetInfo m_tableInfo;

    void TableWidgetInit();
    void ConfigInit();
    void SpaceInit();
    void ConnectInit(Uint8 useRowCount);
    void CheckItem(QString stringValue, float &cycleValue);
private slots:
    void UseMeasure(int);
    void slotDoubleClicked(QTableWidgetItem *item);
    void CheckValue(QTableWidgetItem* item);
    void SlotMaterialLifeCheck();
};

}


#endif // USERESOURCEWIDGET_H
