#include <QGridLayout>
#include "ProLicenseDialog.h"
#include "UI/Frame/Common.h"
#include "System/Types.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/MessageDialog.h"
#include "System/DynamicPassword.h"
#include "Setting/SettingManager.h"
#include "System/ProLicenseManager.h"

using namespace Lua;
using namespace System;
using namespace Configuration;

namespace UI
{

ProLicenseDialog::ProLicenseDialog(QWidget *parent) :
        DropShadowWidget(parent), m_dialogKeyboard(nullptr)
{
    m_superAppear = false;
    this->resize(380, 270);
    setWindowIcon(QIcon(":/img/WQIMC"));

    m_titleLabel = new QLabel();
    m_logoLabel = new QLabel();
    m_codeLabel = new QLabel();
    m_codeEdit = new QLineEdit();

    m_titleLabel->setObjectName("softwareTitle");
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(18);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    m_titleLabel->setContentsMargins(10, 20, 0, 0);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addWidget(m_titleLabel, 0, Qt::AlignTop | Qt::AlignLeft);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0, 5, 10, 0);

    m_logoLabel->setFixedSize(150, 75);
    m_logoLabel->setPixmap(QPixmap(":/img/logo"));
    m_logoLabel->setScaledContents(true);

    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->addStretch();
    logoLayout->addWidget(m_logoLabel);
    logoLayout->setSpacing(10);
    logoLayout->setContentsMargins(0, 20, 15, 0);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addLayout(titleLayout);
    topLayout->addStretch();
    topLayout->addLayout(logoLayout);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QFont font;
    font.setPointSize(16);

    // 开关
    m_codeLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_codeLabel->setFixedSize(100, 30);
    m_codeLabel->setFont(font);
    m_codeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_codeEdit->setObjectName(QStringLiteral("authorizationComboBox"));
    m_codeEdit->setFixedSize(140, 30);
    QFont passwordFont = m_codeEdit->font();
    passwordFont.setPointSize(12);
    m_codeEdit->setFont(passwordFont);
    m_codeEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_dialogKeyboard = new CNumberKeyboard(this);
    m_codeEdit->installEventFilter(m_dialogKeyboard);

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
    gridLayout->addWidget(m_codeLabel, 0, 0);
    gridLayout->addWidget(m_codeEdit, 0, 1, 1, 2);
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

    this->ShowContent();
}

ProLicenseDialog::~ProLicenseDialog()
{
    if (m_dialogKeyboard)
    {
        delete m_dialogKeyboard;
        m_dialogKeyboard = nullptr;
    }
}

void ProLicenseDialog::TranslateLanguage()
{
    m_titleLabel->setText(tr("Pro版本激活"));
    m_codeLabel->setText(tr("激活码:"));
    m_loginButton->setText(tr("确定"));
    m_cancelButton->setText(tr("取消"));
}

void ProLicenseDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    this->ShowContent();
}

void ProLicenseDialog::SlotloginButton()
{
    QString passWord = m_codeEdit->text();
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    OOLUA::Script* lua = luaEngine->GetEngine();

//    ProLicenseManager::Instance()->SetProLicense(true);
//    if(ProLicenseManager::Instance()->GetProLicense())
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

            MessageDialog msg(tr("\r确认激活Pro版本？\n\r激活后将进行用户管理及权限管理，并且关键操作需进行电子签名！"), this, MsgStyle::OKANDCANCEL, true);
            if(msg.exec()== QDialog::Rejected)
            {
                return ;
            }

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

            ProLicenseManager::Instance()->SetProLicense(true);

            SettingManager::Instance()->AuthorizationSave();
            this->close();
        }
        else
        {
            MessageDialog msg(tr("激活码错误！\n"), this);
            msg.exec();
            m_codeEdit->clear();
            m_codeEdit->setFocus();
        }
    }
}

void ProLicenseDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    int height = 100;
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

void ProLicenseDialog::ShowContent()
{
    m_codeEdit->clear();
    m_codeEdit->setFocus();
}

}
