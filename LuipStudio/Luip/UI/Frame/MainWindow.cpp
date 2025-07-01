#include <QApplication>
#include "UI/Frame/NumberKeyboard.h"
#include "Common.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "MainWindow.h"
#include "SignalManager/SignalManager.h"
#ifdef _CS_ARM_LINUX
using namespace System::Screen;
#endif

namespace UI
{

MainWindow::MainWindow(QWidget *parent) :
        DropShadowWidget(parent)
{
    setMinimumSize(800, 600);
#ifdef _CS_ARM_LINUX
    move(0, 0);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    setWindowIcon(QIcon(":/img/WQIMC"));

    this->SetupTitleWidget();
    this->SetupBottomWidget();
    //点击管理员图片按钮上下文菜单
    this->setupContextMenu();
    this->SetupUIWidget();

    m_updaterSignalTimer = new QTimer(this);
    connect(m_updaterSignalTimer, SIGNAL(timeout()), this, SLOT(SlotUpdaterSignalTimer()));
    m_updaterSignalTimer->start(1000);

#ifdef _CS_ARM_LINUX
    //读取屏保参数，并启动
    m_screenProtection = Screenprotection::Instance();
    m_screenProtection->Init();
    connect(m_screenProtection,SIGNAL(ScreenUpdateRequest()),this,SLOT(ScreenUpdate()));

    system("echo [data]=0 >/opt/MigrateData");
    system("killall -9 Monitor SuperTool");
    QString cmd = QString("/opt/luip/Monitor &");
    qDebug() <<  "cmd : " << cmd;
    system(cmd.toStdString().c_str());
#endif
    
}

MainWindow::~MainWindow()
{
    if (m_titleWidget)
        delete m_titleWidget;
    if (m_bottomWidget)
        delete m_bottomWidget;
    if (m_runStatusWindow)
        delete m_runStatusWindow;
    if (m_measureDataWindow)
        delete m_measureDataWindow;
    if (m_singalWindow)
        delete m_singalWindow;
    if (m_machineParamWindow)
        delete m_machineParamWindow;
    if (m_remoteMainTainWindow)
        delete m_remoteMainTainWindow;
    if (m_systemInfoWindow)
        delete m_systemInfoWindow;
}

void MainWindow::SlotUpdaterSignalTimer()
{
    SignalManager::Instance()->UpdateSignal();
}

void MainWindow::SetupTitleWidget()
{
    m_titleWidget = new TitleWidget();

    connect(m_titleWidget, SIGNAL(TurnPage(int)), this, SLOT(SlotTurnPage(int)));
    connect(m_titleWidget, SIGNAL(ChangeUserInterface()),this, SLOT(SlotChangeUserInterface()));

#ifdef _CS_X86_WINDOWS
    connect(m_titleWidget, SIGNAL(ShowMin()), this, SLOT(showMinimized()));
    connect(m_titleWidget, SIGNAL(CloseWidget()), qApp, SLOT(quit()));
#endif

}

void MainWindow::SetupBottomWidget()
{
    m_bottomWidget = new BottomWidget();
}

void MainWindow::SetupUIWidget()
{
    m_statkedWidget = new QStackedWidget();
    m_runStatusWindow = new RunStatusWindow();
    m_measureDataWindow = new MeasureDataWindow();
    m_singalWindow = new SingalWindow();
    m_machineParamWindow = new MachineParamWindow();
    m_remoteMainTainWindow = new RemoteMainTainWindow();
    m_systemInfoWindow = new SystemInfoWindow();
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(236, 236, 236));

    m_statkedWidget->setPalette(palette);
    m_statkedWidget->setAutoFillBackground(true);


    m_statkedWidget->addWidget(m_runStatusWindow);
    m_statkedWidget->addWidget(m_measureDataWindow);
    m_statkedWidget->addWidget(m_singalWindow);
    m_statkedWidget->addWidget(m_machineParamWindow);
    m_statkedWidget->addWidget(m_remoteMainTainWindow);
    m_statkedWidget->addWidget(m_systemInfoWindow);
    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->addWidget(m_statkedWidget);
    centerLayout->setSpacing(0);
    centerLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *bottomWidgetLayout = new QVBoxLayout();
    bottomWidgetLayout->addWidget(m_bottomWidget);
    bottomWidgetLayout->setContentsMargins(0, 0, 0, 0);
    bottomWidgetLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(m_titleWidget);
    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(bottomWidgetLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH,
            SHADOW_WIDTH);

    this->setLayout(mainLayout);

    m_titleWidget->SlotTurnPage("0");
    connect(m_runStatusWindow, SIGNAL(SignalOneKeyChangeReagent()), m_remoteMainTainWindow, SIGNAL(SignalOneKeyChangeReagentWindow()));
    connect(m_measureDataWindow,SIGNAL(SignalWindowAlarmClear()),m_bottomWidget,SLOT(ClearAlarm()));
}

void MainWindow::SlotTurnPage(int currentPage)
{
    if (currentPage == 0)
    {
        m_statkedWidget->setCurrentWidget(m_runStatusWindow);
    }
    else if (currentPage == 1)
    {
        m_statkedWidget->setCurrentWidget(m_measureDataWindow);
    }
    else if (currentPage == 2)
    {
        m_statkedWidget->setCurrentWidget(m_singalWindow);
    }
    else if (currentPage == 3)
    {
        m_statkedWidget->setCurrentWidget(m_machineParamWindow);
    }
    else if (currentPage == 4)
    {
        m_statkedWidget->setCurrentWidget(m_remoteMainTainWindow);
        emit remoteUpdate();
    }
    else if (currentPage == 5)
    {
        m_statkedWidget->setCurrentWidget(m_systemInfoWindow);
    }

    CNumberKeyboard::Instance()->hide();
}

bool MainWindow::IsRunning()
{
    return m_isRunning;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    DropShadowWidget::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawPixmap(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        //skip esc
        case Qt::Key_Escape:
            break;

        default:
            QDialog::keyPressEvent(event);
    }
}

void MainWindow::ShowWidget()
{
    this->showNormal();
}

void MainWindow::setupContextMenu()
{
}

void MainWindow::SlotChangeUserInterface()
{
    MaintainUserChange *MaintainUserChange = MaintainUserChange::Instance();
    MaintainUserChange->UpdateUserInterfaceResult();
    update();
}

void MainWindow::ScreenUpdate()
{
    this->update();
}
}
