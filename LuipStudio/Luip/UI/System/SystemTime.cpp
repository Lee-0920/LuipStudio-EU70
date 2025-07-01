#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "SystemTime.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include <time.h>
#include <stdio.h>
#ifndef  _CS_X86_WINDOWS
#include <sys/time.h>
#endif
#include "Setting/SettingManager.h"
#include "Controller/ControllerManager.h"
#include "AlarmManager/AlarmManager.h"
#include "Log.h"
#include "System/Types.h"
#include "oolua.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/MessageDialog.h"
#include "FlowManager/MeasureScheduler.h"
#include "FlowManager/FlowManager.h"
#include "System/SetSystemTime.h"

using namespace System;
using namespace Controller;
using namespace Communication;
using namespace Configuration;
using namespace Measure;
using namespace Lua;
using namespace Flow;
using namespace OOLUA;
using namespace Flow;

namespace UI
{

SystemTimeWidget::SystemTimeWidget(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);

    QFont font;                           //字体
    font.setPointSize(16);                //字体大小

    this->resize(650, 400);

    QLabel *dayLabel = new QLabel();
    dayLabel->setText(tr("日期"));
    dayLabel->setFixedSize(50, 50);
    dayEdit = new QMyEdit();
    dayEdit->setFixedSize(200, 50);
    dayEdit->setPlaceholderText("示例  2023-01-01");
    connect(dayEdit,SIGNAL(editingFinished()),this,SLOT(CheckDaySlot()));

    QHBoxLayout *dayLayout = new QHBoxLayout();
    dayLayout->addWidget(dayLabel);
    dayLayout->addWidget(dayEdit);
    dayEdit->installEventFilter(CNumberKeyboard::Instance());

    dayLabel->setFont(font);
    dayEdit->setFont(font);
    dayLayout->setContentsMargins(5, 30, 0, 0);
    dayLayout->addStretch();

    QLabel *timeLabel = new QLabel();
    timeLabel->setText(tr("时间"));
    timeLabel->setFixedSize(50, 50);
    timeEdit = new QMyEdit();
    timeEdit->setFixedSize(200, 50);
    QHBoxLayout *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(timeEdit);
    timeLabel->setFont(font);
    timeEdit->setFont(font);
    timeLayout->setContentsMargins(5, 10, 0, 0);
    timeLayout->addStretch();
    timeEdit->setPlaceholderText("示例  08:00:00");

    QFont btnFont;                           //字体
    btnFont.setPointSize(18);                //字体大小
    QHBoxLayout *confirmLayout = new QHBoxLayout();
    confirmButton = new QPushButton();
    confirmButton->setObjectName("brownButton");
    confirmButton->setText(tr("确认"));
    confirmButton->setFont(btnFont);
    confirmButton->setFixedSize(120, 50);
    confirmLayout->addStretch();
    confirmLayout->addWidget(confirmButton);
    confirmLayout->setContentsMargins(0, 0, 30, 0);


    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addLayout(dayLayout);
    mainLayout->addLayout(timeLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(confirmLayout);
    this->setLayout(mainLayout);

    timeEdit->installEventFilter(CNumberKeyboard::Instance());
    OnUserChange();

    connect(dayEdit, SIGNAL(LineEditClicked()), this, SLOT(ChoseMinDay()));
    connect(timeEdit, SIGNAL(LineEditClicked()), this, SLOT(ChoseMinTime()));
    connect(confirmButton, SIGNAL(clicked(bool)), this, SLOT(SetTime()));
}

void SystemTimeWidget::SetTime()
{
    if (!FlowManager::Instance()->IsFlowEnable())  // 空闲状态
    {
        MessageDialog msg(tr("非空闲，禁止修改系统时间！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }

    QString dayValue = dayEdit->text();
    QStringList dayList = dayValue.split("-");

    if(!dayValue.isEmpty())
    {
        if(dayList.at(0).toInt() < 1970 || dayList.at(0).toInt()>= 2038)
        {
            MessageDialog msg1(tr("输入年份超范围！\n"), this,MsgStyle::ONLYOK);
            if(msg1.exec()== QDialog::Accepted)
            {
                return;
            }
        }
    }

    QString timeValue = timeEdit->text();
    QStringList timeList = timeValue.split(":");

    QString tmpPattern("^\\d{1,2}$");  //匹配1-2位整数
    QRegExp tmpRx(tmpPattern);

    if(!timeValue.isEmpty())
    {
        if(timeList.size() != 3 ||
           !tmpRx.exactMatch(timeList.at(0)) || timeList.at(0).toInt() < 0 || timeList.at(0).toInt() > 23 ||
           !tmpRx.exactMatch(timeList.at(1)) || timeList.at(1).toInt() < 0 || timeList.at(1).toInt() > 59 ||
           !tmpRx.exactMatch(timeList.at(2)) || timeList.at(2).toInt() < 0 || timeList.at(2).toInt() > 59 )
        {
            MessageDialog msg2(tr("输入时间格式不正确！\n"), this,MsgStyle::ONLYOK);
            if(msg2.exec()== QDialog::Accepted)
            {
                return;
            }
        }
    }

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    bool bridgeMode;
    luaEngine->GetLuaValue(state, "config.system.bridgeMode", bridgeMode);
    if (bridgeMode)
    {
        MainController * MC = ControllerManager::Instance()->MC;

        QDateTime localTime(QDate(dayList.at(0).toInt(), dayList.at(1).toInt(), dayList.at(2).toInt()),
                            QTime(timeList.at(0).toInt(), timeList.at(1).toInt(), timeList.at(2).toInt()));
        QDateTime utcTime(localTime.date(),localTime.time(),Qt::UTC);
        qDebug("SetSystemTime %s , %d", utcTime.toString("yyyy-MM-dd hh:mm:ss").toLatin1().data(),
               utcTime.toTime_t());
        try
        {
            if (!MC->IInstrumentInfo->SetSystemTime(utcTime.toTime_t()))
            {
                qDebug("SetSystemTime false");
            }
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
        struct tm *t_tm;
        time_t t_timet;

        t_timet=time(NULL);
        t_tm=localtime(&t_timet);
        if(timeList.size() == 3)
        {
            t_tm->tm_hour = timeList.at(0).toInt();
            t_tm->tm_min = timeList.at(1).toInt();
            t_tm->tm_sec = timeList.at(2).toInt();
        }

        if(dayList.size() == 3)
        {
            t_tm->tm_year = dayList.at(0).toInt() - 1900;
            t_tm->tm_mon = dayList.at(1).toInt() - 1;
            t_tm->tm_mday = dayList.at(2).toInt();
        }

        System::SetSystemTime::SetTime(t_tm->tm_year + 1900, t_tm->tm_mon + 1, t_tm->tm_mday,
                                       t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);
        return;
    }
}

void SystemTimeWidget::ChoseMinDay()
{
    CNumberKeyboard::Instance()->move(450,150);
    CNumberKeyboard::Instance()->show();
}

void SystemTimeWidget::ChoseMinTime()
{
    CNumberKeyboard::Instance()->move(450,180);
    CNumberKeyboard::Instance()->show();
}

void SystemTimeWidget::OnUserChange()
{
//    if((LoginDialog::userType == Administrator))
//    {
//        confirmButton->setEnabled(true);
//        dayEdit->setEnabled(true);
//        timeEdit->setEnabled(true);
//    }
//    else
//    {
//        confirmButton->setEnabled(false);
//        dayEdit->setEnabled(false);
//        timeEdit->setEnabled(false);
//    }
}

void SystemTimeWidget::showEvent(QShowEvent *event)
{
    (void)event;
    this->setFocus();
}

void SystemTimeWidget::CheckDaySlot()
{
    QString tmpPattern("^\\d{4}\\-(\\d{1,2})\\-(\\d{1,2})$");  //匹配1-2位整数
    QRegExp tmpRx(tmpPattern);

    if ("" != dayEdit->text())
    {
        if("" != dayEdit->text() && !tmpRx.exactMatch(dayEdit->text()))
        {
            MessageDialog msg(tr("请按照示例格式输入 (xxxx-xx-xx)\n"), this,MsgStyle::ONLYOK);
            if(msg.exec()== QDialog::Accepted)
            {
                dayEdit->clear();
                return;
            }
        }
        else
        {
            QString dayValue = dayEdit->text();
            QStringList dayList = dayValue.split("-");

            if(dayList.at(0).toInt() < 1970 || dayList.at(0).toInt()>= 2038)
            {
                MessageDialog msg1(tr("输入年份超范围！\n"), this,MsgStyle::ONLYOK);
                if(msg1.exec()== QDialog::Accepted)
                {
                    dayEdit->clear();
                    return;
                }
            }

            QDate date(1970,1,1);
            if (!date.setDate(dayList.at(0).toInt(),dayList.at(1).toInt(),dayList.at(2).toInt()))
            {
                MessageDialog msg1(tr("输入的年月日不存在！\n"), this,MsgStyle::ONLYOK);
                if(msg1.exec()== QDialog::Accepted)
                {
                    dayEdit->clear();
                    return;
                }
            }

            if (dayList.at(1).toInt() < 10 || dayList.at(2).toInt() < 10)
            {
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

                dateString = QString::number(dayList.at(0).toInt())+monthsign+QString::number(dayList.at(1).toInt())+daysign+QString::number(dayList.at(2).toInt());
                dayEdit->setText(dateString);
            }
        }
    }
}

SystemTimeWidget::~SystemTimeWidget()
{

}

}

