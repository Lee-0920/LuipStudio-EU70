#include "Interface/Wqimc/WqimcManager.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "Log.h"
#include "System/Types.h"
#include "Setting/SettingManager.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "ReagentManagerWidget.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/ControlConfig.h"

using namespace Configuration;
using System::Uint32;
using namespace OOLUA;
using namespace std;
using namespace Lua;
using namespace Interface::Wqimc;

namespace UI
{

ReagentManagerWidget::ReagentManagerWidget(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);

    Script *lua = LuaEngine::Instance()->GetEngine();
    Table table, itermsTable;
    itermsTable.bind_script(*lua);
    itermsTable.set_table("setting");
    itermsTable.at("ui", table);
    table.at("reagentManager", itermsTable);

    QFont headFont;                           //字体
    headFont.setPointSize(11);                //字体大小

    QFont font;                           //字体
    font.setPointSize(12);                //字体大小

    this->resize(650,430);

    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("rowCount", useRowCount);
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("administratorRow", administratorRow);

    ReagentManagerTableWidget = new MQtableWidget();

    ReagentManagerTableWidget->setColumnCount(6);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 6;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(ReagentManagerTableWidget, m_tableInfo);

    ReagentManagerTableWidget->setFixedSize(590,405);

    QFont headerFont;                           //字体
    headerFont.setPointSize(12);                //字体大小
    headerFont.setWeight(QFont::Medium);

    QTableWidgetItem *column0 = new QTableWidgetItem(tr("耗材"));
    column0->setFont(headerFont);
    ReagentManagerTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem("上次维护");
    column1->setFont(headerFont);
    ReagentManagerTableWidget->setHorizontalHeaderItem(1, column1);

    QTableWidgetItem *column2 = new QTableWidgetItem(tr("余量(mL)"));
    column2->setFont(headerFont);
    ReagentManagerTableWidget->setHorizontalHeaderItem(2, column2);

    QTableWidgetItem *column3 = new QTableWidgetItem(tr("总量(mL)"));
    column3->setFont(headerFont);
    ReagentManagerTableWidget->setHorizontalHeaderItem(3, column3);

    QTableWidgetItem *column4 = new QTableWidgetItem(tr("周期(月)"));
    column4->setFont(headerFont);
    ReagentManagerTableWidget->setHorizontalHeaderItem(4, column4);

    QTableWidgetItem *column5 = new QTableWidgetItem(tr("操作"));
    column5->setFont(headerFont);
    ReagentManagerTableWidget->setHorizontalHeaderItem(5, column5);

    ReagentManagerTableWidget->horizontalHeader()->setFixedHeight(39); // 设置表头的高度
    ReagentManagerTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        ReagentManagerTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background-color:rgb(200,200,200); width: 36;}"
                 "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background-color:rgb(240,240,240)}");
    }

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(ReagentManagerTableWidget);
    measureResultTableLayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *leftLayout = new QVBoxLayout();

    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    this->setLayout(mainLayout);

    //设置每行或每列的高度或宽度
    if (m_tableInfo.userRow <= 6)
    {
        ReagentManagerTableWidget->setFixedWidth(554);
    }

    ReagentManagerTableWidget->setColumnWidth(0,110);
    ReagentManagerTableWidget->setColumnWidth(1,110);
    ReagentManagerTableWidget->setColumnWidth(2,79);
    ReagentManagerTableWidget->setColumnWidth(3,79);
    ReagentManagerTableWidget->setColumnWidth(4,74);
    ReagentManagerTableWidget->setColumnWidth(5,100);

    for(int i = 0;i < ReagentManagerTableWidget->rowCount();i++)
    {
        ReagentManagerTableWidget->setRowHeight(i, 60);
    }
    SpaceInit();

    int type = SettingManager::Instance()->GetParamType();

    QFont textFont;
    textFont.setPointSize(13);
    textFont.setWeight(50);

    Uint8 row = 0;
    oolua_ipairs(itermsTable)
    {
        Table itermTable;
        lua->pull(itermTable);

        int targetPrivilege = 0;
        bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

        if (isTarget == false || (type & targetPrivilege))
        {
            ReagentManagerItem reagentManagerItem;

            itermTable.at("name", reagentManagerItem.name);
            itermTable.at("text", reagentManagerItem.text);
            QString qText;
            qText = QString::fromStdString(reagentManagerItem.text);

            ReagentManagerTableWidget->item(row, 0)->setFont(textFont);

            QFont reagentNameFont = ReagentManagerTableWidget->item(row, 0)->font();
            QFontMetrics fontWidth(reagentNameFont);
            int textWidth = fontWidth.width(qText);
            if (textWidth > 160)
            {
                int size = reagentNameFont.pointSize() - (int)((textWidth-160)/40) - 2;
                if (size < 8)
                {
                    size = 8;
                }
                reagentNameFont.setPointSize(size);
                ReagentManagerTableWidget->item(row, 0)->setFont(reagentNameFont);
            }

            ReagentManagerTableWidget->item(row, 0)->setText(QObject::tr(reagentManagerItem.text.c_str()));//显示名称

            itermTable.at("writePrivilege", reagentManagerItem.writePrivilege);
            itermTable.at("readPrivilege", reagentManagerItem.readPrivilege);

            m_items.insert(make_pair(row, reagentManagerItem));
            row++;
        }
    }
    oolua_ipairs_end()
    itermsTable.at("changeReagent", m_changeReagentFunc);
    itermsTable.at("checkReagentDate", m_checkReagentDate);

    if(m_items.size() < ReagentManagerTableWidget->rowCount())
    {
        for(int i = m_items.size();i < m_tableInfo.userRow; i++)
        {
            ReagentManagerTableWidget->setRowHidden(i,true);
        }
    }

    if (m_items.size() <=  6)
    {
        ReagentManagerTableWidget->setFixedSize(554,405);
    }

    ConnectInit(m_items.size());
    ConfigInit();
    OnUserChange();

    connect(ReagentManagerTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(slotDoubleClicked(QTableWidgetItem*)));
    connect(ReagentManagerTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(CheckValue(QTableWidgetItem*)));

    m_reagentDateTimer = new QTimer();
    m_reagentDateTimer->setInterval(1000 * 3600);
    m_reagentDateTimer->start();
    connect(m_reagentDateTimer,SIGNAL(timeout()),this,SLOT(SlotReagentDateCheck()));

}

void ReagentManagerWidget::SlotReagentDateCheck()
{
    Script *lua = LuaEngine::Instance()->GetEngine();
    lua->call(m_checkReagentDate);
}

void ReagentManagerWidget::ConnectInit(Uint8 useRowCount)
{
    QFont font;                           //字体
    font.setPointSize(16);
    font.setWeight(60);
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(int i = 0; i < useRowCount; i++)
    {
        QHBoxLayout *hLayout = new QHBoxLayout();;
        QWidget *widget =  new QWidget();
        widget->setFixedSize(98,60);

        QPushButton *button = new QPushButton();
        button->setObjectName("brownButton");
        button->setFixedSize(96,55);
        button->setFont(font);

        button->setText(tr("更换"));

        hLayout->addWidget(button);
        hLayout->setAlignment(button, Qt::AlignCenter);
        hLayout->setMargin(0);
        widget->setLayout(hLayout);

        QFont font;
        font.setPointSize(14);
        QLabel *label= new QLabel();
        label->setAlignment(Qt::AlignCenter);
        label->setFont(font);

        m_labelList.append(label);
        ReagentManagerTableWidget->setCellWidget(i,1,label);

        m_buttonList.append(button);
        ReagentManagerTableWidget->setCellWidget(i,5,widget);
        connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(button, i);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(SlotUseMeasure(int)));
}

void ReagentManagerWidget::SlotOneKeyChangeReagent()
{
    for(int i = 0; i < (int)m_items.size(); i++ )
    {
        this->SlotUseMeasure(i);
        QThread::msleep(20);
    }
}

void ReagentManagerWidget::SlotUseMeasure(int index)
{
    QDateTime oldDate;
    oldDate = QDateTime::currentDateTime();
    float vol = 0;
    LuaEngine *luaEngine = LuaEngine::Instance();

    if (m_items.count(index))
    {
        Script *lua = LuaEngine::Instance()->GetEngine();
        ReagentManagerItem &reagentManagerItem = m_items[index];
        String name = reagentManagerItem.name;
        String text = reagentManagerItem.text;

        luaEngine->SetLuaValue(String("config.remain." + name + ".lastTime"), oldDate.toTime_t());
        luaEngine->SetLuaValue(String("config.remain." + name + ".cycle"), ReagentManagerTableWidget->item(index, 4)->text().toInt());
        vol = ReagentManagerTableWidget->item(index, 3)->text().toFloat();
        luaEngine->SetLuaValue(String("config.remain." + name + ".total"), vol);
        lua->call(m_changeReagentFunc, name, vol);
        QString strLog = "更换" + QString::fromStdString(text);
        logger->info(strLog.toStdString().c_str());
    }

//    int cycle = ReagentManagerTableWidget->item(index,4)->text().toInt();
//    QString strNewDate = oldDate.addMonths(cycle).toString("yyyy.MM.dd");
//    QString strBlueColor = QString("<font color=green>%1").arg(oldDate.toString("yyyy.MM.dd"));
//    QString strRedColor = QString("<font color=red>%1").arg(strNewDate);
    m_labelList.at(index)->setText(oldDate.toString("yyyy.MM.dd"));

    SettingManager::Instance()->RemainSave();
    WqimcManager::Instance()->uploadReagentProfile();
}
void ReagentManagerWidget::showEvent(QShowEvent *event)
{
    CNumberKeyboard *numberKey = new CNumberKeyboard(this);
    if (numberKey)
    {
        delete numberKey;
        numberKey = nullptr;
    }
    (void)event;
    this->ConfigInit();
}

void ReagentManagerWidget::ConfigInit()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    for (map<Uint8, ReagentManagerItem>::iterator iter = m_items.begin();iter != m_items.end(); ++iter)
    {
        Uint8 row = iter->first;
        ReagentManagerItem &reagentManagerItem = iter->second;
        String name = reagentManagerItem.name;

        float cycle;
        luaEngine->GetLuaValue(state, String("config.remain."+ name +".cycle"), cycle);

        int lastTime;
        luaEngine->GetLuaValue(state, String("config.remain."+ name +".lastTime"), lastTime);

//        QString strBlueColor = QString("<font color=green>%1").arg(QDateTime::fromTime_t(lastTime).toString("yyyy.MM.dd"));
//        QString strRedColor = QString("<font color=red>%1").arg(QDateTime::fromTime_t(lastTime).addMonths(cycle).toString("yyyy.MM.dd"));
        m_labelList.at(row)->setText(QDateTime::fromTime_t(lastTime).toString("yyyy.MM.dd"));

        QFont textFont;
        textFont.setPointSize(16);
        textFont.setWeight(60);

        float vol;
        luaEngine->GetLuaValue(state, String("status.remain."+ name), vol);

        float total;
        luaEngine->GetLuaValue(state, String("config.remain."+ name +".total"), total);
        ReagentManagerTableWidget->item(row, 2)->setText(QString::number((int)vol));
        ReagentManagerTableWidget->item(row, 2)->setFont(textFont);
        ReagentManagerTableWidget->item(row, 3)->setText(QString::number((int)total));
        ReagentManagerTableWidget->item(row, 3)->setFont(textFont);
        ReagentManagerTableWidget->item(row, 4)->setText(QString::number(cycle));
        ReagentManagerTableWidget->item(row, 4)->setFont(textFont);
    }
}

void ReagentManagerWidget::slotDoubleClicked(QTableWidgetItem *item)
{
    int useRow = 0;
    if (m_items.size() < m_tableInfo.userRow)
    {
        useRow = m_items.size();
    }
    else
    {
        useRow = m_tableInfo.userRow;
    }
    if((item->column() == 3 || item->column() == 4) &&
        item->row() < useRow)
    {
        int curX = cursor().pos().x();
        int curY = cursor().pos().y();
        int curWidth = ReagentManagerTableWidget->columnWidth(item->column());
        int curHeight = ReagentManagerTableWidget->rowHeight(item->row());

        CNumberKeyboard *numKbd = CNumberKeyboard::Instance();
        if(true == numKbd->isHidden())
        {
            numKbd->myMove(curX, curY, curWidth, curHeight);
            numKbd->show();
        }
    }
}

void ReagentManagerWidget::SpaceInit()
{
    for(int i = 0;i < ReagentManagerTableWidget->rowCount();i++)
    {
        for(int j = 0;j < ReagentManagerTableWidget->columnCount();j++)
        {
            ReagentManagerTableWidget->setItem(i, j, new QTableWidgetItem());
            ReagentManagerTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
            if((j!= 3 && j!= 4) || (i >= m_tableInfo.userRow))
            {
                QTableWidgetItem *item = ReagentManagerTableWidget->item(i, j);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
}

void ReagentManagerWidget::CheckValue(QTableWidgetItem *item)
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    if (item)
    {
        if (item->column() == 3)
        {
            if (m_items.count(item->row()))
            {
                ReagentManagerItem &reagentManagerItem = m_items[item->row()];
                String name = reagentManagerItem.name;
                float total;
                luaEngine->GetLuaValue(state, String("config.remain." + name + ".total"), total);
                CheckVolumeItem(item->text(), total);
            }
        }
        else if (item->column() == 4)
        {
            if (m_items.count(item->row()))
            {
                ReagentManagerItem &reagentManagerItem = m_items[item->row()];
                String name = reagentManagerItem.name;
                float cycle;
                luaEngine->GetLuaValue(state, String("config.remain."+ name +".cycle"), cycle);
                CheckItem(item->text(), cycle);
            }
        }
    }
}

void ReagentManagerWidget::CheckVolumeItem(QString stringValue, float &volume)
{
    bool valueValid = true;
    int index = ReagentManagerTableWidget->currentRow();
    int column = ReagentManagerTableWidget->currentColumn();
    QString tmpPattern("^\\d{1,6}$");  //匹配1-6位整数
    QRegExp tmpRx(tmpPattern);

    valueValid = tmpRx.exactMatch(stringValue);
    if(false == valueValid)
    {
        QTableWidgetItem *item = ReagentManagerTableWidget->item(index, column);
        if (item)
        {
            item->setText(QString::number(volume));
        }
    }
}

void ReagentManagerWidget::CheckItem(QString stringValue, int &cycleValue)
{
    bool valueValid = true;
    int index = ReagentManagerTableWidget->currentRow();
    int column = ReagentManagerTableWidget->currentColumn();
    QString tmpPattern("^\\d{1,2}$");  //匹配1-2位整数
    QRegExp tmpRx(tmpPattern);

    valueValid = tmpRx.exactMatch(stringValue);
    if(false == valueValid)
    {
        QTableWidgetItem *item = ReagentManagerTableWidget->item(index, column);
        if (item)
        {
            item->setText(QString::number(cycleValue));
        }
    }
}

void ReagentManagerWidget::CheckItem(QString stringValue, float &cycleValue)
{
    bool valueValid = true;
    int index = ReagentManagerTableWidget->currentRow();
    int column = ReagentManagerTableWidget->currentColumn();
    QString tmpPattern("^\\d{1,2}$");  //匹配1-2位整数
    QRegExp tmpRx(tmpPattern);
    valueValid = tmpRx.exactMatch(stringValue);
    if(false == valueValid)
    {
        QTableWidgetItem *item = ReagentManagerTableWidget->item(index, column);
        if (item)
        {
            item->setText(QString::number(cycleValue));
        }
    }
}

void ReagentManagerWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        for (map<Uint8, ReagentManagerItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;

            ReagentManagerTableWidget->setRowHidden(row, false);
        }
    }
    else if(LoginDialog::userType == Administrator)
    {
        for (map<Uint8, ReagentManagerItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            ReagentManagerItem &reagentManagerItem = iter->second;
            if (Administrator >= reagentManagerItem.readPrivilege)
            {
                ReagentManagerTableWidget->setRowHidden(row, false);
            }
            else
            {
                ReagentManagerTableWidget->setRowHidden(row, true);
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        for (map<Uint8, ReagentManagerItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            ReagentManagerItem &reagentManagerItem = iter->second;
            if (Maintain >= reagentManagerItem.readPrivilege)
            {
                ReagentManagerTableWidget->setRowHidden(row, false);
            }
            else
            {
                ReagentManagerTableWidget->setRowHidden(row, true);
            }
        }
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(ReagentManagerTableWidget, m_tableInfo);
}

ReagentManagerWidget::~ReagentManagerWidget()
{

}

}
