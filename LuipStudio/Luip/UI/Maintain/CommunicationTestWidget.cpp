#include "Interface/Wqimc/WqimcManager.h"
#include "UseResourceWidget.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "System/Types.h"
#include "CommunicationTestWidget.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MessageDialog.h"
#include "LuaEngine/LuaEngine.h"
#include "StatusManager/StatusManager.h"
#include "Controller/ControllerManager.h"
#include "FlowManager/FlowManager.h"
#include "Setting/SettingManager.h"
#include "UI/ControlConfig.h"

using namespace Configuration;
using namespace Measure;
using System::Uint32;
using namespace std;
using namespace OOLUA;
using namespace Lua;
using namespace Controller;
using namespace Flow;

namespace UI
{

CommunicationTestWidget::CommunicationTestWidget(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this);
    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.diagnosis.communicationCheck", itermsTable);

    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("rowCount", useRowCount);
    itermsTable.at("administratorRow", administratorRow);
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("name", m_suitName);

    QFont font;                           //字体
    font.setPointSize(14);                //字体大小

    communicationTestTableWidget = new MQtableWidget();

    communicationTestTableWidget->setColumnCount(4);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 7;
    m_tableInfo.realRow = 0;

    RowHiddenHandler::Instance()->CreateRow(communicationTestTableWidget, m_tableInfo);

    communicationTestTableWidget->setFixedSize(550, 321);

    QFont headerFont;
    headerFont.setPointSize(15);
    headerFont.setWeight(QFont::DemiBold);

    QTableWidgetItem *column0 = new QTableWidgetItem(tr("类别"));
    column0->setFont(headerFont);
    communicationTestTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem(tr(""));
    column1->setFont(headerFont);
    communicationTestTableWidget->setHorizontalHeaderItem(1, column1);

    QTableWidgetItem *column2 = new QTableWidgetItem(tr("检测项"));
    column2->setFont(headerFont);
    communicationTestTableWidget->setHorizontalHeaderItem(2, column2);

    QTableWidgetItem *column3 = new QTableWidgetItem(tr("结果"));

    column3->setFont(headerFont);
    communicationTestTableWidget->setHorizontalHeaderItem(3, column3);

    communicationTestTableWidget->horizontalHeader()->setFixedHeight(39); // 设置表头的高度
    communicationTestTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        communicationTestTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background:transparent; width: 30px;}");

    }

    QFont dataFont = communicationTestTableWidget->font();
    dataFont.setPointSize(8);
    communicationTestTableWidget->setFont(dataFont);
    communicationTestTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑


    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(communicationTestTableWidget);
    measureResultTableLayout->addStretch();
    measureResultTableLayout->setContentsMargins(0, 0, 0, 0);

    oneCheck = new QPushButton();
    oneCheck->setObjectName("brownButton");
    oneCheck->setText(tr("一键检测"));
    oneCheck->setFont(font);
    oneCheck->setFixedSize(120,50);

    QHBoxLayout *oneChechLayout = new QHBoxLayout();
    oneChechLayout->addStretch();
    oneChechLayout->addWidget(oneCheck);
    oneChechLayout->setContentsMargins(0,30,80,0);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(measureResultTableLayout);
    mainLayout->addLayout(oneChechLayout);
    mainLayout->addStretch();

    this->setLayout(mainLayout);

    communicationTestTableWidget->setColumnWidth(0,123);    //设置每行或每列的高度或宽度
    communicationTestTableWidget->setColumnWidth(1,64);
    communicationTestTableWidget->setColumnWidth(2,163);
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        communicationTestTableWidget->setColumnWidth(3, 167);
    }
    else
    {
        communicationTestTableWidget->setColumnWidth(3, 197);
    }

    for(int i = 0;i < communicationTestTableWidget->rowCount();i++)
    {
        communicationTestTableWidget->setRowHeight(i,40);
    }

    communicationTestTableWidget->setFont(font);

    SpaceInit();
    for(int i = 0;i < m_tableInfo.userRow; i++)
    {
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setObjectName("checkboxtwo");
        checkBox->setCheckState(Qt::Checked);
        communicationTestTableWidget->setCellWidget(i, 1, checkBox);
        m_checkBox.insert(make_pair(i, checkBox));
    }

    int type = SettingManager::Instance()->GetParamType();

    Uint8 row = 0;
    oolua_ipairs(itermsTable)
    {
        Table typeTable;
        String typeText;

        lua->pull(typeTable);
        typeTable.at("text", typeText);
        communicationTestTableWidget->item(row, 0)->setText(QObject::tr(typeText.c_str()));
        oolua_ipairs(typeTable)
        {
            Table itermTable;
            lua->pull(itermTable);

            int targetPrivilege = 0;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

            if (isTarget == false || (type & targetPrivilege))
            {
                String text,name;
                Lua_func_ref func;

                itermTable.at("text", text);
                communicationTestTableWidget->item(row, 2)->setText(QObject::tr(text.c_str()));

                CommunicationCheckItem item;
                itermTable.at("name", item.name);

                itermTable.at("createFlow", item.func);
                itermTable.at("writePrivilege", item.writePrivilege);
                itermTable.at("readPrivilege", item.readPrivilege);

                item.row = row;
                m_items.push_back(item);
                row++;
            }
        }
        oolua_ipairs_end()
    }
    oolua_ipairs_end()

    OnUserChange();
    connect(oneCheck,SIGNAL(clicked(bool)),this, SLOT(SlotOneCheck()));
    connect(this, SIGNAL(UpdateItemTestMessageSignal(QString,QString)),
            this, SLOT(UpdateItemTestMessageSlot(QString,QString)));

    FlowManager::Instance()->RegisterProcessMessageNotifier(this);
    FlowManager::Instance()->RegisterFinishedNotifier(this);
}

void CommunicationTestWidget::SpaceInit()
{
    for(int i = 0;i < communicationTestTableWidget->rowCount();i++)
    {
        for(int j = 0;j < communicationTestTableWidget->columnCount();j++)
        {
            communicationTestTableWidget->setItem(i, j, new QTableWidgetItem());
            communicationTestTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
            QTableWidgetItem *item = communicationTestTableWidget->item(i, j);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setFlags(Qt::ItemIsEnabled);
        }
    }
}

void CommunicationTestWidget::SlotOneCheck()
{
    if (!FlowManager::Instance()->IsFlowEnable())  // 空闲状态
    {
        MessageDialog msg(tr("请确认仪器状态是否为空闲！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()==QDialog::Accepted)
        {
            return;
    }
    }

    bool isSelected = false;
    Script *lua = LuaEngine::Instance()->GetEngine();
    for (map<Uint8, QCheckBox *>::iterator iter = m_checkBox.begin(); iter != m_checkBox.end(); ++iter)
    {
        QCheckBox *checkBox = iter->second;
        if (checkBox->checkState() == Qt::Checked)
        {
            Uint8 row = iter->first;
            communicationTestTableWidget->item(row, 3)->setText(" ");
            for (vector<CommunicationCheckItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
            {
                if ((*it).row == row)
                {
                    Lua_func_ref func = (*it).func;
                    lua->call(func);
                    isSelected = true;
                }
            }
        }
    }
    if (isSelected)
    {
        FlowManager::Instance()->StartFlow();
    }
}

CommunicationTestWidget::~CommunicationTestWidget()
{
    if (communicationTestTableWidget)
        delete communicationTestTableWidget;
}

void CommunicationTestWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        for (vector<CommunicationCheckItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
        {
            Uint8 row = (*it).row;

            communicationTestTableWidget->setRowHidden(row, false);
        }
    }
    else if(LoginDialog::userType == Administrator)
    {
        for (vector<CommunicationCheckItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
        {
            Uint8 row = (*it).row;

            if (Administrator >= (*it).readPrivilege)
            {
                communicationTestTableWidget->setRowHidden(row, false);
            }
            else
            {
                communicationTestTableWidget->setRowHidden(row, true);
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        for (vector<CommunicationCheckItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
        {
            Uint8 row = (*it).row;
            if (Maintain >= (*it).readPrivilege)
            {
                communicationTestTableWidget->setRowHidden(row, false);
            }
            else
            {
                communicationTestTableWidget->setRowHidden(row, true);
            }
        }
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(communicationTestTableWidget, m_tableInfo);
}

void CommunicationTestWidget::OnFlowActivated(String name, long time)
{
    (void)name;
    (void)time;
}

void CommunicationTestWidget::OnFlowFinished(String name, String result)
{
    emit UpdateItemTestMessageSignal(name.c_str(), QObject::tr(result.c_str()));
}

void CommunicationTestWidget::OnFlowProcessMessage(String name, String mag)
{
    emit UpdateItemTestMessageSignal(name.c_str(), QObject::tr(mag.c_str()));
}

void CommunicationTestWidget::UpdateItemTestMessageSlot(QString name, QString mag)
{
    for (vector<CommunicationCheckItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
    {
        if ((*it).name == name.toStdString())
        {
            communicationTestTableWidget->item((*it).row, 3)->setText(mag);
            qDebug() << "mag = " << mag;

            Interface::Wqimc::Api::Manager::DiagnosisResult diagnosisResult;
            diagnosisResult.__set_suit(m_suitName);
            diagnosisResult.__set_name(name.toStdString());
            diagnosisResult.__set_result(mag.toStdString());
            Interface::Wqimc::WqimcManager::Instance()->notifyDiagnosisResult(diagnosisResult);
        }
    }
}

void CommunicationTestWidget::OnFlowThreadExit()
{

}

void CommunicationTestWidget::OnUpdateWidget(UpdateEvent event, System::String message)
{
    (void)message;
    if (event == UpdateEvent::StartCommunicationTest)
    {
        if (FlowManager::Instance()->IsFlowEnable())  // 空闲状态
       {
            FlowManager::Instance()->StartFlow();
       }
    }
}

}
