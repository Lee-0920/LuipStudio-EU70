#ifndef PIPEMANNGER_H
#define PIPEMANNGER_H
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

struct PipeManngerItem
{
    RoleType writePrivilege;
    RoleType readPrivilege;
    OOLUA::Lua_func_ref createFlow;
};

class PipeManngerWidget: public QWidget , public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit PipeManngerWidget(QWidget *parent = 0);
    ~PipeManngerWidget();
    void OnUserChange();

private:
    QAbstractItemModel *standardItemModel;
    MQtableWidget *pipeManngerTableWidget;
    QPushButton *stopButton;
    QWidget *rightWidget;
    QLabel *typeLabel;
    QComboBox *typeComboBox;
    QAbstractItemModel *model;
    QList<QPushButton *> m_buttonList;
    std::map<System::Uint8, PipeManngerItem> m_items;
    OOLUA::Lua_func_ref m_checkvalueFunc;
    TableWidgetInfo m_tableInfo;
    bool m_addAccurateMeter;        //是否增加了精确定量方式

    void SpaceInit();
    void CheckItem(QTableWidgetItem* item);
    void showEvent(QShowEvent *event);

private slots:
    void MeasureStart(int index);
    void ChoseDay();
    void StopMeasure();
    void CheckValue(QTableWidgetItem* item);
    void SlotIndexChanged(int);

};

}
#endif // PIPEMANNGER_H
