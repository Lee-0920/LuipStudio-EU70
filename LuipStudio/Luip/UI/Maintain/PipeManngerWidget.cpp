#include "Common.h"
#include "UseResourceWidget.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <vector>
#include <QDateTime>
#include "PipeManngerWidget.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MessageDialog.h"
#include "LuaEngine/LuaEngine.h"
#include "StatusManager/StatusManager.h"
#include "FlowManager/FlowManager.h"
#include "Log.h"
#include "Setting/SettingManager.h"
#include "UI/ControlConfig.h"

static long long g_LastClicktimes = 0;

using namespace Configuration;
using System::Uint32;
using namespace Measure;
using namespace std;
using namespace OOLUA;
using namespace Lua;
using namespace Flow;

namespace UI
{

PipeManngerWidget::PipeManngerWidget(QWidget *parent) :
        QWidget(parent),m_addAccurateMeter(false)
{
    MaintainUserChange::Instance()->Register(this);

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.operation.liquidOperator", itermsTable);

    QFont font;                           //字体
    font.setPointSize(16);                //字体大小

    this->resize(650,430);

    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("rowCount", useRowCount);
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("administratorRow", administratorRow);

    pipeManngerTableWidget = new MQtableWidget();

    pipeManngerTableWidget->setColumnCount(2);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 8;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(pipeManngerTableWidget, m_tableInfo);

    pipeManngerTableWidget->setFixedSize(415,400);

    QFont headerFont;
    headerFont.setPointSize(15);
    headerFont.setWeight(QFont::DemiBold);

    QTableWidgetItem *column0 = new QTableWidgetItem(tr("操作"));
    column0->setFont(headerFont);
    pipeManngerTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem(tr("体积(ml)"));
    column1->setFont(headerFont);
    pipeManngerTableWidget->setHorizontalHeaderItem(1, column1);

    pipeManngerTableWidget->horizontalHeader()->setFixedHeight(38); // 设置表头的高度
    pipeManngerTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");

    QFont dataFont = pipeManngerTableWidget->font();
    dataFont.setPointSize(12);
    pipeManngerTableWidget->setFont(dataFont);
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        pipeManngerTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background-color:rgb(200,200,200); width: 45;}"
                 "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background-color:rgb(240,240,240)}");
    }

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(pipeManngerTableWidget);
    measureResultTableLayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *leftLayout = new QVBoxLayout();

    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();

    typeLabel = new QLabel();
    typeLabel->setFont(headerFont);
    typeLabel->setFixedSize(180,50);
    typeLabel->setText(tr("操作方式"));
    typeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QFont comboBoxFont;
    comboBoxFont.setPointSize(16);

    typeComboBox = new QComboBox();
    typeComboBox->setObjectName("bigComboBox");
    typeComboBox->addItem(tr("泵计步定量"));
    typeComboBox->addItem(tr("光学定量"));
    typeComboBox->setCurrentIndex(1);
    typeComboBox->setFont(comboBoxFont);
    typeComboBox->setFixedSize(180,50);

    stopButton = new QPushButton();
    stopButton->setObjectName("redButton");
    stopButton->setText(tr("停止"));
    stopButton->setFont(font);
    stopButton->setFixedSize(130,54);

    QVBoxLayout*operateTypeVLayout = new QVBoxLayout();
    operateTypeVLayout->addWidget(typeLabel);
    operateTypeVLayout->addWidget(typeComboBox);

    QHBoxLayout *operateTypeLayout = new QHBoxLayout();
    operateTypeLayout->addStretch();
    operateTypeLayout->addLayout(operateTypeVLayout);
    operateTypeLayout->addStretch();
    operateTypeLayout->setContentsMargins(0, 50, 0, 0);

    QHBoxLayout *stopButtonLayout = new QHBoxLayout();
    stopButtonLayout->addSpacing(25);
    stopButtonLayout->addWidget(stopButton);
//    stopButtonLayout->addStretch();

    QVBoxLayout *rightLayout = new QVBoxLayout();
//    rightLayout->addLayout(operateTypeLayout);
    rightLayout->addStretch();
    rightLayout->addLayout(stopButtonLayout);
    rightLayout->addStretch();
    rightLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    mainLayout->addStretch();
    this->setLayout(mainLayout);

    pipeManngerTableWidget->setColumnWidth(0,240);    //设置每行或每列的高度或宽度
    if (m_tableInfo.userRow > 10)
    {
        pipeManngerTableWidget->setColumnWidth(1,128);
    }
    else
    {
        pipeManngerTableWidget->setColumnWidth(1,158);
    }

    for(int i = 0;i < pipeManngerTableWidget->rowCount();i++)
    {
        pipeManngerTableWidget->setRowHeight(i, 60);
    }

    pipeManngerTableWidget->setFont(font);

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
            PipeManngerItem pipeManngerItem;

            itermTable.at("text", text);
            itermTable.at("data", data);
            itermTable.at("createFlow", pipeManngerItem.createFlow);
            itermTable.at("writePrivilege", pipeManngerItem.writePrivilege);
            itermTable.at("readPrivilege", pipeManngerItem.readPrivilege);

            m_items.insert(make_pair(row, pipeManngerItem));

            QHBoxLayout *hLayout = new QHBoxLayout();;
            QWidget *widget =  new QWidget();
            widget->setFixedSize(240,60);
            QPushButton *button = new QPushButton();
            button->setObjectName("brownButton");
            button->setFixedSize(238,57);
            button->setFont(font);
            button->setText(QObject::tr(text.c_str()));
            m_buttonList.append(button);
            hLayout->addWidget(button);
            hLayout->setAlignment(button, Qt::AlignCenter);
            hLayout->setMargin(0);
            widget->setLayout(hLayout);
            pipeManngerTableWidget->setCellWidget(row, 0, widget);
            connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
            signalMapper->setMapping(button, row);

            pipeManngerTableWidget->item(row, 1)->setText(QString::number(data));

            row++;
        }
    }
    oolua_ipairs_end()

    if(m_items.size() < pipeManngerTableWidget->rowCount())
    {
        for(int index = m_items.size();index < pipeManngerTableWidget->rowCount();index++)
        {
            pipeManngerTableWidget->setRowHidden(index, true);
        }
    }

    itermsTable.at("checkvalue", m_checkvalueFunc);
    OnUserChange();

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(MeasureStart(int)));
    connect(pipeManngerTableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(ChoseDay()));
    connect(stopButton, SIGNAL(clicked(bool)), this, SLOT(StopMeasure()));
    connect(pipeManngerTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this ,SLOT(CheckValue(QTableWidgetItem*)));
    connect(typeComboBox, SIGNAL(currentIndexChanged(int)), this ,SLOT(SlotIndexChanged(int)));

}

void PipeManngerWidget::SpaceInit()
{
    for(int i = 0;i < pipeManngerTableWidget->rowCount();i++)
    {
        for(int j = 0;j < pipeManngerTableWidget->columnCount();j++)
        {
            pipeManngerTableWidget->setItem(i, j, new QTableWidgetItem());
            pipeManngerTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
            if(j!=1 || (i >= m_tableInfo.userRow))
            {
                QTableWidgetItem *item = pipeManngerTableWidget->item(i, j);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
        }
    }
}

void PipeManngerWidget::MeasureStart(int index)
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
        PipeManngerItem &pipeManngerItem = m_items[index];
        Lua_func_ref func = pipeManngerItem.createFlow;

        int type = typeComboBox->currentIndex();
        float vol = pipeManngerTableWidget->item(index, 1)->text().toFloat();
        if (lua->call(func, type, vol))
        {
            FlowManager::Instance()->StartFlow();
        }
        else
        {
            logger->warn("管道操作启动流失败");
        }
    }
}

void PipeManngerWidget::ChoseDay()
{
//    if(LoginDialog::userType == Administrator)
//    {
        QTableWidgetItem *item = pipeManngerTableWidget->currentItem();
        if(item->column() == 1 && item->row() < m_tableInfo.userRow)
        {
            int curX = cursor().pos().x();
            int curY = cursor().pos().y();
            int curWidth = pipeManngerTableWidget->columnWidth(item->column());
            int curHeight = pipeManngerTableWidget->rowHeight(item->row());

            CNumberKeyboard *numKbd = CNumberKeyboard::Instance();
            if(true == numKbd->isHidden())
            {
                numKbd->myMove(curX, curY, curWidth, curHeight);
                numKbd->show();
            }
        }
//    }
}

void PipeManngerWidget::StopMeasure()
{
    FlowManager::Instance()->StopFlow();
}

void PipeManngerWidget::showEvent(QShowEvent *event)
{
    CNumberKeyboard *numberKey = new CNumberKeyboard(this);
    if (numberKey)
    {
        delete numberKey;
        numberKey = nullptr;
    }
}

void PipeManngerWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        if (!m_addAccurateMeter)
        {
            m_addAccurateMeter = true;
            typeComboBox->addItem(tr("精确定量"));
        }

        for (map<Uint8, PipeManngerItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;

            pipeManngerTableWidget->setRowHidden(row, false);
        }
    }
    else if(LoginDialog::userType == Administrator)
    {
        if (m_addAccurateMeter)
        {
            m_addAccurateMeter = false;
            if (typeComboBox->currentIndex() > 1)
            {
                typeComboBox->setCurrentIndex(1);
            }
            typeComboBox->removeItem(2);
        }
        for (map<Uint8, PipeManngerItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            PipeManngerItem &pipeManngerItem = iter->second;
            if (Administrator >= pipeManngerItem.readPrivilege)
            {
                pipeManngerTableWidget->setRowHidden(row, false);
            }
            else
            {
                pipeManngerTableWidget->setRowHidden(row, true);
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        if (m_addAccurateMeter)
        {
            m_addAccurateMeter = false;
            if (typeComboBox->currentIndex() > 1)
            {
                typeComboBox->setCurrentIndex(1);
            }
            typeComboBox->removeItem(2);
        }
        for (map<Uint8, PipeManngerItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            PipeManngerItem &pipeManngerItem = iter->second;
            if (Maintain >= pipeManngerItem.readPrivilege)
            {
                pipeManngerTableWidget->setRowHidden(row, false);
            }
            else
            {
                pipeManngerTableWidget->setRowHidden(row, true);
            }
        }
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(pipeManngerTableWidget, m_tableInfo);
}


void PipeManngerWidget::SlotIndexChanged(int)
{
    for(int i = 0; i < m_tableInfo.userRow; i++)
    {
        CheckItem(pipeManngerTableWidget->item(i, 1));
    }
}

void PipeManngerWidget::CheckValue(QTableWidgetItem* item)
{
    if (item)
    {
        CheckItem(item);
    }
}



void PipeManngerWidget::CheckItem(QTableWidgetItem *item)
{
    Script *lua = LuaEngine::Instance()->GetEngine();
    int type = typeComboBox->currentIndex();
    String ret = item->text().toStdString();

    lua->call(m_checkvalueFunc, type, ret);
    lua->pull(ret);
    item->setText(ret.c_str());
}

PipeManngerWidget::~PipeManngerWidget()
{

}
}
