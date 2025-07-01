#include "Log.h"
#include "MethodWidget.h"
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

using namespace Configuration;
using namespace System;
using namespace Lua;
using namespace OOLUA;
using namespace std;

#define FIXED_HIGH 40

namespace UI
{

unique_ptr<MethodWidget> MethodWidget::m_instance(nullptr);

MethodWidget::MethodWidget(QWidget *parent) : DropShadowWidget(parent)
{
    this->setFixedSize(800, 500);
    this->setWindowModality(Qt::WindowModal);

    #ifdef _CS_X86_WINDOWS
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);
    #endif

    QFont font;
    font.setPointSize(18);

    m_nameLabel = new QLabel("名称：");
    m_nameLabel->setFont(font);
    m_nameLabel->setFixedSize(80,FIXED_HIGH);

    m_nameEdit = new QMyEdit();
    m_nameEdit->setFont(font);
    m_nameEdit->setText("sample");
    m_nameEdit->setFixedSize(120,FIXED_HIGH);

    m_onlineCheckBox = new QCheckBox();
    m_onlineCheckBox->setFixedSize(120,FIXED_HIGH);
    m_onlineCheckBox->setFont(font);
    m_onlineCheckBox->setText("在线");
    m_onlineCheckBox->setObjectName("checkboxone");
    m_onlineCheckBox->setCheckState(Qt::Checked);
//    m_onlineCheckBox->setChecked(true);

    m_offlineCheckBox = new QCheckBox();
    m_offlineCheckBox->setFixedSize(120,FIXED_HIGH);
    m_offlineCheckBox->setText("离线");
    m_offlineCheckBox->setFont(font);
    m_offlineCheckBox->setCheckState(Qt::Unchecked);
    m_offlineCheckBox->setObjectName("checkboxone");

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addSpacing(50);
    topLayout->addWidget(m_nameLabel);
    topLayout->addWidget(m_nameEdit);
    topLayout->addSpacing(30);
    topLayout->addWidget(m_onlineCheckBox);
    topLayout->addSpacing(5);
    topLayout->addWidget(m_offlineCheckBox);
    topLayout->addStretch();

    m_turboCheckBox = new QCheckBox();
    m_turboCheckBox->setText("Turbo");
//    m_turboCheckBox->setFixedSize(80,FIXED_HIGH);
    m_turboCheckBox->setFixedHeight(FIXED_HIGH);
    m_turboCheckBox->setFont(font);
    m_turboCheckBox->setObjectName("checkboxone");

    m_ICRCheckBox = new QCheckBox();
    m_ICRCheckBox->setText("ICR");
//    m_ICRCheckBox->setFixedSize(80,FIXED_HIGH);
    m_ICRCheckBox->setFixedHeight(FIXED_HIGH);
    m_ICRCheckBox->setFont(font);
    m_ICRCheckBox->setObjectName("checkboxone");

    m_TOCCheckBox = new QCheckBox();
    m_TOCCheckBox->setText("TOC");
    m_TOCCheckBox->setFixedHeight(FIXED_HIGH);
    m_TOCCheckBox->setFont(font);
    m_TOCCheckBox->setObjectName("checkboxone");
//    m_TOCCheckBox->setCheckState(Qt::Checked);

    m_ECCheckBox = new QCheckBox();
    m_ECCheckBox->setText("电导率(NaCl)");
//    m_ECCheckBox->setFixedSize(170,FIXED_HIGH);
    m_ECCheckBox->setFixedHeight(FIXED_HIGH);
    m_ECCheckBox->setFont(font);
    m_ECCheckBox->setObjectName("checkboxone");

    m_autoReagentCheckBox = new QCheckBox(this);
    m_autoReagentCheckBox->setText("自动加试剂");
//    m_autoReagentCheckBox->setFixedSize(150,FIXED_HIGH);
    m_autoReagentCheckBox->setFixedHeight(FIXED_HIGH);
    m_autoReagentCheckBox->setFont(font);
    m_autoReagentCheckBox->setObjectName("checkboxone");

    QHBoxLayout* checkboxLayout = new QHBoxLayout();
    checkboxLayout->addSpacing(50);
    checkboxLayout->addWidget(m_turboCheckBox);
    checkboxLayout->addWidget(m_ICRCheckBox);
    checkboxLayout->addWidget(m_TOCCheckBox);
    checkboxLayout->addWidget(m_ECCheckBox);
    checkboxLayout->addWidget(m_autoReagentCheckBox);
    checkboxLayout->addStretch();
//    checkboxLayout->setSpacing(20);

    m_reagent1Label = new QLabel("酸剂(uL/分)：");
    m_reagent1Label->setFont(font);
//    m_reagent1Label->setFixedSize(180,FIXED_HIGH);
    m_reagent1Label->setFixedHeight(FIXED_HIGH);
    m_reagent1Edit = new QMyEdit();
    m_reagent1Edit->setFont(font);
    m_reagent1Edit->setText("1.0");
    m_reagent1Edit->setFixedSize(70,FIXED_HIGH);

    m_reagent2Label = new QLabel("氧化剂(uL/分)：");
    m_reagent2Label->setFont(font);
//    m_reagent2Label->setFixedSize(180,FIXED_HIGH);
    m_reagent2Label->setFixedHeight(FIXED_HIGH);
    m_reagent2Edit = new QMyEdit();
    m_reagent2Edit->setFont(font);
    m_reagent2Edit->setText("0.1");
    m_reagent2Edit->setFixedSize(70,FIXED_HIGH);

    QHBoxLayout* reagentLayout = new QHBoxLayout();
    reagentLayout->addSpacing(50);
    reagentLayout->addWidget(m_reagent1Label);
    reagentLayout->addWidget(m_reagent1Edit);
    reagentLayout->addSpacing(30);
    reagentLayout->addWidget(m_reagent2Label);
    reagentLayout->addWidget(m_reagent2Edit);
    reagentLayout->addStretch();

    m_cleanTimeLabel = new QLabel("冲洗(秒)：");
    m_cleanTimeLabel->setFont(font);
//    m_cleanTimeLabel->setFixedSize(120,FIXED_HIGH);
    m_cleanTimeLabel->setFixedHeight(FIXED_HIGH);
    m_cleanTimeEdit = new QMyEdit();
    m_cleanTimeEdit->setFont(font);
    m_cleanTimeEdit->setText("240");
    m_cleanTimeEdit->setFixedSize(70,FIXED_HIGH);

    QHBoxLayout* timeLayout = new QHBoxLayout();
    timeLayout->addSpacing(50);
    timeLayout->addWidget(m_cleanTimeLabel);
    timeLayout->addWidget(m_cleanTimeEdit);
    timeLayout->addStretch();

    m_measureTimesLabel = new QLabel("测量次数：");
    m_measureTimesLabel->setFont(font);
    m_measureTimesLabel->setFixedSize(120,FIXED_HIGH);
    m_measureTimesEdit = new QMyEdit();
    m_measureTimesEdit->setFont(font);
    m_measureTimesEdit->setText("4");
    m_measureTimesEdit->setFixedSize(50,FIXED_HIGH);

    m_rejectTimesLabel = new QLabel("舍弃次数：");
    m_rejectTimesLabel->setFont(font);
    m_rejectTimesLabel->setFixedSize(120,FIXED_HIGH);
    m_rejectTimesEdit = new QMyEdit();
    m_rejectTimesEdit->setFont(font);
    m_rejectTimesEdit->setText("1");
    m_rejectTimesEdit->setFixedSize(50,FIXED_HIGH);

    QHBoxLayout* meaLayout = new QHBoxLayout();
    meaLayout->addSpacing(50);
    meaLayout->addWidget(m_measureTimesLabel);
    meaLayout->addWidget(m_measureTimesEdit);
    meaLayout->addSpacing(50);
    meaLayout->addWidget(m_rejectTimesLabel);
    meaLayout->addWidget(m_rejectTimesEdit);
    meaLayout->addStretch();

    m_saveAsButton = new QPushButton();
    m_saveAsButton->setFixedSize(80,45);
    m_saveAsButton->setText(tr("另存为"));
    m_saveAsButton->setObjectName("brownButton");

    m_saveButton = new QPushButton();
    m_saveButton->setFixedSize(80,45);
    m_saveButton->setText(tr("保存"));
    m_saveButton->setObjectName("brownButton");

    m_applyButton = new QPushButton();
    m_applyButton->setFixedSize(80,45);
    m_applyButton->setText(tr("应用"));
    m_applyButton->setObjectName("brownButton");

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_saveAsButton);
    bottomLayout->addWidget(m_saveButton);
    bottomLayout->addWidget(m_applyButton);
    bottomLayout->addSpacing(15);
    bottomLayout->setSpacing(30);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addSpacing(10);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(checkboxLayout);
    mainLayout->addLayout(reagentLayout);
    mainLayout->addLayout(timeLayout);
    mainLayout->addLayout(meaLayout);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addSpacing(5);

    QIntValidator* vIntTime = new QIntValidator(0,3600,this);
    QIntValidator* vIntMeaTimes = new QIntValidator(3,100,this);
    QDoubleValidator* vDReagent = new QDoubleValidator(0.00,10.00,2,this);
    vDReagent->setNotation(QDoubleValidator::StandardNotation);

    m_reagent1Edit->setValidator(vDReagent);
    m_reagent2Edit->setValidator(vDReagent);
    m_cleanTimeEdit->setValidator(vIntTime);
    m_measureTimesEdit->setValidator(vIntMeaTimes);
    m_rejectTimesEdit->setValidator(vIntMeaTimes);

    m_nameEdit->installEventFilter(InputKeyBoard::Instance());
    m_reagent1Edit->installEventFilter(CNumberKeyboard::Instance());
    m_reagent2Edit->installEventFilter(CNumberKeyboard::Instance());
    m_cleanTimeEdit->installEventFilter(CNumberKeyboard::Instance());
    m_measureTimesEdit->installEventFilter(CNumberKeyboard::Instance());
    m_rejectTimesEdit->installEventFilter(CNumberKeyboard::Instance());

    this->SpaceInit();

    this->setLayout(mainLayout);

    connect(m_nameEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotNameEdit()));
    connect(m_reagent1Edit, SIGNAL(LineEditClicked()), this, SLOT(SlotMyEdit()));
    connect(m_reagent2Edit, SIGNAL(LineEditClicked()), this, SLOT(SlotMyEdit()));
    connect(m_cleanTimeEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotMyEdit()));
    connect(m_measureTimesEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotMyEdit()));
    connect(m_rejectTimesEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotMyEdit()));

    connect(m_onlineCheckBox, SIGNAL(stateChanged(int)), this, SLOT(SlotOnlineCheckBox(int)));
    connect(m_offlineCheckBox, SIGNAL(stateChanged(int)), this, SLOT(SlotOfflineCheckBox(int)));

    connect(m_applyButton, SIGNAL(clicked()), this, SLOT(SlotApplyButton()));
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT(SlotSaveButton()));
    connect(this, SIGNAL(MethodWidgetUpdateSignal(MethodRecord, bool, bool)), this, SLOT(SlotUpdateMethod(MethodRecord, bool, bool)));
}

MethodWidget* MethodWidget::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new MethodWidget);
    }

    return m_instance.get();
}

void MethodWidget::SpaceInit()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* lua = luaEngine->GetThreadState();

    bool turbo,ICR,TOC,EC,autoReagent;
    float reagent1,reagent2;
    int cleanTime =0, meaType = 0;
    String name;
    luaEngine->GetLuaValue(lua, "config.measureParam.methodName", name);
    luaEngine->GetLuaValue(lua, "config.measureParam.meaType", meaType);
    luaEngine->GetLuaValue(lua, "config.measureParam.turboMode", turbo);
    luaEngine->GetLuaValue(lua, "config.measureParam.ICRMode", ICR);
    luaEngine->GetLuaValue(lua, "config.measureParam.TOCMode", TOC);
    luaEngine->GetLuaValue(lua, "config.measureParam.ECMode", EC);
    luaEngine->GetLuaValue(lua, "config.measureParam.autoReagent", autoReagent);
    luaEngine->GetLuaValue(lua, "config.measureParam.reagent1Vol", reagent1);
    luaEngine->GetLuaValue(lua, "config.measureParam.reagent2Vol", reagent2);
    luaEngine->GetLuaValue(lua, "config.measureParam.normalRefreshTime", cleanTime);

    if(turbo)
    {
        m_turboCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        m_turboCheckBox->setCheckState(Qt::Unchecked);
    }

    if(ICR)
    {
        m_ICRCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        m_ICRCheckBox->setCheckState(Qt::Unchecked);
    }

    if(TOC)
    {
        m_TOCCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        m_TOCCheckBox->setCheckState(Qt::Unchecked);
    }

    if(EC)
    {
        m_ECCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        m_ECCheckBox->setCheckState(Qt::Unchecked);
    }

    if(autoReagent)
    {
        m_autoReagentCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        m_autoReagentCheckBox->setCheckState(Qt::Unchecked);
    }
    m_nameEdit->setText(name.c_str());
    m_reagent1Edit->setText(QString::number(reagent1));
    m_reagent2Edit->setText(QString::number(reagent2));
    m_cleanTimeEdit->setText(QString::number(cleanTime));
    if(!meaType) //在线
    {
        m_onlineCheckBox->setCheckState(Qt::CheckState::Checked);
        m_offlineCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }
    else//离线
    {
        m_onlineCheckBox->setCheckState(Qt::CheckState::Unchecked);
        m_offlineCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    if(m_onlineCheckBox->checkState() == Qt::CheckState::Checked)
    {
        this->OffLineSetting_Hide();
    }
}

void MethodWidget::SlotMyEdit(void)
{
    int curX = cursor().pos().x();
    int curY = cursor().pos().y();
    int kbWidth = 300;
    int kbHeigh = 300;
#ifdef _CS_ARM_LINUX
    int x0 = 0;
    int y0 = 0;
#endif
#ifdef _CS_X86_WINDOWS
    int x0 = 540;
    int y0 = 170;
#endif

//    qDebug("x[%d],y[%d]",curX,curY);
    if(curX + kbWidth > 800 + x0)
    {
        curX = m_reagent2Edit->pos().x() - kbWidth - 20;
    }
    if(curY + kbHeigh > 600 + y0)
    {
        curY = this->size().height() + y0 - kbHeigh;
        curX = curX + 20;
    }
    CNumberKeyboard::Instance()->move(curX,curY);
    CNumberKeyboard::Instance()->show();
//    qDebug("x[%d],y[%d]",curX,curY);
//    CNumberKeyboard *numKbd = CNumberKeyboard::Instance();
//    if(true == numKbd->isHidden())
//    {
//        numKbd->myMove(curX, curY, 0, 0);
//        numKbd->show();
//    }
}

void MethodWidget::SlotNameEdit(void)
{
    int curX = cursor().pos().x();
    int curY = cursor().pos().y();    
    InputKeyBoard::Instance()->move(curX,curY);
    InputKeyBoard::Instance()->show();

//    InputKeyBoard *numKbd = InputKeyBoard::Instance();
//    if(curX > 1100)
//    {
//        numKbd->myMove(curX, curY, curWidth, curHeight);
//        numKbd->show();
//    }
}

void MethodWidget::OffLineSetting_Show(void)
{
    m_measureTimesLabel->show();
    m_measureTimesEdit->show();
    m_rejectTimesLabel->show();
    m_rejectTimesEdit->show();
}

void MethodWidget::OffLineSetting_Hide(void)
{
    m_measureTimesLabel->hide();
    m_measureTimesEdit->hide();
    m_rejectTimesLabel->hide();
    m_rejectTimesEdit->hide();
}

/*
*设置在线、离线复选框为互斥
*/
void MethodWidget::SlotOnlineCheckBox(int arg)
{
    if(arg == Qt::CheckState::Checked)
    {
        m_offlineCheckBox->setCheckState(Qt::Unchecked);
        this->OffLineSetting_Hide();
    }
    else if(arg == Qt::CheckState::Unchecked && !m_offlineCheckBox->isChecked())
    {
        m_onlineCheckBox->setCheckState(Qt::Checked);
        this->OffLineSetting_Hide();
    }
}

/*
*设置在线、离线复选框为互斥
*/
void MethodWidget::SlotOfflineCheckBox(int arg)
{
    if(arg == Qt::CheckState::Checked)
    {
        m_onlineCheckBox->setCheckState(Qt::Unchecked);
        this->OffLineSetting_Show();
    }
    else if(arg == Qt::CheckState::Unchecked && !m_onlineCheckBox->isChecked())
    {
        m_offlineCheckBox->setCheckState(Qt::Checked);
        this->OffLineSetting_Show();
    }
}

void MethodWidget::SlotCheckValue(QTableWidgetItem *item)
{

}

void MethodWidget::SlotDoubleClicked(QTableWidgetItem *item)
{

}

/*
*brief：应用到脚本，但不保存到数据库中，立刻会对流程生效
*/
void MethodWidget::SlotApplyButton()
{    
    MethodRecord record;
    QString name = m_nameEdit->text();
    int meaType = 0;
    if(m_offlineCheckBox->isChecked())
    {
        meaType = 1;
    }
    bool turbo = false;
    if(m_turboCheckBox->isChecked())
    {
        turbo = true;
    }
    bool ICR = false;
    if(m_ICRCheckBox->isChecked())
    {
        ICR = true;
    }
    bool TOC = false;
    if(m_TOCCheckBox->isChecked())
    {
        TOC = true;
    }
    bool EC = false;
    if(m_ECCheckBox->isChecked())
    {
        EC = true;
    }
    bool autoReagent = false;
    if(m_autoReagentCheckBox->isChecked())
    {
        autoReagent = true;
    }
    float reagent1 = m_reagent1Edit->text().toFloat();
    float reagent2 = m_reagent2Edit->text().toFloat();
    int cleanTime = m_cleanTimeEdit->text().toInt();
    int meaTimes = m_measureTimesEdit->text().toInt();
    int rejTimes = m_rejectTimesEdit->text().toInt();
    QDateTime curDateTime = QDateTime::currentDateTime();
    record.methodName = name;
    record.createTime = curDateTime.toTime_t();
    record.meaType = meaType;
    record.turboMode = turbo?1:0;
    record.ICRMode = ICR?1:0;
    record.TOCMode = TOC?1:0;
    record.ECMode = EC?1:0;
    record.autoReagent = autoReagent?1:0;
    record.reagent1Vol = reagent1;
    record.reagent2Vol = reagent2;
    record.normalRefreshTime = cleanTime;
    record.measureTimes = meaTimes;
    record.rejectTimes = rejTimes;
//    DataBaseManager::Instance()->GetMethodTable()->InsertData({record});
    LuaEngine* luaEngine = LuaEngine::Instance();
    luaEngine->SetLuaValue("config.measureParam.methodName", name.toStdString());
    luaEngine->SetLuaValue("config.measureParam.meaType", meaType);
    luaEngine->SetLuaValue("config.measureParam.turboMode", turbo);
    luaEngine->SetLuaValue("config.measureParam.ICRMode", ICR);
    luaEngine->SetLuaValue("config.measureParam.TOCMode", TOC);
    luaEngine->SetLuaValue("config.measureParam.ECMode", EC);
    luaEngine->SetLuaValue("config.measureParam.autoReagent", autoReagent);
    luaEngine->SetLuaValue("config.measureParam.reagent1Vol", reagent1);
    luaEngine->SetLuaValue("config.measureParam.reagent2Vol", reagent2);
    luaEngine->SetLuaValue("config.measureParam.normalRefreshTime", cleanTime);
    luaEngine->SetLuaValue("config.measureParam.methodCreateTimeStr", QDateTime::fromTime_t(record.createTime).toString("yyyy-MM-dd HH:mm:ss").toStdString());
    luaEngine->SetLuaValue("config.measureParam.methodCreateTime",
                           record.createTime);
    SettingManager::Instance()->MeasureParamSave();
    MessageDialog msg(tr("方法应用成功！"), this, MsgStyle::ONLYOK);
    msg.exec();
}

/*
*brief：保存到数据库中,但不应用到脚本，需要应用后才会对流程生效
*/
void MethodWidget::SlotSaveButton()
{
    if(m_nameEdit->text().isEmpty())
    {
        MessageDialog msg(tr("方法名称不能为空"), this, MsgStyle::ONLYOK);
        msg.exec();
//        this->show();
    }
    else
    {
        MethodRecord record;
        QString name = m_nameEdit->text();
        int meaType = 0;
        if(m_offlineCheckBox->isChecked())
        {
            meaType = 1;
        }
        bool turbo = false;
        if(m_turboCheckBox->isChecked())
        {
            turbo = true;
        }
        bool ICR = false;
        if(m_ICRCheckBox->isChecked())
        {
            ICR = true;
        }
        bool TOC = false;
        if(m_TOCCheckBox->isChecked())
        {
            TOC = true;
        }
        bool EC = false;
        if(m_ECCheckBox->isChecked())
        {
            EC = true;
        }
        bool autoReagent = false;
        if(m_autoReagentCheckBox->isChecked())
        {
            autoReagent = true;
        }
        float reagent1 = m_reagent1Edit->text().toFloat();
        float reagent2 = m_reagent2Edit->text().toFloat();
        int cleanTime = m_cleanTimeEdit->text().toInt();
        int meaTimes = m_measureTimesEdit->text().toInt();
        int rejTimes = m_rejectTimesEdit->text().toInt();
        QDateTime curDateTime = QDateTime::currentDateTime();
        record.methodName = name;
        record.createTime = curDateTime.toTime_t();
        record.meaType = meaType;
        record.turboMode = turbo?1:0;
        record.ICRMode = ICR?1:0;
        record.TOCMode = TOC?1:0;
        record.ECMode = EC?1:0;
        record.autoReagent = autoReagent?1:0;
        record.reagent1Vol = reagent1;
        record.reagent2Vol = reagent2;
        record.normalRefreshTime = cleanTime;
        record.measureTimes = meaTimes;
        record.rejectTimes = rejTimes;

//        bool isExist = DataBaseManager::Instance()->GetMethodTable()->IsMethodNameExist(record.methodName);
//        if(!isExist)
//        {
            DataBaseManager::Instance()->GetMethodTable()->InsertData({record});
            MessageDialog msg(tr("方法保存成功！"), this, MsgStyle::ONLYOK);
            msg.exec();
//        }
//        else
//        {
//            MessageDialog msg(tr("方法修改无效,名称已存在！"), this, MsgStyle::ONLYOK);
//            msg.exec();
//        }
    }
}


/*
*brief：可选择保存or不保存到数据库,但应用到脚本，立刻会对流程生效
*/
void MethodWidget::SlotUpdateMethod(MethodRecord record, bool isWriteToSql, bool isFromModbus)
{
    m_nameEdit->setText(record.methodName);
    if(!record.meaType)
    {
        m_onlineCheckBox->setCheckState(Qt::CheckState::Checked);
        m_offlineCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }
    else
    {
        m_onlineCheckBox->setCheckState(Qt::CheckState::Unchecked);
        m_offlineCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    m_cleanTimeEdit->setText(QString::number(record.normalRefreshTime));
    m_measureTimesEdit->setText(QString::number(record.measureTimes));
    m_rejectTimesEdit->setText(QString::number(record.rejectTimes));

    LuaEngine* luaEngine = LuaEngine::Instance();
    luaEngine->SetLuaValue("config.measureParam.methodName", record.methodName.toStdString());
    luaEngine->SetLuaValue("config.measureParam.meaType", record.meaType);
    luaEngine->SetLuaValue("config.measureParam.turboMode", record.turboMode?true:false);
    luaEngine->SetLuaValue("config.measureParam.ICRMode", record.ICRMode?true:false);
    luaEngine->SetLuaValue("config.measureParam.TOCMode", record.TOCMode?true:false);
    luaEngine->SetLuaValue("config.measureParam.ECMode", record.ECMode?true:false);
    luaEngine->SetLuaValue("config.measureParam.autoReagent", record.autoReagent?true:false);
    luaEngine->SetLuaValue("config.measureParam.reagent1Vol", record.reagent1Vol);
    luaEngine->SetLuaValue("config.measureParam.reagent2Vol", record.reagent2Vol);
    luaEngine->SetLuaValue("config.measureParam.normalRefreshTime", record.normalRefreshTime);
    luaEngine->SetLuaValue("config.measureParam.methodCreateTimeStr", QDateTime::fromTime_t(record.createTime).toString("yyyy-MM-dd HH:mm:ss").toStdString());
    luaEngine->SetLuaValue("config.measureParam.methodCreateTime",
                           record.createTime);
    SettingManager::Instance()->MeasureParamSave();

    if(isWriteToSql)
    {
//        bool isExist = DataBaseManager::Instance()->GetMethodTable()->IsMethodNameExist(record.methodName);
//        if(!isExist)
//        {
            DataBaseManager::Instance()->GetMethodTable()->InsertData({record});
//        }
//        else if(isExist && !isFromModbus)
//        {
//            MessageDialog msg(tr("方法修改无效,名称已存在！"), this, MsgStyle::ONLYOK);
//            msg.exec();
//        }
    }

    //协议修改方法时不显示弹窗信息
    if(!isFromModbus)
    {
        MessageDialog msg(tr("方法修改成功！"), this, MsgStyle::ONLYOK);
        msg.exec();
    }
}

void MethodWidget::SlotQuitButton()
{
    this->close();
}

void MethodWidget::showEvent(QShowEvent *event)
{
//    m_quitButton->setFocus();
//    QDialog::showEvent(event);
    this->SpaceInit();
//    qDebug("Show");
}

void MethodWidget::mousePressEvent(QMouseEvent *event)
{
    DropShadowWidget::mousePressEvent(event);

    if (!InputKeyBoard::Instance()->contentsRect().contains(InputKeyBoard::Instance()->mapFromGlobal(event->globalPos())))
    {
        InputKeyBoard::Instance()->hide();
    }
}

void MethodWidget::paintEvent(QPaintEvent *event)
{
//    QDialog::paintEvent(event);
//    int height = 50;
//    QPainter painter(this);
//    painter.setPen(Qt::NoPen);

//    painter.drawPixmap(
//            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
//                    this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN));

//    painter.setBrush(Qt::white);

//    painter.drawRect(
//            QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH,
//                    this->height() - height - SHADOW_WIDTH));

//    QPen pen;
//    pen.setColor(QColor(10,105,170));
//    pen.setWidth(3);

//    painter.setPen(pen);
//    painter.drawLine(QPoint(0,0), QPoint(0,this->height()));
//    painter.drawLine(QPoint(0,0), QPoint(this->width(),0));
//    painter.drawLine(QPoint(0,this->height()-1), QPoint(this->width()-1,this->height()-1));
//    painter.drawLine(QPoint(this->width()-1,0), QPoint(this->width()-1,this->height()-1));
}

MethodWidget::~MethodWidget()
{
    if (m_numberKey)
    {
        delete m_numberKey;
        m_numberKey = nullptr;
    }
}

}

