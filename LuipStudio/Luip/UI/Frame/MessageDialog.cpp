#include "MessageDialog.h"
#include <QDebug>
#include <QProcess>
#include <QDebug>
#include "Setting/SettingManager.h"

using Configuration::SettingManager;
using Configuration::ManufactureType;

namespace UI
{

MessageDialog::MessageDialog(QWidget *parent):QDialog(parent),deadTimer(nullptr)
{
    Init("", MsgStyle::ONLYOK);
}

MessageDialog::MessageDialog(const QString &text,
                             QWidget *parent,
                             MsgStyle msgstyle,
                             bool isWarning)
    :QDialog(parent),deadTimer(nullptr)
{
    Init(text, msgstyle, isWarning);
}

MessageDialog::MessageDialog(const QString &text,
                             int deadTime,
                             QWidget *parent,
                             MsgStyle msgstyle,
                             bool isWarning) :QDialog(parent),deadTimer(nullptr)
{
    m_deadTime = deadTime;
    if(m_deadTime > 0)
    {
        Init(text, msgstyle, isWarning, true);
    }
    else
    {
        Init(text, msgstyle, isWarning, false);
    }
}

MessageDialog::MessageDialog(const QString &text1,
                             const QString &text2,
                             QWidget *parent) :QDialog(parent),deadTimer(nullptr)
{
    InitTwoText(text1, text2);
}

void MessageDialog::InitTwoText(const QString &text1,
                         const QString &text2)
{
#ifdef _CS_X86_WINDOWS
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    this->setMinimumSize(400,300);

    m_logoLabel = new QLabel();

    m_logoLabel->setFixedSize(55, 55);
    if(SettingManager::Instance()->GetManufactureType() == ManufactureType::LS)
    {
        m_logoLabel->setPixmap(QPixmap(":/img/WQIMC"));
    }

    m_warningLabel = new QLabel();
    m_warningLabel->setFixedSize(55, 55);
    m_warningLabel->setPixmap(QPixmap(":/img/warning"));
    m_warningLabel->setScaledContents(true);

    m_logoLabel->setScaledContents(true);
    m_logoLabel->setPixmap(QPixmap(":/img/WQIMC"));

    QPushButton *closebtn = new QPushButton();
    closebtn->setFixedSize(90,45);
    QFont bntfont;
    bntfont.setPointSize(16);
    closebtn->setFont(bntfont);
    closebtn->setObjectName("brownButton");
    closebtn->setText("取消");
    connect(closebtn, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));

    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->addStretch();
    logoLayout->addWidget(m_logoLabel,1,Qt::AlignRight|Qt::AlignVCenter);
    logoLayout->setSpacing(0);

    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addLayout(logoLayout);
    topLayout->addStretch();
    topLayout->setSpacing(0);

    QFont font;
    font.setPointSize(16);

    m_textLabel = new QLabel(this);
    m_textLabel->setFont(font);
    m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_textLabel->setText(text1);
    m_textLabel->setWordWrap(true);
    m_textLabel->setFixedSize(300, 45);
    m_textLabel->adjustSize();
//    m_textLabel->setStyleSheet("QLabel{border:1px solid rgb(0, 0, 0);}");

    m_rightTextLabel = new QLabel(this);
    m_rightTextLabel->setFont(font);
    m_rightTextLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_rightTextLabel->setText(text2);
    m_rightTextLabel->setWordWrap(true);
    m_rightTextLabel->setFixedSize(300, 45);
    m_rightTextLabel->adjustSize();
//    m_rightTextLabel->setStyleSheet("QLabel{border:1px solid rgb(0, 0, 0);}");

    QPushButton *btnItem1  = new QPushButton(this);
    btnItem1->setObjectName(QStringLiteral("brownButton"));
    btnItem1->setFixedSize(90, 45);
    btnItem1->setFont(font);
    btnItem1->setFocus();
    btnItem1->setDefault(true);
    btnItem1->setText(tr("执行"));
    connect(btnItem1, SIGNAL(clicked()), this, SLOT(SlotConfirmButton()));

    QPushButton *btnItem2 = new QPushButton(this);
    btnItem2->setObjectName(QStringLiteral("brownButton"));
    btnItem2->setFixedSize(90, 45);
    btnItem2->setFont(font);
    btnItem2->setFocus();
    btnItem2->setDefault(true);
    btnItem2->setText(tr("执行"));
    connect(btnItem2, SIGNAL(clicked()), this, SLOT(SlotSetSecondRetButton()));

    QWidget *item1 = new QWidget();
    item1->setFixedSize(400,60);
    item1->setStyleSheet("QWidget{border:2px solid rgb(130, 130, 130);border-radius:15px;}"
                        "QLabel{border:0px solid rgb(0, 0, 0);}"
                        "QPushButton{border:0px solid rgb(0, 0, 0);}");
    QHBoxLayout *leftLayout = new QHBoxLayout();
    leftLayout->addWidget(btnItem1,0,Qt::AlignVCenter|Qt::AlignLeft);
    leftLayout->addWidget(m_textLabel,0,Qt::AlignVCenter|Qt::AlignLeft);
    item1->setLayout(leftLayout);

    QWidget *item2 = new QWidget();
    item2->setFixedSize(400,60);
    item2->setStyleSheet("QWidget{border:2px solid rgb(130, 130, 130);border-radius:15px;}"
                        "QLabel{border:0px solid rgb(0, 0, 0);}"
                        "QPushButton{border:0px solid rgb(0, 0, 0);}");
    QHBoxLayout *rightLayout = new QHBoxLayout();
    rightLayout->addWidget(btnItem2,0,Qt::AlignVCenter|Qt::AlignLeft);
    rightLayout->addWidget(m_rightTextLabel,0,Qt::AlignVCenter|Qt::AlignLeft);
    item2->setLayout(rightLayout);

    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->addSpacing(10);
    textLayout->addWidget(item1);
    textLayout->addSpacing(10);
    textLayout->addWidget(item2);
    textLayout->addStretch();

    QHBoxLayout *closebtnLayont = new QHBoxLayout();
    closebtnLayont->addStretch();
    closebtnLayont->addWidget(closebtn);
    closebtnLayont->addSpacing(70);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout,1);
    mainLayout->addLayout(textLayout,3);
    mainLayout->addLayout(closebtnLayont);
    mainLayout->setSpacing(10);

    this->setLayout(mainLayout);
}

void MessageDialog::Init(const QString &text,
                         MsgStyle msgstyle,
                         bool isWarning,
                         bool withDeadTime)
{
#ifdef _CS_X86_WINDOWS
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

   // this->resize(280, 205);
    this->setMinimumSize(400,300);

    m_logoLabel = new QLabel(this);

    m_logoLabel->setFixedSize(55, 55);
    if(SettingManager::Instance()->GetManufactureType() == ManufactureType::LS)
    {
        m_logoLabel->setPixmap(QPixmap(":/img/WQIMC"));
    }
    m_logoLabel->setScaledContents(true);

    m_warningLabel = new QLabel();
    m_warningLabel->setFixedSize(55, 55);
    m_warningLabel->setPixmap(QPixmap(":/img/warning"));
    m_warningLabel->setScaledContents(true);

    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->addStretch();
    logoLayout->addWidget(m_logoLabel,1,Qt::AlignRight|Qt::AlignVCenter);
    logoLayout->setSpacing(0);

    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addLayout(logoLayout);
    topLayout->addStretch();
    topLayout->setSpacing(0);

    QFont font;
    font.setPointSize(16);

    m_textLabel = new QLabel(this);
    m_textLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_textLabel->setFont(font);
    m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_textLabel->setText(text);
    m_textLabel->setWordWrap(true);
    m_textLabel->setMinimumSize(360, 120);
    m_textLabel->adjustSize();

    QHBoxLayout *textLayout = new QHBoxLayout();
    textLayout->addStretch();
    if (true == isWarning)
    {
        textLayout->addWidget(m_warningLabel);
        textLayout->addStretch();
    }
    textLayout->addWidget(m_textLabel);
    textLayout->addStretch();

    if(withDeadTime == true)
    {
        m_deadLabel = new QLabel(this);
        m_deadLabel->setObjectName(QStringLiteral("whiteLabel"));
        m_deadLabel->setFont(font);
        m_deadLabel->setFixedSize(30, 30);
        m_deadLabel->setAlignment(Qt::AlignCenter);
        m_deadLabel->setText(QString::number(m_deadTime));
    }

    m_confirmButton = new QPushButton(this);
    m_confirmButton->setObjectName(QStringLiteral("brownButton"));
    m_confirmButton->setFixedSize(90, 45);
    m_confirmButton->setFont(font);
    m_confirmButton->setFocus();
    m_confirmButton->setDefault(true);
    m_confirmButton->setText(tr("确认"));
    connect(m_confirmButton, SIGNAL(clicked()), this, SLOT(SlotConfirmButton()));

    m_cancelButton = new QPushButton(this);
    m_cancelButton->setObjectName(QStringLiteral("brownButton"));
    m_cancelButton->setFixedSize(90, 45);
    m_cancelButton->setFont(font);
    m_cancelButton->setFocus();
    m_cancelButton->setDefault(true);
    m_cancelButton->setText(tr("取消"));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));

    QHBoxLayout *bottomBtnLayout = new QHBoxLayout();
    switch(msgstyle)
    {
    case MsgStyle::ONLYOK:
        bottomBtnLayout->addWidget(m_confirmButton);
        m_cancelButton->hide();
        break;
    case MsgStyle::OKANDCANCEL:
        bottomBtnLayout->addWidget(m_confirmButton);
        bottomBtnLayout->addWidget(m_cancelButton);
        break;
    case MsgStyle::ONLYCANCEL:
        m_confirmButton->hide();
        bottomBtnLayout->addWidget(m_cancelButton);
        break;
    case MsgStyle::YESANDNO:
        bottomBtnLayout->addWidget(m_confirmButton);
        bottomBtnLayout->addWidget(m_cancelButton);
        m_confirmButton->setText("是");
        m_cancelButton->setText("否");
        break;
    }
    bottomBtnLayout->setSpacing(10);

    QVBoxLayout *bottomLayout = new QVBoxLayout();
    bottomLayout->addStretch(1);
//    bottomLayout->addLayout(textLayout,0,Qt::AlignHCenter);//1
    bottomLayout->addLayout(textLayout);
    if(withDeadTime == true)
    {
        bottomLayout->addWidget(m_deadLabel, 0, Qt::AlignHCenter);//1
    }
    bottomLayout->addStretch(1);//2
    bottomLayout->addLayout(bottomBtnLayout,1);
    bottomLayout->addStretch(1);
    bottomLayout->setSpacing(5);//10

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout,1);
    mainLayout->addLayout(bottomLayout,3);
    mainLayout->setSpacing(10);

    if(withDeadTime == true)
    {
        deadTimer = new QTimer(this);
        deadTimer->setInterval(1000);

        connect(this->deadTimer, SIGNAL(timeout()), this, SLOT(SlotDeadTimeCount()));

        connect(this, SIGNAL(SignalDeadTimeReboot()), this, SLOT(SlotDeadTimeReboot()));

        deadTimer->start();
    }

    this->setLayout(mainLayout);
}

MessageDialog::~MessageDialog()
{

}

void MessageDialog::SlotConfirmButton()
{
    if(deadTimer != nullptr)
    {
        deadTimer->stop();
    }
    this->accept();
}

void MessageDialog::SlotSetSecondRetButton()
{
    this->done(2);
}


void MessageDialog::SlotCancelButton()
{
    if(deadTimer != nullptr)
    {
        deadTimer->stop();
    }
    this->reject();
}

void MessageDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    int height = 75;
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawPixmap(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                  this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN)); //DEFAULT_SKIN
    painter.drawRect(
            QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH,
                  this->height() - height - SHADOW_WIDTH));
}


void MessageDialog::SlotDeadTimeCount()
{
    m_deadTime--;
    this->m_deadLabel->setText(QString::number(m_deadTime));
    if(m_deadTime <= 0)
    {
        if(deadTimer != nullptr)
        {
            deadTimer->stop();
        }
        emit SignalDeadTimeReboot();
    }
}

void MessageDialog::SlotDeadTimeReboot()
{
    #ifdef _CS_ARM_LINUX
        QProcess::execute("reboot");
    #endif
    this->accept();
}

void MessageDialog::SetText(const QString &text)
{
    m_textLabel->setText(text);
}
}
