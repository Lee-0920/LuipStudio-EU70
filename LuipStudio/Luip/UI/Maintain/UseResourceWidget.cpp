#include "Interface/Wqimc/WqimcManager.h"
#include "UseResourceWidget.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "Log.h"
#include "System/Types.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "Setting/SettingManager.h"
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

UseResourceWidget::UseResourceWidget(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);

    Script *lua = LuaEngine::Instance()->GetEngine();
    Table table, itermsTable;
    itermsTable.bind_script(*lua);
    itermsTable.set_table("setting");
    itermsTable.at("ui", table);
    table.at("useResourceWidget", itermsTable);

    QFont font;                           //字体
    font.setPointSize(14);                //字体大小

    this->resize(650,430);

    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;
    itermsTable.at("rowCount", useRowCount);
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("administratorRow", administratorRow);

    useResoureTableWidget = new MQtableWidget();

    useResoureTableWidget->setColumnCount(5);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 8;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(useResoureTableWidget, m_tableInfo);

    useResoureTableWidget->setFixedSize(562,400);

    QStringList columnName;
    columnName<<tr("耗材")<<tr("上次维护")<<tr("到期时间")<<tr("周期(月)")<<tr("操作");
    useResoureTableWidget->setColumnAndSize(columnName,15);

    useResoureTableWidget->horizontalHeader()->setFixedHeight(38); // 设置表头的高度
    useResoureTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");

    QFont dataFont = useResoureTableWidget->font();
    dataFont.setPointSize(10);
    useResoureTableWidget->setFont(dataFont);
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        useResoureTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background:transparent; width: 30px;}");
    }
    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(useResoureTableWidget);
    measureResultTableLayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *leftLayout = new QVBoxLayout();

    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    this->setLayout(mainLayout);

    useResoureTableWidget->setColumnWidth(0,145);
    useResoureTableWidget->setColumnWidth(1,115);
    useResoureTableWidget->setColumnWidth(2,115);
    useResoureTableWidget->setColumnWidth(3,85);
    useResoureTableWidget->setColumnWidth(4,100);    //设置每行或每列的高度或宽度

    useResoureTableWidget->setFont(font);

    for(int i = 0;i < useResoureTableWidget->rowCount();i++)
    {
        useResoureTableWidget->setRowHeight(i, 45);
    }

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
            UseResourceItem useResourceItem;

            itermTable.at("name", useResourceItem.name);

            itermTable.at("text", useResourceItem.text);
            useResoureTableWidget->item(row, 0)->setText(QObject::tr(useResourceItem.text.c_str()));//显示名称

            itermTable.at("writePrivilege", useResourceItem.writePrivilege);
            itermTable.at("readPrivilege", useResourceItem.readPrivilege);
            m_items.insert(make_pair(row, useResourceItem));
            row++;
        }
    }
    oolua_ipairs_end()
    itermsTable.at("checkMaterialLife", m_checkMaterialLife);
    itermsTable.at("changeMaterialLife", m_changeMaterialLife);

    ConnectInit(m_tableInfo.userRow);
    ConfigInit();
    OnUserChange();

    m_materialLifeTimer = new QTimer();
    m_materialLifeTimer->setInterval(1000*3600);
    m_materialLifeTimer->start();
    connect(m_materialLifeTimer,SIGNAL(timeout()),this,SLOT(SlotMaterialLifeCheck()));
    connect(useResoureTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(slotDoubleClicked(QTableWidgetItem*)));
    connect(useResoureTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(CheckValue(QTableWidgetItem *)));
}

void UseResourceWidget::SlotMaterialLifeCheck()
{
    Script *lua = LuaEngine::Instance()->GetEngine();
    lua->call(m_checkMaterialLife);
}

void UseResourceWidget::ConnectInit(Uint8 useRowCount)
{
    QFont font;                           //字体
    font.setPointSize(14);
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(int i = 0; i < useRowCount; i++)
    {
        QPushButton *button = new QPushButton();
        button->setFixedSize(98,45);
        button->setFont(font);
        button->setObjectName("brownButton");
        button->setText(tr("更换"));

        m_buttonList.append(button);
        useResoureTableWidget->setCellWidget(i,4,button);
        connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(button, i);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(UseMeasure(int)));
}

void UseResourceWidget::UseMeasure(int index)
{
    QDateTime oldDate;
    QDateTime newDate;
    oldDate = QDateTime::currentDateTime();

    LuaEngine* luaEngine = LuaEngine::Instance();

    if (m_items.count(index))
    {
        UseResourceItem &useResourceItem = m_items[index];
        String name = useResourceItem.name;
        String text = useResourceItem.text;

        luaEngine->SetLuaValue(String("config.consumable." + name + ".lastTime"), oldDate.toTime_t());
        luaEngine->SetLuaValue(String("config.consumable." + name + ".cycle"), useResoureTableWidget->item(index, 3)->text().toInt());

        Script *lua = LuaEngine::Instance()->GetEngine();
        lua->call(m_changeMaterialLife, name);
        QString strLog = "更换" + QString::fromStdString(useResourceItem.text);
        logger->info(strLog.toStdString().c_str());
    }
    newDate = oldDate.addMonths(useResoureTableWidget->item(index, 3)->text().toInt());
    useResoureTableWidget->item(index, 1)->setText(oldDate.toString("yyyy.MM.dd"));
    useResoureTableWidget->item(index, 2)->setText(newDate.toString("yyyy.MM.dd"));

    SettingManager::Instance()->ConsumableSave();
    WqimcManager::Instance()->uploadConsumableProfile();
}

void UseResourceWidget::ConfigInit()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    for (map<Uint8, UseResourceItem>::iterator iter = m_items.begin();iter != m_items.end(); ++iter)
    {
        Uint8 row = iter->first;
        UseResourceItem &useResourceItem = iter->second;
        String name = useResourceItem.name;

        float cycle;
        luaEngine->GetLuaValue(state, String("config.consumable." + name + ".cycle"), cycle);

        int lastTime;
        luaEngine->GetLuaValue(state, String("config.consumable." + name + ".lastTime"), lastTime);

        useResoureTableWidget->item(row, 1)->setText(QDateTime::fromTime_t(lastTime).toString("yyyy.MM.dd"));
        useResoureTableWidget->item(row, 2)->setText(QDateTime::fromTime_t(lastTime).addMonths(cycle).toString("yyyy.MM.dd"));
        useResoureTableWidget->item(row, 3)->setText(QString::number(cycle));
    }
}

void UseResourceWidget::slotDoubleClicked(QTableWidgetItem *item)
{
    if(item->column() == 3 && item->row() < m_tableInfo.userRow)
    {
        int curX = cursor().pos().x();
        int curY = cursor().pos().y();
        int curWidth = useResoureTableWidget->columnWidth(item->column());
        int curHeight = useResoureTableWidget->rowHeight(item->row());

        CNumberKeyboard *numKbd = CNumberKeyboard::Instance();
        if(true == numKbd->isHidden())
        {
            numKbd->myMove(curX, curY, curWidth, curHeight);
            numKbd->show();
        }
    }
}

void UseResourceWidget::SpaceInit()
{
    for(int i = 0;i < useResoureTableWidget->rowCount();i++)
    {
        for(int j = 0;j < useResoureTableWidget->columnCount();j++)
        {
            useResoureTableWidget->setItem(i, j, new QTableWidgetItem());
            useResoureTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
            if((j!= 3) || (i >= m_tableInfo.userRow))
            {
                QTableWidgetItem *item = useResoureTableWidget->item(i, j);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
}

void UseResourceWidget::CheckValue(QTableWidgetItem *item)
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    if (item)
    {
        if (item->column() == 3)
        {
            if (m_items.count(item->row()))
            {
                UseResourceItem &useResourceItem = m_items[item->row()];
                String name = useResourceItem.name;
                float cycle;
                luaEngine->GetLuaValue(state, String("config.consumable." + name + ".cycle"), cycle);
                CheckItem(item->text(), cycle);
            }
        }
    }
}

void UseResourceWidget::CheckItem(QString stringValue, float &cycleValue)
{
    bool valueValid = true;
    int index = useResoureTableWidget->currentRow();
    int column = useResoureTableWidget->currentColumn();
    QString tmpPattern("^\\d{1,2}$");  //匹配1-2位整数
    QRegExp tmpRx(tmpPattern);
    valueValid = tmpRx.exactMatch(stringValue);
    if(false == valueValid)
    {
        QTableWidgetItem *item = useResoureTableWidget->item(index, column);
        if (item)
        {
            item->setText(QString::number(cycleValue));
        }
    }
}

void UseResourceWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        for (map<Uint8, UseResourceItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;

            useResoureTableWidget->setRowHidden(row, false);
        }
    }
    else if(LoginDialog::userType == Administrator)
    {
        for (map<Uint8, UseResourceItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            UseResourceItem &useResourceItem = iter->second;
            if (Administrator >= useResourceItem.readPrivilege)
            {
                useResoureTableWidget->setRowHidden(row, false);
            }
            else
            {
                useResoureTableWidget->setRowHidden(row, true);
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        for (map<Uint8, UseResourceItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            UseResourceItem &useResourceItem = iter->second;
            if (Maintain >= useResourceItem.readPrivilege)
            {
                useResoureTableWidget->setRowHidden(row, false);
            }
            else
            {
                useResoureTableWidget->setRowHidden(row, true);
            }
        }
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(useResoureTableWidget, m_tableInfo);
}

void UseResourceWidget::showEvent(QShowEvent *event)
{
    (void)event;
    this->ConfigInit();
}

UseResourceWidget::~UseResourceWidget()
{

}
}
