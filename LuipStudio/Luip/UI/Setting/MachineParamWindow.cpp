#include "Setting/SettingManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/NumberKeyboard.h"
#include "MachineParamWindow.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/ControlConfig.h"
#include "System/ProLicenseManager.h"

using namespace System;
using namespace OOLUA;
using namespace std;
using namespace Lua;

using namespace Configuration;

namespace UI
{

bool SortMachineParamWidget( const MachineParamWidget &machineParamWidget1, const MachineParamWidget &machineParamWidget2)
{
    return machineParamWidget1.index < machineParamWidget2.index;//升序排列
}

MachineParamWindow::MachineParamWindow(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);

    listView = new QListView();
    listView->setFixedHeight(400);
    listView->setFixedWidth(101);
    listView->setObjectName(QStringLiteral("commonListView"));
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QVBoxLayout *listViewLayout = new QVBoxLayout();
    listViewLayout->addWidget(listView);
    listViewLayout->setContentsMargins(0, 11, 0, 0);
    listViewLayout->addStretch();

    stackedWidget = new QStackedWidget();
    stackedWidget->setFixedHeight(425);
    stackedWidget->setFixedWidth(650);
    stackedWidget->setObjectName(QStringLiteral("stackedWidget"));

    QHBoxLayout *measureLayout = new QHBoxLayout();
    measureLayout->addWidget(stackedWidget);
    measureLayout->addStretch();
    measureLayout->setContentsMargins(10, 0, 0, 0);

    QVBoxLayout *vstackedWidgetLayout = new QVBoxLayout();
    vstackedWidgetLayout->addLayout(measureLayout);
    vstackedWidgetLayout->addStretch();
    vstackedWidgetLayout->setContentsMargins(0, 3, 0, 0);

    autoMeasure = new AutoMeasureWidget();
    stackedWidget->addWidget(autoMeasure);
    m_strList<< tr("测量排期");
	m_privilegeList<<RoleType::General;
	
    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.profile", itermsTable);

    int type = SettingManager::Instance()->GetParamType();

    oolua_pairs(itermsTable)
    {
        Table itermTable;
        lua->pull(itermTable);
        String name;
        itermTable.at("name", name);
        if (name != "hardwareParamIterms" && name != "scheduler"&& name != "curve")
        {
            int targetPrivilege = 0;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

            if (isTarget == false || (type & targetPrivilege))
            {
                MachineParamWidget machineParamWidget;
				String profileTableName = "setting.ui.profile";
				
				profileTableName += "." + name;
				itermTable.at("text", machineParamWidget.text);
                itermTable.at("index", machineParamWidget.index);
				
				machineParamWidget.parameterWidget = new ParameterWidget(profileTableName);

                itermTable.at("readPrivilege", machineParamWidget.privilege);
				
                itermTable.at("isMeaParaml", machineParamWidget.isMeaParaml);
                m_machineParamWidget.push_back(machineParamWidget);
            }
        }
    }
    oolua_pairs_end()

    std::sort(m_machineParamWidget.begin(), m_machineParamWidget.end(), SortMachineParamWidget);
    for (vector<MachineParamWidget>::iterator itx = m_machineParamWidget.begin(); itx != m_machineParamWidget.end(); itx++)
    {
        if (itx->parameterWidget)
        {
            stackedWidget->addWidget(itx->parameterWidget);
            m_strList << QObject::tr(itx->text.c_str());
			m_privilegeList << itx->privilege;
        }
    }

    model = new QStringListModel(m_strList);
    listView->setGridSize(QSize(50,50));
    listView->setModel(model);

    QFont listFont = listView->font();
    listFont.setPointSize(16);
    listFont.setWeight(65);
    listView->setFont(listFont);

    connect(listView, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));

    QHBoxLayout *mainlayout = new QHBoxLayout();

    mainlayout->addLayout(listViewLayout);
    mainlayout->addLayout(vstackedWidgetLayout);
    mainlayout->setContentsMargins(10, 5, 0, 0);

    stackedWidget->setCurrentWidget(autoMeasure);

    this->setLayout(mainlayout);

    ChangeBottomStatus();
}

void MachineParamWindow::itemClicked(QModelIndex index)
{
    if(m_strList.at(0) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(autoMeasure);
    }
    else
    {
        for (int i = 1; i < m_strList.size(); i++)
        {
            if (m_strList.at(i) == index.data().toString())
            {
                 stackedWidget->setCurrentWidget(m_machineParamWidget.at(i - 1).parameterWidget);
            }
        }
    }
    CNumberKeyboard::Instance()->hide();
}

void MachineParamWindow::OnUserChange()
{
    ChangeBottomStatus();
}

void MachineParamWindow::ChangeBottomStatus()
{
    if(ProLicenseManager::Instance()->GetProLicense())
    {
        stackedWidget->setCurrentWidget(autoMeasure);
        QModelIndex index = model->index(0,0);
        listView->setCurrentIndex(index);

        if(LoginDialog::GetUserPrivilege(Authority::MeasureScheduler))
        {
            listView->setRowHidden(0, false);
        }else{
            listView->setRowHidden(0, true);
        }
        if(LoginDialog::GetUserPrivilege(Authority::MeasureParam))
        {
            listView->setRowHidden(1, false);
        }else{
            listView->setRowHidden(1, true);
        }
        if(LoginDialog::GetUserPrivilege(Authority::Interconnection))
        {
            listView->setRowHidden(2, false);
        }else{
            listView->setRowHidden(2, true);
        }
        if(LoginDialog::GetUserPrivilege(Authority::SystemParam))
        {
            listView->setRowHidden(3, false);
        }else{
            listView->setRowHidden(3, true);
        }
    }
    else
    {
        if(LoginDialog::userType == Super)
        {
            stackedWidget->setCurrentWidget(autoMeasure);
            QModelIndex index = model->index(0,0);
            listView->setCurrentIndex(index);

            for (int i = 0; i < m_strList.size(); i++)
            {
                listView->setRowHidden(i, false);
            }
        }
        else if(LoginDialog::userType == Administrator)
        {
            stackedWidget->setCurrentWidget(autoMeasure);
            QModelIndex index = model->index(0,0);
            listView->setCurrentIndex(index);

            for (int i = 0; i < m_strList.size(); i++)
            {
                listView->setRowHidden(i, false);
                if(m_privilegeList.at(i)> Administrator)
                {
                    listView->setRowHidden(i, true);
                }
            }
        }
        else if(LoginDialog::userType == Maintain)
        {
            stackedWidget->setCurrentWidget(autoMeasure);
            QModelIndex index = model->index(0,0);
            listView->setCurrentIndex(index);

            for (int i = 0; i < m_strList.size(); i++)
            {
                listView->setRowHidden(i, false);
                if(m_privilegeList.at(i)> Maintain)
                {
                    listView->setRowHidden(i, true);
                }
            }
        }
        else if(LoginDialog::userType == General)
        {
            LuaEngine* luaEngine = LuaEngine::Instance();
            lua_State* state = luaEngine->GetThreadState();

            bool isDisplayMeaParamInGeneral;
            luaEngine->GetLuaValue(state, "config.system.displayMeaParamInGeneral", isDisplayMeaParamInGeneral);

            listView->setRowHidden(0, true);
            int setCurrentIndex = 0;
            for (int i = m_strList.size() - 2; i >= 0 ; i--)
            {
                if (isDisplayMeaParamInGeneral && m_machineParamWidget.at(i).isMeaParaml && m_machineParamWidget.at(i).privilege == Maintain)
                {
                    listView->setRowHidden(i + 1, false);
                    setCurrentIndex = i;
                }
                else
                {
                    listView->setRowHidden(i + 1, true);
                }
            }

            stackedWidget->setCurrentWidget(m_machineParamWidget.at(setCurrentIndex).parameterWidget);
            QModelIndex index = model->index(0, setCurrentIndex + 1);
            listView->setCurrentIndex(index);
        }
    }
}

MachineParamWindow::~MachineParamWindow()
{
    if (autoMeasure)
        delete autoMeasure;
    if (!m_machineParamWidget.empty())
    {
        vector<MachineParamWidget>::iterator itx;
        for (itx = m_machineParamWidget.begin(); itx != m_machineParamWidget.end(); itx++)
        {
            if (itx->parameterWidget)
                delete itx->parameterWidget;
        }
    }
}

}
