#ifndef UI_DATA_MEASUREDATAWINDOW_H
#define UI_DATA_MEASUREDATAWINDOW_H

#include <QWidget>
#include <QModelIndex>
#include <QtWidgets/QListView>
#include <QtWidgets/QStackedWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QComboBox>
#include "MeasureData.h"
#include "WaveData.h"
#include "CalibrateCurve.h"
#include "WarnMangerWidget.h"
#include "RunLogWidget.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "CurveParamWidget.h"

namespace UI
{

class MeasureDataWindow: public QWidget, public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit MeasureDataWindow(QWidget *parent = 0);
    ~MeasureDataWindow();
    void initWindow();
    void OnUserChange();
private:
    QAbstractItemModel *standardItemModel;
    QListView *listView;
    QLineEdit *textEdit;
    QStackedWidget *stackedWidget;
    QLineEdit *minTime;
    QLineEdit *maxTime;
    QLabel *dateModel;
    QLabel *dateType;
    QComboBox *dateTypeCombobox;
    QComboBox *dateModelCombobox;
    QAbstractItemModel *model;

    std::vector<MeasureData*> m_measureDataWidget;
    std::vector<WaveData*> m_waveDataWidget;
    std::vector<CalibrateCurve*> m_calibrateCurveWidget;
    QStringList m_strList;
    System::Uint8 m_measureDataEndIndex;
    System::Uint8 m_waveDataEndIndex;
    System::Uint8 m_calibrateCurveEndIndex;
    System::Uint8 warnManagerIndex;
    System::Uint8 runLogIndex;

    WarnManager *warnManagerWidget;
    RunLog *runLogWidget;
    CurveTableWidget* curveWidget ;

    void ChangeBottomStatus();

private slots:
    void itemClicked(QModelIndex index);

signals:
    void SignalWindowAlarmClear();
};

}
#endif // UI_DATA_MEASUREDATAWINDOW_H
