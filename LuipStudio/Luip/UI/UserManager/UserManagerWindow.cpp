#include <QApplication>
#include "UI/Frame/NumberKeyboard.h"
#include "Common.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UserManagerWindow.h"
#include "SignalManager/SignalManager.h"
#include "System/ProLicenseManager.h"

using namespace System;
#ifdef _CS_ARM_LINUX
using namespace System::Screen;
#endif

namespace UI
{

UserManagerWindow::UserManagerWindow(QWidget *parent) :
        QDialog(parent)
{    
#ifdef _CS_ARM_LINUX
    setMinimumSize(800, 600);
    move(0, 0);
#endif

#ifdef _CS_X86_WINDOWS
    setMinimumSize(820, 620);
#endif

    setWindowFlags(Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setWindowIcon(QIcon(":/img/WQIMC"));

    this->SetupTitleWidget();
    //点击管理员图片按钮上下文菜单
    this->setupContextMenu();
    this->SetupUIWidget();

    m_updaterSignalTimer = new QTimer(this);
//    connect(m_updaterSignalTimer, SIGNAL(timeout()), this, SLOT(SlotUpdaterSignalTimer()));
    m_updaterSignalTimer->start(1000);
}

UserManagerWindow::~UserManagerWindow()
{
    if (m_titleWidget)
        delete m_titleWidget;
    if (m_userInfoWidget)
        delete m_userInfoWidget;
    if (m_accessLevelWidget)
        delete m_accessLevelWidget;
}

void UserManagerWindow::SlotUpdaterSignalTimer()
{
//    SignalManager::Instance()->UpdateSignal();
}

void UserManagerWindow::SetupTitleWidget()
{
    m_titleWidget = new UserManagerTitleWidget();

    connect(m_titleWidget, SIGNAL(MethodTurnPage(int)), this, SLOT(SlotTurnPage(int)));
    connect(m_titleWidget, SIGNAL(UserManagerQuit()), this, SLOT(hide()));

#ifdef _CS_X86_WINDOWS
    connect(m_titleWidget, SIGNAL(ShowMin()), this, SLOT(showMinimized()));
#endif

}

void UserManagerWindow::SetupBottomWidget()
{

}

void UserManagerWindow::SetupUIWidget()
{
    m_statkedWidget = new QStackedWidget();
//    m_userInfoWidget = new MethodWidget();
    m_userInfoWidget = UserInfoWidget::Instance();
//    m_accessLevelWidget = new AccessLevelWidget();
    m_accessLevelWidget = AccessLevelWidget::Instance();

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(236, 236, 236));

    m_statkedWidget->setPalette(palette);
    m_statkedWidget->setAutoFillBackground(true);


    m_statkedWidget->addWidget(m_userInfoWidget);
    m_statkedWidget->addWidget(m_accessLevelWidget);
    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->addWidget(m_statkedWidget);
    centerLayout->setSpacing(0);
    centerLayout->setContentsMargins(0, 0, 0, 0);

//    QVBoxLayout *bottomWidgetLayout = new QVBoxLayout();
//    bottomWidgetLayout->addWidget(m_bottomWidget);
//    bottomWidgetLayout->setContentsMargins(0, 0, 0, 0);
//    bottomWidgetLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(m_titleWidget);
    mainLayout->addLayout(centerLayout);
//    mainLayout->addLayout(bottomWidgetLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH,
            SHADOW_WIDTH);

    this->setLayout(mainLayout);

    m_titleWidget->SlotTurnPage("0");
//    connect(m_accessLevelWidget, SIGNAL(MethodUpdateSignal(MethodRecord, bool, bool)), m_userInfoWidget, SIGNAL(MethodWidgetUpdateSignal(MethodRecord, bool, bool)));
//    connect(m_runStatusWindow, SIGNAL(SignalOneKeyChangeReagent()), m_remoteMainTainWindow, SIGNAL(SignalOneKeyChangeReagentWindow()));
//    connect(m_measureDataWindow,SIGNAL(SignalWindowAlarmClear()),m_bottomWidget,SLOT(ClearAlarm()));
}

void UserManagerWindow::SlotTurnPage(int currentPage)
{
    if (currentPage == 0)
    {
        m_statkedWidget->setCurrentWidget(m_userInfoWidget);
    }
    else if (currentPage == 1)
    {
        m_statkedWidget->setCurrentWidget(m_accessLevelWidget);
    }    

    CNumberKeyboard::Instance()->hide();
}

bool UserManagerWindow::IsRunning()
{
    return m_isRunning;
}

void UserManagerWindow::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawPixmap(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN));
}

void UserManagerWindow::keyPressEvent(QKeyEvent *event)
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

void UserManagerWindow::ShowWidget()
{
    this->showNormal();
}

void UserManagerWindow::setupContextMenu()
{
}

void UserManagerWindow::ScreenUpdate()
{
    this->update();
}
}
