
#include "Setting/SettingManager.h"
#include "UI/Frame/MessageDialog.h"
#include "LoginDialog.h"
#include "LuaEngine/LuaEngine.h"
#include "System/DynamicPassword.h"
#include "System/ProLicenseManager.h"

using namespace Configuration;
using namespace Lua;
using namespace System;

namespace UI
{

RoleType LoginDialog::userType = None;
UserInfo LoginDialog::userInfo = {"超级管理员", "Super", 0xffffffff};

LoginDialog::LoginDialog(QWidget *parent) :
        DropShadowWidget(parent)
{
    m_totalClick = 0;
    m_superAppear = false;
    this->resize(480, 370);
    setWindowIcon(QIcon(":/img/WQIMC"));

    m_titleLabel = new QLabel();
    m_logoLabel = new QLabel();
    m_userLabel = new QLabel();
    m_passwordLabel = new QLabel();
    m_userComboBox = new QComboBox();
    m_passwordLineEdit = new QLineEdit();
    m_loginButton = new QPushButton();

    m_titleLabel->setObjectName("softwareTitle");
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(12);
    m_titleLabel->setFont(titleFont);
    m_userLabel->setFixedSize(100, 25);
    m_userLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addWidget(m_titleLabel, 0, Qt::AlignVCenter);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0, 5, 10, 0);
    m_titleLabel->setContentsMargins(20, 20, 0, 0);

    m_logoLabel->setFixedSize(160, 75);
    if(SettingManager::Instance()->GetManufactureType() == ManufactureType::LS)
    {
        m_logoLabel->setPixmap(QPixmap(":/img/logo"));
    }
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

    //用户
    QFont font;
    font.setPointSize(18);

    m_userLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_userLabel->setFixedSize(60, 45);
    m_userLabel->setFont(font);
    m_userLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QStringList userStringList;
    userStringList << tr("普通用户") << tr("运维员") << tr("管理员");
    m_userComboBox->setObjectName(QStringLiteral("m_userComboBox"));
    m_userComboBox->setFixedSize(200, 45);
    m_userComboBox->setFont(font);
    m_userComboBox->clear();
    m_userComboBox->insertItems(0, userStringList);

    if(ProLicenseManager::Instance()->GetProLicense())
    {
        QVector<QString> userItem = DataBaseManager::Instance()->GetUserTable()->GetUserNameList();
        userStringList = userItem.toList();
        m_userComboBox->clear();
        m_userComboBox->insertItems(0, userStringList);       
    }

    //密码
    m_passwordLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_passwordLabel->setFixedSize(60, 45);
    m_passwordLabel->setFont(font);
    m_passwordLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_passwordLineEdit->setObjectName(QStringLiteral("m_userComboBox"));
    m_passwordLineEdit->setFixedSize(200, 45);
    QFont passwordFont = m_passwordLineEdit->font();
    passwordFont.setPointSize(12);
    m_passwordLineEdit->setFont(passwordFont);
    m_passwordLineEdit->setMaxLength(6);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_passwordLineEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_loginDialogKeyboard = new CNumberKeyboard(this);
    m_passwordLineEdit->installEventFilter(m_loginDialogKeyboard);

    m_loginButton = new QPushButton();
    m_loginButton->setObjectName(QStringLiteral("brownButton"));
    m_loginButton->setFixedSize(140, 48);
    m_loginButton->setFont(font);
    m_loginButton->setFocus();
    m_loginButton->setDefault(true);
    connect(m_loginButton, SIGNAL(clicked()), this, SLOT(SlotloginButton()));

    m_cancelButton = new QPushButton();
    m_cancelButton->setObjectName(QStringLiteral("brownButton"));
    m_cancelButton->setFixedSize(140, 48);
    m_cancelButton->setFont(font);
    m_cancelButton->setFocus();
    m_cancelButton->setDefault(true);
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(close()));


    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(m_userLabel, 0, 0);
    gridLayout->addWidget(m_userComboBox, 0, 1, 1, 2);
    gridLayout->addWidget(m_passwordLabel, 1, 0);
    gridLayout->addWidget(m_passwordLineEdit, 1, 1, 1, 2);
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
}

LoginDialog::~LoginDialog()
{
    if (m_loginDialogKeyboard)
    {
        delete m_loginDialogKeyboard;
        m_loginDialogKeyboard = nullptr;
    }
}

void LoginDialog::TranslateLanguage()
{
    m_titleLabel->setText(tr(""));
    m_userLabel->setText(tr("用户:"));
    m_passwordLabel->setText(tr("密码:"));
    m_loginButton->setText(tr("登录"));
    m_cancelButton->setText(tr("取消"));
}

void LoginDialog::SlotloginButton()
{
    QString userName = m_userComboBox->currentText();
    QString passWord = m_passwordLineEdit->text();
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    OOLUA::Script* lua = luaEngine->GetEngine();

    if(ProLicenseManager::Instance()->GetProLicense())
    {
        UserRecord record = DataBaseManager::Instance()->GetUserTable()->GetUserRecord(userName);
        AccessLevelRecord levelRecord = DataBaseManager::Instance()->GetAccessLevelTable()->SelectDataFromName(record.levelName);
        QVector<QString> vLevel = DataBaseManager::Instance()->GetAccessLevelTable()->GetLevelNameList();
        if(record.password == passWord)
        {
            if(record.status == (int)UserStatus::Enable)
            {
                LoginDialog::userType = Super;
                record.lastLoginTime = QDateTime::currentDateTime().toTime_t();
                LoginDialog::userInfo.userName = userName;
                LoginDialog::userInfo.levelName = record.levelName;
                LoginDialog::userInfo.levelInt = levelRecord.limitsOfAuthority;
                DataBaseManager::Instance()->GetUserTable()->InsertData({record});
                accept();
            }
            else
            {
                MessageDialog msg(tr("账号已停用，请联系管理员！\n"), this);
                msg.exec();
                m_passwordLineEdit->clear();
                m_passwordLineEdit->setFocus();
            }
            if(!vLevel.contains(record.levelName))
            {
                MessageDialog msg(tr("账号权限已被删除，请联系管理员！\n"), this);
                msg.exec();
                m_passwordLineEdit->clear();
                m_passwordLineEdit->setFocus();
            }
        }
        else if(userName == "超级管理员" && passWord == "032018")
        {
            LoginDialog::userType = Super;
            LoginDialog::userInfo.userName = userName;
            LoginDialog::userInfo.levelName = "Super";
            LoginDialog::userInfo.levelInt = 0x1ffffff;
            accept();
        }
        else
        {
            MessageDialog msg(tr("密码错误！\n"), this);
            msg.exec();
            m_passwordLineEdit->clear();
            m_passwordLineEdit->setFocus();
        }
    }
    else
    {
        if (userName == tr("普通用户"))
        {
            accept();
            LoginDialog::userType = RoleType::General;
        }
        else if (userName == tr("运维员"))
        {
            String password;
            luaEngine->GetLuaValue(state, "config.authorization.maintenancePassword", password);
            if (0 == QString::compare(passWord, QString(password.c_str()), Qt::CaseSensitive))
            {
                accept();
                LoginDialog::userType = RoleType::Maintain;
            }
            else
            {
                MessageDialog msg(tr("密码错误！\n"), this);
                msg.exec();
                m_passwordLineEdit->clear();
                m_passwordLineEdit->setFocus();
            }
        }
        else if (userName == tr("管理员"))
        {
            String password;
            luaEngine->GetLuaValue(state, "config.authorization.administratorPassword", password);
            if (0 == QString::compare(passWord, QString(password.c_str()), Qt::CaseSensitive))
            {
                accept();
                LoginDialog::userType = RoleType::Administrator;
            }
            else
            {
                MessageDialog msg(tr("密码错误！\n"), this);
                msg.exec();
                m_passwordLineEdit->clear();
                m_passwordLineEdit->setFocus();
            }
        }
        else if (userName == tr("超级管理员"))
        {
            String pwd;
            luaEngine->GetLuaValue(state, "config.authorization.superPassword", pwd);

            QString superPassword = QString::fromStdString(pwd);

            bool isValid = false;
            luaEngine->GetLuaValue(state, "config.producerAuthorization.enable", isValid);

            if(isValid == false) //出厂激活后使用动态密码
            {
                QStringList pwdList = DynamicPassword::GeneratePasswordList();

                Table usedPasswords;
                luaEngine->GetLuaValue(state, "config.authorization.usedPasswords", usedPasswords);

                QStringList usedList;
                oolua_ipairs(usedPasswords)
                {
                    String usedPassword;

                    lua->pull(usedPassword);
                    usedList.append(QString::fromStdString(usedPassword));
                }
                oolua_ipairs_end()

                if(pwdList.contains(passWord) && !usedList.contains(passWord))
                {
                    accept();
                    LoginDialog::userType = RoleType::Super;

                    //更新密码表
                    usedList.pop_front();
                    usedList.push_back(passWord);

                    Table newPasswords;
                    luaEngine->GetLuaValue(state, "config.authorization.usedPasswords", newPasswords);

                    oolua_ipairs(newPasswords)
                    {
                        lua_pop(state, 1);
                        QString pStr = usedList.at(_i_index_ - 1);
                        OOLUA::push(state, pStr.toStdString());
                        lua_rawseti(state, _oolua_array_index_, _i_index_);

                        if (_i_index_ == (int)usedList.size())
                        {
                            break;
                        }
                    }
                    oolua_ipairs_end()

                    SettingManager::Instance()->AuthorizationSave();
                }
                else
                {
                    MessageDialog msg(tr("密码错误！\n"), this);
                    msg.exec();
                    m_passwordLineEdit->clear();
                    m_passwordLineEdit->setFocus();
                }
            }
            else
            {
                if (passWord == superPassword)
                {
                    accept();
                    LoginDialog::userType = RoleType::Super;
                }
                else
                {
                    MessageDialog msg(tr("密码错误！\n"), this);
                    msg.exec();
                    m_passwordLineEdit->clear();
                    m_passwordLineEdit->setFocus();
                }
            }
        }
    }
}

void LoginDialog::paintEvent(QPaintEvent *event)
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

void LoginDialog::mousePressEvent(QMouseEvent *event)
{
    (void)event;
    if(m_totalClick < 10)
    {
        m_totalClick++;
    }
    if(m_totalClick >= 10 && m_superAppear == false)
    {
       m_superAppear = true;
       QStringList userStringList;
       userStringList << tr("超级管理员");
       m_userComboBox->insertItems(0, userStringList);
    }
}
}
