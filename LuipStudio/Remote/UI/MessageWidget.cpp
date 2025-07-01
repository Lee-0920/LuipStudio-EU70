#include "MessageWidget.h"
#include "Style.h"

MessageWidget::MessageWidget(QWidget *parent) : QDialog(parent)
{
    InitWidget();

    connect(m_cancelBtn, &QPushButton::clicked, this, &MessageWidget::OnCancelClick);
    connect(m_confirmBtn, &QPushButton::clicked, this, &MessageWidget::OnComfirmClick);
}

MessageWidget::~MessageWidget()
{

}

void MessageWidget::Show(QString title, QString msg, bool showCancel)
{
    m_cancelBtn->setVisible(showCancel);
    m_titleLable->setText(title);
    m_msgLable->setText(msg);
    this->show();
    this->exec();
}

void MessageWidget::InitWidget()
{
    //标题
    m_titleLable = Style::CreateHeaderLabel("");
    QHBoxLayout * headerlayout = new QHBoxLayout();
    headerlayout->addWidget(m_titleLable);
    headerlayout->addStretch(1);
    headerlayout->setSpacing(0);

    auto headWidget = new QWidget();
    headWidget->setObjectName("headerWidget");
    headWidget->setLayout(headerlayout);

    //内容
    m_msgLable = Style::CreateLabel("");
    m_confirmBtn = Style::CreateButton("确定");
    m_cancelBtn = Style::CreateButton("取消");


    auto grid = new QGridLayout();
    grid->addWidget(m_msgLable);
    grid->setMargin(30);

    auto btnGrid = new QHBoxLayout();
    btnGrid->addWidget(m_confirmBtn, Qt::AlignRight, Qt::AlignVCenter);
    btnGrid->addWidget(m_cancelBtn, Qt::AlignRight, Qt::AlignVCenter);
    btnGrid->setMargin(10);

    auto layout = new QVBoxLayout();
    layout->setMargin(1);
    layout->addWidget(headWidget);
    layout->addLayout(grid);
    layout->addLayout(btnGrid);

    auto widget = new QWidget();
    widget->setLayout(layout);
    widget->setObjectName("MessageWidget");

    //阴影
    auto backgroundLayout = new QGridLayout(this);
    backgroundLayout->setMargin(0);
    backgroundLayout->setSpacing(0);
    backgroundLayout->addWidget(widget,0,0, Qt::AlignCenter);

    this->setAttribute(Qt::WA_StyledBackground);
    this->setObjectName("shadowWidget");
    this->hide();
}


void MessageWidget::OnComfirmClick()
{
    this->m_isConfirm = true;
    this->hide();
}

void MessageWidget::OnCancelClick()
{
    this->m_isConfirm = false;
    this->hide();
}

