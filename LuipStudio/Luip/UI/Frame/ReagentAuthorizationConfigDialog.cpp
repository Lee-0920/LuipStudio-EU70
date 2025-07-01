#include <QGridLayout>
#include "ReagentAuthorizationConfigDialog.h"
#include "UI/Frame/Common.h"
#include "System/Types.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/MessageDialog.h"
#include "System/DynamicPassword.h"
#include "Setting/SettingManager.h"
#include "System/AuthorizationManager.h"


using namespace Lua;
using namespace System;
using namespace Configuration;

namespace UI
{

ReagentAuthorizationConfigDialog::ReagentAuthorizationConfigDialog(QWidget *parent) :
        DropShadowWidget(parent), m_dialogKeyboard(nullptr)
{
    m_superAppear = false;
    this->resize(480, 370);
    setWindowIcon(QIcon(":/img/WQIMC"));

    m_titleLabel = new QLabel();
    m_logoLabel = new QLabel();
    m_enableLabel = new QLabel();
    m_dateLabel = new QLabel();
    m_authorizationComboBox = new QComboBox();
    m_dateEdit = new QMyEdit();

    m_titleLabel->setObjectName("softwareTitle");
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(18);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addWidget(m_titleLabel, 0, Qt::AlignVCenter);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0, 5, 10, 0);
    m_titleLabel->setContentsMargins(20, 20, 0, 0);

    m_logoLabel->setFixedSize(160, 75);
    m_logoLabel->setPixmap(QPixmap(":/img/logo"));
    m_logoLabel->setScaledContents(true);

    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->addStretch();
    logoLayout->addWidget(m_logoLabel);
    logoLayout->setSpacing(10);
    logoLayout->setContentsMargins(0, 0, 30, 0);

    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addLayout(titleLayout);
    topLayout->addLayout(logoLayout);
    topLayout->addStretch();
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QFont font;
    font.setPointSize(16);

    // 开关
    m_enableLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_enableLabel->setFixedSize(100, 30);
    m_enableLabel->setFont(font);
    m_enableLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QStringList enableStringList;
    enableStringList << tr("关闭") << tr("开启");
    m_authorizationComboBox->setObjectName(QStringLiteral("authorizationComboBox"));
    m_authorizationComboBox->setFixedSize(140, 30);
    m_authorizationComboBox->setFont(font);
    m_authorizationComboBox->clear();
    m_authorizationComboBox->insertItems(0, enableStringList);

    //日期
    m_dateLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_dateLabel->setFixedSize(100, 30);
    m_dateLabel->setFont(font);
    m_dateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_dateEdit->setObjectName(QStringLiteral("authorizationComboBox"));
    m_dateEdit->setFixedSize(140, 30);
    QFont passwordFont = m_dateEdit->font();
    passwordFont.setPointSize(12);
    m_dateEdit->setFont(passwordFont);
    m_dateEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//    m_dialogKeyboard = new CNumberKeyboard(this);
//    m_dateEdit->installEventFilter(m_dialogKeyboard);

    m_loginButton = new QPushButton();
    m_loginButton->setObjectName(QStringLiteral("brownButton"));
    m_loginButton->setFixedSize(140, 38);
    m_loginButton->setFont(font);
    m_loginButton->setFocus();
    m_loginButton->setDefault(true);
    connect(m_loginButton, SIGNAL(clicked()), this, SLOT(SlotloginButton()));

    m_cancelButton = new QPushButton();
    m_cancelButton->setObjectName(QStringLiteral("brownButton"));
    m_cancelButton->setFixedSize(140, 38);
    m_cancelButton->setFont(font);
    m_cancelButton->setFocus();
    m_cancelButton->setDefault(true);
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(m_enableLabel, 0, 0);
    gridLayout->addWidget(m_authorizationComboBox, 0, 1, 1, 2);
    gridLayout->addWidget(m_dateLabel, 1, 0);
    gridLayout->addWidget(m_dateEdit, 1, 1, 1, 2);
    gridLayout->setHorizontalSpacing(20);
    gridLayout->setVerticalSpacing(30);

    QHBoxLayout *gridHLayout = new QHBoxLayout();
    gridHLayout->addStretch();
    gridHLayout->addLayout(gridLayout);
    gridHLayout->addStretch();
    gridHLayout->setSpacing(10);
    gridHLayout->setContentsMargins(0, 25, 40, 0);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_cancelButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_loginButton);
    bottomLayout->addStretch();
    bottomLayout->setSpacing(10);
    bottomLayout->setContentsMargins(0, 20, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(gridHLayout);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addStretch();
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(10, 0, 0, 20);

    this->setLayout(mainLayout);
    this->TranslateLanguage();

    m_dayCaledar = new QCalendarWidget(this);
//    m_dayCaledar->setStyleSheet("QCalendarWidget { \
//                                 border-radius:5px; \
//                                 border:1px solid gray;}");
    if(SettingManager::Instance()->GetAppLanguage() == Language::SimplifiedChinese)
    {
        m_dayCaledar->setLocale(QLocale(QLocale::Chinese, QLocale::China));
    }
    else if(SettingManager::Instance()->GetAppLanguage() == Language::TraditionalChinese)
    {
        m_dayCaledar->setLocale(QLocale(QLocale::Chinese, QLocale::Taiwan));
    }
    else if(SettingManager::Instance()->GetAppLanguage() == Language::English)
    {
        m_dayCaledar->setLocale(QLocale::English);
    }
    m_dayCaledar->hide();

    connect(m_dateEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotChoseDay()));
    connect(m_dayCaledar, SIGNAL(clicked(const QDate &)), this, SLOT(SlotChangeDay()));
    connect(m_authorizationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotEnableChange()));

    this->ShowContent();
    this->SlotEnableChange();
}

ReagentAuthorizationConfigDialog::~ReagentAuthorizationConfigDialog()
{
    if (m_dialogKeyboard)
    {
        delete m_dialogKeyboard;
        m_dialogKeyboard = nullptr;
    }
}

void ReagentAuthorizationConfigDialog::TranslateLanguage()
{
    m_titleLabel->setText(tr("试剂授权"));
    m_enableLabel->setText(tr("授权开关:"));
    m_dateLabel->setText(tr("授权期限:"));
    m_loginButton->setText(tr("保存"));
    m_cancelButton->setText(tr("取消"));
}

void ReagentAuthorizationConfigDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    this->ShowContent();
}

void ReagentAuthorizationConfigDialog::SlotloginButton()
{
    if (true == (bool)m_authorizationComboBox->currentIndex())
    {
        //输入检查
        QString strDate = m_dateEdit->text();
        QDate date = QDate::fromString(strDate, "yyyy-MM-dd");
        if (!date.isValid())
        {
            MessageDialog msg(tr("请选择正确的时间！"), this, MsgStyle::ONLYOK, false);
            msg.exec();
            return;
        }

        MessageDialog msg(tr("\r确认开启授权？\n\r授权到期后，测量将受限！\n\r再次进入需输入授权码！"), this, MsgStyle::OKANDCANCEL, true);
        if(msg.exec()== QDialog::Rejected)
        {
            return ;
        }

        QDateTime dtCurrent;
        dtCurrent.setDate(date);
        dtCurrent.setTime(QTime(0, 0, 0));
        AuthorizationManager::Instance()->SetReagentAuthorization(dtCurrent.toTime_t());
    }
    else
    {
        AuthorizationManager::Instance()->SetReagentAuthorization(0);

        MessageDialog msg(tr("\r试剂授权功能已关闭！"), this, MsgStyle::ONLYOK, true);
        msg.exec();
    }

    this->close();
}

void ReagentAuthorizationConfigDialog::SlotChoseDay()
{
    m_dayCaledar->setFixedSize(300, 250);
    m_dayCaledar->move(75, 60);

    m_dayCaledar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    if (m_dayCaledar->isHidden())
    {
        m_dayCaledar->show();
    }
    else
    {
        m_dayCaledar->hide();
    }
}

void ReagentAuthorizationConfigDialog::SlotChangeDay()
{
    QDate date = m_dayCaledar->selectedDate();
    QString dateString;
    QString monthsign;
    QString daysign;
    if (date.month() >= 10)
    {
        monthsign = "-";
    }
    else
    {
        monthsign = "-0";
    }

    if (date.day() >= 10)
    {
        daysign = "-";
    }
    else
    {
        daysign = "-0";
    }

    dateString = QString::number(date.year()) + monthsign + QString::number(date.month()) + daysign + QString::number(date.day());

    m_dateEdit->setText(dateString);
    m_dayCaledar->hide();
}

void ReagentAuthorizationConfigDialog::SlotEnableChange()
{
    bool enable = (bool)m_authorizationComboBox->currentIndex();
    m_dateEdit->setEnabled(enable);
}

void ReagentAuthorizationConfigDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    int height = 120;
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawPixmap(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN));
    painter.drawRect(
            QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - height - SHADOW_WIDTH));

}

void ReagentAuthorizationConfigDialog::mousePressEvent(QMouseEvent *event)
{
    DropShadowWidget::mousePressEvent(event);

    if (!m_dayCaledar->contentsRect().contains(m_dayCaledar->mapFromGlobal(event->globalPos())))
    {
        m_dayCaledar->hide();
    }
}

void ReagentAuthorizationConfigDialog::ShowContent()
{
    QDate date = AuthorizationManager::Instance()->GetReagentAuthorizationExpirationDate();
    if (date.isValid())
    {
        m_dateEdit->setText(date.toString("yyyy-MM-dd"));
    }
    else
    {
        m_dateEdit->clear();
    }

    int nEnable = AuthorizationManager::Instance()->IsReagentAuthorizationEnable();
    int index = (int)((int)ReagentAuthorizationEnable::Effective == nEnable);
    m_authorizationComboBox->setCurrentIndex(index);
}

}
