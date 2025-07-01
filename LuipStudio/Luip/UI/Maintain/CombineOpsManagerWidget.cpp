#include "Common.h"
#include "UseResourceWidget.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <vector>
#include <QDateTime>
#include "CombineOpsManagerWidget.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MessageDialog.h"
#include "LuaEngine/LuaEngine.h"
#include "StatusManager/StatusManager.h"
#include "FlowManager/FlowManager.h"
#include "Log.h"
#include "Setting/SettingManager.h"
#include "UI/ControlConfig.h"


using namespace Configuration;
using System::Uint32;
using namespace Measure;
using namespace std;
using namespace OOLUA;
using namespace Lua;
using namespace Flow;

namespace UI
{

CombineOpsManagerWidget::CombineOpsManagerWidget(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.operation.combineOperator", itermsTable);

    QFont font;                           //字体
    font.setPointSize(16);
    font.setWeight(60);

    this->resize(650,405);

    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("rowCount", useRowCount);
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("administratorRow", administratorRow);

    combineOpsManngerTableWidget = new MQtableWidget();

    combineOpsManngerTableWidget->setColumnCount(2);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 6;
    m_tableInfo.realRow = 0;

    RowHiddenHandler::Instance()->CreateRow(combineOpsManngerTableWidget, m_tableInfo);

    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        combineOpsManngerTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background-color:rgb(200,200,200); width: 45;}"
                 "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background-color:rgb(240,240,240)}");
    }
    combineOpsManngerTableWidget->setFixedSize(450,405);

    QFont headerFont;
    headerFont.setPointSize(15);
    headerFont.setWeight(QFont::DemiBold);

    QTableWidgetItem *column0 = new QTableWidgetItem(tr("操作"));
    column0->setFont(headerFont);
    combineOpsManngerTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem(tr("体积(ml)"));
    column1->setFont(headerFont);
    combineOpsManngerTableWidget->setHorizontalHeaderItem(1, column1);

    combineOpsManngerTableWidget->horizontalHeader()->setFixedHeight(40); // 设置表头的高度
    combineOpsManngerTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");

    QFont dataFont = combineOpsManngerTableWidget->font();
    dataFont.setPointSize(16);
    dataFont.setWeight(75);
    combineOpsManngerTableWidget->setFont(dataFont);

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(combineOpsManngerTableWidget);
    measureResultTableLayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *leftLayout = new QVBoxLayout();

    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();

    stopButton = new QPushButton();
    stopButton->setObjectName("redButton");
    stopButton->setText(tr("停止"));
    stopButton->setFont(font);
    stopButton->setFixedSize(120,60);


    QHBoxLayout *stopButtonLayout = new QHBoxLayout();
    stopButtonLayout->addStretch();
    stopButtonLayout->addWidget(stopButton);
    stopButtonLayout->addStretch();

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addStretch();
    rightLayout->addLayout(stopButtonLayout);
    rightLayout->addStretch();
    rightLayout->setContentsMargins(50, 0, 0, 0);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    mainLayout->addStretch();
    this->setLayout(mainLayout);

    combineOpsManngerTableWidget->setColumnWidth(0,290);    //设置每行或每列的高度或宽度
    if (m_tableInfo.userRow > 6)
    {
        combineOpsManngerTableWidget->setColumnWidth(1,128);
    }
    else
    {
        combineOpsManngerTableWidget->setColumnWidth(1,168);
    }

    for(int i = 0;i < combineOpsManngerTableWidget->rowCount();i++)
    {
        combineOpsManngerTableWidget->setRowHeight(i, 60);
    }

    combineOpsManngerTableWidget->setFont(font);
    SpaceInit();

    QSignalMapper *signalMapper = new QSignalMapper(this);

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
            String text;
            float data;
            int mode;
            CombineOpsManngerItem combineOpsManngerItem;
            combineOpsManngerItem.disableWrite = false;
            combineOpsManngerItem.isRemind = false;

            itermTable.at("text", text);
            itermTable.at("data", data);
            itermTable.at("mode", combineOpsManngerItem.mode);
            itermTable.at("createFlow", combineOpsManngerItem.createFlow);
            itermTable.at("writePrivilege", combineOpsManngerItem.writePrivilege);
            itermTable.at("readPrivilege", combineOpsManngerItem.readPrivilege);
            itermTable.safe_at("disableWrite", combineOpsManngerItem.disableWrite);
            itermTable.safe_at("isRemind", combineOpsManngerItem.isRemind);
            itermTable.safe_at("remindText", combineOpsManngerItem.remindText);

            m_items.insert(make_pair(row, combineOpsManngerItem));

            QPushButton *button = new QPushButton();
            button->setObjectName("brownButton");
            button->setFixedSize(288,57);
            button->setFont(font);
            button->setText(QObject::tr(text.c_str()));
            m_buttonList.append(button);
            combineOpsManngerTableWidget->setCellWidget(row, 0, button);
            connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
            signalMapper->setMapping(button, row);

            combineOpsManngerTableWidget->item(row, 1)->setText(QString::number(data));

            row++;
        }
    }
    oolua_ipairs_end()

//    if(m_items.size() < combineOpsManngerTableWidget->rowCount())
//    {
//        for(int index = m_items.size();index < combineOpsManngerTableWidget->rowCount();index++)
//        {
//            combineOpsManngerTableWidget->setRowHidden(index, true);
//        }
//    }

    itermsTable.at("checkvalue", m_checkvalueFunc);
    OnUserChange();

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(MeasureStart(int)));
    connect(combineOpsManngerTableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(ChoseDay()));
    connect(stopButton, SIGNAL(clicked(bool)), this, SLOT(StopMeasure()));
    connect(combineOpsManngerTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this ,SLOT(CheckValue(QTableWidgetItem*)));
    connect(combineOpsManngerTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(SlotDoubleClicked(QTableWidgetItem *)));

}

void CombineOpsManagerWidget::SpaceInit()
{
    for(int i = 0;i < combineOpsManngerTableWidget->rowCount(); i++)//行
    {
        for(int j = 0;j < combineOpsManngerTableWidget->columnCount(); j++)//列
        {
            combineOpsManngerTableWidget->setItem(i, j, new QTableWidgetItem());
            combineOpsManngerTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);//设置对齐方式
            if(j!=1 || (i >= m_tableInfo.userRow))//设置不可编辑的单元格
            {
                QTableWidgetItem *item = combineOpsManngerTableWidget->item(i, j);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
}

void CombineOpsManagerWidget::MeasureStart(int index)
{
    if (!FlowManager::Instance()->IsFlowEnable())  // 空闲状态
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
        CombineOpsManngerItem &combineOpsManngerItem = m_items[index];
        Lua_func_ref func = combineOpsManngerItem.createFlow;
        int type = combineOpsManngerItem.mode;
        float vol = combineOpsManngerTableWidget->item(index, 1)->text().toFloat();
        if(combineOpsManngerItem.isRemind)
        {
            QString remindText = QString::fromStdString(combineOpsManngerItem.remindText) + "\n";
            MessageDialog msg(remindText, this, MsgStyle::OKANDCANCEL);
            if(msg.exec()==QDialog::Rejected)
            {
                return;
            }
        }
        if (lua->call(func, type, vol))
        {
            FlowManager::Instance()->StartFlow();
        }
        else
        {
            logger->warn("组合操作启动流失败");
        }
    }
}

void CombineOpsManagerWidget::ChoseDay()
{
    QTableWidgetItem *item = combineOpsManngerTableWidget->currentItem();

    Uint8 row = item->row();
    map<Uint8, CombineOpsManngerItem>::iterator iter = m_items.find(row);
    CombineOpsManngerItem &combineOpsManngerItem = iter->second;

    if(LoginDialog::userType >= combineOpsManngerItem.writePrivilege && combineOpsManngerItem.disableWrite == false)
    {
        if(item->column() == 1 && item->row() < m_tableInfo.userRow)
        {
            int curX = cursor().pos().x();
            int curY = cursor().pos().y();
            int curWidth = combineOpsManngerTableWidget->columnWidth(item->column());
            int curHeight = combineOpsManngerTableWidget->rowHeight(item->row());

            CNumberKeyboard *numKbd = CNumberKeyboard::Instance();
            if(true == numKbd->isHidden())
            {
                numKbd->myMove(curX, curY, curWidth, curHeight);
                numKbd->show();
            }
        }
    }
}

void CombineOpsManagerWidget::StopMeasure()
{
    FlowManager::Instance()->StopFlow();
}

void CombineOpsManagerWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        for (map<Uint8, CombineOpsManngerItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            CombineOpsManngerItem &combineOpsManngerItem = iter->second;

            combineOpsManngerTableWidget->setRowHidden(row, false);

            QTableWidgetItem *item = combineOpsManngerTableWidget->item(row, 1);
            if (Super >= combineOpsManngerItem.writePrivilege && combineOpsManngerItem.disableWrite == false)
            {
                item->setFlags((Qt::ItemFlags)63);
            }
            else
            {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
    else if(LoginDialog::userType == Administrator)
    {
        for (map<Uint8, CombineOpsManngerItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            CombineOpsManngerItem &combineOpsManngerItem = iter->second;
            if (Administrator >= combineOpsManngerItem.readPrivilege)
            {
                combineOpsManngerTableWidget->setRowHidden(row, false);
            }
            else
            {
                combineOpsManngerTableWidget->setRowHidden(row, true);
            }

            QTableWidgetItem *item = combineOpsManngerTableWidget->item(row, 1);
            if (Administrator >= combineOpsManngerItem.writePrivilege && combineOpsManngerItem.disableWrite == false)
            {
                item->setFlags((Qt::ItemFlags)63);
            }
            else
            {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        for (map<Uint8, CombineOpsManngerItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            CombineOpsManngerItem &combineOpsManngerItem = iter->second;
            if (Maintain >= combineOpsManngerItem.readPrivilege)
            {
                combineOpsManngerTableWidget->setRowHidden(row, false);
            }
            else
            {
                combineOpsManngerTableWidget->setRowHidden(row, true);
            }

            QTableWidgetItem *item = combineOpsManngerTableWidget->item(row, 1);
            if (Maintain >= combineOpsManngerItem.writePrivilege && combineOpsManngerItem.disableWrite == false)
            {
                item->setFlags((Qt::ItemFlags)63);
            }
            else
            {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(combineOpsManngerTableWidget, m_tableInfo);
}


void CombineOpsManagerWidget::SlotIndexChanged(int)
{
    for(int i = 0; i < m_tableInfo.userRow; i++)
    {
        CheckItem(combineOpsManngerTableWidget->item(i, 1));
    }
}

void CombineOpsManagerWidget::SlotDoubleClicked(QTableWidgetItem *item)
{
    if(item->row() < m_items.size())
    {
        if(LoginDialog::userType >= m_items.at(item->row()).writePrivilege && m_items.at(item->row()).disableWrite == false)
        {
            if((item->column() == 1 ) &&
                    item->row() < m_tableInfo.userRow)
            {
                int curX = cursor().pos().x();
                int curY = cursor().pos().y();
                int curWidth = combineOpsManngerTableWidget->columnWidth(item->column());
                int curHeight = combineOpsManngerTableWidget->rowHeight(item->row());

                CNumberKeyboard *numKbd = CNumberKeyboard::Instance();
                if(true == numKbd->isHidden())
                {
                    numKbd->myMove(curX, curY, curWidth, curHeight);
                    numKbd->show();
                }
            }
        }
    }
}

void CombineOpsManagerWidget::CheckValue(QTableWidgetItem* item)
{
    if (item)
    {
        CheckItem(item);
    }
}



void CombineOpsManagerWidget::CheckItem(QTableWidgetItem *item)
{
    Script *lua = LuaEngine::Instance()->GetEngine();
    String ret = item->text().toStdString();
    int index = item->row();
    CombineOpsManngerItem &combineOpsManngerItem = m_items[index];
    int type = combineOpsManngerItem.mode;
    lua->call(m_checkvalueFunc, type, ret);
    lua->pull(ret);
    item->setText(ret.c_str());
}

CombineOpsManagerWidget::~CombineOpsManagerWidget()
{

}

void CombineOpsManagerWidget::showEvent(QShowEvent *event)
{
    CNumberKeyboard *numberKey = new CNumberKeyboard(this);
    if (numberKey)
    {
        delete numberKey;
        numberKey = nullptr;
    }
}
}
