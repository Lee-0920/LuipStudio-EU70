
#include "Log.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MessageDialog.h"
#include "StatusManager/StatusManager.h"
#include "FlowManager/FlowManager.h"
#include "LuaEngine/LuaEngine.h"
#include "MaintenanceWidget.h"
#include "Setting/SettingManager.h"
#include "UI/ControlConfig.h"


using namespace Configuration;
using namespace OOLUA;
using namespace Measure;
using namespace std;
using namespace Flow;
using namespace Lua;



namespace UI
{

MaintenanceWidget::MaintenanceWidget(QWidget *parent) :
        QWidget(parent),m_isOEM(0)
{
    MaintainUserChange::Instance()->Register(this);

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.operation.maintain", itermsTable);

    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("rowCount", useRowCount);
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("administratorRow", administratorRow);

    OOLUA::Lua_func_ref checkOEM;
    if (itermsTable.safe_at("checkOEM", checkOEM))
    {
        lua->call(checkOEM);
        lua->pull(m_isOEM);
    }

    m_curRunFlow.isStart = false;
    m_curRunFlow.flowName = "";
    m_curRunFlow.flowTimes = 0 ;

    QFont labelFont;
    labelFont.setPointSize(11);
    m_flowMessage = new QLabel();
    m_flowMessage->setFixedSize(160,40);
    m_flowMessage->setFont(labelFont);
    m_flowMessage->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_flowMessage->setText("");
    m_flowMessage->setStyleSheet("QLabel{border:0px solid rgb(100, 100, 100);}");

    maintenanceTableWidget = new MQtableWidget();
    maintenanceTableWidget->setColumnCount(2);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 8;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(maintenanceTableWidget, m_tableInfo);

    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        maintenanceTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background-color:rgb(200,200,200); width: 45;}"
                 "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background-color:rgb(240,240,240)}");
    }

    maintenanceTableWidget->setFixedSize(415,400);

    QFont headerFont;
    headerFont.setPointSize(15);
    headerFont.setWeight(QFont::DemiBold);

    QTableWidgetItem *column0 = new QTableWidgetItem(tr("项目"));
    column0->setFont(headerFont);
    maintenanceTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem(tr("次数"));
    column1->setFont(headerFont);
    maintenanceTableWidget->setHorizontalHeaderItem(1, column1);

    maintenanceTableWidget->horizontalHeader()->setFixedHeight(38); // 设置表头的高度

    QFont headDataFont = maintenanceTableWidget->font();
    headDataFont.setPointSize(FONT_SIZE_SETTINGS(12));

    QFont dataFont = maintenanceTableWidget->font();
    dataFont.setPointSize(17);

    maintenanceTableWidget->horizontalHeader()->setFont(headDataFont);
    maintenanceTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");
    maintenanceTableWidget->setFont(dataFont);
//    maintenanceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑

    QHBoxLayout *mainLayout = new QHBoxLayout();
    QHBoxLayout *leftLayout = new QHBoxLayout();
    leftLayout->addWidget(maintenanceTableWidget, 0, Qt::AlignTop | Qt::AlignLeft);

    stopButton = new QPushButton();
    stopButton->setObjectName("redButton");
    stopButton->setText(tr("停止"));
    stopButton->setFont(dataFont);
    stopButton->setFixedSize(130,54); 

    QHBoxLayout *stopButtonLayout = new QHBoxLayout();
    stopButtonLayout->addWidget(stopButton);
    stopButtonLayout->addStretch();
    stopButtonLayout->setContentsMargins(35, 0, 0, 0);  

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addStretch();
    rightLayout->addLayout(stopButtonLayout);
    rightLayout->addStretch();
    rightLayout->addWidget(m_flowMessage,0,Qt::AlignBottom | Qt::AlignRight);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    this->setLayout(mainLayout);
    for(int i = 0;i < maintenanceTableWidget->rowCount();i++)
    {
        maintenanceTableWidget->setRowHeight(i, 60);
    }

    if (m_tableInfo.userRow <= 10)
    {
        maintenanceTableWidget->setColumnWidth(0, 270);
    }
    else
    {
        maintenanceTableWidget->setColumnWidth(0, 240);
    }
    maintenanceTableWidget->setColumnWidth(1, 128);

    SpaceInit();

    int type = SettingManager::Instance()->GetParamType();

    Uint8 row = 0;
    oolua_ipairs(itermsTable)
    {
        Table itermTable;
        lua->pull(itermTable);

        int targetPrivilege = 0;
        bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

        if (isTarget == false || (type & targetPrivilege))
        {
            MaintenanceItem maintenanceItem;
            bool rangConfig = false;
            itermTable.at("text", maintenanceItem.text);
            itermTable.at("writePrivilege", maintenanceItem.writePrivilege);
            itermTable.at("readPrivilege", maintenanceItem.readPrivilege);
            itermTable.safe_at("createFlow", maintenanceItem.maintenanceFunc);

            if (itermTable.safe_at("rangConfig", rangConfig))
            {
                if (rangConfig && m_isOEM == 1)
                {
                    maintenanceItem.writePrivilege = RoleType::Super;
                    maintenanceItem.readPrivilege = RoleType::Super;
                }
            }

            maintenanceTableWidget->item(row, 1)->setText("1");//显示名称
            m_items.insert(make_pair(row, maintenanceItem));
            row++;

        }
    }
    oolua_ipairs_end()

    if(maintenanceTableWidget->rowCount() > row && maintenanceTableWidget->rowCount() > 10)
    {
        maintenanceTableWidget->setRowCount(row);
    }

    ConfigInit();
    ConnectInit(row);
    OnUserChange();

    connect(stopButton, SIGNAL(clicked()), this, SLOT(StopMeasure()));
    connect(maintenanceTableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(ShowKeyboard()));
    connect(maintenanceTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this ,SLOT(CheckValue(QTableWidgetItem*)));
    connect(this, SIGNAL(MaintenanceUpdateFlowMessageSignal(void)), this, SLOT(SlotUpdateFlowMessage(void)));
    connect(this, SIGNAL(MaintenanceClearFlowMessageSignal(void)), this, SLOT(SlotClearFlowMessage(void)));

    FlowManager::Instance()->RegisterActivatedNotifier(this);
    FlowManager::Instance()->RegisterFinishedNotifier(this);
}

void MaintenanceWidget::ConnectInit(Uint8 row)
{
    QFont font;                           //字体
    font.setPointSize(17);
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(int i = 0; i < row; i++)
    {
        QHBoxLayout *hLayout = new QHBoxLayout();;
        QWidget *widget =  new QWidget();
        widget->setFixedSize(240,60);
        QPushButton *button = new QPushButton();
        button->setFixedSize(235,57);
        button->setFont(font);
        button->setObjectName("brownButton");
        button->setText(tr(m_items[i].text.c_str()));
        m_buttonList.append(button);
        hLayout->addWidget(button);
        hLayout->setAlignment(button, Qt::AlignCenter);
        hLayout->setMargin(0);
        widget->setLayout(hLayout);
        maintenanceTableWidget->setCellWidget(i,0,widget);
        connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(button, i);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(measureStart(int)));
}

void MaintenanceWidget::SpaceInit()
{
    for(int i = 0;i < maintenanceTableWidget->rowCount();i++)
    {
        for(int j = 0;j < maintenanceTableWidget->columnCount();j++)
        {
            maintenanceTableWidget->setItem(i, j, new QTableWidgetItem());
            maintenanceTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
            if(j!=1 || (i >= m_tableInfo.userRow))
            {
                QTableWidgetItem *item = maintenanceTableWidget->item(i, j);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
}

void MaintenanceWidget::ShowKeyboard()
{
    QTableWidgetItem *item = maintenanceTableWidget->currentItem();
    if(item->column() == 1 && item->row() < m_tableInfo.userRow)
    {
        int curX = cursor().pos().x();
        int curY = cursor().pos().y();
        int curWidth = maintenanceTableWidget->columnWidth(item->column());
        int curHeight = maintenanceTableWidget->rowHeight(item->row());

        CNumberKeyboard *numKbd = CNumberKeyboard::Instance();
        if(true == numKbd->isHidden())
        {
            numKbd->myMove(curX, curY, curWidth, curHeight);
            numKbd->show();
        }
    }
}

void MaintenanceWidget::CheckValue(QTableWidgetItem* item)
{
    int times = item->text().toInt();
    if (times > 0 && times < 100)
    {
        item->setText(QString::number(times));
    }
    else
    {
        item->setText("1");
    }
}

void MaintenanceWidget::showEvent(QShowEvent *event)
{
    CNumberKeyboard *numberKey = new CNumberKeyboard(this);
    if (numberKey)
    {
        delete numberKey;
        numberKey = nullptr;
    }
    stopButton->setFocus();
}

void MaintenanceWidget::StopMeasure()
{
    /*停止维护界面和组合维护的所有动作*/
    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.operation.maintain", itermsTable);

    oolua_ipairs(itermsTable)
    {
       Table itermTable;
       lua->pull(itermTable);

       String name;
       itermTable.at("name", name);

       FlowManager::Instance()->RemoveFlow(name);
    }
    oolua_ipairs_end()

    FlowManager::Instance()->StopFlow();
}

void MaintenanceWidget::measureStart(int index)
{
    if (!FlowManager::Instance()->IsFlowEnable())  // 空闲状态
    {
        MessageDialog msg(tr("请确认仪器状态是否为空闲！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }

    if (!FlowManager::Instance()->IsAuthorize())  // 运行权限
    {
        MessageDialog msg(tr("运行权限已过期！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }
    int times = maintenanceTableWidget->item(index, 1)->text().toInt();
    if (m_items.count(index))
    {
        Script *lua = LuaEngine::Instance()->GetEngine();
        MaintenanceItem item = m_items[index];
        Lua_func_ref func = item.maintenanceFunc;

        logger->debug("Flow will be executed %d times",times);
        bool createFlowRet = true;
        for (int i =0; i<times; i++)
        {
            if (!lua->call(func))
            {
                createFlowRet = false;
            }
        }

        if (createFlowRet)
        {
            m_curRunFlow.isStart = true;
            m_curRunFlow.flowTimes = 0;
            m_curRunFlow.flowMaxTimes = times;
            m_curRunFlow.flowName = m_buttonList[index]->text();
            FlowManager::Instance()->StartFlow();
        }
        else
        {
            logger->warn("MaintenanceWidget call the function Fail on lua");
        }
    }
}

void MaintenanceWidget::ConfigInit()
{

}

void MaintenanceWidget::SlotUpdateFlowMessage()
{
    if (m_curRunFlow.isStart && m_curRunFlow.flowMaxTimes > 1)
    {
        m_curRunFlow.flowTimes++;
        if (m_curRunFlow.flowTimes <= m_curRunFlow.flowMaxTimes)
        {
            QString flowMessage = QString::number(m_curRunFlow.flowTimes) + "/" + QString::number(m_curRunFlow.flowMaxTimes) + " " +m_curRunFlow.flowName;
            m_flowMessage->setText(flowMessage);
            m_flowMessage->setStyleSheet("QLabel{border:2px solid rgb(100, 100, 100);}");
        }
        else
        {
            m_curRunFlow.isStart = false;
            m_flowMessage->setText("");
            m_flowMessage->setStyleSheet("QLabel{border:0px solid rgb(100, 100, 100);}");
        }
    }
}

void MaintenanceWidget::SlotClearFlowMessage()
{
    if (m_curRunFlow.isStart)
    {
        m_curRunFlow.isStart = false;
        m_flowMessage->setText("");
        m_flowMessage->setStyleSheet("QLabel{border:0px solid rgb(100, 100, 100);}");
    }
}

void MaintenanceWidget::OnFlowActivated(String name, long time)
{
    emit MaintenanceUpdateFlowMessageSignal();
}

void MaintenanceWidget::OnFlowFinished(String name, String result)
{

}

void MaintenanceWidget::OnFlowProcessMessage(String name, String mag)
{

}

void MaintenanceWidget::OnFlowThreadExit()
{
    emit MaintenanceClearFlowMessageSignal();
}

void MaintenanceWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        for (map<Uint8, MaintenanceItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;

            maintenanceTableWidget->setRowHidden(row, false);
        }
    }
    else if(LoginDialog::userType == Administrator)
    {
        for (map<Uint8, MaintenanceItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            MaintenanceItem &maintenanceItem = iter->second;
            if (Administrator >= maintenanceItem.readPrivilege)
            {
                maintenanceTableWidget->setRowHidden(row, false);
            }
            else
            {
                maintenanceTableWidget->setRowHidden(row, true);
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        for (map<Uint8, MaintenanceItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            MaintenanceItem &maintenanceItem = iter->second;
            if (Maintain >= maintenanceItem.readPrivilege)
            {
                maintenanceTableWidget->setRowHidden(row, false);
            }
            else
            {
                maintenanceTableWidget->setRowHidden(row, true);
            }
        }
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(maintenanceTableWidget, m_tableInfo);
}

MaintenanceWidget::~MaintenanceWidget()
{

}

}
