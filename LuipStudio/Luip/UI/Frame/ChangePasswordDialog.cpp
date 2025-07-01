#include "Setting/SettingManager.h"
#include "UI/Frame/MessageDialog.h"
#include "ChangePasswordDialog.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/LoginDialog.h"
#include "System/ProLicenseManager.h"

using namespace Configuration;
using namespace Lua;
using namespace System;

namespace UI
{

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent):
        DropShadowWidget(parent)
{
    m_totalClick = 0;
    m_superAppear = false;
    this->resize(480,370);
    setWindowIcon(QIcon(":/img/WQIMC"));

    m_titleLabel = new QLabel();
    m_userLabel = new QLabel();
    m_oldPassword = new QLabel();
    m_newPassword = new QLabel();
    m_newPasswordConfirm = new QLabel();

    m_userComboBox = new QComboBox(); //选择用户
    m_oldPasswordLineEdit = new QLineEdit(); //旧密码编辑框
    m_newPasswordLineEdit = new QLineEdit(); //新密码编辑框
    m_newPasswordConfirmLineEdit = new QLineEdit(); //新密码确认编辑框

    m_passwordChangeConfirmButton = new QPushButton(); //确认修改按钮
    m_newPasswordChangeCancelButton = new QPushButton(); //取消修改按钮

    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(12);

    m_titleLabel->setObjectName(QStringLiteral("softwareTitle"));
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setContentsMargins(20, 20, 0, 0);
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addWidget(m_titleLabel, 0, Qt::AlignVCenter);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0,5,10,0);

    QFont font;
    font.setPointSize(16);

    m_userLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_userLabel->setFixedSize(120,35);
    m_userLabel->setFont(font);
    m_userLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

    QStringList userStringList;
    userStringList<<tr("普通用户")<<tr("运维员")<<tr("管理员");
    m_userComboBox->setFixedSize(140,45);
    m_userComboBox->setFont(font);
    m_userComboBox->clear();
    m_userComboBox->insertItems(0,userStringList);
    if(ProLicenseManager::Instance()->GetProLicense())
    {
        QVector<QString> userItem = DataBaseManager::Instance()->GetUserTable()->GetUserNameList();
        userStringList = userItem.toList();
        m_userComboBox->clear();
        m_userComboBox->insertItems(0, userStringList);
    }

    m_oldPassword->setObjectName(QStringLiteral("whiteLabel"));
    m_oldPassword->setFixedSize(120,35);
    m_oldPassword->setFont(font);

    QFont passwordFont = m_oldPasswordLineEdit->font();
    passwordFont.setPointSize(12);

    m_oldPassword->setObjectName(QStringLiteral("whiteLabel"));
    m_oldPassword->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_oldPasswordLineEdit->setFixedSize(140,45);
    m_oldPasswordLineEdit->setFont(passwordFont);
    m_oldPasswordLineEdit->setMaxLength(6);
    m_oldPasswordLineEdit->setEchoMode(QLineEdit::Password);
    m_oldPasswordLineEdit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_umberKeyboard = new CNumberKeyboard(this);
    m_oldPasswordLineEdit->installEventFilter(m_umberKeyboard);

    m_newPassword->setObjectName(QStringLiteral("whiteLabel"));
    m_newPassword->setFixedSize(120,35);
    m_newPassword->setFont(font);
    m_newPassword->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_newPasswordLineEdit->setFixedSize(140,45);
    m_newPasswordLineEdit->setFont(passwordFont);
    m_newPasswordLineEdit->setMaxLength(6);
    m_newPasswordLineEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordLineEdit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    m_newPasswordLineEdit->installEventFilter(m_umberKeyboard);

    m_newPasswordConfirm->setObjectName(QStringLiteral("whiteLabel"));
    m_newPasswordConfirm->setFixedSize(120,35);
    m_newPasswordConfirm->setFont(font);
    m_newPasswordConfirm->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_newPasswordConfirmLineEdit->setFixedSize(140,45);
    m_newPasswordConfirmLineEdit->setFont(passwordFont);
    m_newPasswordConfirmLineEdit->setMaxLength(6);
    m_newPasswordConfirmLineEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordConfirmLineEdit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_newPasswordConfirmLineEdit->installEventFilter(m_umberKeyboard);

    m_passwordChangeConfirmButton->setObjectName(QStringLiteral("brownButton"));
    m_passwordChangeConfirmButton->setFixedSize(140,48);
    m_passwordChangeConfirmButton->setFont(font);
    connect(m_passwordChangeConfirmButton,SIGNAL(clicked()),this,SLOT(SlotPasswordChangeConfirmButton()));

    m_newPasswordChangeCancelButton->setObjectName(QStringLiteral("brownButton"));
    m_newPasswordChangeCancelButton->setFixedSize(140,48);
    m_newPasswordChangeCancelButton->setFont(font);
    connect(m_newPasswordChangeCancelButton,SIGNAL(clicked()),this,SLOT(SlotnewPasswordChangeCancelButton()));


    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(m_userLabel,0,0);
    gridLayout->addWidget(m_userComboBox,0,1,1,2);
    gridLayout->addWidget(m_oldPassword,1,0);
    gridLayout->addWidget(m_newPassword,2,0);
    gridLayout->addWidget(m_newPasswordConfirm,3,0);
    gridLayout->addWidget(m_oldPasswordLineEdit,1,1,1,2);
    gridLayout->addWidget(m_newPasswordLineEdit,2,1,1,2);
    gridLayout->addWidget(m_newPasswordConfirmLineEdit,3,1,1,3);
    gridLayout->setHorizontalSpacing(20);
    gridLayout->setVerticalSpacing(20);

    QHBoxLayout *gridHLayout = new QHBoxLayout();
    gridHLayout->addStretch();
    gridHLayout->addLayout(gridLayout);
    gridHLayout->addStretch();
    gridHLayout->setSpacing(10);
    gridHLayout->setContentsMargins(0, 25, 40, 0);


    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_passwordChangeConfirmButton);
    bottomLayout->addWidget(m_newPasswordChangeCancelButton);
    bottomLayout->addStretch();
    bottomLayout->setSpacing(30);
    bottomLayout->setContentsMargins(0, 10, 0, 0);


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(gridHLayout);
    mainLayout->addLayout(bottomLayout);

    mainLayout->addStretch();
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(10, 0, 0, 20);

    this->setLayout(mainLayout);
    this->TranslateLanguage();

}

ChangePasswordDialog::~ChangePasswordDialog()
{
    if (m_umberKeyboard)
    {
        delete m_umberKeyboard;
        m_umberKeyboard = nullptr;
    }
}

void ChangePasswordDialog::TranslateLanguage()
{
    m_titleLabel->setText(tr("密码修改"));
    m_userLabel->setText(tr("选择用户"));
    m_oldPassword->setText(tr("旧 密 码"));
    m_newPassword->setText(tr("新 密 码"));
    m_newPasswordConfirm->setText(tr("确认密码"));
    m_newPasswordChangeCancelButton->setText(tr("取消"));
    m_passwordChangeConfirmButton->setText(tr("确认"));
}

void ChangePasswordDialog::SlotPasswordChangeConfirmButton()
{
    QString userName = m_userComboBox->currentText();
    QString oldPassword = m_oldPasswordLineEdit->text();
    QString newPassword = m_newPasswordLineEdit->text();
    QString newPasswordConfirm = m_newPasswordConfirmLineEdit->text();
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    if(ProLicenseManager::Instance()->GetProLicense())
    {
        UserRecord record = DataBaseManager::Instance()->GetUserTable()->GetUserRecord(userName);
        if((newPassword.isEmpty())||(newPasswordConfirm.isEmpty()))
        {
            MessageDialog msg(tr("请输入新密码\n"), this, MsgStyle::ONLYOK);
            msg.exec();
            return;
        }
        if(oldPassword != record.password)
        {
            MessageDialog msg(tr("旧密码输入错误，请重新输入\n"), this, MsgStyle::ONLYOK);
            msg.exec();
            return;
        }
        if(newPassword != newPasswordConfirm)
        {
            MessageDialog msg(tr("两次密码输入不一致，请重新输入\n"), this, MsgStyle::ONLYOK);
            msg.exec();
            return;
        }

        record.password = newPassword;
        DataBaseManager::Instance()->GetUserTable()->InsertData({record});
        MessageDialog msg(tr("密码修改完成"), this, MsgStyle::ONLYOK);
        msg.exec();
        accept();
    }
    else
    {
        if (userName == tr("普通用户"))
        {

            accept();
        }
        else if (userName == tr("运维员"))
        {
            String password;
            luaEngine->GetLuaValue(state, "config.authorization.maintenancePassword", password);
            if (0 == QString::compare(oldPassword,  QString(password.c_str()), Qt::CaseSensitive))
            {
                if((newPassword.isEmpty())||(newPasswordConfirm.isEmpty()))
                {
                     MessageDialog msg(tr("请输入新密码\n"), this);
                     msg.exec();
                }
                else
                {
                    if (newPassword == newPasswordConfirm )
                    {
                        luaEngine->SetLuaValue("config.authorization.maintenancePassword", newPassword.toStdString());
                        SettingManager::Instance()->AuthorizationSave();
                        accept();
                    }
                    else
                    {
                        MessageDialog msg(tr("新密码设置错误！\n"), this);
                        msg.exec();
                        m_newPasswordConfirmLineEdit->clear();
                        m_newPasswordLineEdit->clear();
                    }
                }

            }
            else
            {
                MessageDialog msg(tr("旧密码错误！\n"), this);
                msg.exec();
                m_oldPasswordLineEdit->clear();
            }
        }

        else if (userName == tr("管理员"))
        {
            String password;
            luaEngine->GetLuaValue(state, "config.authorization.administratorPassword", password);
            if (0 == QString::compare(oldPassword, QString(password.c_str()), Qt::CaseSensitive))
            {
                if((newPassword.isEmpty())||(newPasswordConfirm.isEmpty()))
                {
                     MessageDialog msg(tr("请输入新密码\n"), this);
                     msg.exec();
                }
                else
                {
                    if (newPassword == newPasswordConfirm )
                    {
                        luaEngine->SetLuaValue("config.authorization.administratorPassword", newPassword.toStdString());
                        SettingManager::Instance()->AuthorizationSave();
                        accept();
                    }
                    else
                    {
                        MessageDialog msg(tr("新密码设置错误！\n"), this);
                        msg.exec();
                        m_newPasswordConfirmLineEdit->clear();
                        m_newPasswordLineEdit->clear();
                    }
                }
            }
            else
            {
                MessageDialog msg(tr("旧密码错误！\n"), this);
                msg.exec();
                m_oldPasswordLineEdit->clear();
            }
        }

        else if (userName == tr("超级管理员"))
        {
            String password;
            luaEngine->GetLuaValue(state, "config.authorization.superPassword", password);
            if (0 == QString::compare(oldPassword, QString(password.c_str()), Qt::CaseSensitive))
            {
                if((newPassword.isEmpty())||(newPasswordConfirm.isEmpty()))
                {
                     MessageDialog msg(tr("请输入新密码\n"), this);
                     msg.exec();
                }
                else
                {
                    if (newPassword == newPasswordConfirm )
                    {
                        luaEngine->SetLuaValue("config.authorization.superPassword", newPassword.toStdString());
                        SettingManager::Instance()->AuthorizationSave();
                        accept();
                    }
                    else
                    {
                        MessageDialog msg(tr("新密码设置错误！\n"), this);
                        msg.exec();
                        m_newPasswordConfirmLineEdit->clear();
                        m_newPasswordLineEdit->clear();
                    }
                }
            }
            else
            {
                MessageDialog msg(tr("旧密码错误！\n"), this);
                msg.exec();
                m_oldPasswordLineEdit->clear();
            }
        }
    }
}

void ChangePasswordDialog::SlotnewPasswordChangeCancelButton()
{
     accept();
}

void ChangePasswordDialog::paintEvent(QPaintEvent *event)
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
            QRect(SHADOW_WIDTH, 70, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - height - SHADOW_WIDTH +50));
}

void ChangePasswordDialog::mousePressEvent(QMouseEvent *event)
{
    (void)event;

    if(m_totalClick < 10)
    {
        m_totalClick++;
    }
    if(m_totalClick >= 10 && LoginDialog::userType == RoleType::Super && m_superAppear == false && SettingManager::Instance()->IsProducerStatus())
    {
       m_superAppear = true;
       QStringList userStringList;
       userStringList << tr("超级管理员");
       m_userComboBox->insertItems(0, userStringList);
    }
}

}
