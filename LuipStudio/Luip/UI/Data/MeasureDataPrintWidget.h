#ifndef MEASUREDATAPRINTWIDGET_H
#define MEASUREDATAPRINTWIDGET_H

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

namespace UI
{

struct PrintItem
{
    String name;
    String header;
    String format;
    String content;
    String unit;
    bool isUnitChange;
    OOLUA::Lua_func_ref unitChangeFunc;
    int width;
};

class  MeasureDataPrintWidget : public QWidget, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    MeasureDataPrintWidget(String dataName, QWidget *parent = 0);
    void Show(QWidget *parent, int roleType = 0);

    QString GetPrinterAddress();

    int GetPrinterConnectType();
    QString GetPrintHeadString();
    QString GetPrintItemString();
    QString GetPrintNewestString();

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
    void SlotPrintNewMeasureData();

signals:
    void SignalPrintNewMeasureData();

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

    QLabel* m_markLabel;
    QComboBox* m_markBox;

    QLabel* m_typeLabel;
    QComboBox* m_typeBox;

    QGroupBox* m_printGroup;

    QPushButton* m_headButton;
    QPushButton* m_cutButton;
    QPushButton* m_printButton;
    QPushButton* m_closeButton;

    PrinterSpace::Printer* m_printer;

    String m_resultName;
    Result::MeasureRecordFile *m_resultFiles;
    std::vector<Result::ShowField> m_showFields;
    QVector<PrintItem> m_printItems;

    int m_totalWidth;

    String m_timeFlag;
    String m_typeFlag;
    String m_markFlag;
};

}

#endif // MEASUREDATAPRINTWIDGET_H
