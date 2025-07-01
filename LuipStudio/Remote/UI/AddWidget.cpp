#include "AddWidget.h"
#include "VncPlugin/VncManager.h"
#include <QHostAddress>

AddWidget::AddWidget(QWidget *parent) : QWidget(parent)
{
    InitWidget();

    connect(m_cancelBtn, &QPushButton::clicked, this, [this]{this->hide();});
    connect(m_confirmBtn, &QPushButton::clicked, this, &AddWidget::OnComfirmClick);
}

void AddWidget::Show()
{
    m_ipEdit->setText("");
    m_portEdit->setText("5900");
    m_msgLable->setText("");

    this->show();
}

void AddWidget::InitWidget()
{
    //标题
    QHBoxLayout * headerlayout = new QHBoxLayout();
    headerlayout->addWidget(Style::CreateHeaderLabel("添加设备"));
    headerlayout->addStretch(1);
    headerlayout->setSpacing(0);

    auto headWidget = new QWidget();
    headWidget->setObjectName("headerWidget");
    headWidget->setLayout(headerlayout);

    //内容
    m_ipEdit = Style::CreateLineEdit();
    m_portEdit = Style::CreateLineEdit();
    m_nameEdit = Style::CreateLineEdit();

    m_confirmBtn = Style::CreateButton("确定");
    m_cancelBtn = Style::CreateButton("取消");

    m_msgLable = Style::CreateLabel("");

    auto grid = new QGridLayout();
    grid->addWidget(Style::CreateLabel("地址:"));
    grid->addWidget(Style::CreateLabel("端口:"), 1, 0);
    grid->addWidget(Style::CreateLabel("名称:"), 2, 0);
    grid->addWidget(m_ipEdit, 0, 1);
    grid->addWidget(m_portEdit, 1, 1);
    grid->addWidget(m_nameEdit, 2, 1);
    grid->setMargin(10);

    auto btnGrid = new QHBoxLayout();
    btnGrid->addWidget(m_confirmBtn, Qt::AlignRight, Qt::AlignVCenter);
    btnGrid->addWidget(m_cancelBtn, Qt::AlignRight, Qt::AlignVCenter);
    btnGrid->setMargin(10);

    auto layout = new QVBoxLayout();
    layout->setMargin(1);
    layout->addWidget(headWidget);
    layout->addLayout(grid);
    layout->addLayout(btnGrid);
    layout->addWidget(m_msgLable);

    auto widget = new QWidget();
    widget->setLayout(layout);
    widget->setObjectName("addWidget");

    //阴影
    auto backgroundLayout = new QGridLayout(this);
    backgroundLayout->setMargin(0);
    backgroundLayout->setSpacing(0);
    backgroundLayout->addWidget(widget,0,0, Qt::AlignCenter);

    this->setAttribute(Qt::WA_StyledBackground);
    this->setObjectName("shadowWidget");
    this->hide();
}


void AddWidget::OnComfirmClick()
{
    m_msgLable->setText("");

    QString ip = m_ipEdit->text();
    int port = m_portEdit->text().toUInt();
    QString name = m_nameEdit->text();

    QHostAddress test;
    if (!test.setAddress(ip) || port <= 0 || port >= 65535)
    {
        m_msgLable->setText("地址或端口号错误！");
        return;
    }

    if (name.isEmpty())
        name = ip;

    DeviceInfo info;
    info.ip = ip;
    info.port = port;
    info.name = name;
    info.password = "";

    if (VncSpace::VncManager::Instance()->Contains(ip))
    {
        m_msgLable->setText("地址已存在！");
        return;
    }

    emit SignalAddDevice(info);
    this->hide();
}

