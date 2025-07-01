#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMetaType>

#include "VncPlugin/VncManager.h"
#include "UdpPlugin/UdpManager.h"
#include "NFSManager.h"
#include "Treasure/Tool/NetTool.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_QuitOnClose, true);
    this->setAttribute(Qt::WA_StyledBackground);
    this->setObjectName("mainWidget");
    InitWidget();

    qRegisterMetaType<DeviceInfo>("DeviceInfo");

    //绑定事件
    connect(m_exitButton, &QPushButton::clicked, this, [this]{this->close();});
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::OnRefresh);
    connect(m_addButton, &QPushButton::clicked, this, [this]{this->m_addWidget->Show();});
    connect(m_addWidget, &AddWidget::SignalAddDevice, this, &MainWindow::OnAddDevice);
    connect(m_clientListWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::OnSelectDevice);
    connect(m_reconnectButton, &QPushButton::clicked, this, &MainWindow::OnReconnect);
    connect(m_openNFSPathButton, &QPushButton::clicked, this, &MainWindow::OnOpenNFSPath);
    connect(m_udhcpdButton, &QPushButton::clicked, this, &MainWindow::OnUdhcpdMode);
    connect(m_localAreaButton, &QPushButton::clicked, this, &MainWindow::OnLocalAreaMode);
    connect(m_leftButton, &QPushButton::clicked, this, [this]{
        m_leftButton->hide();
        m_leftWidget->hide();
        m_rightButton->show();
    });
    connect(m_rightButton, &QPushButton::clicked, this, [this]{
        m_leftButton->show();
        m_leftWidget->show();
        m_rightButton->hide();
    });
    connect(this, &MainWindow::SignalAddDevice, this, &MainWindow::OnAddDevice, Qt::UniqueConnection);
    connect(&m_timer, &QTimer::timeout, this, [this]{
        auto entrys = Treasure::NetTool::GetNetEntrys(Treasure::NetTool::NetEntry::Type::Ethernet);
        if (entrys.size() <= 0)
            m_statusButton->setText("地址:无法获取");
        else
            m_statusButton->setText(entrys[0].ip);
    });

    auto onRecvDevice = [=](DeviceInfo device){
        this->SignalAddDevice(device);
    };

    //设置UDP接收接口
    UdpSpace::UdpManager::Instance()->SetOnRecvDeviceFunc(onRecvDevice);

    //检查NFS服务
    NFSManager::Instance()->Check();

    m_timer.start(5000);
}


MainWindow::~MainWindow()
{
    m_clientListWidget = Treasure::Memory::Delete(m_clientListWidget);
    m_exitButton = Treasure::Memory::Delete(m_exitButton);
    m_refreshButton = Treasure::Memory::Delete(m_refreshButton);
    m_addWidget = Treasure::Memory::Delete(m_addWidget);
    m_explorerWidget = Treasure::Memory::Delete(m_explorerWidget);
    m_clientLayout = Treasure::Memory::Delete(m_clientLayout);
}

void MainWindow::InitWidget()
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(1);
    layout->setSpacing(0);
    layout->addWidget(InitLeftGrid(), Qt::AlignLeft);
    layout->addWidget(InitRightGrid(), Qt::AlignRight);

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);
    gridLayout->addLayout(layout, 0, 0);
    gridLayout->addLayout(InitExpandGrid(), 0, 0, Qt::AlignLeft | Qt::AlignTop);
    gridLayout->addWidget(Style::Instance()->messageBox, 0, 0);

//#ifdef _WIN32
    m_rightButton->hide();
//#else
//    m_leftButton->hide();
//    m_leftWidget->hide();
//#endif
}

QLayout* MainWindow::InitExpandGrid()
{
    m_rightButton = Style::CreateIconButton("right.svg");

    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(0);
    layout->setMargin(1);
    layout->addWidget(m_rightButton, 0, 0);
    return layout;
}

QWidget* MainWindow::InitLeftGrid()
{
    m_clientListWidget = Style::CreateClientListWidget();
    m_logoButton =  Style::CreateTitleButton("仪器远程v"+QString(APP_VERSION));
    m_addButton = Style::CreateToolButton("添加", "toolbutton/add.svg");
    m_refreshButton = Style::CreateToolButton("搜索仪器", "toolbutton/refresh.svg");
    m_exitButton = Style::CreateExitButton("退出", "toolbutton/exit.svg");
    m_openNFSPathButton = Style::CreateToolButton("打开导出目录", "toolbutton/server.svg");
    m_leftButton = Style::CreateExitButton("隐藏菜单", "left.svg");
    m_udhcpdButton = Style::CreateToolButton("直连模式");
    m_localAreaButton = Style::CreateToolButton("组网模式");
    m_statusButton = Style::CreateToolButton("");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(5);
    layout->addWidget(m_logoButton, Qt::AlignTop | Qt::AlignHCenter);
    layout->addWidget(Style::CreateToolLabel("设备列表"), Qt::AlignTop | Qt::AlignHCenter);
    layout->addWidget(m_clientListWidget, Qt::AlignTop | Qt::AlignHCenter);
    layout->addWidget(m_refreshButton, Qt::AlignBottom | Qt::AlignHCenter);
#ifdef HAVE_NETMODE_FUNC
    layout->addWidget(m_udhcpdButton, Qt::AlignBottom | Qt::AlignHCenter);
    layout->addWidget(m_localAreaButton, Qt::AlignBottom | Qt::AlignHCenter);
#endif
#ifdef _WIN32
    //layout->addWidget(m_addButton, Qt::AlignBottom | Qt::AlignHCenter);
#endif
#ifdef HAVE_NFS_SERVER
    layout->addWidget(m_openNFSPathButton, Qt::AlignBottom | Qt::AlignHCenter);
#endif
    layout->addWidget(m_leftButton, Qt::AlignBottom | Qt::AlignHCenter);
#ifdef HAVE_EXIT_BUTTON
    layout->addWidget(m_exitButton, Qt::AlignBottom | Qt::AlignHCenter);
#endif
    layout->addWidget(m_statusButton, Qt::AlignBottom | Qt::AlignHCenter);

    m_leftWidget = new QWidget();
    m_leftWidget->setLayout(layout);
    m_leftWidget->setObjectName("leftWidget");
    return m_leftWidget;
}

QWidget* MainWindow::InitRightGrid()
{
    //手动添加窗口
    m_addWidget = new AddWidget();

    //文件管理器窗口
    m_explorerWidget = new ExplorerWidget();

    //VNC窗口
    m_clientLayout = new QGridLayout();
    m_clientLayout->setMargin(1);

    auto clientWidget = new QWidget();
    clientWidget->setLayout(m_clientLayout);

    //重连按钮
    m_reconnectButton = Style::CreateButton("重连服务", "", "Reconnect");
    m_reconnectButton->hide();

    //主窗
    auto layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(clientWidget, 0, 0);
    layout->addWidget(m_reconnectButton, 0, 0, Qt::AlignCenter);
    layout->addWidget(m_addWidget, 0, 0);
    layout->addWidget(m_explorerWidget, 0, 0);

    m_rightWidget = new QWidget();
    m_rightWidget->setLayout(layout);
    m_rightWidget->setObjectName("rightWidget");
    return m_rightWidget;
}



void MainWindow::OnRefresh()
{
    UdpSpace::UdpManager::Instance()->SendSearch();
#ifndef QT_DEBUG
#ifdef HAVE_NFS_SERVER
    UdpSpace::UdpManager::Instance()->SendNFSServerNotify(true);
#else
    UdpSpace::UdpManager::Instance()->SendNFSServerNotify(false);
#endif
#endif
}

void MainWindow::OnAddDevice(DeviceInfo device)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);
    if (VncSpace::VncManager::Instance()->Contains(device.ip))
        return;

    auto client = VncSpace::VncManager::Instance()->Add(device.ip, device.port, VncSpace::AdaptMode::AspectRation);
    m_clientLayout->addWidget(client.get(),0,0);
    m_devices.append(device);

    auto item = Style::CreateClientItem(device);
    m_clientListWidget->addItem(item);
    m_clientListWidget->setCurrentItem(item);

    client->SetStatusChangeFunc([=](bool check){
        if (check)
            return;
        OnSelectDevice();
    });
}

void MainWindow::OnSelectDevice()
{
    if (m_clientListWidget == NULL || m_clientListWidget->currentItem() == NULL)
        return;

    MyListWidgetItem* item = (MyListWidgetItem*)m_clientListWidget->currentItem();
    QString ip = item->m_info.ip;

    m_explorerWidget->hide();
    VncSpace::VncManager::Instance()->HideAll();
    VncSpace::VncManager::Instance()->Show(ip);

    if (VncSpace::VncManager::Instance()->IsConnected(ip))
        m_reconnectButton->hide();
    else
    {
        m_reconnectButton->setText(QString("重连服务[%1]").arg(ip));
        m_reconnectButton->show();
    }
}

void MainWindow::OnReconnect()
{
    static QString lastIP = "";
    static int failedCount = 0;

    MyListWidgetItem* item = (MyListWidgetItem*)m_clientListWidget->currentItem();
    QString ip = item->m_info.ip;

    if (ip != lastIP)
    {
        lastIP = ip;
        failedCount = 0;
    }

    if (VncSpace::VncManager::Instance()->ReConnect(ip))
    {
        m_reconnectButton->hide();
        VncSpace::VncManager::Instance()->Show(ip);
    }
    else
    {
        failedCount++;
        m_reconnectButton->show();
        m_reconnectButton->setText(QString("重连服务[%2]失败%1次,继续重连").arg(failedCount).arg(ip));
    }
}

void MainWindow::OnOpenNFSPath()
{
    auto path = NFSManager::Instance()->GetExportPath();
#ifdef _WIN32
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
#else
    m_clientListWidget->clearSelection();
    m_explorerWidget->Show(path);
#endif
}

void MainWindow::OnUdhcpdMode()
{
#ifndef _WIN32
    QString appPath = QApplication::applicationDirPath();
    QString confPath = appPath + "/udhcpd.conf";

    QFileInfo confInfo(confPath);
    if (!confInfo.exists() || confInfo.size() <= 0)
    {
        if (!Treasure::File::Copy(":/udhcpd.conf", confPath))
            trLogger->debug("OnUdhcpdMode copy udhcpd.conf failed.");
    }

    QProcess::startDetached(QString("killall -9 udhcpc"));
    QProcess::startDetached(QString("ifconfig eth0 192.168.32.10 netmask 255.255.254.0"));
    QProcess::startDetached(QString("udhcpd -fS -I 192.168.32.10 %1").arg(confPath));
#endif
    CleanClientList();

    Style::SetButtonIcon(m_udhcpdButton, "toolbutton/yes.svg");
    Style::SetButtonIcon(m_localAreaButton, "toolbutton/no.svg");

}

void MainWindow::OnLocalAreaMode()
{
#ifndef _WIN32
    QProcess::startDetached(QString("killall -9 udhcpc"));
    QProcess::startDetached(QString("killall -9 udhcpd"));
    QProcess::startDetached(QString("/sbin/udhcpc"));
#endif
    CleanClientList();

    Style::SetButtonIcon(m_udhcpdButton, "toolbutton/no.svg");
    Style::SetButtonIcon(m_localAreaButton, "toolbutton/yes.svg");
}

void MainWindow::CleanClientList()
{
    m_clientListWidget->clear();
    while(true)
    {
        auto item = m_clientLayout->itemAt(0);
        if (item == NULL)
            break;
        m_clientLayout->removeItem(item);
    }
    VncSpace::VncManager::Instance()->RemoveAll();
    m_clientListWidget->clear();
}
