#ifndef UI_SETTING_CurveTableWidget_H
#define UI_SETTING_CurveTableWidget_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QShowEvent>
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/Types.h"
#include "UI/Frame/LoginDialog.h"
#include "oolua.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include "UI/Setting/ParameterWidget.h"
#include <QCheckBox>
#include "UI/Frame/RowHiddenHandler.h"

namespace UI
{
class CurveTableWidget: public QWidget, public IUserChangeNotifiable, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    explicit CurveTableWidget(System::String profileTableName = "setting.ui.curve", QWidget *parent = 0);
    ~CurveTableWidget();

   void OnUserChange();
   void OnUpdateWidget(UpdateEvent event, System::String message);

protected:
    void showEvent(QShowEvent *event);

private:
    MQtableWidget *m_parameterTableWidget;
    OOLUA::Script *m_lua;
    OOLUA::Lua_func_ref m_getCalibrationTime;
    OOLUA::Lua_func_ref m_getCurveK;
    OOLUA::Lua_func_ref m_getCurveB;
    OOLUA::Lua_func_ref m_getZeroConsistency;
    OOLUA::Lua_func_ref m_getRangeConsistency;
    TableWidgetInfo m_tableInfo;
    System::String m_addr;

    void ConfigInit();
    void SpaceInit();

private slots:

};
}
#endif // UI_SETTING_CurveTableWidget_H
