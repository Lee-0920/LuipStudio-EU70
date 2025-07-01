#ifndef UI_SETTING_PARAMETERWIDGET_H
#define UI_SETTING_PARAMETERWIDGET_H

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
#include "UI/Frame/RowHiddenHandler.h"
#include <memory>
#include "LuaEngine/LuaEngine.h"
#include "System/Types.h"

namespace UI
{

enum class DataType
{
    Bool = 0,                //1个字节，bool类型, 下拉列表，只有开和关两个选项
    Option = 1,              //1个字节, Byte类型, 下拉列表，选项由脚本提供
    Int = 2,           //4个字节，int 类型
    Float = 3,         //4个字节, float 类型
    IP = 4,                  //String 类型
    IntArray = 5,            //4 * len 个字节，int 类型数组
    Double = 9,
    String = 10,
};

struct ParameterItem
{
    System::String name;
    DataType type;
    RoleType writePrivilege;
    RoleType readPrivilege;
    OOLUA::Lua_func_ref checkValueFunc;
    bool isCheckValue;
    OOLUA::Lua_func_ref unitChangeFunc;
    bool isUnitChange;
    OOLUA::Lua_func_ref curveParamCurveXYChange;
    bool isCurveParamCurveXYChange;
};

struct SwitchGroupParam
{
    System::Uint8 row;
    RoleType privilege;
};

class ParameterWidget: public QWidget, public IUserChangeNotifiable, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    explicit ParameterWidget(System::String profileTableName, QWidget *parent = 0);
    ~ParameterWidget();

   void OnUserChange();
   void OnUpdateWidget(UpdateEvent event, System::String message);

protected:
    void showEvent(QShowEvent *event);

private:
    MQtableWidget *m_parameterTableWidget;
    System::String m_profileTableText;
    QPushButton *m_defaultButtom;
    QPushButton *m_confirmButtom;
    std::map<System::String, QComboBox*> m_boolComboBox;
    std::map<System::String, QComboBox*> m_optionComboBox;
    std::map<System::Uint8, ParameterItem> m_items;
    std::map<System::Uint8, SwitchGroupParam> m_switchGroup;
    std::map<System::Uint8, System::String> m_switchText;
    OOLUA::Script *m_lua;
    bool m_isMeaParaml;
    OOLUA::Lua_func_ref m_defaultRestore;
    OOLUA::Lua_func_ref m_saveFile;
    bool isMeasureParamItems;
    OOLUA::Lua_func_ref m_getCurveKBRange;
    OOLUA::Lua_func_ref m_updaterCurveParam;
    UpdateEvent m_eventName;
    TableWidgetInfo m_tableInfo;
    bool m_isConfigInit;
    int m_rangeParamStart;
    int m_rangeParamEnd;
    int RangeMasterRowNum;
    int CalibrateMasterRowNum;
    int RangeRowNum[3];
    int m_isOEM;
    int m_currentSetPointRow;
    std::map<System::Uint8, System::String> m_mapSetPointRow;
    QPushButton * m_checkOddButton;
    QPushButton * m_checkEvenButton;
    QPushButton * m_checkAllButton;
    QDialog *m_setPointDialog;
    QCheckBox *m_check[24];
    int m_phaLimited;
    QStringList m_phaList;
    std::map<System::Uint8,  int*>  m_mapRowToTimePointSet;

    void ConfigInit(bool changeGroup);
    void SpaceInit();
    bool IsSolidifyMeaParamFromUI();
    bool IsDisplayMeaParamInGeneral();
    bool IsInternationalSupported();
    void RefreshUnit(int index,bool useMeasureParamConfig);
    void ShowSwitchText();
    void SetEnableDialogInit();
    void SetEnableDialogShow(int row);
    std::vector<bool> GetTimedPoint(System::String name);
    bool SetTimedPoint(System::String name, std::vector<bool> point);
    bool CheckSignatureTimeout();
private slots:
    void DefaultRestoreSlot();
    void ConfigRefreshSlot();
    void DoubleClickedSlot(QTableWidgetItem *item);
    void CheckValueSlot(QTableWidgetItem *item);
    void ChangeIndexSlot(int index);
    void CalibrateChangeIndexSlot(int index);
    void RangeNum1ChangeIndexSlot(int);
    void RangeNum2ChangeIndexSlot(int);
    void RangeNum3ChangeIndexSlot(int);
    int GetPointSize(QString& text, int limitWidth);
    void OneCheckAllSlot();
};
}
#endif // UI_SETTING_PARAMETERWIDGET_H
