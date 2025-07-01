#ifndef CALIBRATEDATAPRINTWIDGET_H
#define CALIBRATEDATAPRINTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QDateTimeEdit>
#include <QDateTime>
#include "PrinterPlugin/Printer.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include "ResultManager/ResultManager.h"
#include "System/Types.h"
#include "TextfileParser.h"
#include "MeasureDataPrintWidget.h"

namespace UI
{

//struct PrintItem
//{
//    String name;
//    String header;
//    String format;
//    String content;
//    String unit;
//    bool isUnitChange;
//    OOLUA::Lua_func_ref unitChangeFunc;
//    int width;
//};

class  CalibrateDataPrintWidget : public QWidget, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    CalibrateDataPrintWidget(String dataName, QWidget *parent = 0);
    void Show(QWidget *parent, int roleType = 0);

    QString GetPrinterAddress();

    int GetPrinterConnectType();
    QString GetPrintHeadString();
    QString GetPrintItemString();
    QString GetPrintCalibrateString();

    void OnUpdateWidget(UpdateEvent event, System::String message);
protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);

private slots:
    void SlotPrintButton();
    void SlotConnectButton();
    void SlotCloseButton();
    void SlotCutButton();
    void SlotHeadButton();
    void SlotBeginTimeCheck(QDateTime);
    void SlotEndTimeCheck(QDateTime);
    void SlotPrintLimitCheck(QString);
    QStringList SetDate(QDateTime& theMinDateTime, QDateTime& theMaxDateTime, QStringList& strlist, QString pattern);
private:
    QLabel* m_ipLabel;
    QLineEdit* m_ipEdit;

    QLabel* m_statusLabel;
    QPushButton* m_connectButton;

    QGroupBox* m_configGroup;

    QLabel* m_beginTimeLabel;
    QDateTimeEdit* m_beginTimeEdit;

    QLabel* m_endTimeLabel;
    QDateTimeEdit* m_endTimeEdit;

    QLabel* m_limitLabel;
    QLineEdit* m_limitEdit;

    QGroupBox* m_printGroup;

    QPushButton* m_headButton;
    QPushButton* m_cutButton;
    QPushButton* m_printButton;
    QPushButton* m_closeButton;

    PrinterSpace::Printer* m_printer;

    String m_fileName;
    Result::CalibrateRecordFile *m_resultFiles;
    std::vector<Result::ShowField> m_showFields;
    QVector<PrintItem> m_printItems;

    int m_totalWidth;

    TextfileParser *m_txtLog;
};

}

#endif // CALIBRATEDATAPRINTWIDGET_H
