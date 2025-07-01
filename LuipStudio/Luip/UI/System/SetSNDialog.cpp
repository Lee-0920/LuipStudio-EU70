#include "Log.h"
#include "SetSNDialog.h"
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
#include "UI/ControlConfig.h"
#include "Lua/App.h"
#include<iostream>
#include<fstream>

#ifdef  _CS_ARM_LINUX
#include <stdio.h>
#include <stdlib.h>
#endif

using namespace Configuration;
using namespace System;
using namespace Lua;
using namespace OOLUA;
using namespace std;

namespace UI
{

SetSNDialog::SetSNDialog(QWidget *parent) : QDialog(parent),m_typeChangesCount(0),isShowTypeChanges(true)
{
    this->setFixedSize(400, COMMON_BUTTONS_COEFFICIENT_VERTICAL(460));
    this->setWindowModality(Qt::WindowModal);

    #ifdef _CS_X86_WINDOWS
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);
    #endif

    QFont font;
    font.setPointSize(FONT_SIZE_SETTINGS(12));

    m_manufactureTypeLabel = new QLabel();
    m_manufactureTypeLabel->setObjectName("dialogLabel");
    m_manufactureTypeLabel->setText(tr("制造类型") + ":");
    m_manufactureTypeLabel->setFixedSize(100, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_manufactureTypeLabel->setFont(font);
    m_manufactureTypeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QStringList list;
    list<<tr("标准版")<<tr("空白版")<<tr("定制版");

    m_manufactureTypeBox = new QComboBox();
    m_manufactureTypeBox->addItems(list);
    m_manufactureTypeBox->setFixedSize(220, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_manufactureTypeBox->setFont(font);

    QHBoxLayout *manufactureTypeLayout = new QHBoxLayout();
    manufactureTypeLayout->addStretch();
    manufactureTypeLayout->addWidget(m_manufactureTypeLabel);
    manufactureTypeLayout->addWidget(m_manufactureTypeBox);
    manufactureTypeLayout->addStretch();

    m_modelLabel = new QLabel();
    m_modelLabel->setObjectName("dialogLabel");
    m_modelLabel->setText(tr("仪器型号") + ":");
    m_modelLabel->setFixedSize(105, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_modelLabel->setFont(font);
    m_modelLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_modelEdit = new QLabel();
    m_modelEdit->setObjectName("borderLabel");
    m_modelEdit->setFixedSize(220, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_modelEdit->setFont(font);
    m_modelEdit->setText("");

    QHBoxLayout *modelLayout = new QHBoxLayout();
    modelLayout->addStretch();
    modelLayout->addWidget(m_modelLabel);
    modelLayout->addWidget(m_modelEdit);
    modelLayout->addStretch();

    m_nameLabel = new QLabel();
    m_nameLabel->setObjectName("dialogLabel");
    m_nameLabel->setText(tr("仪器名称") + ":");
    m_nameLabel->setFixedSize(105, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_nameLabel->setFont(font);
    m_nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_nameEdit = new QLabel();
    m_nameEdit->setObjectName("borderLabel");
    m_nameEdit->setFixedSize(220, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_nameEdit->setFont(font);
    m_nameEdit->setText("");

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addStretch();
    nameLayout->addWidget(m_nameLabel);
    nameLayout->addWidget(m_nameEdit);
    nameLayout->addStretch();

    m_mfrsLabel = new QLabel();
    m_mfrsLabel->setObjectName("dialogLabel");
    m_mfrsLabel->setText(tr("生产厂商") + ":");
    m_mfrsLabel->setFixedSize(105, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_mfrsLabel->setFont(font);
    m_mfrsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_mfrsEdit = new QLabel();
    m_mfrsEdit->setObjectName("borderLabel");
    m_mfrsEdit->setFixedSize(220, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_mfrsEdit->setFont(font);
    m_mfrsEdit->setText("");

    QHBoxLayout *mfrsLayout = new QHBoxLayout();
    mfrsLayout->addStretch();
    mfrsLayout->addWidget(m_mfrsLabel);
    mfrsLayout->addWidget(m_mfrsEdit);
    mfrsLayout->addStretch();

    m_readDefaultButton = new QPushButton();
    m_readDefaultButton->setObjectName("brownButton");
    m_readDefaultButton->setText(tr("恢复默认"));
    m_readDefaultButton->setFixedSize(80, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_readDefaultButton->setFont(font);

    m_readInputButton = new QPushButton();
    m_readInputButton->setObjectName("brownButton");
    m_readInputButton->setText(tr("导入配置"));
    m_readInputButton->setFixedSize(80, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_readInputButton->setFont(font);

    QHBoxLayout *readLayout = new QHBoxLayout();
    readLayout->addStretch();
    readLayout->addWidget(m_readDefaultButton);
    readLayout->addWidget(m_readInputButton);
    readLayout->addStretch();
    readLayout->setSpacing(30);
    readLayout->setContentsMargins(0, 0, 0, 0);

    LuaEngine* luaEngine = LuaEngine::Instance();
    m_lua = luaEngine->GetEngine();
    lua_State* state = luaEngine->GetThreadState();
    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.instrument.typeChanges", itermsTable);
    QStringList typeChangesList;
    oolua_ipairs(itermsTable)
    {
        Table typeTable;
        m_lua->pull(typeTable);

        System::String type = "";
        System::String ShowDetails = "";
        System::String bindCmd = "";
        System::String unBindCmd = "";
        if (typeTable.safe_at("type", type))
        {
            typeTable.safe_at("ShowDetails", ShowDetails);
            typeChangesItem param;
            param.type = type;
            if (typeTable.safe_at("bindCmd", bindCmd))
            {
                param.bindCmd = bindCmd;
            }
            if (typeTable.safe_at("unBindCmd", unBindCmd))
            {
                param.unBindCmd = unBindCmd;
            }
            typeTable.safe_at("isReStart", param.isReStart);
            QString listType = QString(type.c_str()) + "-" + QString(ShowDetails.c_str());
            m_typeItems.insert(make_pair(m_typeChangesCount++, param));
            typeChangesList << listType;
        }
    }
    oolua_ipairs_end()
    luaEngine->GetLuaValue(state, "config.producerAuthorization.enable", isShowTypeChanges);

    m_typeChangesLabel = new QLabel();
    m_typeChangesLabel->setObjectName("dialogLabel");
    m_typeChangesLabel->setText(tr("仪器型码") + ":");
    m_typeChangesLabel->setFixedSize(105, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_typeChangesLabel->setFont(font);
    m_typeChangesLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_typeChangesBox = new QComboBox();
    m_typeChangesBox->addItems(typeChangesList);
    m_typeChangesBox->setFixedSize(220, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_typeChangesBox->setFont(font);

    QHBoxLayout *typeChangesLayout = new QHBoxLayout();
    typeChangesLayout->addStretch();
    typeChangesLayout->addWidget(m_typeChangesLabel);
    typeChangesLayout->addWidget(m_typeChangesBox);
    typeChangesLayout->addStretch();
    typeChangesLayout->setContentsMargins(0, 0, 0, 0);

    m_snLabel = new QLabel();
    m_snLabel->setObjectName("dialogLabel");
    m_snLabel->setText(tr("序列号(SN)") + ":");
    m_snLabel->setFixedSize(125, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_snLabel->setFont(font);
    m_snLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_charLabel = new QLabel();
    m_charLabel->setFixedSize(50, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_charLabel->setFont(font);
    m_charLabel->setText("");
    m_charLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    m_snEdit = new QLineEdit();
    m_snEdit->setFixedSize(140, COMMON_BUTTONS_COEFFICIENT_VERTICAL(30));
    m_snEdit->setFont(font);
    m_snEdit->setText("");

    m_numberKey = new CNumberKeyboard(m_snEdit);
    m_snEdit->installEventFilter(m_numberKey);

    QHBoxLayout *snLayout = new QHBoxLayout();
    snLayout->addStretch();
    snLayout->addWidget(m_snLabel);
    snLayout->addWidget(m_charLabel);
    snLayout->addWidget(m_snEdit);
    snLayout->addStretch();
    snLayout->setContentsMargins(0, 0, 0, 0);

    QFont btnFont;
    btnFont.setPointSize(16);
    m_okButton = new QPushButton();
    m_okButton->setObjectName("brownButton");
    m_okButton->setText(tr("确定"));
    m_okButton->setFixedSize(120, 50);
    m_okButton->setFont(btnFont);

    m_quitButton = new QPushButton();
    m_quitButton->setObjectName("brownButton");
    m_quitButton->setText(tr("退出"));
    m_quitButton->setFixedSize(120, 50);
    m_quitButton->setFont(btnFont);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_quitButton);
    buttonLayout->addStretch();
    buttonLayout->setSpacing(30);

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addStretch();
    dialogLayout->addLayout(manufactureTypeLayout);
    dialogLayout->addLayout(modelLayout);
    dialogLayout->addLayout(nameLayout);
    dialogLayout->addLayout(mfrsLayout);
    dialogLayout->addLayout(readLayout);
    dialogLayout->addStretch();
    dialogLayout->addLayout(typeChangesLayout);
    dialogLayout->addStretch();
    dialogLayout->addLayout(snLayout);
    dialogLayout->addStretch();
    dialogLayout->addLayout(buttonLayout);
    dialogLayout->addStretch();
    dialogLayout->setSpacing(10);
    dialogLayout->setContentsMargins(0, 50, 0, 0);

    this->setLayout(dialogLayout);

    this->DisplayInfo();

    connect(m_readDefaultButton, SIGNAL(clicked()), this, SLOT(SlotReadDefaultButton()));
    connect(m_readInputButton, SIGNAL(clicked()), this, SLOT(SlotReadInputButton()));
    connect(m_okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
    connect(m_quitButton, SIGNAL(clicked()), this, SLOT(SlotQuitButton()));
}

void SetSNDialog::DisplayInfo()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    try
    {
        int manufactureType = 0;
        luaEngine->GetLuaValue(state, "config.info.instrument.manufactureType", manufactureType);
        m_manufactureTypeBox->setCurrentIndex(manufactureType);

        String showModel;
        luaEngine->GetLuaValue(state, "config.info.instrument.showModel", showModel);
        m_modelEdit->setText(QString::fromStdString(showModel));

        String showName;
        luaEngine->GetLuaValue(state, "config.info.instrument.showName", showName);
        m_nameEdit->setText(QString::fromStdString(showName));

        String mfrs;
        luaEngine->GetLuaValue(state, "config.info.instrument.manuFacturer", mfrs);
        m_mfrsEdit->setText(QString::fromStdString(mfrs));

        String instrumentType;
        luaEngine->GetLuaValue(state, "config.info.instrument.type", instrumentType);
        String Type = instrumentType.substr(0, instrumentType.find("-"));
        bool checkOtherType = false;
        if (m_typeChangesCount)
        {
            for(int i=0; i<m_typeChangesCount; i++)
            {
                if(Type == m_typeItems[i].type)
                {
                        m_typeChangesBox->setCurrentIndex(i);
                        checkOtherType = true;
                }
            }
        }

        luaEngine->GetLuaValue(state, "config.producerAuthorization.enable", isShowTypeChanges);

        if (!checkOtherType)
        {
            m_typeChangesBox->insertItem(m_typeChangesCount,QString(Type.c_str()));
            m_typeChangesBox->setCurrentIndex(m_typeChangesCount);
            typeChangesItem param ;
            param.type = Type;
            m_typeItems.insert(make_pair(m_typeChangesCount++, param));
        }

        String sn;
        luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);

        QString strSN = QString(sn.c_str());

        QString tmpPattern("^([A-Z]+)(\\d{1,8})$");  //匹配n位大写字母+1~8位整数
        QRegExp tmpRx(tmpPattern);
        strSN.indexOf(tmpRx);

        QString head = tmpRx.cap(1);
        m_charLabel->setFixedWidth(head.size()*15);
        m_charLabel->setText(head);

        m_snEdit->setText(tmpRx.cap(2));

        if (!isShowTypeChanges)
        {
            m_typeChangesLabel->hide();
            m_typeChangesBox->hide();
        }
    }
    catch(OOLUA::LuaException e)
    {
        logger->warn("SetSNDialog::DisplayInfo() => %s", e.What().c_str());
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("SetSNDialog::DisplayInfo() => %s", e.what());
    }
    catch (std::exception e)
    {
        logger->warn("SetSNDialog::DisplayInfo() => %s", e.what());
    }
}

void SetSNDialog::SlotReadDefaultButton()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();

    try
    {
        Table defaultInfoTable;
        SettingManager::Instance()->LoadDefaultInstrumentInfo(defaultInfoTable);

        String showModel;
        defaultInfoTable.at("showModel", showModel);
        m_modelEdit->setText(QString::fromStdString(showModel));

        String showName;
        defaultInfoTable.at("showName", showName);
        m_nameEdit->setText(QString::fromStdString(showName));

        String mfrs;
        defaultInfoTable.at("manuFacturer", mfrs);
        m_mfrsEdit->setText(QString::fromStdString(mfrs));
    }
    catch(OOLUA::LuaException e)
    {
        logger->warn("SetSNDialog::SlotReadDefaultButton() => %s", e.What().c_str());
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("SetSNDialog::SlotReadDefaultButton() => %s", e.what());
    }
    catch (std::exception e)
    {
        logger->warn("SetSNDialog::SlotReadDefaultButton() => %s", e.what());
    }
}

void SetSNDialog::SlotReadInputButton()
{
    String path = Environment::Instance()->GetUpdatePath() + "/Manufacturer.ini";
    QSettings settings(QString::fromStdString(path), QSettings::IniFormat);

    QVariant model = settings.value("Info/Model");
    QVariant name = settings.value("Info/Name");
    QVariant mfrs = settings.value("Info/Mfrs");

    if(model.toString().size() > 0)
    {
        m_modelEdit->setText(model.toString());
    }

    if(name.toString().size() > 0)
    {
        m_nameEdit->setText(name.toString());
    }

    if(mfrs.toString().size() > 0)
    {
        m_mfrsEdit->setText(mfrs.toString());
    }
}

void SetSNDialog::SlotOkButton()
{
    int newIndex = m_typeChangesBox->currentIndex();
    int oldIndex = 0;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    String instrumentType;
    luaEngine->GetLuaValue(state, "config.info.instrument.type", instrumentType);

    String oldType = instrumentType.substr(0, instrumentType.find("-"));
    String newType = m_typeItems[newIndex].type;

    int manufactureType = m_manufactureTypeBox->currentIndex();
    QString strModel = m_modelEdit->text();
    QString strName = m_nameEdit->text();
    QString strMfrs = m_mfrsEdit->text();
    QString strSN = m_charLabel->text() +m_snEdit->text();
    QString strTpye = QString(m_typeItems[newIndex].type.c_str()) + QString(instrumentType.substr(instrumentType.find("-"),instrumentType.length()).c_str());

    QString tmpPattern("^([A-Z]+)(\\d{1,8})$");  //匹配1+位大写字母+8位整数
    QRegExp tmpRx(tmpPattern);

    if(tmpRx.exactMatch(strSN))
    {
        luaEngine->SetLuaValue("config.info.instrument.manufactureType", manufactureType);
        luaEngine->SetLuaValue("config.info.instrument.showModel", strModel.toStdString());
        luaEngine->SetLuaValue("config.info.instrument.showName", strName.toStdString());
        luaEngine->SetLuaValue("config.info.instrument.manuFacturer", strMfrs.toStdString());
        luaEngine->SetLuaValue("config.info.instrument.sn", strSN.toStdString());
        if (oldType != newType)
        {
            luaEngine->SetLuaValue("config.info.instrument.type", strTpye.toStdString());
            for(int i=0; i<m_typeChangesCount; i++)
            {
                if(oldType == m_typeItems[i].type)
                {
                    if (m_typeItems[i].unBindCmd != "")
                    {
                        oldIndex = i;
                        qDebug("unbind cmd %s",m_typeItems[i].unBindCmd.c_str());
                        #ifdef _CS_ARM_LINUX
                        system(m_typeItems[i].unBindCmd.c_str());
                        #endif
                    }
                }
            }
            if (m_typeItems[newIndex].bindCmd != "")
            {
                qDebug("bind cmd %s",m_typeItems[newIndex].bindCmd.c_str());
                #ifdef _CS_ARM_LINUX
                system(m_typeItems[newIndex].bindCmd.c_str());
                #endif
            }
        }

        SettingManager::Instance()->InstrumentInfoSave();

        UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::SystemInfoChanged, "");

        if (m_typeItems[newIndex].isReStart || m_typeItems[oldIndex].isReStart)
        {
            QString text = "设置成功，重启仪器后生效，是否立即重启？\n";
            MessageDialog msg(text, 30, this, MsgStyle::ONLYCANCEL);
            if(msg.exec()==QDialog::Rejected)
            {
                return;
            }
        }
        else
        {
            MessageDialog msg(tr("设置成功"), this, MsgStyle::ONLYOK);
            msg.exec();
            this->accept();
        }
    }
    else
    {
        MessageDialog msg(tr("请输入1-8位数字"), this, MsgStyle::ONLYOK);
        msg.exec();
    }
}

void SetSNDialog::SlotQuitButton()
{
    this->close();
}

void SetSNDialog::showEvent(QShowEvent *event)
{
    this->DisplayInfo();
    m_quitButton->setFocus();
    QDialog::showEvent(event);
}

void SetSNDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    int height = 50;
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    painter.drawPixmap(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN));

    painter.setBrush(Qt::white);

    painter.drawRect(
            QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH,
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

SetSNDialog::~SetSNDialog()
{
    if (m_numberKey)
    {
        delete m_numberKey;
        m_numberKey = nullptr;
    }
}

}

