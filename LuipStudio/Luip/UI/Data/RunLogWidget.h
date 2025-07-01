#ifndef UI_DATA_RUNLOGWIDGET_H
#define UI_DATA_RUNLOGWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QtWidgets/QStackedWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QFont>
#include <QCalendarWidget>
#include <QFile>
#include <QFileInfoList>
#include <QFileInfo>
#include <QLabel>
#include "Log.h"
#include "TextfileParser.h"
#include "System/CopyFile.h"
#include "UI/Frame/NumberKeyboard.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/QMyEdit.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/FileDetecter.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include "memory.h"
#include "LogPrintWidget.h"

using System::CopyFileAction;
using System::FileDetecter;

namespace UI
{

class RunLog: public QWidget , public IUserChangeNotifiable , public IUpdateWidgetNotifiable
{
Q_OBJECT
public:
    explicit RunLog(QWidget *parent = 0);
    ~RunLog();
    void OnUserChange();
    void OnUpdateWidget(UpdateEvent event, System::String message);

protected:
    virtual void showEvent(QShowEvent *event);

private:
    QAbstractItemModel *standardItemModel;
    QLineEdit *textEdit;
    QStackedWidget *stackedWidget;
    MQtableWidget *runLogTableWidget;
    QMyEdit *minTime;
    QMyEdit *maxTime;

    QAbstractItemModel *model;
    QPushButton *toTopButton;
    QPushButton *toBackButton;
    QPushButton *toNextButton;
    QPushButton *toBottomButton;
    QPushButton *toMapButton;
    QPushButton *exportButton;
    QPushButton *searchButton;
    QPushButton *clearButton;

    QCalendarWidget *minDayCaledar;
    QCalendarWidget *maxDayCaledar;
    QComboBox *dateModelCombobox;
    ///TextfileParser *logFile;
    std::unique_ptr<TextfileParser> logFile;
    int itemSumLine;
    int currentItemLine;  
    int m_nextFlag;
    int m_backFlag;
    int m_currentpage;     // 在块中的位置
    long long m_currentBlock;  //当前的块数
    int currentFile;        //当前读的文件序号
    QFileInfoList usefulFileList;
    int sumLine;
    CopyFileAction mcopyfile;

    QStringList usefulList;         //显示框链表
    bool hasNext;
    bool hasBack;
    QLabel *m_loadHintLabel;      //日志加载信息
    bool m_isLoaded;

    LogPrintWidget* m_logPrintWidget;
    QPushButton *printerButton;

    int FindPointSize(QString& text, int limitWidth);
    void FillTable();

    QString GetMinTime();
    QString GetMaxTime();
    QDate GetCurrentMinDate();
    QDate GetCurrentMaxDate();
    void ViewRefresh();
    void SpaceInit();  
    void ChangeBottomStatus();
    QFileInfoList GetLogFileList();
    QStringList SetDate(QDateTime& theMinDateTime, QDateTime& theMaxDateTime, QStringList& strlist, QString pattern);
    QString TranslateLauguage(QString englishStr);

    void DisplayHint();
    void ClearHint();

private slots:
    void ToTop();
    void ToBottom();
    void ToBack();
    void ToNext();
    void ChoseMinDay();
    void ChoseMaxDay();
    void ChangeMinDay();
    void ChangeMaxDay();
    void ChangePage();
    void SlotExportButton();
    void SlotClearButton();
    void SlotPrinterButton();
};
}

#endif // UI_DATA_RUNLOGWIDGET_H
