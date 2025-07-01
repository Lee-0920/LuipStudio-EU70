#include "UI/Frame/LoginDialog.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/NumberKeyboard.h"
#include "RemoteMaintainWindow.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/ControlConfig.h"
#include "System/ProLicenseManager.h"

using namespace System;

#define MAX_LISTVIEW 8         //需要调节界面宽度的维护项数量阀值（即在此数量及以下不需要滚动条）

using namespace Lua;
using namespace OOLUA;

namespace UI
{

RemoteMainTainWindow::RemoteMainTainWindow(QWidget *parent) :
        QWidget(parent)
{
    m_hasCombineOperation = HasCombineOperation();
    if (!m_hasCombineOperation)
    {
        logger->warn("没有组合操作界面 请检查代码完整性");
    }

    MaintainUserChange::Instance()->Register(this);

    listView = new QListView();
    listView->setFixedHeight(402);
    listView->setFixedWidth(146);
    listView->setObjectName(QStringLiteral("commonListView"));
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QVBoxLayout *listViewLayout = new QVBoxLayout();
    listViewLayout->addWidget(listView);
    listViewLayout->setContentsMargins(0, 11, 0, 0);
    listViewLayout->addStretch();

    stackedWidget = new QStackedWidget();
    stackedWidget->setFixedHeight(425);
    stackedWidget->setFixedWidth(620);
    stackedWidget->setObjectName(QStringLiteral("stackedWidget"));

    QHBoxLayout *stackedWidgetLayout = new QHBoxLayout();
    stackedWidgetLayout->addWidget(stackedWidget);
    stackedWidgetLayout->addStretch();
    stackedWidgetLayout->setContentsMargins(10, 0, 0, 0);

    QVBoxLayout *vstackedWidgetLayout = new QVBoxLayout();
    vstackedWidgetLayout->addLayout(stackedWidgetLayout);
    vstackedWidgetLayout->addStretch();
    vstackedWidgetLayout->setContentsMargins(0, 3, 0, 0);

    m_strList<<tr("维护")<<tr("管道操作")<<tr("硬件测试")<<tr("试剂管理")<<tr("硬件校准")<<tr("组合操作")<<tr("升级")
                    <<tr("耗材管理")<<tr("通信检测")<<tr("智能诊断");

    model = new QStringListModel(m_strList);
    listView->setGridSize(QSize(50,50));
    listView->setModel(model);

    QFont listFont = listView->font();
    listFont.setPointSize(16);
    listFont.setWeight(65);
    listView->setFont(listFont);

    listView->verticalScrollBar()->setStyleSheet
            ("QScrollBar{background-color:rgb(200,200,200); width: 36;}"
             "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background-color:rgb(240,240,240)}");

    Maintenance = new MaintenanceWidget();
//    MaintenanceCombine = new MaintenanceCombineWidget();
    reagentManager = new ReagentManagerWidget();
    userResource = new UseResourceWidget();
    pipeMannger = new PipeManngerWidget();
    hardwareCheck = new HardwareCheckWidget();
    intellectCheck = new IntellectCheck();
    communicationTest = new CommunicationTestWidget();
    hardwareTest = new HardwareTestWidget();
    combineOpsManager = new CombineOpsManagerWidget();
    updateWidget = new UpdateWidget();

    stackedWidget->addWidget(Maintenance);
//    stackedWidget->addWidget(MaintenanceCombine);
    stackedWidget->addWidget(pipeMannger);
    stackedWidget->addWidget(hardwareTest);
    stackedWidget->addWidget(reagentManager);
    stackedWidget->addWidget(hardwareCheck);
    stackedWidget->addWidget(combineOpsManager);
    stackedWidget->addWidget(updateWidget);
    stackedWidget->addWidget(userResource);
    stackedWidget->addWidget(communicationTest);
    stackedWidget->addWidget(intellectCheck);

    stackedWidget->setCurrentWidget(Maintenance);

    connect(listView, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
    QHBoxLayout *mainlayout = new QHBoxLayout();

    mainlayout->addLayout(listViewLayout);
    mainlayout->addLayout(vstackedWidgetLayout);
    mainlayout->setContentsMargins(10, 5, 0, 0);

    this->setLayout(mainlayout);

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    RoleType privilege;
    luaEngine->GetLuaValue(state, "setting.ui.operation.maintain.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.operation.maintainCombine.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.operation.liquidOperator.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.operation.hardwareTest.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.reagentManager.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.profile.hardwareParamIterms.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.operation.combineOperator.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.update.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.useResourceWidget.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.diagnosis.communicationCheck.readPrivilege", privilege);
    m_privilegeList << privilege;

    luaEngine->GetLuaValue(state, "setting.ui.diagnosis.smartDetect.readPrivilege", privilege);
    m_privilegeList << privilege;

    ChangeBottomStatus();

    connect(this,SIGNAL(SignalOneKeyChangeReagentWindow()), reagentManager, SLOT(SlotOneKeyChangeReagent()));
}

void RemoteMainTainWindow::itemClicked(QModelIndex index)
{
    if(m_strList.at(0) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(Maintenance);
    }
//    else if(m_strList.at(1) == index.data().toString())
//    {
//        stackedWidget->setCurrentWidget(MaintenanceCombine);
//    }
    else if(m_strList.at(1) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(pipeMannger);
    }
    else if(m_strList.at(2) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(hardwareTest);
    }
    else if(m_strList.at(3) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(reagentManager);
    }
    else if(m_strList.at(4) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(hardwareCheck);
    }
    else if(m_strList.at(5) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(combineOpsManager);
    }
    else if(m_strList.at(6) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(updateWidget);
    }
    else if(m_strList.at(7) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(userResource);
    }
    else if (m_strList.at(8) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(communicationTest);
    }
    else
    {
        stackedWidget->setCurrentWidget(intellectCheck);
    }

    CNumberKeyboard::Instance()->hide();
}

RemoteMainTainWindow::~RemoteMainTainWindow()
{
    if (Maintenance)
        delete Maintenance;
//    if (MaintenanceCombine)
//        delete MaintenanceCombine;
    if (reagentManager)
        delete reagentManager;
    if (userResource)
        delete userResource;
    if (pipeMannger)
        delete pipeMannger;
    if (hardwareCheck)
        delete hardwareCheck;
    if (hardwareTest)
        delete hardwareTest;
    if (intellectCheck)
        delete intellectCheck;
    if (communicationTest)
        delete communicationTest;
    if (combineOpsManager)
        delete combineOpsManager;
    if (updateWidget)
        delete updateWidget;
}

void RemoteMainTainWindow::OnUserChange()
{
    ChangeBottomStatus();
}

void RemoteMainTainWindow::ConfigInit(int showListNum)
{
    if(showListNum > MAX_LISTVIEW)
    {
        listView->setFixedWidth(146);
    }
    else
    {
        listView->setFixedWidth(101);
    }
}

bool RemoteMainTainWindow::HasCombineOperation()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    Table uiOps;
    luaEngine->GetLuaValue(state, "setting.ui.operation", uiOps);
    Table combineOperation;
    return uiOps.safe_at("combineOperator", combineOperation);

}

void RemoteMainTainWindow::ChangeBottomStatus()
{
    int showListNum = 0;
    if(ProLicenseManager::Instance()->GetProLicense())
    {
        stackedWidget->setCurrentWidget(Maintenance);
        QModelIndex index = model->index(0,0);
        listView->setCurrentIndex(index);

        for (int i = 0; i < m_strList.size(); i++)
        {
            int index = (int)Authority::Maintain + i;
            if(!LoginDialog::GetUserPrivilege((Authority)index))
            {
                listView->setRowHidden(i, true);
            }
            else
            {
                listView->setRowHidden(i, false);
                showListNum++;
            }
        }
    }
    else
    {
        if(LoginDialog::userType == Super)
        {
            stackedWidget->setCurrentWidget(Maintenance);
            QModelIndex index = model->index(0,0);
            listView->setCurrentIndex(index);

            for (int i = 0; i < m_strList.size(); i++)
            {
                listView->setRowHidden(i, false);
                showListNum++;
            }
        }
        else if(LoginDialog::userType == Administrator)
        {
            stackedWidget->setCurrentWidget(Maintenance);
            QModelIndex index = model->index(0,0);
            listView->setCurrentIndex(index);

            for (int i = 0; i < m_strList.size(); i++)
            {
                if(m_privilegeList.at(i)> Administrator)
                {
                    listView->setRowHidden(i, true);
                }
                else
                {
                    listView->setRowHidden(i, false);
                    showListNum++;
                }
            }
        }
        else if(LoginDialog::userType == Maintain)
        {
            stackedWidget->setCurrentWidget(Maintenance);
            QModelIndex index = model->index(0,0);
            listView->setCurrentIndex(index);

            for (int i = 0; i < m_strList.size(); i++)
            {
                if(m_privilegeList.at(i)> Maintain)
                {
                    listView->setRowHidden(i, true);
                }
                else
                {
                    listView->setRowHidden(i, false);
                    showListNum++;
                }
            }
        }
        else if(LoginDialog::userType == General)
        {
            for (int i = 0; i < m_strList.size(); i++)
            {
                listView->setRowHidden(i, true);
                showListNum++;
            }
        }
    }

    this->ConfigInit(showListNum);
}

}

