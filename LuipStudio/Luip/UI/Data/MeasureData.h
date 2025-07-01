#if !defined(UI_DATA_MEASUREDATA_H)
#define UI_DATA_MEASUREDATA_H

#include <QWidget>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QtWidgets/QListView>
#include <QtWidgets/QStackedWidget>
#include <QLineEdit>
#include <QtWidgets/QStackedWidget>
#include <QLabel>
#include <QTableWidget>
#include <QDebug>
#include <QStringListModel>
#include <QComboBox>
#include <QPushButton>
#include <QFont>
#include <QCalendarWidget>
#include "System/CopyFile.h"
#include "ResultManager/ResultManager.h"
#include "UI/Frame/NumberKeyboard.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/QMyEdit.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/FileDetecter.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include "ResultDetailManager/ResultDetailManager.h"
#include "MeasureDataPrintWidget.h"
#include "DataBasePlugin/DataBaseManager.h"

using System::CopyFileAction;
using System::FileDetecter;
using System::Uint16;
using Result::IResultNotifiable;
using Result::RecordFields;
using Result::MeasureRecordFile;
using Result::RecordField;
using Result::ShowField;
using Result::MeasureType;
using namespace Lua;
using namespace DataBaseSpace;

namespace UI
{

struct DetailData
{
    int time;
    float consistency;
    MeasureType resultType;
    float addstandardConsistency;
    int range;
};

class MeasureData: public QWidget , public IUserChangeNotifiable , public IResultNotifiable , public IUpdateWidgetNotifiable
{
Q_OBJECT
public:
    explicit MeasureData(System::String resultDataname, QWidget *parent = 0);
    ~MeasureData();
    void readMeasureResultLogFile();
    void OnUserChange();
    virtual void OnMeasureResultAdded(String name, ResultData::RecordData result);
    virtual void OnCalibrateResultAdded(String name, ResultData::RecordData result);
    void OnUpdateWidget(UpdateEvent event, System::String message);

protected:
    virtual void showEvent(QShowEvent *event);

private:
    QAbstractItemModel *standardItemModel;
    QListView *listView;
    QLineEdit *textEdit;
    QStackedWidget *stackedWidget;
    MQtableWidget *measureResultTableWidget;

    QMyEdit *minTime;
    QMyEdit *maxTime;
    QLabel *dateModel;
    QLabel *toTime;
    QLabel *dateType;
    QComboBox *dateTypeCombobox;
    QComboBox *dateModelCombobox;
    QAbstractItemModel *model;
    QPushButton *toTopButton;
    QPushButton *toBackButton;
    QPushButton *toNextButton;
    QPushButton *toBottomButton;
    QPushButton *exportButton;
    QPushButton *searchButton;
    QPushButton *clearButton;
    QPushButton *manualUpButton;

    QGroupBox *detailBox;
    QLabel* detailLabel;

    double pageIndex;
    int m_nextFlag;
    int m_backFlag;
    QString GetMinTime();
    QString GetMaxTime();
    QDate GetCurrentMinDate();
    QDate GetCurrentMaxDate();
    void TableExtraInfoAdjust(bool isEnable);
    void UpdaterData();

    QCalendarWidget *minDayCaledar;
    QCalendarWidget *maxDayCaledar;

    MeasureRecordFile *m_resultFiles;
    MeasureRecordFile *m_resultDetailFiles;
    Int64 resultReadPos[10];

    std::vector<ShowField> m_showFields;
    const String m_mode;
    const String m_datetime;
    const String m_resultType;
    OOLUA::Script *m_lua;
    QStringList m_columnName;
    System::String m_resultDataname;

    ResultDetail::ResultDetailWidget* m_resultDetailWidget;
    QPushButton *detailButton;

    MeasureDataPrintWidget* m_measureDataPrintWidget;
    QPushButton *printerButton;

    bool m_isUnitChange;
    OOLUA::Lua_func_ref m_unitChangeFunc;
    String m_showModel;
private slots:
    void SearchResult();
    void ToTop();
    void ToBottom();
    void ToBack();
    void ToNext();
    void ChoseMinDay();
    void ChoseMaxDay();
    void ChangeMinDay();
    void ChangeMaxDay();
    void SlotExportButton();
    void SlotClearButton();
    void SlotManualUpButton();
    void SlotDetailButton();
    void SlotPrinterButton();
    void SlotShowExtraInformation();

private:
    void SpaceInit();
    void ViewRefresh();
    void ChangeBottomStatus();
    void ShowRow(Uint16 row);
    QString ResultInfoAnalysis(ResultData::RecordData result);
    bool ExtractDetailData(ResultData::RecordData &result, DetailData &detailData);
};

}

#endif // UI_DATA_MEASUREDATA_H
