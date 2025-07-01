#include "Log.h"
#include "UserEditWidget.h"
#include "LuaException.h"
#include "LuaEngine/LuaEngine.h"
#include "Setting/SettingManager.h"
#include "UI/Frame/MessageDialog.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QSettings>
#include "Setting/Environment.h"
#include <QSignalMapper>
#include "UI/Frame/InputKeyBoard.h"
#include "UI/Frame/LoginDialog.h"

using namespace Configuration;
using namespace System;
using namespace Lua;
using namespace OOLUA;

#define ROW_LINE 13
#define TABLE_WIDTH 800
#define TABLE_HIGH  460

#ifdef _CS_X86_WINDOWS
#define TITLE_HIGH  70
#endif

#ifdef _CS_ARM_LINUX
#define TITLE_HIGH  70
#endif

#define FIXED_HIGH 40

namespace UI
{
UserEditWidget::UserEditWidget(UserRecord record, QWidget *parent) : DropShadowWidget(parent)
{
#ifdef _CS_ARM_LINUX
    setMinimumSize(600, 600);
    move(100, 0);
#endif
    this->setWindowModality(Qt::WindowModal);

#ifdef _CS_X86_WINDOWS
    this->setFixedSize(600, 600);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
#endif
    m_userRecord = record;
    this->SpaceInit();
    this->EditSpaceInit();
}

void UserEditWidget::SpaceInit()
{
    QFont font;
    font.setPointSize(18);

    m_titleLabel = new QLabel("添加用户");
    m_titleLabel->setFont(font);
    m_titleLabel->setFixedSize(120,FIXED_HIGH);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(m_titleLabel);
    topLayout->addStretch();

    m_nameLabel = new QLabel("用户名称：");
    m_nameLabel->setFont(font);
    m_nameLabel->setFixedSize(120,FIXED_HIGH);

    m_nameEdit = new QMyEdit();
    m_nameEdit->setFont(font);
    m_nameEdit->setText("");
    m_nameEdit->setFixedSize(200,FIXED_HIGH);

//    m_nameEdit->installEventFilter(InputKeyBoard::Instance());
    connect(m_nameEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotNameEdit()));

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addSpacing(130);
    nameLayout->addWidget(m_nameLabel);
//    nameLayout->addSpacing(10);
    nameLayout->addWidget(m_nameEdit);
    nameLayout->addStretch();

    m_passwordLabel = new QLabel("新 密 码：");
    m_passwordLabel->setFont(font);
    m_passwordLabel->setFixedSize(120,FIXED_HIGH);

    m_passwordEdit = new QMyEdit();
    m_passwordEdit->setFont(font);
    m_passwordEdit->setText("");
    m_passwordEdit->setFixedSize(200,FIXED_HIGH);
    m_passwordEdit->setMaxLength(6);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

//    m_passwordEdit->installEventFilter(InputKeyBoard::Instance());
    connect(m_passwordEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotNameEdit()));

    QHBoxLayout *pass1Layout = new QHBoxLayout();
    pass1Layout->addSpacing(130);
    pass1Layout->addWidget(m_passwordLabel);
//    pass1Layout->addSpacing(10);
    pass1Layout->addWidget(m_passwordEdit);
    pass1Layout->addStretch();

    m_passwordConfirmLabel = new QLabel("确认密码：");
    m_passwordConfirmLabel->setFont(font);
    m_passwordConfirmLabel->setFixedSize(120,FIXED_HIGH);

    m_passwordConfirmEdit = new QMyEdit();
    m_passwordConfirmEdit->setFont(font);
    m_passwordConfirmEdit->setText("");
    m_passwordConfirmEdit->setFixedSize(200,FIXED_HIGH);
    m_passwordConfirmEdit->setMaxLength(6);
    m_passwordConfirmEdit->setEchoMode(QLineEdit::Password);

//    m_passwordConfirmEdit->installEventFilter(InputKeyBoard::Instance());
    connect(m_passwordConfirmEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotNameEdit()));

    QHBoxLayout *pass2Layout = new QHBoxLayout();
    pass2Layout->addSpacing(130);
    pass2Layout->addWidget(m_passwordConfirmLabel);
//    pass2Layout->addSpacing(10);
    pass2Layout->addWidget(m_passwordConfirmEdit);
    pass2Layout->addStretch();

    m_userLevelLabel = new QLabel("权限等级：");
    m_userLevelLabel->setFont(font);
    m_userLevelLabel->setFixedSize(120,FIXED_HIGH);

    m_userLevelCombox = new QComboBox();
    m_userLevelCombox->setFixedSize(200, 36);
    m_userLevelCombox->setFont(font);
    QVector<QString> leveList = DataBaseManager::Instance()->GetAccessLevelTable()->GetLevelNameList();
    QStringList userLevelList = leveList.toList();
    m_userLevelCombox->insertItems(0, userLevelList);

    QHBoxLayout *levelLayout = new QHBoxLayout();
    levelLayout->addSpacing(130);
    levelLayout->addWidget(m_userLevelLabel);
//    levelLayout->addSpacing(10);
    levelLayout->addWidget(m_userLevelCombox);
    levelLayout->addStretch();

    m_userStatusLabel = new QLabel("状    态：");
    m_userStatusLabel->setFont(font);
    m_userStatusLabel->setFixedSize(120,FIXED_HIGH);

    m_userStatusCombox = new QComboBox();
    m_userStatusCombox->setFixedSize(200, 36);
    m_userStatusCombox->setFont(font);
    QStringList userStatusList;
    userStatusList << "启用" << "停用" << "未知";
    m_userStatusCombox->insertItems(0, userStatusList);

    QHBoxLayout *statusLayout = new QHBoxLayout();
    statusLayout->addSpacing(130);
    statusLayout->addWidget(m_userStatusLabel);
//    statusLayout->addSpacing(10);
    statusLayout->addWidget(m_userStatusCombox);
    statusLayout->addStretch();

    okButton = new QPushButton();
    okButton->setObjectName("brownButton");
    okButton->setText(tr("确定"));
    okButton->setFont(font);
    okButton->setFixedSize(80,40);
    connect(okButton,SIGNAL(clicked()), this, SLOT(SlotOkButton()));

    quitButton = new QPushButton();
    quitButton->setObjectName("brownButton");
    quitButton->setText(tr("取消"));
    quitButton->setFont(font);
    quitButton->setFixedSize(80,40);
    connect(quitButton,SIGNAL(clicked()), this, SLOT(SlotQuitButton()));
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(okButton);
    bottomLayout->addSpacing(50);
    bottomLayout->addWidget(quitButton);
    bottomLayout->addStretch();

    QVBoxLayout *midLayout = new QVBoxLayout();
    midLayout->addLayout(nameLayout);
    midLayout->addLayout(pass1Layout);
    midLayout->addLayout(pass2Layout);
    midLayout->addLayout(levelLayout);
    midLayout->addLayout(statusLayout);
    midLayout->addStretch();
    midLayout->setSpacing(30);

    QVBoxLayout *mainLayout = new QVBoxLayout();
//    mainLayout->addSpacing(10);
    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(midLayout);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setContentsMargins(0, 20, 0, 15);

    this->setLayout(mainLayout);
}

void UserEditWidget::SlotOkButton()
{
    if(m_nameEdit->text().isEmpty())
    {
        MessageDialog msg(tr("用户名称不能为空！"), this, MsgStyle::ONLYOK);
        msg.exec();
        return;
    }
    if(m_passwordEdit->text().isEmpty())
    {
        MessageDialog msg(tr("密码不能为空！"), this, MsgStyle::ONLYOK);
        msg.exec();
        return;
    }
    if(m_passwordEdit->text() != m_passwordConfirmEdit->text())
    {
        MessageDialog msg(tr("两次密码输入不一致，请重新输入\n"), this, MsgStyle::ONLYOK);
        msg.exec();
        return;
    }
    UserRecord record;
    record.dataTime = QDateTime::currentDateTime().toTime_t();
    record.userName = m_nameEdit->text();
    record.levelName = m_userLevelCombox->currentText();
    record.password = m_passwordEdit->text();
    record.status = m_userStatusCombox->currentIndex();   
    QString msgStr = "用户创建完成";
    if(!m_userRecord.userName.isEmpty())
    {
        msgStr = "用户修改完成";
        record.dataTime = m_userRecord.dataTime;
        record.lastLoginTime = m_userRecord.lastLoginTime;
        record.lastEditTime = QDateTime::currentDateTime().toTime_t();
    }
    AuditTrail(m_userRecord, record);
    DataBaseManager::Instance()->GetUserTable()->InsertData({record});
    MessageDialog msg(msgStr, this, MsgStyle::ONLYOK);
    msg.exec();
    this->close();
}


void UserEditWidget::SlotQuitButton()
{
    this->close();
}

void UserEditWidget::SlotNameEdit(void)
{
    int curX = cursor().pos().x();
    int curY = cursor().pos().y();

    int kbWidth = 530;
    int kbHeigh = 235;
#ifdef _CS_ARM_LINUX
    int x0 = 0;
    int y0 = 0;
#endif
#ifdef _CS_X86_WINDOWS
    int x0 = 550;
    int y0 = 170;
#endif

//    qDebug("x[%d],y[%d]",curX,curY);
    curX = x0 + 135;
    curY = curY + 40;

    InputKeyBoard::Instance()->move(curX,curY);
    InputKeyBoard::Instance()->show();
}


void UserEditWidget::EditSpaceInit()
{
    if(!m_userRecord.userName.isEmpty())
    {
        m_titleLabel->setText("修改信息");
        m_nameEdit->setText(m_userRecord.userName);
        m_nameEdit->setEnabled(false);
        m_nameEdit->setReadOnly(true);
        // 设置样式表：背景为灰色
        m_nameEdit->setStyleSheet("QLineEdit { background-color:rgb(220,220,220); }");
        m_passwordEdit->setText(m_userRecord.password);
        m_passwordConfirmEdit->setText(m_userRecord.password);
        m_userLevelCombox->setCurrentText(m_userRecord.levelName);
        m_userStatusCombox->setCurrentIndex(m_userRecord.status);
    }
}

void UserEditWidget::AuditTrail(UserRecord oldRecord, UserRecord newRecord)
{
    QString userName = UI::LoginDialog::userInfo.userName;
    QString levelName = UI::LoginDialog::userInfo.levelName;
    QString event = "创建用户-" + newRecord.userName;
    QString oldSetting = "--";
    QString newSetting = "--";
    QString detail = "--";
    QStringList detailList;
    QString oldStatusStr;
    QString newStatusStr;
    switch (oldRecord.status)
    {
        case (int)UserStatus::Enable:
            oldStatusStr = "启用";
            break;
        case (int)UserStatus::Disable:
            oldStatusStr = "停用";
            break;
        default:
            oldStatusStr = "未知";
            break;
    }
    switch (newRecord.status)
    {
        case (int)UserStatus::Enable:
            newStatusStr = "启用";
            break;
        case (int)UserStatus::Disable:
            newStatusStr = "停用";
            break;
        default:
            newStatusStr = "未知";
            break;
    }
    if(!oldRecord.userName.isEmpty())
    {
        event = "编辑用户-" + newRecord.userName;
        if(oldRecord.password != newRecord.password)
        {
            detailList.append("修改密码");
        }
        if(oldRecord.levelName != newRecord.levelName)
        {
            detailList.append("修改权限:" + oldRecord.levelName + "->" + newRecord.levelName);
        }
        if(oldRecord.status != newRecord.status)
        {
            detailList.append("修改状态:" + oldStatusStr + "->" + newStatusStr);
        }
        detail = detailList.join(", ");
    }
    else
    {
        detailList.append(newRecord.levelName);
        detailList.append(newStatusStr);
        detail = detailList.join(", ");
    }
    //审计追踪
    DataBaseManager::Instance()->GetAuditTrailTable()->InsertAuditTrail(userName, levelName, event, oldSetting, newSetting, detail);
}

void UserEditWidget::showEvent(QShowEvent *event)
{
//    m_quitButton->setFocus();
//    QDialog::showEvent(event);
}

void UserEditWidget::mousePressEvent(QMouseEvent *event)
{
    DropShadowWidget::mousePressEvent(event);

    if (!InputKeyBoard::Instance()->contentsRect().contains(InputKeyBoard::Instance()->mapFromGlobal(event->globalPos())))
    {
        InputKeyBoard::Instance()->hide();
    }
}

void UserEditWidget::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    int height = TITLE_HIGH;
    QPainter painter(this);
    painter.setPen(Qt::white);

//    painter.setColor(QColor(115, 115, 140));
    painter.setBrush(QBrush(QColor(115, 115, 140)));
    painter.drawRect(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    height - SHADOW_WIDTH));

//    painter.drawPixmap(
//            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
//                    this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN));

    painter.setBrush(Qt::white);

    painter.drawRect(
            QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH,
                    TABLE_HIGH - SHADOW_WIDTH));

    painter.setBrush(QBrush(QColor(115, 115, 140)));
    painter.drawRect(
            QRect(SHADOW_WIDTH, height + TABLE_HIGH, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - height - SHADOW_WIDTH));

    QPen pen;
    pen.setColor(QColor(10,105,170));
    pen.setWidth(3);

    painter.setPen(pen);
    painter.drawLine(QPoint(0,0), QPoint(0,this->height()));
    painter.drawLine(QPoint(0,0), QPoint(this->width(),0));
    painter.drawLine(QPoint(0,this->height()-1), QPoint(this->width()-1,this->height()-1));
    painter.drawLine(QPoint(this->width()-1,0), QPoint(this->width()-1,this->height()-1));
}

UserEditWidget::~UserEditWidget()
{

}

}

