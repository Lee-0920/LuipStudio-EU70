#if !defined(UI_DATA_CALIBRATECURVE_H)
#define UI_DATA_CALIBRATECURVE_H

#include <QWidget>
#include <QtWidgets/QListView>
#include <QLineEdit>
#include <QtWidgets/QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include "System/CopyFile.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/QMyEdit.h"
#include "UI/Frame/MQtableWidget.h"
#include "ResultManager/ResultManager.h"
#include "System/FileDetecter.h"
#include "LuaEngine/LuaEngine.h"
#include <QCalendarWidget>
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include "ResultDetailManager/ResultDetailManager.h"
#include "CalibrateDataPrintWidget.h"

using System::CopyFileAction;
using System::FileDetecter;
using namespace Result;
using namespace System;
using namespace Lua;

namespace UI
{

class CalibrateCurve: public QWidget , public IUserChangeNotifiable , public IResultNotifiable, public IUpdateWidgetNotifiable
{
Q_OBJECT
public:
    explicit CalibrateCurve(System::String resultDataname, System::String profileTableName, System::String profileFileName, System::String curveParamName, QWidget *parent = 0);
    ~CalibrateCurve();
    void readMeasureResultLogFile();
    void ReadResult();
    void writeConfig();
    void SpaceInit();
    void ViewRefresh();
    void OnUserChange();
    virtual void OnCalibrateResultAdded(String name, ResultData::RecordData result);
    virtual void OnMeasureResultAdded(String name, ResultData::RecordData result);
    void OnUpdateWidget(UpdateEvent event, System::String message);
    void DisplayHiddenInfo();
    void TableExtraInfoAdjust(bool isEnable);

protected:
    void showEvent(QShowEvent *event);
    void showCurveXYChangeButton();

private:
    QAbstractItemModel *standardItemModel;
    QListView *listView;
    QLineEdit *textEdit;
    QStackedWidget *stackedWidget;
    MQtableWidget *calibrateTableWidget;

    //std::map<System::Uint8, RoleType> m_items;
    std::map<System::Uint8, bool> m_items;

    QMyEdit *minTime;
    QMyEdit *maxTime;
    QLabel *dateModel;
    QLabel *toTime;
    QAbstractItemModel *model;
    QPushButton *toTopButton;
    QPushButton *toBackButton;
    QPushButton *toNextButton;
    QPushButton *toBottomButton;
    QPushButton *toMapButton;
    QPushButton *searchButton;
    QPushButton *curveXYChangeButton;
    QPushButton *inputButton;
    QPushButton *exportButton;
    QPushButton *clearButton;

    CalibrateDataPrintWidget* m_calPrintWidget;
    QPushButton *printerButton;

    QFont font;

    QLabel *currentCurve;     //当前标线
    QLabel *m_ExtraInfoLabel;        //标点浓度
    double m_currentCurveK[10];  //用于存储当前界面显示标线斜率；
    double m_currentCurveB[10];  //用于存储当前界面显示标线截距；
    double m_currentCurveKValue; //用于存储当前标线斜率；
    double m_currentCurveBValue; //用于存储当前标线截距；

    int nextFlag;
    int backFlag;
    QCalendarWidget *minDayCaledar;
    QCalendarWidget *maxDayCaledar;

    RecordFields *m_resultFields;
    CalibrateRecordFile *m_resultFiles;
    CalibrateRecordFile *m_resultDetailFiles;

    std::vector<ShowField> m_showFields;
    const String m_dateTimeStr;
    const String m_curveBStr;
    const String m_curveKStr;
    int m_dateTimeStart;
    int m_curveBStart;
    int m_curveKStart;

    OOLUA::Script *m_lua;
    QStringList m_columnName;
    QString m_curveType;
    QString m_curveX;
    OOLUA::Lua_func_ref m_defaultRestore;
    OOLUA::Lua_func_ref m_saveFile;
    OOLUA::Lua_func_ref m_changeCurve;
    OOLUA::Lua_func_ref m_checkCurveExist;
    OOLUA::Lua_func_ref m_getCurrentCurve;
//    OOLUA::Lua_func_ref m_checkHistoryCurve;
    System::String m_resultDataname;
    System::String m_profileTableName;
    System::String m_profileFileName;
    System::String m_curveParamName;
    System::String m_targetName;
    OOLUA::Lua_func_ref m_getExtraInfo;
    bool m_isDisplayExtraInfo;

    OOLUA::Lua_func_ref m_getHiddenInfo;
    bool m_isDisplayHiddenInfo;

    OOLUA::Lua_func_ref m_extraToMap;
    bool m_isExtraToMap;
    OOLUA::Lua_func_ref m_toInput;

    bool m_isUnitChange;
    OOLUA::Lua_func_ref m_unitChangeFunc;

    bool m_isCurveParamCurveXYChange;
    OOLUA::Lua_func_ref m_curveParamCurveXYChangeFunc;
    bool m_isXYChangeing;

    Uint8 m_measureRangeIndex;

    QString GetMinTime();
    QString GetMaxTime();
    QDate GetCurrentMinDate();
    QDate GetCurrentMaxDate();
    void ChangeBottomStatus();
    void DisplayCurrentCurve(double curveK, double curveB);
    void DisplayExtraInfo();
    void ShowRow(Uint16 row);
    void UpdaterCalibrate(double curveB, double curveK);
    void UpdateWidget();
    void UpdateCurveXYChangeButtonText();
    void DataSelfAdaption(int row, int column, QString text);

    bool FindStringInList(QStringList srcList, QString dstStr);

    ResultDetail::ResultDetailWidget* m_resultDetailWidget;
    QPushButton *detailButton;
private slots:
    void SearchResult();
    void ToTop();
    void ToBottom();
    void ToNext();
    void ToBack();
    void ToMap();   //设为当前
    void ToInput(); //输入按钮信号
    void SlotExportButton();
    void SlotClearButton();
    void SlotDetailButton();
    void SlotCurveXYChangeButton();

    void ChoseMinDay();
    void ChoseMaxDay();
    void ChangeMinDay();
    void ChangeMaxDay();
    void SlotPrinterButton();
};
}
#endif // UI_DATA_CALIBRATECURVE_H
