#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "InstrumentInformationWidget.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "Setting/SettingManager.h"
#include "Log.h"
#include "UI/Frame/NumberKeyboard.h"
#include "Communication/CommunicationException.h"
#include "Controller/ControllerManager.h"
#include "AlarmManager/AlarmManager.h"
#include "LuaEngine/LuaEngine.h"
#include "System/PlatformInfo.h"
#include <qrencode.h>
#include <QDebug>
#include "Interface/Wqimc/WqimcManager.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "UI/ControlConfig.h"

using namespace Configuration;
using namespace System;
using namespace Controller;
using namespace Communication;
using namespace Measure;
using namespace Lua;
using namespace Interface::Wqimc;

namespace UI
{

InstrumentInformationWidget::InstrumentInformationWidget(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this);

    QFont font;
    font.setPointSize(15);
    font.setWeight(60);

    QLabel *logoLabel = new QLabel();
    logoLabel->setFixedSize(160, 74);
    if(SettingManager::Instance()->GetManufactureType() == ManufactureType::LS)
    {
        logoLabel->setPixmap(QPixmap(":/img/logo"));
    }
    logoLabel->setScaledContents(true);
    QHBoxLayout *logoLabelLayout = new QHBoxLayout();
    logoLabelLayout->addWidget(logoLabel);

    QLabel *type = new QLabel();
    type->setText(tr("仪器型号") + ":");
    type->setFixedSize(105, 45);
    type->setFont(font);
    type->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    typeName = new QLabel();
    typeName->setFixedSize(400, 45);
    typeName->setFont(font);

    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->addWidget(type);
    typeLayout->addWidget(typeName);

    QLabel *name = new QLabel();
    name->setText(tr("仪器名称") + ":");
    name->setFixedSize(105, 45);
    name->setFont(font);
    name->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    nameValue = new QLabel();
    nameValue->setFixedSize(400, 45);
    nameValue->setFont(font);

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(name);
    nameLayout->addWidget(nameValue);

    QLabel *argument = new QLabel();
    argument->setText(tr("测定项目") + ":");
    argument->setFixedSize(105, 45);
    argument->setFont(font);
    argument->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    argumentName = new QLabel();
    argumentName->setFixedSize(400, 45);
    argumentName->setFont(font);

    QHBoxLayout *argumentLayout = new QHBoxLayout();
    argumentLayout->addWidget(argument);
    argumentLayout->addWidget(argumentName);

    QLabel *serialnumber = new QLabel();
    serialnumber->setText(tr("序列号") + ":");
    serialnumber->setFixedSize(105, 45);
    serialnumber->setFont(font);
    serialnumber->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    serialnumberName = new QLabel();
    serialnumberName->setFixedSize(400, 45);
    serialnumberName->setFont(font);

    QHBoxLayout *serialnumberLayout = new QHBoxLayout();
    serialnumberLayout->addWidget(serialnumber);
    serialnumberLayout->addWidget(serialnumberName);

    QLabel *typeNumber = new QLabel();
    typeNumber->setText(tr("仪器型码") + ":");
    typeNumber->setFixedSize(105, 45);
    typeNumber->setFont(font);
    typeNumber->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    typeNumberName = new QLabel();
    typeNumberName->setFixedSize(400, 45);
    typeNumberName->setFont(font);

    QHBoxLayout *typeNumberLayout = new QHBoxLayout();
    typeNumberLayout->addWidget(typeNumber);
    typeNumberLayout->addWidget(typeNumberName);

    QLabel *version = new QLabel();
    version->setText(tr("仪器版本") + ":");
    version->setFixedSize(105, 45);
    version->setFont(font);
    version->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    versionName = new QLabel();
    versionName->setFixedSize(400, 45);
    versionName->setFont(font);

    QHBoxLayout *versionLayout = new QHBoxLayout();
    versionLayout->addWidget(version);
    versionLayout->addWidget(versionName);

    QLabel *platform = new QLabel();
    platform->setText(tr("平台版本") + ":");
    platform->setFixedSize(105, 45);
    platform->setFont(font);
    platform->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    platformName = new QLabel();
    platformName->setFixedSize(400, 45);
    platformName->setFont(font);

    QHBoxLayout *platformLayout = new QHBoxLayout();
    platformLayout->addWidget(platform);
    platformLayout->addWidget(platformName);

    QLabel *firm = new QLabel();
    firm->setText(tr("生产厂商") + ":");
    firm->setFixedSize(105, 45);
    firm->setFont(font);
    firm->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    firmName = new QLabel();
    firmName->setFixedSize(400, 45);
    firmName->setFont(font);

    QHBoxLayout *firmLayout = new QHBoxLayout();
    firmLayout->addWidget(firm);
    firmLayout->addWidget(firmName);

    QFont wordFont;
    wordFont.setPointSize(12);
    QLabel *wordLabel = new QLabel();
    wordLabel->setAlignment(Qt::AlignCenter);
    wordLabel->setText(tr("[微信扫码  轻松售后]"));
    //wordLabel->setStyleSheet("color:#191970;");
    wordLabel->setFixedSize(180, 30);
    wordLabel->setFont(wordFont);

    qrencodeLabel = new QLabel();
    qrencodeLabel->setFixedSize(180,180);

    m_setSNButton = new QPushButton();
    m_setSNButton->setObjectName("brownButton");
    m_setSNButton->setText(tr("修改序列号"));
    QFont SNButtonFont = m_setSNButton->font();
    SNButtonFont.setPointSize(12);
    m_setSNButton->setFont(SNButtonFont);
    m_setSNButton->setFixedSize(100, 40);
    m_setSNButton->setContentsMargins(0, 0, 20, 0);

    m_blankLabel = new QLabel();
    m_blankLabel->setFixedSize(100, 40);
    m_blankLabel->setContentsMargins(0, 0, 20, 0);

    QHBoxLayout *SNButtonLayout = new QHBoxLayout();
    SNButtonLayout->addStretch();
    SNButtonLayout->addWidget(m_blankLabel);
    SNButtonLayout->addWidget(m_setSNButton);
    SNButtonLayout->addStretch();

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addStretch();
    if(SettingManager::Instance()->GetManufactureType() != ManufactureType::OEM)
    {
        leftLayout->addLayout(typeLayout);
    }
    leftLayout->addLayout(nameLayout);
    leftLayout->addLayout(argumentLayout);
    leftLayout->addLayout(typeNumberLayout);
    leftLayout->addLayout(serialnumberLayout);
    leftLayout->addLayout(versionLayout);
    leftLayout->addLayout(platformLayout);
    if(SettingManager::Instance()->GetManufactureType() != ManufactureType::OEM)
    {
        leftLayout->addLayout(firmLayout);
    }
    leftLayout->addStretch();
    leftLayout->setContentsMargins(10, 0, 0, 0);

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addLayout(logoLabelLayout);
    rightLayout->addStretch();
    rightLayout->addWidget(wordLabel);
    rightLayout->addWidget(qrencodeLabel);
    rightLayout->addStretch();
    rightLayout->addLayout(SNButtonLayout);

    if(SettingManager::Instance()->GetManufactureType() != ManufactureType::LS)
    {
        bool useQrencode = true;
        luaEngine->GetLuaValue(state, "config.system.useQrencode", useQrencode);
        if(useQrencode == false)
        {
            wordLabel->setVisible(false);
            qrencodeLabel->setVisible(false);
        }
    }

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    mainLayout->addStretch();
    this->setLayout(mainLayout);
    this->InfoDisplay();
    this->ChangeBottomStatus();
    this->UpdateQrencode();

    m_setSNDialog = new SetSNDialog(this);
    m_setSNDialog->setVisible(false);

    connect(m_setSNButton, SIGNAL(clicked(bool)), this, SLOT(SlotSetSNButton()));
}

void InstrumentInformationWidget::showEvent(QShowEvent *event)
{
    (void)event;
    this->InfoDisplay();
}

void InstrumentInformationWidget::InfoDisplay()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    bool bridgeMode;
    luaEngine->GetLuaValue(state, "config.system.bridgeMode", bridgeMode);
    if (bridgeMode)
    {
        MainController * MC = ControllerManager::Instance()->MC;
        try
        {
            typeName->setText(MC->IInstrumentInfo->GetModel().c_str());
            serialnumberName->setText(MC->IInstrumentInfo->GetSn().c_str());

            String paramType = SettingManager::Instance()->GetParamTypeName();
            if(!paramType.empty())
            {
                typeNumberName->setText((MC->IInstrumentInfo->GetType() + " (" + paramType + ")").c_str());
            }
            else
            {
                typeNumberName->setText(MC->IInstrumentInfo->GetType().c_str());
            }

            firmName->setText(QObject::tr(MC->IInstrumentInfo->GetManuFacturer().c_str()));
            versionName->setText(MC->IInstrumentInfo->GetSoftwareVersion().ToString().c_str());
            platformName->setText(MC->IInstrumentInfo->GetPlatFormVersion().ToString().c_str());
        }
        catch(CommandTimeoutException e)
        {
            e.m_name = "主控板";
            AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
        }
        catch (std::exception e)
        {

            AlarmManager::Instance()->AddExceptionAlarm(e);
        }
    }
    else
    {

        String showModel;
        luaEngine->GetLuaValue(state, "config.info.instrument.showModel", showModel);
        if(SettingManager::Instance()->GetManufactureType() != ManufactureType::OEM)
        {
            typeName->setText(showModel.c_str());
        }

        String sn;
        luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);
        serialnumberName->setText(sn.c_str());

        String type;
        luaEngine->GetLuaValue(state, "config.info.instrument.type", type);

        String paramType = SettingManager::Instance()->GetParamTypeName();
        if(!paramType.empty())
        {
            typeNumberName->setText((type + " ("+ paramType + ")").c_str());
        }
        else
        {
            typeNumberName->setText(type.c_str());
        }

        String manuFacturer;
        luaEngine->GetLuaValue(state, "config.info.instrument.manuFacturer", manuFacturer);
        firmName->setText(QObject::tr(manuFacturer.c_str()));

        String systemVersion;
        luaEngine->GetLuaValue(state, "setting.version.system", systemVersion);

        if(SettingManager::Instance()->GetManufactureType() == ManufactureType::OEM)
        {
            systemVersion += " BL";
        }
        else if(SettingManager::Instance()->GetManufactureType() == ManufactureType::DIY)
        {
            systemVersion += " CP";     //Custom Products
        }

        versionName->setText(systemVersion.c_str());

        platformName->setText(System::PlatformVersion.c_str());
    }
    String instrumentInfoName;
    luaEngine->GetLuaValue(state, "config.info.instrument.showName", instrumentInfoName);
    nameValue->setText(QObject::tr(instrumentInfoName.c_str()));

    argumentName->setText(QObject::tr(SettingManager::Instance()->GetValidInstrumentInfoArgus().c_str()));
}

void InstrumentInformationWidget::UpdateQrencode()
{
    QString urlStr = "https://www.watertestcloud.com";

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String type;
    luaEngine->GetLuaValue(state, "config.info.instrument.type", type);
    QString typeStr;
    typeStr = "t=" + QString::fromStdString(type);

    String sn;
    luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);
    QString snStr;
    snStr = "s=" + QString::fromStdString(sn);

    String systemVersion;
    luaEngine->GetLuaValue(state, "setting.version.system", systemVersion);
    QString verStr;
    verStr = "v=" + QString::fromStdString(systemVersion);
    if(SettingManager::Instance()->GetManufactureType() == ManufactureType::OEM)
    {
        verStr = verStr + "_BL";
    }
    else if(SettingManager::Instance()->GetManufactureType() == ManufactureType::DIY)
    {
        verStr = verStr + "_CP";
    }

    String modbus;
    try
    {
        OOLUA::get_global(state, "modbusStr", modbus);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("OOLUA::get_global modbusStr fail. OOLUA::Exception ==> %s.", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("OOLUA::get_global modbusStr fail. std::exception ==> %s.", e.what());
    }
    QString modbusStr;
    modbusStr = "m=" + QString::fromStdString(modbus);

    QStringList strList;
    strList.append(typeStr);
    strList.append(snStr);
    strList.append(verStr);
    strList.append(modbusStr);
    QString paramStr = strList.join('&');

    QString tempStr = urlStr + "?" + paramStr;
    //qDebug()<<"qrencode text = "<<tempStr;

    QRcode *qrcode; //二维码数据
    qrcode = QRcode_encodeString(tempStr.toStdString().c_str(), 2, QR_ECLEVEL_Q, QR_MODE_8, 1);

    qint32 tempWidth = qrencodeLabel->width(); //二维码图片的大小
    qint32 tempHeight = qrencodeLabel->height();
    qint32 qrcodeWidth = qrcode->width > 0 ? qrcode->width : 1;

    double scaleX = (double)tempWidth / (double)qrcodeWidth; //二维码图片的缩放比例
    double scaleY = (double) tempHeight / (double) qrcodeWidth;

    QImage img = QImage(tempWidth, tempHeight, QImage::Format_ARGB32);

    QPainter painter(&img);

    QColor background(Qt::white);
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, tempWidth, tempHeight);

    QColor foreground(Qt::black);
    painter.setBrush(foreground);

    for( qint32 y = 0; y < qrcodeWidth; y ++)
    {
        for(qint32 x = 0; x < qrcodeWidth; x++)
        {
            unsigned char b = qrcode->data[y * qrcodeWidth + x];
            if(b & 0x01)
            {
                QRectF r(x * scaleX, y * scaleY, scaleX, scaleY);
                painter.drawRects(&r, 1);
            }
        }
    }

    QPixmap pixmap = QPixmap::fromImage(img);
    qrencodeLabel->setPixmap(pixmap);
    //qrencodeLabel->setVisible(true);
}

void InstrumentInformationWidget::SlotSetSNButton()
{
    m_setSNDialog->exec();
}

void InstrumentInformationWidget::OnUserChange()
{
    this->ChangeBottomStatus();
}

void InstrumentInformationWidget::OnUpdateWidget(UpdateEvent event, System::String message)
{
    (void)message;
    if(event == UpdateEvent::SystemInfoChanged)
    {
        this->InfoDisplay();
        this->UpdateQrencode();
        WqimcManager::Instance()->ChangeSN();
    }
}

void InstrumentInformationWidget::ChangeBottomStatus()
{
    if (LoginDialog::userType == Super)
    {
        m_setSNButton->show();
        m_blankLabel->hide();
    }
    else if (LoginDialog::userType == Administrator)
    {
        m_setSNButton->hide();
        m_blankLabel->show();
    }
    else if (LoginDialog::userType == Maintain)
    {
        m_setSNButton->hide();
        m_blankLabel->show();
    }
    else if (LoginDialog::userType == General)
    {
        m_setSNButton->hide();
        m_blankLabel->show();
    }
}

InstrumentInformationWidget::~InstrumentInformationWidget()
{
    if (m_setSNDialog)
    {
        delete m_setSNDialog;
        m_setSNDialog = nullptr;
    }
}
}
