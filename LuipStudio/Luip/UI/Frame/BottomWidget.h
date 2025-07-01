#ifndef UI_FRAME_BOTTOMWIDGET_H
#define UI_FRAME_BOTTOMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include "StatusManager/IStatusNotifiable.h"
#include "StatusManager/RunStatus.h"
#include "StatusManager/RunAction.h"
#include "AlarmManager/IAlarmNotifiable.h"
#include "AlarmManager/AlarmInfoFile.h"
#include "AlarmManager/Alarm.h"
#include "SignalManager/IUpdateSignalNotifiable.h"
#include "ToolButton.h"
#include "MeasureProgress.h"
#include "LoginDialog.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "QclickLabel.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include "Interface/Wqimc/IWqimcStatusNotifiable.h"

using Measure::IStatusNotifiable;
using Measure::IAlarmNotifiable;
using Measure::RunStatus;
using Measure::RunAction;
using Measure::Alarm;

using namespace Measure;

namespace UI
{

class BottomWidget:
    public QWidget,
    public IStatusNotifiable,
    public IAlarmNotifiable,
    public IUpdateSignalNotifiable,
    public IUserChangeNotifiable,
    public IUpdateWidgetNotifiable,
    public Interface::Wqimc::IWqimcStatusNotifiable
{
    Q_OBJECT
public:
    explicit BottomWidget(QWidget *parent = 0);
    ~BottomWidget();
    void setPilotLamplist();

    void OnStattusChanged(RunStatus status);
    void OnStattusDelay(int time);
    void OnActionChanged(RunAction action);
    void OnActionDelay(int time);

    void OnAlamChanged(Alarm alarmAll);
    void OnUserChange();
    void OnUpdateSignal();    

    void OnUpdateWidget(UpdateEvent event, System::String message);

    void OnChangeStatus(Interface::Wqimc::WqimcStatus wqimcStatus);
private:
    QLabel *m_softwareTitle; //标题
    QPushButton *m_closeButton; //关闭
    QWidget *bottomWidget;
    QWidget *otherWidget;
    QLabel *temperatureLabel;
    ClickableLabel *alarmPixMapLabel;
    ClickableLabel *onlineLabel;
    ClickableLabel *networkLabel;
    QLabel *timeLabel;
    QList<ToolButton *> m_buttonList;
    MeasureProgress *StatusProgress;
    MeasureProgress *ActionProgress;
    QLabel *limitLampLabel;
    QFrame *line;
    QColor pilotLampList;
    QTimer *curDateTimer;
	String signalName;
	Interface::Wqimc::WqimcStatus m_wqimcStatus;     
signals:
  void ChangeWqimcStatusSignal();
  void AlamChangedSignal(QString level);
private slots:
    void slotUpdataDateTime();
    void ClearAlarm();
    void ChangeWqimcStatusSlot();
    void AlamChangedSlot(QString level);
};
}

#endif // UI_FRAME_BOTTOMWIDGET_H
