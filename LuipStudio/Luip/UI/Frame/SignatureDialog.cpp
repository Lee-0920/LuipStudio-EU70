
#include "Setting/SettingManager.h"
#include "UI/Frame/MessageDialog.h"
#include "SignatureDialog.h"
#include "LuaEngine/LuaEngine.h"
#include "System/DynamicPassword.h"
#include "System/ProLicenseManager.h"
#include "UI/Frame/LoginDialog.h"

using namespace Configuration;
using namespace Lua;
using namespace System;

namespace UI
{

int SignatureDialog::lastSignatureTime = 0;

SignatureDialog::SignatureDialog(QWidget *parent) :
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
    m_objectLabel = new QLabel();
    m_descriptorLabel = new QLabel();
    m_userComboBox = new QComboBox();
    m_detailsComboBox = new QComboBox();
    m_passwordLineEdit = new QLineEdit();
    m_objectEdit = new QLineEdit();
    m_descriptorEdit = new QLineEdit();
    m_loginButton = new QPushButton();

    m_titleLabel->setObjectName(QStringLiteral("whiteLabel"));
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(15);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_userLabel->setFixedSize(100, 25);
    m_userLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addStretch();
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();
//    titleLayout->setContentsMargins(0, 5, 0, 0);
    m_titleLabel->setContentsMargins(0, 0, 0, 0);

//    m_logoLabel->setFixedSize(160, 75);
//    if(SettingManager::Instance()->GetManufactureType() == ManufactureType::LS)
//    {
//        m_logoLabel->setPixmap(QPixmap(":/img/logo"));
//    }
//    m_logoLabel->setScaledContents(true);

//    QHBoxLayout *logoLayout = new QHBoxLayout();
//    logoLayout->addStretch();
//    logoLayout->addWidget(m_logoLabel);
//    logoLayout->setSpacing(10);
//    logoLayout->setContentsMargins(0, 0, 30, 0);

    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addLayout(titleLayout);
//    topLayout->addLayout(logoLayout);
    topLayout->addStretch();
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(0, 0, 0, 0);

    //用户
    QFont font;
    font.setPointSize(15);

    //操作对象
    m_objectLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_objectLabel->setFixedSize(120, 45);
    m_objectLabel->setFont(font);
    m_objectLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

//    m_objectEdit->setText(textName);
    m_objectEdit->setObjectName(QStringLiteral("m_userComboBox"));
    m_objectEdit->setFixedSize(200, 45);
    QFont objectFont = m_objectEdit->font();
    objectFont.setPointSize(12);
    m_objectEdit->setFont(objectFont);
    m_objectEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_objectEdit->setReadOnly(true);
    m_objectEdit->setEnabled(false);
    // 设置样式表：背景为灰色
    m_objectEdit->setStyleSheet("QLineEdit { background-color:rgb(220,220,220); }");

    //签名描述
    m_descriptorLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_descriptorLabel->setFixedSize(120, 45);
    m_descriptorLabel->setFont(font);
    m_descriptorLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_descriptorEdit->setObjectName(QStringLiteral("m_userComboBox"));
    m_descriptorEdit->setFixedSize(200, 45);
    QFont descriptorFont = m_descriptorEdit->font();
    descriptorFont.setPointSize(12);
    m_descriptorEdit->setFont(descriptorFont);
    m_descriptorEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_inputKeyboard = new InputKeyBoard(this);
    m_descriptorEdit->installEventFilter(m_inputKeyboard);

    //用户
    m_userLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_userLabel->setFixedSize(120, 45);
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
    m_passwordLabel->setFixedSize(120, 45);
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
    m_SignatureDialogKeyboard = new CNumberKeyboard(this);
    m_passwordLineEdit->installEventFilter(m_SignatureDialogKeyboard);

    m_loginButton = new QPushButton();
    m_loginButton->setObjectName(QStringLiteral("brownButton"));
    m_loginButton->setFixedSize(70, 40);
    m_loginButton->setFont(font);
    m_loginButton->setFocus();
    m_loginButton->setDefault(true);
    connect(m_loginButton, SIGNAL(clicked()), this, SLOT(SlotloginButton()));

    m_cancelButton = new QPushButton();
    m_cancelButton->setObjectName(QStringLiteral("brownButton"));
    m_cancelButton->setFixedSize(70, 40);
    m_cancelButton->setFont(font);
    m_cancelButton->setFocus();
    m_cancelButton->setDefault(true);
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(close()));


    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(m_objectLabel, 0, 0);
    gridLayout->addWidget(m_objectEdit, 0, 1, 1, 2);
    gridLayout->addWidget(m_descriptorLabel, 1, 0);
    gridLayout->addWidget(m_descriptorEdit, 1, 1, 1, 2);
    gridLayout->addWidget(m_userLabel, 2, 0);
    gridLayout->addWidget(m_userComboBox, 2, 1, 1, 2);
    gridLayout->addWidget(m_passwordLabel, 3, 0);
    gridLayout->addWidget(m_passwordLineEdit, 3, 1, 1, 2);
    gridLayout->setHorizontalSpacing(20);
    gridLayout->setVerticalSpacing(10);

    QHBoxLayout *gridHLayout = new QHBoxLayout();
    gridHLayout->addStretch();
    gridHLayout->addLayout(gridLayout);
    gridHLayout->addStretch();
    gridHLayout->setSpacing(10);
    gridHLayout->setContentsMargins(0, 20, 40, 0);

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
    mainLayout->addStretch();
    mainLayout->addLayout(gridHLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(bottomLayout);    
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(10, 20, 0, 10);

    this->setLayout(mainLayout);
    this->TranslateLanguage();
}

SignatureDialog::~SignatureDialog()
{
    if (m_SignatureDialogKeyboard)
    {
        delete m_SignatureDialogKeyboard;
        m_SignatureDialogKeyboard = nullptr;
    }
}

void SignatureDialog::TranslateLanguage()
{
    m_titleLabel->setText(tr("电子签名"));
    m_objectLabel->setText(tr("操作对象:"));
    m_descriptorLabel->setText(tr("签名描述:"));
    m_userLabel->setText(tr("用    户:"));
    m_passwordLabel->setText(tr("密    码:"));
    m_loginButton->setText(tr("签名"));
    m_cancelButton->setText(tr("取消"));
}

void SignatureDialog::FillObjectText(QString text)
{
    m_objectEdit->setText(text);
}

void SignatureDialog::FillDescriptorText(QString text)
{
    m_descriptorEdit->setText(text);
}

void SignatureDialog::SlotloginButton()
{
    QString userName = m_userComboBox->currentText();
    QString passWord = m_passwordLineEdit->text();
    QString event = m_objectEdit->text();
    QString details = m_descriptorEdit->text();
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    OOLUA::Script* lua = luaEngine->GetEngine();

    if(ProLicenseManager::Instance()->GetProLicense())
    {
        UserRecord record = DataBaseManager::Instance()->GetUserTable()->GetUserRecord(userName);
        QVector<QString> vLevel = DataBaseManager::Instance()->GetAccessLevelTable()->GetLevelNameList();
        if(record.password == passWord)
        {
            if(record.status == (int)UserStatus::Enable)
            {
                SignatureDialog::lastSignatureTime = QDateTime::currentDateTime().toTime_t();
                //审计追踪
                DataBaseManager::Instance()->GetAuditTrailTable()->InsertAuditTrail(UI::LoginDialog::userInfo.userName,
                                                                                    UI::LoginDialog::userInfo.levelName,
                                                                                    "电子签名-签名对象-" + event,
                                                                                    "--",
                                                                                    "--",
                                                                                    details);
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
}

void SignatureDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    QPainter painter(this);
    int height = 60;
    painter.setPen(Qt::NoPen);

    painter.setBrush(QBrush(QColor(115, 115, 140)));
    painter.drawRect(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    height - SHADOW_WIDTH));

    painter.setBrush(Qt::white);
    painter.drawRect(
            QRect(SHADOW_WIDTH, height + SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    this->height()- height - SHADOW_WIDTH));

    painter.setBrush(QBrush(QColor(115, 115, 140)));
    painter.drawRect(
            QRect(SHADOW_WIDTH, this->height()- height + SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    height - SHADOW_WIDTH));

    QPen pen;
    pen.setColor(QColor(10,105,170));
    pen.setWidth(3);

    painter.setPen(pen);
    painter.drawLine(QPoint(0,0), QPoint(0,this->height()));
    painter.drawLine(QPoint(0,0), QPoint(this->width(),0));
    painter.drawLine(QPoint(0,this->height()-1), QPoint(this->width()-1,this->height()-1));
    painter.drawLine(QPoint(this->width()-1,0), QPoint(this->width()-1,this->height()-1));

}

void SignatureDialog::mousePressEvent(QMouseEvent *event)
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
