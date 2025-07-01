#include "UseResourceWidget.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QSignalMapper>
#include "Log.h"
#include "Common.h"
#include "System/Exception.h"
#include "HardwareTestWidget.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MessageDialog.h"
#include "LuaEngine/LuaEngine.h"
#include "StatusManager/StatusManager.h"
#include "Controller/ControllerManager.h"
#include "FlowManager/FlowManager.h"
#include "FlowManager/MeasureScheduler.h"
#include "Setting/SettingManager.h"
#include "UI/ControlConfig.h"

using namespace Configuration;
using namespace System;
using namespace Lua;
using namespace std;
using namespace OOLUA;
using namespace Measure;
using namespace Controller;
using namespace Flow;


namespace UI
{

HardwareTestWidget::HardwareTestWidget(QWidget *parent) :
        QWidget(parent),m_isOEM(0)
{
    MaintainUserChange::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this);
    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();
    OOLUA::Lua_func_ref showCheck;
    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.operation.hardwareTest", itermsTable);

    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("rowCount", useRowCount);
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("administratorRow", administratorRow);
    itermsTable.safe_at("isShowCheck", showCheck);

    OOLUA::Lua_func_ref checkOEM;
    if (itermsTable.safe_at("checkOEM", checkOEM))
    {
        lua->call(checkOEM);
        lua->pull(m_isOEM);
    }

    QFont font;                           //字体
    font.setPointSize(14);                //字体大小

    this->resize(650,430);

    hardwareTestTableWidget = new QTableWidget();

    hardwareTestTableWidget->setColumnCount(3);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 8;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(hardwareTestTableWidget, m_tableInfo);

    hardwareTestTableWidget->setFixedSize(528,400);

    QFont headerFont;
    headerFont.setPointSize(15);
    headerFont.setWeight(QFont::DemiBold);

    QTableWidgetItem *column0 = new QTableWidgetItem(tr("类别"));
    column0->setFont(headerFont);
    hardwareTestTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column2 = new QTableWidgetItem(tr("测试项"));
    column2->setFont(headerFont);
    hardwareTestTableWidget->setHorizontalHeaderItem(1, column2);

    QTableWidgetItem *column3 = new QTableWidgetItem(tr("状态"));
    column3->setFont(headerFont);
    hardwareTestTableWidget->setHorizontalHeaderItem(2, column3);

    hardwareTestTableWidget->horizontalHeader()->setSectionsClickable(false); //设置表头不可点击（默认点击后进行排序）
    hardwareTestTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    hardwareTestTableWidget->horizontalHeader()->setHighlightSections(false); // 点击表时不对表头行光亮（获取焦点）
    hardwareTestTableWidget->horizontalHeader()->setFixedHeight(40); // 设置表头的高度
    hardwareTestTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");

    QFont dataFont = hardwareTestTableWidget->font();
    dataFont.setPointSize(15);
    hardwareTestTableWidget->setFont(dataFont);
    hardwareTestTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑
    hardwareTestTableWidget->setFrameShape(QFrame::StyledPanel); //设置无边框
    hardwareTestTableWidget->setShowGrid(true); //设置不显示格子线
    hardwareTestTableWidget->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色

   // 设置水平滚动条样式
    hardwareTestTableWidget->horizontalScrollBar()->setStyleSheet
            ("QScrollBar{background:transparent; height:1px;}"
             "QScrollBar::handle{background:lightgray; border:1px solid transparent; border-radius:1px;}"
             "QScrollBar::handle:hover{background:gray;}"
             "QScrollBar::sub-line{background:transparent;}"
             "QScrollBar::add-line{background:transparent;}");
    hardwareTestTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);// 设置不可拖拽
    hardwareTestTableWidget->horizontalScrollBar()->hide();

    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        hardwareTestTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background-color:rgb(200,200,200); width: 45;}"
                 "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background-color:rgb(240,240,240)}");
    }

    hardwareTestTableWidget->verticalHeader()->setVisible(false); // 隐藏列表头
    hardwareTestTableWidget->horizontalScrollBar()->setVisible(false);
    hardwareTestTableWidget->horizontalScrollBar()->setDisabled(true);

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(hardwareTestTableWidget);
    measureResultTableLayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *leftLayout = new QVBoxLayout();

    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    this->setLayout(mainLayout);

    hardwareTestTableWidget->setColumnWidth(0,160);    //设置每行或每列的高度或宽度
    hardwareTestTableWidget->setColumnWidth(1,200);
    hardwareTestTableWidget->setColumnWidth(2,120);
    if (m_tableInfo.userRow > 12)
    {
        hardwareTestTableWidget->setColumnWidth(2, 120);
    }
    else
    {
        hardwareTestTableWidget->setColumnWidth(2, 150);
    }

    for(int i = 0;i<hardwareTestTableWidget->rowCount();i++)
    {
        hardwareTestTableWidget->setRowHeight(i,60);
    }

    hardwareTestTableWidget->setFont(font);
    SpaceInit();

    int type = SettingManager::Instance()->GetParamType();

    QFont btnFont;                           //字体
    btnFont.setPointSize(16);                //字体大小
    btnFont.setWeight(60);

    QFont statusFont;
    statusFont.setPointSize(16);
    statusFont.setWeight(75);

    QSignalMapper *signalMapper = new QSignalMapper(this);
    Uint8 row = 0;
    oolua_ipairs(itermsTable)
    {
        Table typeTable;
        String typeText;

        lua->pull(typeTable);
        typeTable.at("text", typeText);
        hardwareTestTableWidget->item(row, 0)->setText(QObject::tr(typeText.c_str()));
        hardwareTestTableWidget->item(row, 0)->setFont(btnFont);
        oolua_ipairs(typeTable)
        {
            Table itermTable;
            lua->pull(itermTable);

            int targetPrivilege = 0;
            bool checkResult;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
            bool isShowCheck = itermTable.safe_at("showCheck", checkResult);
            if (isShowCheck == false)
            {
                checkResult = true;
            }
            else
            {
                lua->call(showCheck);
                lua->pull(checkResult);
            }
            if ((isTarget == false || (type & targetPrivilege)) && checkResult == true)
            {
                HardwareTestItem hardwareTestItem;
                String text;
                bool analogConfig = false;
				itermTable.at("name", hardwareTestItem.name);
                itermTable.at("text", text);
                hardwareTestTableWidget->item(row, 2)->setText("关闭");
                hardwareTestTableWidget->item(row, 2)->setFont(statusFont);
                hardwareTestTableWidget->item(row, 2)->setTextColor(QColor(255, 0, 0));

                QPushButton *button = new QPushButton();
                button->setObjectName("brownButton");
                button->setFixedSize(198,57);
                button->setFont(btnFont);
                button->setText(QObject::tr(text.c_str()));
                m_buttonList.append(button);
                hardwareTestTableWidget->setCellWidget(row,1,button);
                connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
                signalMapper->setMapping(button, row);

                itermTable.at("createFlow", hardwareTestItem.createFlow);
                hardwareTestItem.isButtonPress = false;
                itermTable.at("writePrivilege", hardwareTestItem.writePrivilege);
                itermTable.at("readPrivilege", hardwareTestItem.readPrivilege);

                if (itermTable.safe_at("analogConfig", analogConfig))
                {
                    if (analogConfig && m_isOEM == 1)
                    {
                        hardwareTestItem.writePrivilege = RoleType::Super;
                        hardwareTestItem.readPrivilege = RoleType::Super;
                    }
                }

                m_items.insert(make_pair(row, hardwareTestItem));
                row++;
            }
        }
        oolua_ipairs_end()
    }
    oolua_ipairs_end()

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(SlotButton(int)));

    if(hardwareTestTableWidget->rowCount() > row && hardwareTestTableWidget->rowCount() > 10)
    {
        hardwareTestTableWidget->setRowCount(row);
    }

    OnUserChange();

}

void HardwareTestWidget::SpaceInit()
{
    for(int i = 0;i < hardwareTestTableWidget->rowCount();i++)
    {
        for(int j = 0;j < hardwareTestTableWidget->columnCount();j++)
        {
            hardwareTestTableWidget->setItem(i, j, new QTableWidgetItem());
            hardwareTestTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
            QTableWidgetItem *item = hardwareTestTableWidget->item(i, j);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setFlags(Qt::ItemIsEnabled);
        }
    }
}

bool HardwareTestWidget::IsHardwareTestStatus()
{
    bool status = false;

    if (StatusManager::Instance()->GetStatus().name == QObject::tr("HardwareTest").toStdString())
    {
        status = true;
    }

    return status;
}

void HardwareTestWidget::SlotButton(int index)
{
    QThread::msleep(100);
    StatusManager *statusManager = StatusManager::Instance();
    bool ret;

    if (!ControllerManager::Instance()->IsAllConnected())
    {
        return;
    }

    if (FlowManager::Instance()->IsFlowRunning())
    {
        MessageDialog msg(tr("请确认仪器状态是否为空闲！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()==QDialog::Accepted)
        {
            return;
        }
    }

    if (m_items.count(index))
    {
        Script *lua = LuaEngine::Instance()->GetEngine();
        HardwareTestItem &hardwareTestItem = m_items[index];
        if (!hardwareTestItem.isButtonPress)
        {
            if (statusManager->IsIdleStatus() || this->IsHardwareTestStatus())
            {
                lua->call(hardwareTestItem.createFlow, true);
                lua->pull(ret);
                if(ret) //操作成功才改变状态
                {
                    this->ChangeButtonStatus(index, true);
                    hardwareTestItem.isButtonPress = true;
                }
            }
            else
            {
                MessageDialog msg(tr("请确认仪器状态是否为空闲！\n"), this,MsgStyle::ONLYOK);
                if(msg.exec()==QDialog::Accepted)
                {
                    return;
                }
            }
        }
        else
        {
            if (this->IsHardwareTestStatus())
            {
                lua->call(hardwareTestItem.createFlow, false);
                lua->pull(ret);
                if(ret)
                {
                    this->ChangeButtonStatus(index, false);
                    hardwareTestItem.isButtonPress = false;
                }
            }
        }
    }

    RunStatus testStatus("HardwareTest", "硬件测试");
    for(int i = 0; i < m_items.size(); i++)
    {
        HardwareTestItem &item = m_items[i];
        if(!item.isButtonPress)
        {
            if(i == m_items.size()-1)
            {
                statusManager->ResetDeviceStatus();  //回归空闲/故障状态
                logger->info("硬件测试结束");
                MeasureScheduler::Instance()->OnFlowThreadExit();
            }
        }
        else
        {
            if(!this->IsHardwareTestStatus())
            {
                statusManager->SetStatus(testStatus);
                MeasureScheduler::Instance()->OnFlowActivated("HardwareTest", 0);
            }
            break;
        }
    }
}

void HardwareTestWidget::ChangeButtonStatus(int index, bool status)
{
    if (status)
    {
        hardwareTestTableWidget->item(index, 2)->setText("开启");
        hardwareTestTableWidget->item(index, 2)->setTextColor(QColor(55, 200, 55));
    }
    else
    {
        hardwareTestTableWidget->item(index, 2)->setText("关闭");
        hardwareTestTableWidget->item(index, 2)->setTextColor(QColor(255, 0, 0));
    }
}

void HardwareTestWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        for (map<Uint8, HardwareTestItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;

            hardwareTestTableWidget->setRowHidden(row, false);
        }
    }
    else if(LoginDialog::userType == Administrator)
    {
        for (map<Uint8, HardwareTestItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            HardwareTestItem &hardwareTestItem = iter->second;
            if (Administrator >= hardwareTestItem.readPrivilege)
            {
                hardwareTestTableWidget->setRowHidden(row, false);
            }
            else
            {
                hardwareTestTableWidget->setRowHidden(row, true);
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        for (map<Uint8, HardwareTestItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            HardwareTestItem &hardwareTestItem = iter->second;
            if (Maintain >= hardwareTestItem.readPrivilege)
            {
                hardwareTestTableWidget->setRowHidden(row, false);
            }
            else
            {
                hardwareTestTableWidget->setRowHidden(row, true);
            }
        }
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(hardwareTestTableWidget, m_tableInfo);
}

HardwareTestWidget::~HardwareTestWidget()
{   
    if (hardwareTestTableWidget)
        delete hardwareTestTableWidget;
}

void HardwareTestWidget::OnUpdateWidget(UpdateEvent event, System::String message)
{
    if (event == UpdateEvent::StartHardwareTestSuccess)  //硬件测试操作成功事件UI更新
    {
        StatusManager *statusManager = StatusManager::Instance();
        if (! ControllerManager::Instance()->IsAllConnected())
        {
            return;
        }

        QString value = message.c_str();
        QStringList messageList = value.split(QRegExp("[,]"));
        for (std::map<System::Uint8, HardwareTestItem>::iterator iter = m_items.begin(); iter != m_items.end(); iter++)
        {
            HardwareTestItem &hardwareTestItem = iter->second;
            int index = iter->first;
            if (messageList.at(0).toStdString() == hardwareTestItem.name)
            {
                if(messageList.at(1).toStdString() == "1") //操作为打开
                {
                    this->ChangeButtonStatus(index, true);
                    hardwareTestItem.isButtonPress = true;
                }
                else if(messageList.at(1).toStdString() == "0") //操作为关闭
                {
                    this->ChangeButtonStatus(index, false);
                    hardwareTestItem.isButtonPress = false;
                }
                else
                {
                    return;
                }

                RunStatus testStatus("HardwareTest", "硬件测试");
                for(int i = 0; i < m_items.size(); i++)
                {
                    HardwareTestItem &item = m_items[i];
                    if(!item.isButtonPress)
                    {
                        if(i == m_items.size()-1)
                        {
                            statusManager->ResetDeviceStatus();
							MeasureScheduler::Instance()->OnFlowThreadExit();
                        }
                    }
                    else
                    {
                        if(!this->IsHardwareTestStatus())
                        {
                            statusManager->SetStatus(testStatus);
							MeasureScheduler::Instance()->OnFlowActivated("HardwareTest", 0);
                        }
                        break;
                    }
                }
            }
        }
    }
    else if (event == UpdateEvent::StopHardwareTest)
    {
        Script *lua = LuaEngine::Instance()->GetEngine();
        StatusManager *statusManager = StatusManager::Instance();
        for (std::map<System::Uint8, HardwareTestItem>::iterator iter = m_items.begin(); iter != m_items.end(); iter++)
        {
            HardwareTestItem &hardwareTestItem = iter->second;
            int index = iter->first;
            if (true == hardwareTestItem.isButtonPress)
            {
                if (this->IsHardwareTestStatus())
                {
                    bool ret;
                    lua->call(hardwareTestItem.createFlow, false);
                    lua->pull(ret);
                    if(ret)
                    {
                        this->ChangeButtonStatus(index, false);
                        hardwareTestItem.isButtonPress = false;
                    }
                }
            }
        }
        RunStatus testStatus("HardwareTest", "硬件测试");
        for(int i = 0; i < m_items.size(); i++)
        {
            HardwareTestItem &item = m_items[i];
            if(!item.isButtonPress)
            {
                if(i == m_items.size()-1)
                {
                    statusManager->ResetDeviceStatus();
					MeasureScheduler::Instance()->OnFlowThreadExit();
                }
            }
            else
            {
                if(!this->IsHardwareTestStatus())
                {
                    statusManager->SetStatus(testStatus);
					MeasureScheduler::Instance()->OnFlowActivated("HardwareTest", 0);
                }
                break;
            }
        }
    }
}
}
