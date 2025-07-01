#if !defined(UI_DATA_WARNMANGERWIDGET_H)
#define UI_DATA_WARNMANGERWIDGET_H

#include <QWidget>
#include <QtWidgets/QListView>
#include <QtWidgets/QStackedWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QCalendarWidget>
#include "System/CopyFile.h"
#include "TextfileParser.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/QMyEdit.h"
#include "UI/Frame/MQtableWidget.h"
#include "AlarmManager/IAlarmNotifiable.h"
#include "System/FileDetecter.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include "LogPrintWidget.h"

using System::CopyFileAction;
using System::FileDetecter;

using namespace Measure;

namespace UI
{

class WarnManager: public QWidget , public IUserChangeNotifiable , public IAlarmNotifiable  , public IUpdateWidgetNotifiable
{
Q_OBJECT
public:
    explicit WarnManager(QWidget *parent = 0);
    ~WarnManager();
    void OnUserChange();
    virtual void OnAlamChanged(Alarm alarmAll);
    void OnUpdateWidget(UpdateEvent event, System::String message);

protected:
    virtual void showEvent(QShowEvent *event);

private:
    QAbstractItemModel *standardItemModel;
    QLineEdit *textEdit;
    QStackedWidget *stackedWidget;
    MQtableWidget *warnManngerTableWidget;
    QMyEdit *minTime;
    QMyEdit *maxTime;

    QAbstractItemModel *model;
    QPushButton *toTopButton;
    QPushButton *toBackButton;
    QPushButton *toNextButton;
    QPushButton *toBottomButton;
    QPushButton *exportButton;
    QPushButton *searchButton;
    QPushButton *clearButton;

    QCalendarWidget *minDayCaledar;
    QCalendarWidget *maxDayCaledar;
    QComboBox *dateModelCombobox;
    int m_nextFlag;
    int m_backFlag;
    TextfileParser *warnLog;
    int m_currentPage;
    int m_currentBlock;

    LogPrintWidget* m_logPrintWidget;
    QPushButton *printerButton;

    CopyFileAction mcopyfile;

    void SpaceInit();
    QString GetMinTime();
    QString GetMaxTime();
    QDate GetCurrentMinDate();
    QDate GetCurrentMaxDate();
    void ViewRefresh();
    void ChangeBottomStatus();
    QStringList SetTypeAndDate(QString type, QDateTime& theMinDateTime, QDateTime& theMaxDateTime, QStringList& strlist, QString pattern);
signals:
    void UpdaterWarnSignals();
    void SignalAlarmClear();

private slots:
    void ToTop();
    void ToBottom();
    void ToBack();
    void ToNext();
    void SlotExportButton();
    void SlotClearButton();
    void ChoseMinDay();
    void ChoseMaxDay();
    void ChangeMinDay();
    void ChangeMaxDay();
    void ChangePage();
    void UpdaterWarnSlot();
    void SlotPrinterButton();
};
}
#endif // UI_DATA_WARNMANGERWIDGET_H
