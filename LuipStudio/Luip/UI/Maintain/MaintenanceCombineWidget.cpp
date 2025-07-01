#include "Log.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MessageDialog.h"
#include "StatusManager/StatusManager.h"
#include "FlowManager/FlowManager.h"
#include "LuaEngine/LuaEngine.h"
#include "MaintenanceCombineWidget.h"
#include "Setting/SettingManager.h"
#include "UI/ControlConfig.h"
#include "Controller/ControllerManager.h"
#include "FlowManager/MeasureScheduler.h"

#define     PERIODIC_INTERVAL_SEC           1*3600            //周期模式间隔时间(秒)
#define     TIMED_INTERVAL_SEC                 1*3600            //整点模式间隔时间(秒)

using namespace Configuration;
using namespace OOLUA;
using namespace Measure;
using namespace std;
using namespace Flow;
using namespace Lua;
using namespace Controller;

namespace UI
{

MaintenanceCombineWidget::MaintenanceCombineWidget(QWidget *parent) :
        QWidget(parent),m_flowStart(false),m_mearsureMode(0)
{
    MaintainUserChange::Instance()->Register(this);

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.operation.maintainCombine", itermsTable);
    itermsTable.safe_at("ClearFlowList", m_clearFlowList);

    maintenanceTableWidget = new MQtableWidget();
    maintenanceTableWidget->setColumnCount(6);

    m_tableInfo.userRow = MAX_FLOW_ITEMS;
    m_tableInfo.hiddenRow = 0;
    m_tableInfo.perPageRow = 5;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(maintenanceTableWidget, m_tableInfo);

    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        maintenanceTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background:transparent; width: 36px;}");
    }

    maintenanceTableWidget->setFixedSize(550,268);

    QFont headerFont;
    headerFont.setPointSize(10);

    QFont nameHeaderFont;
    nameHeaderFont.setPointSize(15);
    nameHeaderFont.setWeight(QFont::DemiBold);

    QTableWidgetItem *column0 = new QTableWidgetItem(tr("序号"));
    column0->setFont(nameHeaderFont);
    maintenanceTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem(tr("流程名称"));
    column1->setFont(nameHeaderFont);
    maintenanceTableWidget->setHorizontalHeaderItem(1, column1);

    QTableWidgetItem *column2 = new QTableWidgetItem(tr("次数"));
    column2->setFont(nameHeaderFont);
    maintenanceTableWidget->setHorizontalHeaderItem(2, column2);

    QTableWidgetItem *column3 = new QTableWidgetItem("");
    column3->setFont(headerFont);
    maintenanceTableWidget->setHorizontalHeaderItem(3, column3);

    QTableWidgetItem *column4 = new QTableWidgetItem("");
    column4->setFont(headerFont);
    maintenanceTableWidget->setHorizontalHeaderItem(4, column4);

    QTableWidgetItem *column5 = new QTableWidgetItem("");
    column5->setFont(headerFont);
    maintenanceTableWidget->setHorizontalHeaderItem(5, column5);

    maintenanceTableWidget->horizontalHeader()->setFixedHeight(45); // 设置表头的高度
    maintenanceTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");

    QFont dataFont = maintenanceTableWidget->font();
    dataFont.setPointSize(15);

    maintenanceTableWidget->horizontalHeader()->setFont(dataFont);
    maintenanceTableWidget->setFont(dataFont);
//    maintenanceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QFont labelFont = maintenanceTableWidget->font();
    labelFont.setPointSize(11);
    m_flowMessage = new QLabel();
    m_flowMessage->setFixedSize(185,40);
    m_flowMessage->setFont(labelFont);
    m_flowMessage->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_flowMessage->setText("");
    m_flowMessage->setStyleSheet("QLabel{border:0px solid rgb(100, 100, 100);}");

    QFont messageFont;
    messageFont.setPointSize(14);
    m_nextStartTimeMessage = new QLabel();
    m_nextStartTimeMessage->setFixedSize(400,30);
    m_nextStartTimeMessage->setFont(messageFont);
    m_nextStartTimeMessage->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    m_nextStartTimeMessage->setText("<font color=red>*周期测试和整点测试间隔均为1小时");

    QPushButton *startButton;
    startButton = new QPushButton();
    startButton->setObjectName("brownButton");
    startButton->setText(tr("启动"));
    startButton->setFont(dataFont);
    startButton->setFixedSize(100,40);
    connect(startButton, SIGNAL(clicked()), this, SLOT(StartMeasure()));

    QPushButton *stopAllButton;
    stopAllButton = new QPushButton();
    stopAllButton->setObjectName("redButton");
    stopAllButton->setText(tr("停止"));
    stopAllButton->setFont(dataFont);
    stopAllButton->setFixedSize(100,40);
    connect(stopAllButton, SIGNAL(clicked()), this, SLOT(StopAllMeasure()));

    QHBoxLayout *startAndStopButtonLayout = new QHBoxLayout();
    startAndStopButtonLayout->addStretch();
    startAndStopButtonLayout->addWidget(m_flowMessage);
    startAndStopButtonLayout->addSpacing(135);
    startAndStopButtonLayout->addWidget(startButton);
    startAndStopButtonLayout->addSpacing(20);
    startAndStopButtonLayout->addWidget(stopAllButton);
    startAndStopButtonLayout->addSpacing(50);
    startAndStopButtonLayout->setContentsMargins(0, 5, 0, 0);

    for(int i = 0;i < maintenanceTableWidget->rowCount();i++)
    {
        maintenanceTableWidget->setRowHeight(i, 44);
    }

    maintenanceTableWidget->setColumnWidth(0, 50);
    maintenanceTableWidget->setColumnWidth(1, 240);
    maintenanceTableWidget->setColumnWidth(2, 50);
    maintenanceTableWidget->setColumnWidth(3, 70);
    maintenanceTableWidget->setColumnWidth(4, 70);
    maintenanceTableWidget->setColumnWidth(5, 70);

    SpaceInit();

    int type = SettingManager::Instance()->GetParamType();

    QStringList boxList;
    Uint8 row = 0;
    oolua_ipairs(itermsTable)
    {
        Table itermTable;
        lua->pull(itermTable);

        int targetPrivilege = 0;
        bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

        if (isTarget == false || (type & targetPrivilege))
        {
            MaintenanceCombineItem maintenanceItem;

            itermTable.at("text", maintenanceItem.text);
            itermTable.safe_at("createFlow", maintenanceItem.maintenanceFunc);

            boxList.append(QString::fromStdString(maintenanceItem.text));
            m_items.insert(make_pair(row, maintenanceItem));
            row++;

        }
    }
    oolua_ipairs_end()

    m_listBox = new QComboBox(this);
    m_listBox->setFixedSize(200,45);
    m_listBox->setObjectName("bigComboBox");
    m_listBox->addItems(boxList);
    m_listBox->setFont(nameHeaderFont);

    QStringList schedulerMode;
    schedulerMode << "连续测试" << "周期测试" << "整点测试" ;
    m_modeBox = new QComboBox(this);
    m_modeBox->setFixedSize(120,45);
    m_modeBox->setObjectName("bigComboBox");
    m_modeBox->addItems(schedulerMode);
    m_modeBox->setFont(nameHeaderFont);
    m_mearsureMode = m_modeBox->currentIndex();
    connect(m_modeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotCurrentIndexChanged(int)));

    m_timesEdit = new QLineEdit();
    m_timesEdit->setFixedSize(60, 45);
    m_timesEdit->setText("1");
    m_timesEdit->setFont(nameHeaderFont);
    m_timesEdit->installEventFilter(CNumberKeyboard::Instance());

    QLabel *timesLabel;
    timesLabel = new QLabel();
    timesLabel->setFixedSize(45,45);
    timesLabel->setFont(nameHeaderFont);
    timesLabel->setText("次");
    timesLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QPushButton *addFlowButton;
    addFlowButton = new QPushButton();
    addFlowButton->setObjectName("brownButton");
    addFlowButton->setText("添加");
    addFlowButton->setFont(dataFont);
    addFlowButton->setFixedSize(100,40);
    connect(addFlowButton, SIGNAL(clicked()), this, SLOT(SlotsAddItems()));

    QHBoxLayout *choseFlowLayout = new QHBoxLayout();
    choseFlowLayout->addWidget(m_listBox);
    choseFlowLayout->addSpacing(20);
    choseFlowLayout->addWidget(m_timesEdit);
    choseFlowLayout->addWidget(timesLabel);
    choseFlowLayout->addSpacing(20);
    choseFlowLayout->addWidget(addFlowButton);
    choseFlowLayout->addSpacing(20);
    choseFlowLayout->addWidget(m_modeBox);
    choseFlowLayout->addStretch();

    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addLayout(choseFlowLayout);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(maintenanceTableWidget, 0, Qt::AlignTop | Qt::AlignLeft);

    QHBoxLayout *messageLayout = new QHBoxLayout();
    messageLayout->addWidget(m_nextStartTimeMessage);
    messageLayout->addStretch();

    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addLayout(messageLayout);
    mainLayout->addLayout(startAndStopButtonLayout);
    this->setLayout(mainLayout);

    ConfigInit();
    ConnectInit(MAX_FLOW_ITEMS);
    OnUserChange();

    for(int i=0;i<MAX_FLOW_ITEMS;i++)
    {
        maintenanceTableWidget->item(i, 0)->setText(QString::number(i+1));
        maintenanceTableWidget->setRowHidden(i, true);
    }

    flowCounter.curFlowIndex = 0;
    flowCounter.curFlowTimes = 0;

    flowItem.curFlowIndex   = 0;
    flowItem.curFlowTimes  = 0;
    flowItem.flowMaxTimes  = 0;
    m_timer = new QTimer();

    connect(m_timer, SIGNAL(timeout()), this, SLOT(SlotTimeout()));
    connect(maintenanceTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(DoubleClickedSlot(QTableWidgetItem*)));
    connect(maintenanceTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(CheckValueSlot(QTableWidgetItem*)));
    connect(this, SIGNAL(UpdateFlowMessageSignal(bool)), this, SLOT(SlotUpdateFlowMessage(bool)));
    connect(this, SIGNAL(ClearFlowMessageSignal(void)), this, SLOT(SlotClearFlowMessage(void)));
    connect(this, SIGNAL(UpdaterTimerSignal(void)), this, SLOT(SlotUpdaterTimer(void)));

    FlowManager::Instance()->RegisterActivatedNotifier(this);
    FlowManager::Instance()->RegisterFinishedNotifier(this);
    FlowManager::Instance()->RegisterProcessMessageNotifier(this);
}

void MaintenanceCombineWidget::ConnectInit(Uint8 row)
{
    QFont font;                           //字体
    font.setPointSize(12);

    //上移键
    QSignalMapper *upSignalMapper = new QSignalMapper(this);
    for(int i = 0; i < row; i++)
    {
        QPushButton *button = new QPushButton();
        button->setFixedSize(70,44);
        button->setFont(font);
        button->setText("上移");
        if(i == 0)
        {
            button->setEnabled(false);
        }
        m_buttonList.append(button);
        maintenanceTableWidget->setCellWidget(i,3,button);
        connect(button, SIGNAL(clicked()), upSignalMapper, SLOT(map()));
        upSignalMapper->setMapping(button, i);
    }
    connect(upSignalMapper, SIGNAL(mapped(int)), this, SLOT(SlotsUpItems(int)));

    //下移键
    QSignalMapper *downSignalMapper = new QSignalMapper(this);
    for(int i = 0; i < row; i++)
    {
        QPushButton *button = new QPushButton();
        button->setFixedSize(70,44);
        button->setFont(font);
        button->setText("下移");
        m_buttonList.append(button);
        maintenanceTableWidget->setCellWidget(i,4,button);
        connect(button, SIGNAL(clicked()), downSignalMapper, SLOT(map()));
        downSignalMapper->setMapping(button, i);
    }
    connect(downSignalMapper, SIGNAL(mapped(int)), this, SLOT(SlotsDownItems(int)));

    //删除键
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(int i = 0; i < row; i++)
    {
        QPushButton *button = new QPushButton();
        button->setFixedSize(70,44);
        button->setFont(font);
        button->setText("删除");
        m_buttonList.append(button);
        maintenanceTableWidget->setCellWidget(i,5,button);
        connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(button, i);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(SlotsDeleteItems(int)));
}

void MaintenanceCombineWidget::SpaceInit()
{
    for(int i = 0;i < maintenanceTableWidget->rowCount(); i++)//行
    {
        for(int j = 0;j < maintenanceTableWidget->columnCount(); j++)//列
        {
            maintenanceTableWidget->setItem(i, j, new QTableWidgetItem());
            maintenanceTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);//设置对齐方式
            if(j!=2)
            {
                QTableWidgetItem *item = maintenanceTableWidget->item(i, j);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
}

bool MaintenanceCombineWidget::CheckStatus()
{
    if (!FlowManager::Instance()->IsFlowEnable() || m_flowStart)  // 空闲状态
    {
        MessageDialog msg(tr("请确认仪器状态是否为空闲！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return false;
        }
    }

    if (!FlowManager::Instance()->IsAuthorize())  // 运行权限
    {
        MessageDialog msg(tr("运行权限已过期！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return false;
        }
    }

    return true;
}

void MaintenanceCombineWidget::StopAllMeasure()
{
    /*停止组合维护*/
    FlowManager::Instance()->UpdateFlowMessage("MaintainCombine","Stop");

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.operation.maintainCombine", itermsTable);

    oolua_ipairs(itermsTable)
    {
        Table itermTable;
        lua->pull(itermTable);

        String name;
        itermTable.at("name", name);

        FlowManager::Instance()->RemoveFlow(name);
    }
    oolua_ipairs_end()
}

void MaintenanceCombineWidget::StartMeasure()
{
    QWriteLocker locker(&m_lock);

    if(!this->CheckStatus())
    {
        return;
    }

    Script *lua = LuaEngine::Instance()->GetEngine();
    lua->call(m_clearFlowList);

    if (m_timer->isActive())
    {
        m_timer->stop();
    }

    //  检查是否有流程
    if(m_flowCombineItems[0].isExist)
    {
        for (map<Uint8, MaintenanceCombineItem>::iterator iter = m_items.begin();iter != m_items.end(); ++iter)
        {
            MaintenanceCombineItem &item = iter->second;
            if(item.text == m_flowCombineItems[0].text.toStdString())
            {
                if(m_flowCombineItems[0].times > 0)
                {
                    m_flowStart = true;

                    flowCounter.curFlowIndex = 0;
                    flowCounter.curFlowTimes = 0;

                    flowItem.curFlowIndex   = 0;
                    flowItem.curFlowTimes  = 0;
                    flowItem.flowMaxTimes  = m_flowCombineItems[0].times;
                    flowItem.maintenanceFunc  = item.maintenanceFunc;
                    if (0 == m_mearsureMode || 1 == m_mearsureMode)                 //连续测量或周期模式
                    {
                        m_timer->start(1);
                    }
                    else if (2 == m_mearsureMode)        //整点测量
                    {
                        QDateTime curTime = QDateTime::currentDateTime();
                        QDateTime startTime = curTime;
                        startTime.setTime(QTime(startTime.toString("hh").toInt(), 0, 0));
                        startTime = startTime.addSecs(TIMED_INTERVAL_SEC);
                        m_nextStartTime = startTime.toTime_t();
                        m_timer->start(1000 * (startTime.toTime_t() -  curTime.toTime_t()));
                        m_nextStartTimeMessage->setText(QString( "将于"+ startTime.toString("yyyy-MM-dd hh:mm:ss") + " 启动" + "序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次"));
                        MeasureScheduler::Instance()->UpdateExpectInfo(startTime.toTime_t(),QString("启动组合维护 序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次").toStdString());
                    }
                }
                break;
            }
        }
    }
}

void MaintenanceCombineWidget::SlotsDeleteItems(int index)
{
    if(!this->CheckStatus())
    {
        return;
    }

    if (index == MAX_FLOW_ITEMS-1)
    {
        m_flowCombineItems[MAX_FLOW_ITEMS-1].text = "";
        m_flowCombineItems[MAX_FLOW_ITEMS-1].times = 0;
        m_flowCombineItems[MAX_FLOW_ITEMS-1].isExist = false;
    }
    else
    {
        for(int i=index;i<MAX_FLOW_ITEMS-1;i++)
        {
            m_flowCombineItems[i] = m_flowCombineItems[i+1];
        }

        if(m_flowCombineItems[MAX_FLOW_ITEMS-1].isExist)
        {
            m_flowCombineItems[MAX_FLOW_ITEMS-1].text = "";
            m_flowCombineItems[MAX_FLOW_ITEMS-1].times = 0;
            m_flowCombineItems[MAX_FLOW_ITEMS-1].isExist = false;
        }
    }

    this->ConfigInit();

    if(index>0)
    {
        int moveToRow = index;
        if (!m_flowCombineItems[moveToRow].isExist)
        {
            moveToRow--;
        }
        maintenanceTableWidget->setCurrentCell(moveToRow,0);
    }
}

void MaintenanceCombineWidget::SlotsAddItems()
{
    if(!this->CheckStatus())
    {
        return;
    }

    int freeIndex;
    for(freeIndex = 0;freeIndex<MAX_FLOW_ITEMS;freeIndex++)
    {
        if(m_flowCombineItems[freeIndex].isExist == false)
        {
            break;
        }
    }

    if(freeIndex>=MAX_FLOW_ITEMS)
    {
        MessageDialog msg(tr("最多添加组合20个流程！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }

    m_flowCombineItems[freeIndex].isExist = true;
    m_flowCombineItems[freeIndex].text = m_listBox->currentText();
    m_flowCombineItems[freeIndex].times = m_timesEdit->text().toInt();

    this->ConfigInit();

    maintenanceTableWidget->setCurrentCell(freeIndex,0);
}


void MaintenanceCombineWidget::SlotsUpItems(int index)
{
    if(!this->CheckStatus())
    {
        return;
    }

    FlowCombineItem tempItem;
    tempItem = m_flowCombineItems[index];
    m_flowCombineItems[index] = m_flowCombineItems[index-1];
    m_flowCombineItems[index-1] = tempItem;

    this->ConfigInit();

    if(index>0)
    {
        int moveToRow = index;
        if (!m_flowCombineItems[moveToRow].isExist)
        {
            moveToRow--;
        }
        maintenanceTableWidget->setCurrentCell(moveToRow,0);
    }
}

void MaintenanceCombineWidget::SlotsDownItems(int index)
{
    if(!this->CheckStatus())
    {
        return;
    }

    if(index>=(MAX_FLOW_ITEMS-1) || (!m_flowCombineItems[index+1].isExist))
    {
        return;
    }

    FlowCombineItem tempItem;
    tempItem = m_flowCombineItems[index];
    m_flowCombineItems[index] = m_flowCombineItems[index+1];
    m_flowCombineItems[index+1] = tempItem;

    this->ConfigInit();

    if(index>0)
    {
        int moveToRow = index;
        if (!m_flowCombineItems[moveToRow].isExist)
        {
            moveToRow--;
        }
        maintenanceTableWidget->setCurrentCell(moveToRow,0);
    }
}


void MaintenanceCombineWidget::ConfigInit()
{
    int curMaxIndex;
    for(curMaxIndex = 0;curMaxIndex<MAX_FLOW_ITEMS;curMaxIndex++)
    {
        if(m_flowCombineItems[curMaxIndex].isExist == false)
        {
            break;
        }
    }

    if (curMaxIndex >5)
    {
        maintenanceTableWidget->setFixedSize(588,266);
        this->update();
    }
    else
    {
        maintenanceTableWidget->setFixedSize(550,268);
        this->update();
    }

    for(int i = 0;i<MAX_FLOW_ITEMS;i++)
    {
        if (i<curMaxIndex)
        {
            maintenanceTableWidget->item(i, 1)->setText(m_flowCombineItems[i].text);
            maintenanceTableWidget->item(i, 2)->setText(QString::number(m_flowCombineItems[i].times));
            maintenanceTableWidget->setRowHidden(i, false);
        }
        else
        {
            maintenanceTableWidget->setRowHidden(i, true);
        }
    }
}

void MaintenanceCombineWidget::SlotTimeout()
{
    m_timer->stop();
    if (!FlowManager::Instance()->IsFlowRunning() &&
        FlowManager::Instance()->IsFlowEnable() &&
        ControllerManager::Instance()->IsAllConnected())
    {
        if (flowItem.curFlowTimes < flowItem.flowMaxTimes)
        {
            flowItem.curFlowTimes++;
            Script *lua = LuaEngine::Instance()->GetEngine();
            lua->call(flowItem.maintenanceFunc);
            flowCounter.curFlowIndex = flowItem.curFlowIndex;
            flowCounter.curFlowTimes = flowItem.curFlowTimes;
            FlowManager::Instance()->StartFlow();
        }

        //加载下个流程
        if (flowItem.curFlowTimes == flowItem.flowMaxTimes)
        {
            flowItem.curFlowIndex++;
            int index = flowItem.curFlowIndex;
            if(m_flowCombineItems[index].isExist)
            {
                for (map<Uint8, MaintenanceCombineItem>::iterator iter = m_items.begin();iter != m_items.end(); ++iter)
                {
                    MaintenanceCombineItem &item = iter->second;
                    if(item.text == m_flowCombineItems[index].text.toStdString())
                    {
                        if(m_flowCombineItems[index].times > 0)
                        {
                            flowItem.curFlowTimes  = 0;
                            flowItem.flowMaxTimes  = m_flowCombineItems[index].times;
                            flowItem.maintenanceFunc  = item.maintenanceFunc;
                        }
                        break;
                    }
                }
            }
        }

        if(m_flowCombineItems[flowItem.curFlowIndex].isExist)
        {
            QDateTime curTime = QDateTime::currentDateTime();
            QDateTime startTime = curTime;
            if (1 == m_mearsureMode)        //周期测量
            {
                startTime = startTime.addSecs(PERIODIC_INTERVAL_SEC);
                m_nextStartTime = startTime.toTime_t();
                m_timer->start(1000 * (startTime.toTime_t() -  curTime.toTime_t()));
                m_nextStartTimeMessage->setText(QString( "将于"+ startTime.toString("yyyy-MM-dd hh:mm:ss") + " 启动" + "序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次"));
                MeasureScheduler::Instance()->UpdateExpectInfo(startTime.toTime_t(),QString("启动组合维护 序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次").toStdString());
            }
            else if (2 == m_mearsureMode) //整点测量
            {
                startTime.setTime(QTime(startTime.toString("hh").toInt(), 0, 0));
                startTime = startTime.addSecs(TIMED_INTERVAL_SEC);
                m_nextStartTime = startTime.toTime_t();
                m_timer->start(1000 * (startTime.toTime_t() -  curTime.toTime_t()));
                m_nextStartTimeMessage->setText(QString( "将于"+ startTime.toString("yyyy-MM-dd hh:mm:ss") + " 启动" + "序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次"));
                MeasureScheduler::Instance()->UpdateExpectInfo(startTime.toTime_t(),QString("启动组合维护 序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次").toStdString());
            }
        }
    }
    else if (2 == m_mearsureMode)
    {
        QDateTime curTime = QDateTime::currentDateTime();
        QDateTime startTime = curTime;
        startTime.setTime(QTime(startTime.toString("hh").toInt(), 0, 0));
        startTime = startTime.addSecs(TIMED_INTERVAL_SEC);
        m_nextStartTime = startTime.toTime_t();
        m_timer->start(1000 * (startTime.toTime_t() -  curTime.toTime_t()));
        m_nextStartTimeMessage->setText(QString( "将于"+ startTime.toString("yyyy-MM-dd hh:mm:ss") + " 启动" + "序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次"));
        MeasureScheduler::Instance()->UpdateExpectInfo(startTime.toTime_t(),QString("启动组合维护 序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次").toStdString());
    }
}

void MaintenanceCombineWidget::OnUserChange()
{

}

MaintenanceCombineWidget::~MaintenanceCombineWidget()
{

}

void MaintenanceCombineWidget::DoubleClickedSlot(QTableWidgetItem *item)
{
    if(item->column() != 2 || !this->CheckStatus())
    {
        return;
    }
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

void MaintenanceCombineWidget::CheckValueSlot(QTableWidgetItem *item)
{
    if(item->column() == 2)
    {
        QString tmpPattern("^(\\d?\\d)$");  //检查数字是否合法
        QRegExp tmpRx(tmpPattern);
        if(tmpRx.exactMatch(item->text()))
        {
            m_flowCombineItems[item->row()].times = item->text().toInt();
        }
        else
        {
            MessageDialog msg(tr("次数输入不正确! \n正确输入范围[0-99]！"), this,MsgStyle::ONLYOK);
            msg.exec();
            m_flowCombineItems[item->row()].times = 0;
            maintenanceTableWidget->item(item->row(),item->column())->setText("0");
        }
    }
}

void MaintenanceCombineWidget::SlotUpdateFlowMessage(bool status)
{
    if (m_flowStart)
    {
        int index = flowCounter.curFlowIndex + 1;
        int times = flowCounter.curFlowTimes;
        QString flowMessage = "正在运行序号" + QString::number(index) + "第" + QString::number(times) + "次";
        m_flowMessage->setText(flowMessage);
        m_flowMessage->setStyleSheet("QLabel{border:2px solid rgb(100, 100, 100);}");
    }

    if (!status)
    {
        m_flowStart = status;
    }
}

void MaintenanceCombineWidget::SlotClearFlowMessage()
{
    m_flowMessage->setText("");
    m_flowMessage->setStyleSheet("QLabel{border:0px solid rgb(100, 100, 100);}");
    if (!m_flowStart)
    {
        m_nextStartTimeMessage->setText("<font color=red>*周期测试和整点测试间隔均为1小时");
        MeasureScheduler::Instance()->UpdateExpectInfo(-1,"");
    }

    if (false == m_timer->isActive())
    {
        m_nextStartTimeMessage->setText("<font color=red>*周期测试和整点测试间隔均为1小时");
        MeasureScheduler::Instance()->UpdateExpectInfo(-1,"");
    }

    if (m_flowStart && false == m_timer->isActive())
    {
        m_timer->start(1);
    }
}

void MaintenanceCombineWidget::SlotUpdaterTimer()
{
    if (m_timer->isActive())
    {
        m_timer->stop();
        QDateTime curTime = QDateTime::currentDateTime();
        qDebug("SlotUpdaterTimer %d",(m_nextStartTime-  curTime.toTime_t()));
        m_timer->start(1000 * (m_nextStartTime-  curTime.toTime_t()));
        m_nextStartTimeMessage->setText(QString( "将于"+ QDateTime::fromTime_t(m_nextStartTime).toString("yyyy-MM-dd hh:mm:ss") + " 启动" + "序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次"));
        MeasureScheduler::Instance()->UpdateExpectInfo(m_nextStartTime,QString("启动组合维护 序号"+QString::number(flowItem.curFlowIndex + 1) + "第" + QString::number(flowItem.curFlowTimes + 1) + "次").toStdString());
    }
}

void MaintenanceCombineWidget::SlotCurrentIndexChanged(int index)
{
    if (m_mearsureMode != index && (m_flowStart || !FlowManager::Instance()->IsFlowEnable()))  // 空闲状态
    {
        m_modeBox->setCurrentIndex(m_mearsureMode);
        MessageDialog msg(tr("请确认仪器状态是否为空闲！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }
    else
    {
        m_mearsureMode = index;
    }
}

void MaintenanceCombineWidget::OnFlowActivated(String name, long time)
{
    if(m_flowStart && false == m_flowCombineItems[flowItem.curFlowIndex + 1].isExist && flowItem.curFlowTimes == flowItem.flowMaxTimes)
    {
        MeasureScheduler::Instance()->Enabled(true);
        emit UpdateFlowMessageSignal(false);
    }
    else if (m_flowStart)
    {
        MeasureScheduler::Instance()->Enabled(false);
        emit UpdateFlowMessageSignal(true);
    }
}

void MaintenanceCombineWidget::OnFlowFinished(String name, String result)
{
}

void MaintenanceCombineWidget::OnFlowProcessMessage(String name, String mag)
{
        if (name == "MaintainCombine" && mag == "Stop")
        {
            m_flowStart = false;
            MeasureScheduler::Instance()->Enabled(true);
            if (m_timer->isActive())
            {
                m_timer->stop();
            }
            emit ClearFlowMessageSignal();
            MeasureScheduler::Instance()->Update();
        }
        else if (name == "MaintainCombine" && mag == "UpdateTime")
        {
            if (m_flowStart)
            {
                emit UpdaterTimerSignal();
                MeasureScheduler::Instance()->Update();
            }
        }
}

void MaintenanceCombineWidget::OnFlowThreadExit()
{
    emit ClearFlowMessageSignal();
}


}
