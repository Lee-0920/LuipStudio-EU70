#ifndef UI_SETTING_AUTOMEASUREWIDGET_H
#define UI_SETTING_AUTOMEASUREWIDGET_H

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
#include "ParameterWidget.h"
#include <QCheckBox>
#include "UI/Frame/RowHiddenHandler.h"

namespace UI
{

class AutoMeasureWidget: public QWidget, public IUserChangeNotifiable, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    explicit AutoMeasureWidget(System::String profileTableName = "setting.ui.profile.scheduler", QWidget *parent = 0);
    ~AutoMeasureWidget();
    QTimer *timer = new QTimer();
   void OnUserChange();
   void OnUpdateWidget(UpdateEvent event, System::String message);

protected:
    void showEvent(QShowEvent *event);

private:
    MQtableWidget *m_parameterTableWidget;
    QPushButton *m_defaultButtom;
    QPushButton *m_confirmButtom;
    std::map<System::String, QComboBox*> m_boolComboBox;
    std::map<System::String, QComboBox*> m_optionComboBox;
    std::map<System::Uint8, ParameterItem> m_items;
    OOLUA::Script *m_lua;
    bool m_isMeaParaml;
    bool m_isAddRemind;
    OOLUA::Lua_func_ref m_defaultRestore;
    OOLUA::Lua_func_ref m_saveFile;
    UpdateEvent m_eventName;
    TableWidgetInfo m_tableInfo;

    void ConfigInit();
    void SpaceInit();
    bool IsSolidifyMeaParamFromUI();
    bool IsDisplayMeaParamInGeneral();
    void SetPointDialogInit();
    void SetPointDialogShow(int row);
    int SetPointCheck(int rowSetPoint, int indexCheck);
    int GetPointSize(QString& text, int limitWidth);
    void DataSelfAdaption(int row, int column, QString text);
    bool CheckSignatureTimeout();

    int m_currentSetPointRow;
    std::map<System::Uint8, System::String> m_mapSetPointRow;
    QPushButton * m_checkOddButton;
    QPushButton * m_checkEvenButton;
    QPushButton * m_checkAllButton;
    QDialog *m_setPointDialog;
    QCheckBox *m_check[24];
//    int timePointSet[24];
    std::map<System::Uint8,  int*>  m_mapRowToTimePointSet;

private slots:
    void DefaultRestoreSlot();
    void ConfigRefreshSlot();
    void DoubleClickedSlot(QTableWidgetItem *item);
    void CheckValueSlot(QTableWidgetItem *item);
    void OneCheckOddSlot();
    void OneCheckEvenSlot();
    void OneCheckAllSlot();
    void SetPointCheckSlot(int index);
    void time();
};
}
#endif // UI_SETTING_AUTOMEASUREWIDGET_H
