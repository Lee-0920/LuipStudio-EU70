#include "Log.h"
#include "LevelEditWidget.h"
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
#define TABLE_WIDTH 600
#define TABLE_HIGH  460

#ifdef _CS_X86_WINDOWS
#define TITLE_HIGH  70
#endif

#ifdef _CS_ARM_LINUX
#define TITLE_HIGH  50
#endif

#define FIXED_HIGH 40

#define COMBOBOX_HIGH 40
#define COMBOBOX_WIDTH 380

namespace UI
{
LevelEditWidget::LevelEditWidget(AccessLevelRecord record, QWidget *parent) : DropShadowWidget(parent)
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
    m_accessLevelRecord = record;
    this->SpaceInit();
    this->EditSpaceInit();
}

void LevelEditWidget::SpaceInit()
{              
    QFont font;
    font.setPointSize(18);

    m_titleLabel = new QLabel("添加权限");
    m_titleLabel->setFont(font);
    m_titleLabel->setFixedSize(120,FIXED_HIGH);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(m_titleLabel);
    topLayout->addStretch();

    m_nameLabel = new QLabel("权限名称：");
    m_nameLabel->setFont(font);
    m_nameLabel->setFixedSize(120,FIXED_HIGH);
    m_nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QFont editFont;
    editFont.setPointSize(13);
    m_nameEdit = new QMyEdit();
    m_nameEdit->setFont(editFont);
    m_nameEdit->setText("");
    m_nameEdit->setFixedSize(200,FIXED_HIGH);

    m_nameEdit->installEventFilter(InputKeyBoard::Instance());
    connect(m_nameEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotNameEdit()));

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addSpacing(30);
    nameLayout->addWidget(m_nameLabel);
    nameLayout->addWidget(m_nameEdit);
    nameLayout->addStretch();

    QStringList authorityList = DataBaseManager::Instance()->GetAccessLevelTable()->ConvertLimitsOfAuthority(0xffffffff);;
    QStringList signalList = authorityList.mid((int)Authority::BasicSignal
                                               , (int)Authority::TrendDiagram - (int)Authority::BasicSignal + 1);
    QStringList settingList = authorityList.mid((int)Authority::MeasureScheduler
                                               , (int)Authority::SystemParam - (int)Authority::MeasureScheduler + 1);
    QStringList maintainList = authorityList.mid((int)Authority::Maintain
                                               , (int)Authority::SmartDetect - (int)Authority::Maintain + 1);
    QStringList systemList = authorityList.mid((int)Authority::InstrumentInformation
                                               , (int)Authority::BasicSignal - (int)Authority::AccessLevelManagement + 1);

    QLabel *signalLabel = new QLabel("信号权限：");
    signalLabel->setFont(font);
    signalLabel->setFixedSize(120,FIXED_HIGH);
    signalLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    signalComboBox = new MultiSelectComboBox(this);
    signalComboBox->myAddItems(signalList);
    signalComboBox->setFixedSize(COMBOBOX_WIDTH, COMBOBOX_HIGH);

    QHBoxLayout *signalLayout = new QHBoxLayout();
    signalLayout->addSpacing(30);
    signalLayout->addWidget(signalLabel);
    signalLayout->addWidget(signalComboBox);
    signalLayout->addStretch();

    QLabel *settingLabel = new QLabel("设置权限：");
    settingLabel->setFont(font);
    settingLabel->setFixedSize(120,FIXED_HIGH);
    settingLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    settingComboBox = new MultiSelectComboBox(this);
    settingComboBox->myAddItems(settingList);
    settingComboBox->setFixedSize(COMBOBOX_WIDTH, COMBOBOX_HIGH);

    QHBoxLayout *settingLayout = new QHBoxLayout();
    settingLayout->addSpacing(30);
    settingLayout->addWidget(settingLabel);
    settingLayout->addWidget(settingComboBox);
    settingLayout->addStretch();

    QLabel *maintainLabel = new QLabel("维护权限：");
    maintainLabel->setFont(font);
    maintainLabel->setFixedSize(120,FIXED_HIGH);
    maintainLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    maintainComboBox = new MultiSelectComboBox(this);
    maintainComboBox->myAddItems(maintainList);
    maintainComboBox->setFixedSize(COMBOBOX_WIDTH, COMBOBOX_HIGH);

    QHBoxLayout *maintainLayout = new QHBoxLayout();
    maintainLayout->addSpacing(30);
    maintainLayout->addWidget(maintainLabel);
    maintainLayout->addWidget(maintainComboBox);
    maintainLayout->addStretch();

    QLabel *systemLabel = new QLabel("系统权限：");
    systemLabel->setFont(font);
    systemLabel->setFixedSize(120,FIXED_HIGH);
    systemLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    systemComboBox = new MultiSelectComboBox(this);
    systemComboBox->myAddItems(systemList);
    systemComboBox->setFixedSize(COMBOBOX_WIDTH, COMBOBOX_HIGH);

    QHBoxLayout *systemLayout = new QHBoxLayout();
    systemLayout->addSpacing(30);
    systemLayout->addWidget(systemLabel);
    systemLayout->addWidget(systemComboBox);
    systemLayout->addStretch();

    checkAllButton = new QPushButton();
    checkAllButton->setObjectName("brownButton");
    checkAllButton->setText(tr("全选"));
    checkAllButton->setFont(font);
    checkAllButton->setFixedSize(80,40);
    connect(checkAllButton,SIGNAL(clicked()), this, SLOT(SlotCheckAll()));

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
    bottomLayout->addSpacing(30);
    bottomLayout->addWidget(checkAllButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(okButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(quitButton);
    bottomLayout->addSpacing(30);

    QVBoxLayout *mainLayout = new QVBoxLayout();    
    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(nameLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(signalLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(settingLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(maintainLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(systemLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(bottomLayout);
    mainLayout->setContentsMargins(0, 20, 0, 15);

    this->setLayout(mainLayout);
}

int LevelEditWidget::GetLevelMap()
{
    QList<QString> signalList = signalComboBox->SelectedItemsData();;
    QList<QString> settingList = settingComboBox->SelectedItemsData();
    QList<QString> maintainList = maintainComboBox->SelectedItemsData();
    QList<QString> systemList = systemComboBox->SelectedItemsData();
    int level  = DataBaseManager::Instance()->GetAccessLevelTable()->ConvertLevelListToMap(
                signalList + settingList + maintainList + systemList);
    return level;
}

void LevelEditWidget::SlotOkButton()
{
    int level = GetLevelMap();
    if(m_nameEdit->text().isEmpty())
    {
        MessageDialog msg(tr("权限名称不能为空！"), this, MsgStyle::ONLYOK);
        msg.exec();
        m_nameEdit->setFocus();
        return;
    }
    if(!level)
    {
        MessageDialog msg(tr("权限无效！"), this, MsgStyle::ONLYOK);
        msg.exec();
        return;
    }
    AccessLevelRecord record;
    record.levelName = m_nameEdit->text();
    record.dataTime = QDateTime::currentDateTime().toTime_t();
    record.levelName = m_nameEdit->text();
    record.limitsOfAuthority = level;    
    QString msgStr = "权限创建完成";
    if(!m_accessLevelRecord.levelName.isEmpty())
    {
        msgStr = "权限修改完成";
        record.dataTime = m_accessLevelRecord.dataTime;
        if(m_accessLevelRecord.levelName != m_nameEdit->text())
        {
            DataBaseManager::Instance()->GetAccessLevelTable()->DeleteDataFromName(m_accessLevelRecord.levelName);
            DataBaseManager::Instance()->GetUserTable()->UpdateLevelName(m_accessLevelRecord.levelName, m_nameEdit->text());
        }
    }    
    AuditTrail(m_accessLevelRecord, record);
    DataBaseManager::Instance()->GetAccessLevelTable()->InsertData({record});
    MessageDialog msg(msgStr, this, MsgStyle::ONLYOK);
    msg.exec();
    this->close();
}

void LevelEditWidget::SlotQuitButton()
{
    this->close();
}

void LevelEditWidget::SlotCheckAll()
{
    bool ret = (GetLevelMap()==0 || GetLevelMap()<0x1ffffff)?true:false;
    signalComboBox->SetAllItem(ret);
    settingComboBox->SetAllItem(ret);
    maintainComboBox->SetAllItem(ret);
    systemComboBox->SetAllItem(ret);
    if(ret)
    {
        checkAllButton->setText("全不选");
    }
    else
    {
        checkAllButton->setText("全选");
    }
}

void LevelEditWidget::EditSpaceInit()
{
    if(!m_accessLevelRecord.levelName.isEmpty())
    {
        m_titleLabel->setText("修改权限");
        m_nameEdit->setText(m_accessLevelRecord.levelName);
        QStringList list = DataBaseManager::Instance()->GetAccessLevelTable()->ConvertLimitsOfAuthority(m_accessLevelRecord.limitsOfAuthority);
        QList<QString> dataList(list);
        signalComboBox->SetItemsData(dataList);
        settingComboBox->SetItemsData(dataList);
        maintainComboBox->SetItemsData(dataList);
        systemComboBox->SetItemsData(dataList);
    }
    else
    {
        QList<QString> dataList = {"基本信号", "测量排期", "维护", "仪器信息"};
        signalComboBox->SetItemsData(dataList);
        settingComboBox->SetItemsData(dataList);
        maintainComboBox->SetItemsData(dataList);
        systemComboBox->SetItemsData(dataList);
    }
}

void LevelEditWidget::AuditTrail(AccessLevelRecord oldRecord, AccessLevelRecord newRecord)
{
    QString userName = UI::LoginDialog::userInfo.userName;
    QString levelName = UI::LoginDialog::userInfo.levelName;
    QString event = "--";
    QString oldSetting = "--";
    QString newSetting = "--";
    QString detail = "--";
    QStringList oldSettingList;
    QStringList newSettingList;

    if(!oldRecord.levelName.isEmpty())
    {
        event = "编辑权限-" + newRecord.levelName;
        if(oldRecord.levelName != newRecord.levelName)
        {
            detail = "修改名称:" + oldRecord.levelName + "->" + newRecord.levelName;
        }
        if(oldRecord.limitsOfAuthority != newRecord.limitsOfAuthority)
        {
            oldSettingList = DataBaseManager::Instance()->GetAccessLevelTable()->ConvertLimitsOfAuthority(oldRecord.limitsOfAuthority);
            newSettingList = DataBaseManager::Instance()->GetAccessLevelTable()->ConvertLimitsOfAuthority(newRecord.limitsOfAuthority);
        }
        oldSetting = oldSettingList.join(", ");
        newSetting = newSettingList.join(", ");
    }
    else
    {
        event = "创建权限-" + newRecord.levelName;
        newSettingList = DataBaseManager::Instance()->GetAccessLevelTable()->ConvertLimitsOfAuthority(newRecord.limitsOfAuthority);
        newSetting = newSettingList.join(", ");
    }
    //审计追踪
    DataBaseManager::Instance()->GetAuditTrailTable()->InsertAuditTrail(userName, levelName, event, oldSetting, newSetting, detail);
}

void LevelEditWidget::SlotNameEdit(void)
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


void LevelEditWidget::showEvent(QShowEvent *event)
{
//    m_quitButton->setFocus();
//    QDialog::showEvent(event);
}

void LevelEditWidget::mousePressEvent(QMouseEvent *event)
{
    DropShadowWidget::mousePressEvent(event);

    if (!InputKeyBoard::Instance()->contentsRect().contains(InputKeyBoard::Instance()->mapFromGlobal(event->globalPos())))
    {
        InputKeyBoard::Instance()->hide();
    }
}

void LevelEditWidget::paintEvent(QPaintEvent *event)
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

LevelEditWidget::~LevelEditWidget()
{

}

}

