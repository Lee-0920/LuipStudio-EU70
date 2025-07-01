#ifndef HARDWARECHECK_H
#define HARDWARECHECK_H
#include <QPushButton>
#include <QFont>
#include <QTableWidget>
#include <QWidget>
#include <QDebug>
#include <QTableWidget>
#include <QCheckBox>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTimer>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/Types.h"
#include "oolua.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/RowHiddenHandler.h"
#include "AutoTempCalibrateDialog.h"
#include "ExAutoTempCalibrateDialog.h"

using System::String;
using System::Uint8;

namespace UI
{

struct HardwareItem
{
    String refData;
    OOLUA::Lua_func_ref get;
    OOLUA::Lua_func_ref set;
    OOLUA::Lua_func_ref checkValueFunc;
    System::String name;
    RoleType writePrivilege;
    RoleType readPrivilege;
};

struct HardwareCategory
{
    OOLUA::Lua_func_ref get;
    OOLUA::Lua_func_ref set;
    std::map<Uint8, HardwareItem> items;
};

class HardwareCheckWidget: public QWidget , public IUserChangeNotifiable, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    explicit HardwareCheckWidget(QWidget *parent = 0);
    ~HardwareCheckWidget();
     void OnUserChange();    
     void OnUpdateWidget(UpdateEvent event, System::String message);

private:
    QAbstractItemModel *standardItemModel;
    MQtableWidget *hardwareCheckTableWidget;
    QWidget *rightWidget;
    QAbstractItemModel *model;
    QPushButton *applicationButton;
    QTimer *m_getParamTimer, *m_setParamTimer;//用于获取参数和设置参数的定时器
    bool m_isChangeParam;//是否是正在设置参数
    bool m_isGetParam;//是否是在获取参数
    std::vector<HardwareCategory> m_category;
    OOLUA::Lua_func_ref m_synchronize;
    System::String m_profileName;
    TableWidgetInfo m_tableInfo;

    AutoTempCalibrateDialog* m_tempCalibrateDialog;
    QPushButton* m_tempCalibrateButton;
    ExAutoTempCalibrateDialog* m_exTempCalibrateDialog;
    QPushButton* m_exTempCalibrateButton;

    std::map<Uint8, HardwareItem> m_singleItems;

    OOLUA::Lua_func_ref m_backupParam;
    QPushButton *backupButton;

    float *originalData;
    void ConfigInit();
    void SpaceInit();

protected:
    void showEvent(QShowEvent *event);

signals:
    void UpdatePumpFactorSignal(float result);

private slots:
    void SlotUpdateData();
    void SlotChangeParam();
    void SlotDoubleClicked(QTableWidgetItem *item);
    void ConfigApplication();
    void SlotTempCalibrateButton();
    void SlotExTempCalibrateButton();
    void SlotBackupButton();
    void CheckValue(QTableWidgetItem *item);
    int GetPointSize(QString& text, int limitWidth);
};

}
#endif // HARDWARECHECK_H
