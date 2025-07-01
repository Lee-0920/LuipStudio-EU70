#include "UI/UserChangeManager/UserChangeManager.h"
#include "Log.h"
#include "UI/Frame/MessageDialog.h"
#include "UI/Frame/NumberKeyboard.h"
#include "LuaEngine/LuaEngine.h"
#include "CurveParamWidget.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "Setting/SettingManager.h"
#include "FlowManager/MeasureScheduler.h"
#include "UI/Frame/LoginDialog.h"

using namespace OOLUA;
using namespace std;
using namespace System;
using namespace Lua;
using namespace Configuration;
using namespace Flow;

namespace UI
{
CurveTableWidget::CurveTableWidget(System::String profileTableName, QWidget *parent):
    QWidget(parent)
{
    m_addr = profileTableName;
    MaintainUserChange::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this); //向窗口更新器注册

    LuaEngine* luaEngine = LuaEngine::Instance();
    m_lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, profileTableName, itermsTable);

    String configTableName;
    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("rowCount", useRowCount);
    itermsTable.at("getCalibrationTime", m_getCalibrationTime);
    itermsTable.at("getCurveK", m_getCurveK);
    itermsTable.at("getCurveB", m_getCurveB);
    itermsTable.at("getZeroConsistency", m_getZeroConsistency);
    itermsTable.at("getRangeConsistency", m_getRangeConsistency);
    itermsTable.at("name", configTableName);
    configTableName = "config." + configTableName + ".";

    this->setFixedHeight(418);
    m_parameterTableWidget = new MQtableWidget();
    m_parameterTableWidget->setColumnCount(6);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 7;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(m_parameterTableWidget, m_tableInfo);

    m_parameterTableWidget->setFixedSize(620,391);

    QFont headerFont;
    headerFont.setPointSize(15);
    headerFont.setWeight(QFont::DemiBold);

    QTableWidgetItem *column0 = new QTableWidgetItem(tr("量程"));
    column0->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem(tr("校准时间"));
    column1->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(1, column1);

    QTableWidgetItem *column2 = new QTableWidgetItem(tr("零点浓度"));
    column2->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(2, column2);

    QTableWidgetItem *column3 = new QTableWidgetItem(tr("标液浓度"));
    column3->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(3, column3);

    QTableWidgetItem *column4 = new QTableWidgetItem(tr("K"));
    column4->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(4, column4);

    QTableWidgetItem *column5 = new QTableWidgetItem(tr("B"));
    column5->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(5, column5);

    m_parameterTableWidget->horizontalHeader()->setFixedHeight(39); // 设置表头的高度
    m_parameterTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        m_parameterTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background:transparent; width: 30px;}");
    }

    QFont dataFont = m_parameterTableWidget->font();
    dataFont.setPointSize(14);
    m_parameterTableWidget->setFont(dataFont);

    SpaceInit();

    for(int i = 0;i < m_parameterTableWidget->rowCount();i++)
    {
        m_parameterTableWidget->setRowHeight(i,50);
    }

    m_parameterTableWidget->setColumnWidth(0,85);    //设置每行或每列的高度或宽度
    m_parameterTableWidget->setColumnWidth(1,173);
    m_parameterTableWidget->setColumnWidth(2,90);
    m_parameterTableWidget->setColumnWidth(3,90);
    m_parameterTableWidget->setColumnWidth(4,90);
    m_parameterTableWidget->setColumnWidth(5,90);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(m_parameterTableWidget);
    leftLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    this->setLayout(mainLayout);

    ConfigInit();
}

CurveTableWidget::~CurveTableWidget()
{

}

void CurveTableWidget::SpaceInit()
{
    for(int i = 0;i < m_parameterTableWidget->rowCount(); i++)//行
    {
        for(int j = 0;j < m_parameterTableWidget->columnCount(); j++)//列
        {
            m_parameterTableWidget->setItem(i, j, new QTableWidgetItem());
            m_parameterTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);//设置对齐方式
            QTableWidgetItem *item = m_parameterTableWidget->item(i, j);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setFlags(Qt::ItemIsEnabled);
        }
    }
}

void CurveTableWidget::ConfigInit()          //读取配置文件的值
{
    Table itermsTable;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    luaEngine->GetLuaValue(state, m_addr, itermsTable);
    Uint8 row = 0;
    oolua_ipairs(itermsTable)
    {
        Table typeTable;
        String typeText,TimeData;
        float zero,stand;
        float curveK,curveB;

        m_lua->pull(typeTable);
        typeTable.at("range", typeText);
        m_parameterTableWidget->item(row, 0)->setText(QObject::tr(typeText.c_str()));//显示类别
        m_lua->call(m_getCalibrationTime, row);
        m_lua->pull(TimeData);
        m_lua->call(m_getZeroConsistency, row);
        m_lua->pull(zero);
        m_lua->call(m_getRangeConsistency, row);
        m_lua->pull(stand);
        m_lua->call(m_getCurveK, row);
        m_lua->pull(curveK);
        m_lua->call(m_getCurveB, row);
        m_lua->pull(curveB);
        m_parameterTableWidget->item(row, 1)->setText(QObject::tr(TimeData.c_str()));//显示类别
        m_parameterTableWidget->item(row, 2)->setText(QString::number(zero));
        m_parameterTableWidget->item(row, 3)->setText(QString::number(stand));
        m_parameterTableWidget->item(row, 4)->setText(QString::number(curveK));
        m_parameterTableWidget->item(row, 5)->setText(QString::number(curveB));
        row++;
    }
    oolua_ipairs_end()

    m_parameterTableWidget->setRowCount(row);
}

void CurveTableWidget::OnUserChange()
{

}

void CurveTableWidget::showEvent(QShowEvent *event)
{
    (void)event;
    this->ConfigInit();
}

void CurveTableWidget::OnUpdateWidget(UpdateEvent event, System::String message)
{

}
}
