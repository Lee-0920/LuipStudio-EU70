#include <QVBoxLayout>
#include <QDateTime>
#include "StatusManager/StatusManager.h"
#include "AlarmManager/AlarmManager.h"
#include "Setting/SettingManager.h"
#include "MeasureProgress.h"
#include "BottomWidget.h"
#include "SignalManager/SignalManager.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "Interface/Wqimc/WqimcManager.h"
#include "LuaEngine/LuaEngine.h"
#include <QMovie>

using namespace Measure;
using namespace Configuration;
using namespace Lua;

namespace UI
{

BottomWidget::BottomWidget(QWidget *parent) :
        QWidget(parent),signalName(""),m_wqimcStatus(Interface::Wqimc::WqimcStatus::Disconnected)
{    
    StatusManager::Instance()->Register(this);  // 向状态管理器注册
    AlarmManager::Instance()->Register(this);  // 向告警管理器注册
    SignalManager::Instance()->Register(this);
    MaintainUserChange::Instance()->Register(this); //向用户切换注册
    UpdateWidgetManager::Instance()->Register(this); //向窗口更新器注册
    Interface::Wqimc::WqimcManager::Instance()->Register(this);

    otherWidget = new QWidget();
    temperatureLabel = new QLabel();
    alarmPixMapLabel = new ClickableLabel();
    onlineLabel = new ClickableLabel();
    networkLabel = new ClickableLabel();
    timeLabel = new QLabel();
    StatusProgress = new MeasureProgress(1);
    ActionProgress = new MeasureProgress(0);

    StatusProgress->setGeometry(QRect(0, 0, 0, 0));
    StatusProgress->setFixedSize(207,36);
    StatusProgress->setAlignment(Qt::AlignTop);
    StatusProgress->setTextVisible(false);

    QString estimateTitle = tr("空闲");
    StatusProgress->OnUpdateInfo(estimateTitle, 0);

    ActionProgress->setGeometry(QRect(20, 0, 0, 0));
    ActionProgress->setFixedSize(173,36);
    ActionProgress->setAlignment(Qt::AlignTop);
    ActionProgress->setValue(0);
    ActionProgress->setTextVisible(false);

    QString cleanBeforeTestTitle = tr("空闲");
    ActionProgress->OnUpdateInfo(cleanBeforeTestTitle, 0);

    setPilotLamplist();

    alarmPixMapLabel->setFixedSize(30,30);
    alarmPixMapLabel->setScaledContents(true); 

    QHBoxLayout *alarmPixMapLayout = new QHBoxLayout();
    alarmPixMapLayout->addWidget(alarmPixMapLabel);
    alarmPixMapLayout->setContentsMargins(10, 0, 0, 0);

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    temperatureLabel->setFixedSize(60 ,40);
    QFont estimateResultTypeFont = temperatureLabel->font();
    estimateResultTypeFont.setPointSize(13);
    temperatureLabel->setFont(estimateResultTypeFont);
    temperatureLabel->setAlignment(Qt::AlignCenter);

    Table signalTable;
    luaEngine->GetLuaValue(state, "setting.ui.signalIterms", signalTable);
    signalTable.safe_at("bottomTemp", signalName);

    QHBoxLayout *onlineLayout = new QHBoxLayout();
    onlineLayout->addWidget(onlineLabel);
    onlineLayout->setContentsMargins(5, 0, 0, 0);

    onlineLabel->setFixedSize(30,30);

    ReportMode reportMode;
    luaEngine->GetLuaValue(state, "config.interconnection.reportMode", reportMode);
    if(reportMode == ReportMode::OnLine)
    {
        onlineLabel->setPixmap(QPixmap(":/img/data-online"));
    }
    else if(reportMode == ReportMode::OffLine)
    {
        onlineLabel->setPixmap(QPixmap(":/img/data-offline"));
    }
    else if(reportMode == ReportMode::Maintain)
    {
        onlineLabel->setPixmap(QPixmap(":/img/data-maintain"));
    }
    else if(reportMode == ReportMode::Fault)
    {
        onlineLabel->setPixmap(QPixmap(":/img/data-fault"));
    }
    else if(reportMode == ReportMode::Calibrate)
    {
        onlineLabel->setPixmap(QPixmap(":/img/data-calibrate"));
    }
    else if(reportMode == ReportMode::Debugging)
    {
        onlineLabel->setPixmap(QPixmap(":/img/data-maintain"));
    }
    onlineLabel->setScaledContents(true);

    QHBoxLayout *networkLayout = new QHBoxLayout();
    networkLayout->addWidget(networkLabel);
    networkLayout->setContentsMargins(5, 0, 0, 0);

    networkLabel->setFixedSize(30,30);
    networkLabel->setScaledContents(true);

    QHBoxLayout *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(timeLabel);
    timeLayout->setContentsMargins(0, 0, 20, 0);

    curDateTimer = new QTimer(/*this*/);
    connect(curDateTimer, SIGNAL(timeout()), this, SLOT(slotUpdataDateTime()));
    curDateTimer->start(1000);

    QDateTime currentTime = QDateTime::currentDateTime();//获取系统现在的时间
    QString currentTimeStr = currentTime.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
    char*  systemTime;
    QByteArray currentTimeByte = currentTimeStr.toLatin1();
    systemTime = currentTimeByte.data();

    timeLabel->setFixedSize(170,48);//150
    QFont timeLabelTypeFont = timeLabel->font();
    timeLabelTypeFont.setPointSize(12);
    timeLabel->setFont(timeLabelTypeFont);
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setText(tr(systemTime));  

    QHBoxLayout *estimateResultLayout = new QHBoxLayout();
    estimateResultLayout->addWidget(temperatureLabel);

    QHBoxLayout *estimateLayout = new QHBoxLayout();
    estimateLayout->addWidget(StatusProgress);

//    QHBoxLayout *cleanLayout = new QHBoxLayout();
//    cleanLayout->addWidget(ActionProgress);

    estimateLayout->setContentsMargins(0, 0, 118, 0);
//    cleanLayout->setContentsMargins(10, 0, 0, 0);

    line = new QFrame(this);
    line->setObjectName(QStringLiteral("line"));
    line->setGeometry(QRect(0, 0, 810, 3));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QHBoxLayout *hProgressLayout = new QHBoxLayout();
    hProgressLayout->addLayout(estimateLayout);
//    hProgressLayout->addLayout(cleanLayout);

    QVBoxLayout *yProgressLayout = new QVBoxLayout();
    yProgressLayout->addStretch();
    yProgressLayout->addLayout(hProgressLayout);
    yProgressLayout->addStretch();

    QHBoxLayout *hStateLayout = new QHBoxLayout();
    hStateLayout->addStretch();
    hStateLayout->addLayout(estimateResultLayout);
    hStateLayout->addStretch();
    hStateLayout->addLayout(alarmPixMapLayout);
    hStateLayout->addLayout(networkLayout);
    hStateLayout->addLayout(onlineLayout);
    hStateLayout->addStretch();
    hStateLayout->addLayout(timeLayout);
    hStateLayout->setSpacing(10);
    hStateLayout->setContentsMargins(20, 0, 0, 0);

    QHBoxLayout *stateLayout = new QHBoxLayout();
    stateLayout->addLayout(yProgressLayout);
    stateLayout->addLayout(hStateLayout);
    stateLayout->setSpacing(0);
    stateLayout->setContentsMargins(10, 0, 0, 0);

#ifdef _CS_ARM_LINUX
    setFixedHeight(50);
#endif

#ifdef _CS_X86_WINDOWS
    setFixedHeight(65);
    setFixedWidth(818);
#endif

    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(236, 236, 236));//一个常规的背景颜色
    this->setPalette(palette);
    this->setLayout(stateLayout);

    connect(alarmPixMapLabel, SIGNAL(clicked()), this, SLOT(ClearAlarm()));
    connect(onlineLabel, &ClickableLabel::clicked, this, [=]{this->ClearAlarm();});
    connect(networkLabel, &ClickableLabel::clicked, this, [=]{this->ClearAlarm();});
    connect(this, SIGNAL(ChangeWqimcStatusSignal()), this, SLOT(ChangeWqimcStatusSlot()));
    connect(this, SIGNAL(AlamChangedSignal(QString)), this, SLOT(AlamChangedSlot(QString)));
}

void BottomWidget::setPilotLamplist()
{
    pilotLampList = QColor(0, 255, 0);
}

void BottomWidget::OnStattusChanged(RunStatus status)
{
//    logger->debug("### BottomWidget::OnStattusChanged thread = %d", QThread::currentThreadId());

    StatusProgress->OnUpdateInfo(QObject::tr(status.content.c_str()), status.time);
}

void BottomWidget::OnStattusDelay(int time)
{
    StatusProgress->SetDelayTime(time);
}

void BottomWidget::ClearAlarm()
{
    alarmPixMapLabel->clear();
    AlarmManager::Instance()->ClearAlarm();
    AlarmManager::Instance()->SetUpLoadStatus(false);
}

void BottomWidget::OnActionChanged(RunAction action)
{
//    logger->debug("### BottomWidget::OnActionChanged thread = %d", QThread::currentThreadId());

     ActionProgress->OnUpdateInfo(QObject::tr(action.content.c_str()), action.time);
}

void BottomWidget::OnActionDelay(int time)
{
    ActionProgress->SetDelayTime(time);
}

void BottomWidget::OnUserChange()
{
}

void BottomWidget::OnAlamChanged(Alarm alarm)
{
    emit AlamChangedSignal(alarm.level.c_str());
}

void BottomWidget::AlamChangedSlot(QString level)
{
    if(level == "Error" || level == "Fatal")
    {
        alarmPixMapLabel->setPixmap(QPixmap(":/img/alert"));
    }
}

void BottomWidget::OnUpdateSignal()
{
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::black);
    String name = "rTemp1";

    if(signalName.size()>0)
    {
        name = signalName;
    }

    QString str;
    double temp = SignalManager::Instance()->GetSignal(name);
    str.sprintf("%.1f", temp);
    temperatureLabel->setText(str +"℃");
}

void BottomWidget::slotUpdataDateTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();//获取系统现在的时间
    QString currentTimeStr = currentTime.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
    char*  systemTime;
    QByteArray currentTimeByte = currentTimeStr.toLatin1();
    systemTime=currentTimeByte.data();

    timeLabel->setText(tr(systemTime));
}

BottomWidget::~BottomWidget()
{
    if (StatusProgress)
        delete StatusProgress;
    if (ActionProgress)
        delete ActionProgress;
}

void BottomWidget::OnUpdateWidget(UpdateEvent event, System::String message)
{
    (void)event;
    (void)message;
    if (event == UpdateEvent::ChangeConfigParam ||
        event == UpdateEvent::ChangeInterconnectionParam ||
        event == UpdateEvent::WqimcChangeParam)
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State* state = luaEngine->GetThreadState();

        ReportMode reportMode;
        luaEngine->GetLuaValue(state, "config.interconnection.reportMode", reportMode);

        if(reportMode == ReportMode::OnLine)
        {
            onlineLabel->setPixmap(QPixmap(":/img/data-online"));
        }
        else if(reportMode == ReportMode::OffLine)
        {
            onlineLabel->setPixmap(QPixmap(":/img/data-offline"));
        }
        else if(reportMode == ReportMode::Maintain)
        {
            onlineLabel->setPixmap(QPixmap(":/img/data-maintain"));
        }
        else if(reportMode == ReportMode::Fault)
        {
            onlineLabel->setPixmap(QPixmap(":/img/data-fault"));
        }
        else if(reportMode == ReportMode::Calibrate)
        {
            onlineLabel->setPixmap(QPixmap(":/img/data-calibrate"));
        }
        else if(reportMode == ReportMode::Debugging)
        {
            onlineLabel->setPixmap(QPixmap(":/img/data-maintain"));
        }
    }    
}

void BottomWidget::OnChangeStatus(Interface::Wqimc::WqimcStatus wqimcStatus)
{
    m_wqimcStatus = wqimcStatus;
    emit ChangeWqimcStatusSignal();
}

void BottomWidget::ChangeWqimcStatusSlot()
{
    if (m_wqimcStatus == Interface::Wqimc::WqimcStatus::Connected)
    {
        networkLabel->setPixmap(QPixmap(":/img/cloud-connected"));
    }
    else
    {
        networkLabel->setPixmap(QPixmap(":/img/cloud-disconnected"));
    }
}
}
