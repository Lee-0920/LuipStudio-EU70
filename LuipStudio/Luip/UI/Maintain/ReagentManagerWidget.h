#if !defined(UI_REAGENTMANAGERWIDGET_H_)
#define UI_REAGENTMANAGERWIDGET_H_

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

struct ReagentManagerItem
{
    System::String name;
    System::String text;
    RoleType writePrivilege;
    RoleType readPrivilege;
};

class ReagentManagerWidget: public QWidget , public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit ReagentManagerWidget(QWidget *parent = 0);
    ~ReagentManagerWidget();
    void OnUserChange();
protected:
    void showEvent(QShowEvent *event);
private:
    QAbstractItemModel *standardItemModel;
    MQtableWidget *ReagentManagerTableWidget;

    MQtableWidget *rightWidget;
    QAbstractItemModel *model;
    QList<QLabel *> m_labelList;
    QList<QPushButton *> m_buttonList;
    std::map<Uint8, ReagentManagerItem> m_items;
    OOLUA::Lua_func_ref m_changeReagentFunc;
    OOLUA::Lua_func_ref m_checkReagentDate;
    QTimer *m_reagentDateTimer;
    TableWidgetInfo m_tableInfo;

    void ConfigInit();
    void SpaceInit();
    void ConnectInit(Uint8 useRowCount);
    void CheckVolumeItem(QString stringValue, float &volume);
    void CheckItem(QString stringValue, int &cycleValue);
    void CheckItem(QString stringValue, float &cycleValue);
private slots:
    void SlotUseMeasure(int);
    void slotDoubleClicked(QTableWidgetItem *item);
    void CheckValue(QTableWidgetItem* item);
    void SlotReagentDateCheck();
    void SlotOneKeyChangeReagent();
};

}


#endif // UI_REAGENTMANAGERWIDGET_H_
