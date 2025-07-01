#include "UI/UserChangeManager/UserChangeManager.h"
#include "Log.h"
#include "UI/Frame/MessageDialog.h"
#include "UI/Frame/NumberKeyboard.h"
#include "UI/Frame/InputKeyBoard.h"
#include "LuaEngine/LuaEngine.h"
#include "ParameterWidget.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "Setting/SettingManager.h"
#include "Lua/App.h"
#include "UI/ControlConfig.h"
#include "checkscreen.h"
#include "UI/Frame/SignatureDialog.h"
#include "System/ProLicenseManager.h"

#define PARAMRTER_LIMIT_SIZE FONT_SIZE_SETTINGS(13)

using namespace OOLUA;
using namespace std;
using namespace System;
using namespace Lua;
using namespace Configuration;

namespace UI
{
ParameterWidget::ParameterWidget(System::String profileTableName, QWidget *parent):
    QWidget(parent),
    m_isConfigInit(false),m_profileTableText(""),m_rangeParamStart(0),m_rangeParamEnd(0),RangeMasterRowNum(0),isMeasureParamItems(false),m_isOEM(0)
{
    MaintainUserChange::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this); //向窗口更新器注册

    LuaEngine* luaEngine = LuaEngine::Instance();

    m_lua = luaEngine->GetEngine();
    lua_State* state = luaEngine->GetThreadState();
    Table itermsTable;
    luaEngine->GetLuaValue(state, profileTableName, itermsTable);

    String configTableName;
    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("updateEvent", m_eventName);
    itermsTable.at("rowCount", useRowCount);
    itermsTable.at("administratorRow", administratorRow);
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("isMeaParaml", m_isMeaParaml);
    itermsTable.at("defaultRestore", m_defaultRestore);
    itermsTable.at("saveFile", m_saveFile);
    OOLUA::Lua_func_ref showCheck;
    itermsTable.safe_at("isShowCheck", showCheck);

    OOLUA::Lua_func_ref checkOEM;
    if (itermsTable.safe_at("checkOEM", checkOEM))
    {
        m_lua->call(checkOEM);
        m_lua->pull(m_isOEM);
    }

    if (itermsTable.safe_at("getCurveKBRange", m_getCurveKBRange)
            && itermsTable.safe_at("updaterCurveParam", m_updaterCurveParam))
    {
        isMeasureParamItems = true;
    }
    itermsTable.at("text", m_profileTableText);
    itermsTable.at("name", configTableName);
    configTableName = "config." + configTableName + ".";

    this->setFixedHeight(418);
    m_parameterTableWidget = new MQtableWidget();
    m_parameterTableWidget->setColumnCount(4);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 9;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(m_parameterTableWidget, m_tableInfo);

    m_parameterTableWidget->setFixedSize(585,349);

    QFont headerFont;
    headerFont.setPointSize(15);
    headerFont.setWeight(QFont::DemiBold);

    QTableWidgetItem *column0 = new QTableWidgetItem(tr("类别"));
    column0->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem(tr("参数项"));
    column1->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(1, column1);

    QTableWidgetItem *column2 = new QTableWidgetItem(tr("值"));
    column2->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(2, column2);

    QTableWidgetItem *column3 = new QTableWidgetItem(tr("单位"));
    column3->setFont(headerFont);
    m_parameterTableWidget->setHorizontalHeaderItem(3, column3);

    m_parameterTableWidget->horizontalHeader()->setFixedHeight(39); // 设置表头的高度
    m_parameterTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");

    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        m_parameterTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background-color:rgb(200,200,200); width: 45;}"
                 "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background-color:rgb(240,240,240)}");
    }

    QFont dataFont = m_parameterTableWidget->font();
    dataFont.setPointSize(PARAMRTER_LIMIT_SIZE);
    m_parameterTableWidget->setFont(dataFont);

    SpaceInit();

    for(int i = 0;i < m_parameterTableWidget->rowCount();i++)
    {
        m_parameterTableWidget->setRowHeight(i,COMMON_BUTTONS_COEFFICIENT_VERTICAL(37));
    }

    m_parameterTableWidget->setColumnWidth(0,115);    //设置每行或每列的高度或宽度
    m_parameterTableWidget->setColumnWidth(1,180);
    m_parameterTableWidget->setColumnWidth(2,155);
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        m_parameterTableWidget->setColumnWidth(3,87);
    }
    else
    {
        m_parameterTableWidget->setColumnWidth(3,126);
    }

    m_defaultButtom = new QPushButton();
    QFont buttomFont = m_defaultButtom->font();
    buttomFont.setPointSize(15);
    buttomFont.setWeight(70);
    m_defaultButtom->setFont(buttomFont);
    m_defaultButtom->setObjectName("brownButton");
    m_defaultButtom->setText(tr("恢复默认"));
    m_defaultButtom->setFixedSize(120,42);

    m_confirmButtom = new QPushButton();
    m_confirmButtom->setFont(buttomFont);
    m_confirmButtom->setObjectName("brownButton");
    m_confirmButtom->setText(tr("保存"));
    m_confirmButtom->setFixedSize(120,42);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(m_parameterTableWidget);
    leftLayout->addStretch();

    QHBoxLayout *rightLayout = new QHBoxLayout();
    rightLayout->addWidget(m_defaultButtom);
    rightLayout->addStretch(440);
    rightLayout->addWidget(m_confirmButtom);
    rightLayout->addStretch(75);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(rightLayout);
    this->setLayout(mainLayout);

    QFont comboBoxFont = m_parameterTableWidget->font();
    comboBoxFont.setPointSize(FONT_SIZE_SETTINGS(13));

    int type = SettingManager::Instance()->GetParamType();

    Uint8 row = 0;
    bool IsSolidifyMeaParamFromUI = this->IsSolidifyMeaParamFromUI();

    oolua_ipairs(itermsTable)
    {
        Table typeTable;
        String typeText;

        m_lua->pull(typeTable);

        typeTable.at("text", typeText);
        QString qTypeTest;
        qTypeTest = QString::fromStdString(typeText);
        QFont typeTestFont = m_parameterTableWidget->font();
        typeTestFont.setPointSize(GetPointSize(qTypeTest, m_parameterTableWidget->columnWidth(0)-10));
        m_parameterTableWidget->item(row, 0)->setFont(typeTestFont);
        m_parameterTableWidget->item(row, 0)->setText(QObject::tr(typeText.c_str(), "Param"));//显示类别

        bool isInFactory = SettingManager::Instance()->IsProducerStatus();

        oolua_ipairs(typeTable)
        {
            Table itermTable;
            m_lua->pull(itermTable);

            int targetPrivilege = 0;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
            bool checkResult;
            bool isShowCheck = itermTable.safe_at("showCheck", checkResult);
            if (isShowCheck == false)
            {
                checkResult = true;
            }
            else
            {
                m_lua->call(showCheck);
                m_lua->pull(checkResult);
            }

            bool isFactoryCtrl = false;
            itermTable.safe_at("isFactoryCtrl", isFactoryCtrl);

            bool isUsed = true;
            if(isFactoryCtrl && !isInFactory)
            {
                isUsed = false;
            }

            bool isSolidifyFromUI = false;
            itermTable.safe_at("isSolidifyFromUI", isSolidifyFromUI);

            bool isSolidify = false;
            if(isSolidifyFromUI && IsSolidifyMeaParamFromUI)
            {
                isSolidify = true;
            }

            if ((isTarget == false || (type & targetPrivilege)) && isUsed && !isSolidify && checkResult == true)
            {
                bool remote = false;
                if(itermTable.safe_at("remote", remote))
                {
                    if((true == remote)&&(false == CheckScreen::Instance()->isVNC()))
                    {
                        break;
                    }
                }
                ParameterItem parameterItem;
                String text, unit, switchText;
                SwitchGroupParam switchGroup;
                bool result;
                bool switchGroupMaster =false;
                bool calibrateGroupMaster = false;
                bool isRange = false;
                Uint8 rangeNum = 0;

                bool rangConfig = false;
                bool analogConfig = false;

                itermTable.at("name", parameterItem.name);
                parameterItem.name = configTableName + parameterItem.name;
                itermTable.at("type", parameterItem.type);
                itermTable.at("writePrivilege", parameterItem.writePrivilege);
                itermTable.at("readPrivilege", parameterItem.readPrivilege);
                parameterItem.isCheckValue = itermTable.safe_at("checkValue", parameterItem.checkValueFunc);
                parameterItem.isUnitChange = itermTable.safe_at("unitChange", parameterItem.unitChangeFunc);
                parameterItem.isCurveParamCurveXYChange = itermTable.safe_at("curveParamCurveXYChange", parameterItem.curveParamCurveXYChange);

                itermTable.at("text", text);
                QString qTest;
                qTest = QString::fromStdString(text);
                QFont testFont = m_parameterTableWidget->font();
                testFont.setPointSize(GetPointSize(qTest, m_parameterTableWidget->columnWidth(1)-10));
                m_parameterTableWidget->item(row, 1)->setFont(testFont);
                m_parameterTableWidget->item(row, 1)->setText(QObject::tr(text.c_str(), "Param"));//显示名称

                if (itermTable.safe_at("switchText", switchText))
                {
                    m_switchText.insert(make_pair(row, switchText));
                }

                if (itermTable.safe_at("switchGroupMaster", switchGroupMaster))
                {
                    switchGroupMaster = true;
                    RangeMasterRowNum = row;
                }

                if (itermTable.safe_at("calibrateGroupMaster", calibrateGroupMaster))
                {
                    calibrateGroupMaster = true;
                    CalibrateMasterRowNum = row;
                }

                if (itermTable.safe_at("switchGroup", switchGroup.row))
                {
                    switchGroup.privilege = parameterItem.readPrivilege;
                    m_switchGroup.insert(make_pair(row, switchGroup));
                }

                if (itermTable.safe_at("rangNum", rangeNum))
                {
                    isRange = true;
                }

                bool rangChoose = false;
                if (itermTable.safe_at("rangChoose", rangChoose))
                {
                    if (rangChoose && m_isOEM == 1)
                    {
                        parameterItem.writePrivilege = RoleType::Super;
                        parameterItem.readPrivilege = RoleType::Maintain;
                    }
                }

                if (itermTable.safe_at("rangConfig", rangConfig))
                {
                    if (rangConfig && m_isOEM == 1)
                    {
                        parameterItem.writePrivilege = RoleType::Super;
                        parameterItem.readPrivilege = RoleType::Super;
                    }
                }

                if (itermTable.safe_at("analogConfig", analogConfig))
                {
                    if (analogConfig && m_isOEM == 1)
                    {
                        parameterItem.writePrivilege = RoleType::Super;
                        parameterItem.readPrivilege = RoleType::Super;
                    }
                }

                if (itermTable.safe_at("rangeParamStart", result))
                {
                    m_rangeParamStart = row;
                }

                if (itermTable.safe_at("rangeParamEnd",result))
                {
                    m_rangeParamEnd = row;
                }

                if (itermTable.safe_at("unit", unit))
                {
                    if (parameterItem.isUnitChange)
                    {
                        m_lua->call(parameterItem.unitChangeFunc, unit, UnitChange::Read);
                        m_lua->pull(unit);
                    }

                    QString qunit;
                    qunit = QString::fromStdString(unit);
                    QFont unitFont = m_parameterTableWidget->font();
                    unitFont.setPointSize(GetPointSize(qunit, m_parameterTableWidget->columnWidth(3)-10));
                    m_parameterTableWidget->item(row, 3)->setFont(unitFont);
                    m_parameterTableWidget->item(row, 3)->setText(QObject::tr(unit.c_str()));//显示单位
                }

                if (parameterItem.type == DataType::Bool)
                {
                    QComboBox *comboBox = new QComboBox();
                    comboBox->setObjectName("brownComboBox");
                    comboBox->addItem(tr("关"));
                    comboBox->addItem(tr("开"));
                    comboBox->setFont(comboBoxFont);
                    m_boolComboBox.insert(make_pair(parameterItem.name, comboBox));

                    m_parameterTableWidget->setCellWidget(row, 2, comboBox);
                }
                else if (parameterItem.type == DataType::Option)
                {
                    Table optionsTable;
                    itermTable.at("options", optionsTable);
                    QComboBox *comboBox = new QComboBox();
                    comboBox->setObjectName("brownComboBox");
                    oolua_ipairs(optionsTable)
                    {
                        String str;
                        m_lua->pull(str);

                        if (parameterItem.isUnitChange)
                        {
                            m_lua->call(parameterItem.unitChangeFunc, str, UnitChange::Read);
                            m_lua->pull(str);
                        }

                        comboBox->addItem(QObject::tr(str.c_str()));
                    }
                    oolua_ipairs_end()
                    comboBox->setFont(comboBoxFont);
                    m_optionComboBox.insert(make_pair(parameterItem.name, comboBox));

                    if (true == switchGroupMaster && m_isOEM != 1)
                    {
                        connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeIndexSlot(int)));
                    }
                    else if (true == switchGroupMaster && m_isOEM == 1)
                    {
                        comboBox->setCurrentIndex(0);
                        parameterItem.writePrivilege = RoleType::Super;
                        parameterItem.readPrivilege = RoleType::Maintain;
                    }

                    if (true == calibrateGroupMaster && m_isOEM != 1)
                    {
                        connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(CalibrateChangeIndexSlot(int)));
                    }
                    else if (true == calibrateGroupMaster && m_isOEM == 1)
                    {
                        comboBox->setCurrentIndex(0);
                        parameterItem.writePrivilege = RoleType::Super;
                        parameterItem.readPrivilege = RoleType::Maintain;
                    }

                    if (true == isRange && m_isOEM != 1)
                    {
                        if (1==rangeNum)
                        {
                            RangeRowNum[0] = row;
                            connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(RangeNum1ChangeIndexSlot(int)));
                        }
                        else if (2==rangeNum)
                        {
                            RangeRowNum[1] = row;
                            connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(RangeNum2ChangeIndexSlot(int)));
                        }
                        else if (3==rangeNum)
                        {
                            RangeRowNum[2] = row;
                            connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(RangeNum3ChangeIndexSlot(int)));
                        }
                    }
                    else if (true == isRange && m_isOEM == 1)
                    {
                        if (1==rangeNum)
                        {
                            parameterItem.writePrivilege = RoleType::Super;
                            parameterItem.readPrivilege = RoleType::Maintain;
                        }
                        else if (2==rangeNum)
                        {
                            parameterItem.writePrivilege = RoleType::Super;
                            parameterItem.readPrivilege = RoleType::Super;
                        }
                        else if (3==rangeNum)
                        {
                            parameterItem.writePrivilege = RoleType::Super;
                            parameterItem.readPrivilege = RoleType::Super;
                        }
                    }
                    m_parameterTableWidget->setCellWidget(row, 2, comboBox);
                }
                m_items.insert(make_pair(row, parameterItem));

                if (parameterItem.type == DataType::IntArray)
                {
                    m_mapSetPointRow.insert(std::make_pair(row, typeText));
                }

                row++;
            }
        }
        oolua_ipairs_end()
    }
    oolua_ipairs_end()

    if(m_items.size() >= 9)
    {
        for(int i = m_items.size();i < m_tableInfo.userRow; i++)
        {
            m_parameterTableWidget->setRowHidden(i,true);
        }
    }

    if(m_isMeaParaml)
    {
        SetEnableDialogInit();
    }

    if (isMeasureParamItems)
    {
        m_lua->call(m_updaterCurveParam,0,true);
    }
    ConfigInit(false);
    if (isMeasureParamItems)
    {
        this->RefreshUnit(0,true);
    }
    OnUserChange();
    ShowSwitchText();


    connect(m_defaultButtom, SIGNAL(clicked(bool)), this, SLOT(DefaultRestoreSlot()));
    connect(m_parameterTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(DoubleClickedSlot(QTableWidgetItem*)));
    connect(m_confirmButtom, SIGNAL(clicked(bool)), this, SLOT(ConfigRefreshSlot()));
    connect(m_parameterTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(CheckValueSlot(QTableWidgetItem*)));
}

ParameterWidget::~ParameterWidget()
{
    std::map<System::Uint8, int*>::iterator iter = m_mapRowToTimePointSet.begin();

    while (iter != m_mapRowToTimePointSet.end())
    {
        free(iter->second);
        iter->second = NULL;
        iter = m_mapRowToTimePointSet.erase(iter);
    }
}

void ParameterWidget::SpaceInit()
{
    for(int i = 0;i < m_parameterTableWidget->rowCount(); i++)//行
    {
        for(int j = 0;j < m_parameterTableWidget->columnCount(); j++)//列
        {
            m_parameterTableWidget->setItem(i, j, new QTableWidgetItem());
            m_parameterTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);//设置对齐方式
            if(j != 2 || (i >= m_tableInfo.userRow))//设置不可编辑的单元格
            {
                QTableWidgetItem *item = m_parameterTableWidget->item(i, j);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
}

void ParameterWidget::ShowSwitchText()
{
     if(m_switchText.size() > 0)
     {
         for (map<Uint8, System::String>::iterator iter = m_switchText.begin();iter != m_switchText.end(); ++iter)
         {
             Uint8 row = iter->first;
             String text = iter->second;
             QString qTest;
             qTest = QString::fromStdString(text);
             QFont testFont = m_parameterTableWidget->font();
             testFont.setPointSize(GetPointSize(qTest, m_parameterTableWidget->columnWidth(1)-10));
             m_parameterTableWidget->item(row, 1)->setFont(testFont);
             m_parameterTableWidget->item(row, 1)->setText(QObject::tr(text.c_str(), "Param"));//显示名称
         }
     }

}

std::vector<bool> ParameterWidget::GetTimedPoint(System::String name)
{
    bool result;
    vector<bool> point;

    if (name.empty())
    {
        return point;
    }

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Table timedPoinTtable;
    luaEngine->GetLuaValue(state, "config.measureParam.pharmacopoeia", timedPoinTtable);

    //从下标为1开始，按顺序把表中键为数字的值压人栈顶。直到当前下标的值为空，才结束循环
    oolua_ipairs(timedPoinTtable)
    {
        OOLUA::pull(state, result);
        point.push_back(result);
    }
    oolua_ipairs_end()
    return point;
}
bool ParameterWidget::SetTimedPoint(System::String name, std::vector<bool> point)
{
    bool result = false;

    if (name.empty())
    {
        return result;
    }

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Table timedPoinTtable;
    luaEngine->GetLuaValue(state, "config.measureParam.pharmacopoeia", timedPoinTtable);

    //从下标为1开始，按顺序把表中键为数字的值压人栈顶。直到当前下标的值为空，才结束循环
    oolua_ipairs(timedPoinTtable)
    {
        lua_pop(state, 1);//把键为_i_index_的值弹出栈
        result = point[_i_index_ - 1];
        OOLUA::push(state, result);//把需要给键_i_index_赋值的值压人栈
        //把栈顶的值赋给索引_oolua_array_index_指向的表（即m_timedPoinTtable）的键_i_index_
        lua_rawseti(state, _oolua_array_index_, _i_index_);

        if (_i_index_ == (int)point.size())
        {
            break;
        }
    }
    oolua_ipairs_end()
    return true;
}
void ParameterWidget::ConfigInit(bool changeGroup)          //读取配置文件的值
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    m_isConfigInit = true;

    for (map<Uint8, ParameterItem>::iterator iter = m_items.begin();iter != m_items.end(); ++iter)
    {
        bool isConfig  = false;
        Uint8 row = iter->first;
        ParameterItem &item = iter->second;
        if (changeGroup && row>=m_rangeParamStart && row<=m_rangeParamEnd)
        {
            isConfig = true;
        }
        else if (changeGroup == false)
        {
            isConfig = true;
        }
        if (isConfig)
        {
            switch(item.type)
            {
                case DataType::Bool:
                {
                    if (m_boolComboBox.count(item.name))
                    {
                        bool ret;
                        luaEngine->GetLuaValue(state, item.name, ret);
                        if(ret)
                        {
                            m_boolComboBox[item.name]->setCurrentIndex(1);
                        }
                        else
                        {
                            m_boolComboBox[item.name]->setCurrentIndex(0);
                        }
                    }
                }
                break;
                case DataType::Option:
                {
                    if (m_optionComboBox.count(item.name))
                    {
                        Byte ret;
                        luaEngine->GetLuaValue(state, item.name, ret);
                        m_optionComboBox[item.name]->setCurrentIndex(ret);
                    }
                }
                break;
                case DataType::Int:
                {
                    int ret;
                    luaEngine->GetLuaValue(state, item.name, ret);
                    if (item.isUnitChange)
                    {
                        m_lua->call(item.unitChangeFunc, ret, UnitChange::Read);
                        m_lua->pull(ret);
                    }

                    if (item.isCurveParamCurveXYChange)
                    {
                        m_lua->call(item.curveParamCurveXYChange, ret, true);
                        m_lua->pull(ret);
                    }

                    m_parameterTableWidget->item(row, 2)->setText(QString::number(ret));
                }
                break;
                case DataType::Float:
                {
                    float ret;
                    luaEngine->GetLuaValue(state, item.name, ret);
                    if (item.isUnitChange)
                    {
                        m_lua->call(item.unitChangeFunc, ret, UnitChange::Read);
                        m_lua->pull(ret);
                    }

                    if (item.isCurveParamCurveXYChange)
                    {
                        m_lua->call(item.curveParamCurveXYChange, ret, true);
                        m_lua->pull(ret);
                    }

                    m_parameterTableWidget->item(row, 2)->setText(QString::number(ret));
                }
                break;
                case DataType::Double:
                {
                    double ret;
                    luaEngine->GetLuaValue(state, item.name, ret);
                    if (item.isUnitChange)
                    {
                        m_lua->call(item.unitChangeFunc, ret, UnitChange::Read);
                        m_lua->pull(ret);
                    }

                    if (item.isCurveParamCurveXYChange)
                    {
                        m_lua->call(item.curveParamCurveXYChange, ret, true);
                        m_lua->pull(ret);
                    }

                    m_parameterTableWidget->item(row, 2)->setText(QString("%1").arg(ret, 0, 'f', 9).replace(rx,""));
                }
                break;
                case DataType::IP:
                {
                    String ret;
                    luaEngine->GetLuaValue(state, item.name, ret);
                    m_parameterTableWidget->item(row, 2)->setText(ret.c_str());
                }
                break;
                case DataType::String:
                {
                    String ret;
                    QFont StringFont;
                    StringFont.setPointSize(12);
                    luaEngine->GetLuaValue(state, item.name, ret);
                    m_parameterTableWidget->item(row, 2)->setFont(StringFont);
                    m_parameterTableWidget->item(row, 2)->setText(ret.c_str());
                }
                break;
                case DataType::IntArray:
                {
                    if (m_mapSetPointRow.find(row) != m_mapSetPointRow.end())
                    {
                        QString thePoint = "";
                        vector<bool> timedPoint =  GetTimedPoint(item.name);
//                        vector<bool> timedPoint = {false,false,false,false,false,false,false};
                        int k = 0;
                        int* timePointSet = NULL;
                        if (m_mapRowToTimePointSet.find(row) == m_mapRowToTimePointSet.end())
                        {
                            timePointSet = (int *)malloc(sizeof(int) * m_phaLimited);
                            memset(timePointSet, 0, sizeof(int) * m_phaLimited);
                            m_mapRowToTimePointSet.insert(std::make_pair(row, timePointSet));
                        }
                        else
                        {
                            timePointSet = m_mapRowToTimePointSet[row];
                        }

                        for (vector<bool>::iterator iter = timedPoint.begin(); iter != timedPoint.end(); ++iter, ++k)
                        {
                            if (k >= m_phaLimited)
                            {
                                break;
                            }

                            if(*iter)
                            {
                                thePoint += (m_phaList.at(k)+",");
                                timePointSet[k] = 1;
                            }
                            else
                            {
                                timePointSet[k] = 0;
                            }
                        }

                        timePointSet = NULL;
                        m_parameterTableWidget->item(row, 2)->setText(thePoint);
                    }
                }
                break;
            }

        }
    }

    m_isConfigInit = false;
}

bool ParameterWidget::IsSolidifyMeaParamFromUI()
{
    bool ret = false;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "config.system.solidifyMeaParamFromUI", ret);
    return ret;
}

bool ParameterWidget::IsDisplayMeaParamInGeneral()
{
    bool ret;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "config.system.displayMeaParamInGeneral", ret);
    return ret;
}

bool ParameterWidget::IsInternationalSupported()
{
    bool ret = false;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "config.system.internationalSupported", ret);
    return ret;
}

void ParameterWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        for (map<Uint8, ParameterItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            ParameterItem &parameterItem = iter->second;

            m_parameterTableWidget->setRowHidden(row, false);

            if(SettingManager::Instance()->GetManufactureType() == ManufactureType::LS)
            {
                if (parameterItem.name == "config.system.useQrencode")   //LS版本，隐藏二维码开关
                {
                    m_parameterTableWidget->setRowHidden(row, true);
                }
            }

            switch(parameterItem.type)
            {
                case DataType::Bool:
                    if (m_boolComboBox.count(parameterItem.name))
                    {
                        m_boolComboBox[parameterItem.name]->setEnabled(true);
                    }
                break;
                case DataType::Option:
                    if (m_optionComboBox.count(parameterItem.name))
                    {
                        m_optionComboBox[parameterItem.name]->setEnabled(true);
                    }
                break;
                case DataType::Int:case DataType::Float:case DataType::IP:case DataType::IntArray:case DataType::Double:case DataType::String:
                {
                     QTableWidgetItem *item = m_parameterTableWidget->item(row, 2);
                     item->setFlags((Qt::ItemFlags)63);
                }
                break;
            }
        }
        m_confirmButtom->setHidden(false);
        m_defaultButtom->setHidden(false);
    }
    else if(LoginDialog::userType == Administrator)
    {
        bool isEnabledChange = true;
        if (m_isMeaParaml == true && IsSolidifyMeaParamFromUI() == true)
        {
            isEnabledChange = false;
            m_confirmButtom->setHidden(true);
            m_defaultButtom->setHidden(true);
        }
        else
        {
            m_confirmButtom->setHidden(false);
            m_defaultButtom->setHidden(false);
        }

        for (map<Uint8, ParameterItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            ParameterItem &parameterItem = iter->second;

            if (Administrator >= parameterItem.readPrivilege)
            {
                m_parameterTableWidget->setRowHidden(row, false);
            }
            else
            {
                m_parameterTableWidget->setRowHidden(row, true);
            }

            switch(parameterItem.type)
            {
                case DataType::Bool:
                    if (m_boolComboBox.count(parameterItem.name))
                    {
                        if (isEnabledChange == true && Administrator >= parameterItem.writePrivilege)
                        {
                            m_boolComboBox[parameterItem.name]->setEnabled(true);
                        }
                        else
                        {
                            m_boolComboBox[parameterItem.name]->setEnabled(false);
                        }
                    }
                break;
                case DataType::Option:
                    if (m_optionComboBox.count(parameterItem.name))
                    {
                        if (isEnabledChange == true && Administrator >= parameterItem.writePrivilege)
                        {
                            m_optionComboBox[parameterItem.name]->setEnabled(true);
                        }
                        else
                        {
                            m_optionComboBox[parameterItem.name]->setEnabled(false);
                        }

                        if (parameterItem.name == "config.system.language")   //禁止切换语言
                        {
                            if(!IsInternationalSupported())
                            {
                                m_optionComboBox[parameterItem.name]->setEnabled(false);
                            }
                        }
                    }
                break;
                case DataType::Int:case DataType::Float:case DataType::IP:case DataType::IntArray:case DataType::Double:case DataType::String:
                {
                     QTableWidgetItem *item = m_parameterTableWidget->item(row, 2);
                    if (isEnabledChange == true && Administrator >= parameterItem.writePrivilege)
                    {
                        item->setFlags((Qt::ItemFlags)63);
                    }
                    else
                    {
                        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                        item->setFlags(Qt::ItemIsEnabled);
                    }
                }
                break;
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        bool isEnabledChange = true;
        if (m_isMeaParaml == true && IsSolidifyMeaParamFromUI() == true)
        {
            isEnabledChange = false;
            m_confirmButtom->setHidden(true);
            m_defaultButtom->setHidden(true);
        }
        else
        {
            m_confirmButtom->setHidden(false);
            m_defaultButtom->setHidden(false);
        }

        for (map<Uint8, ParameterItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            ParameterItem &parameterItem = iter->second;

            if (Maintain >= parameterItem.readPrivilege)
            {
                m_parameterTableWidget->setRowHidden(row, false);
            }
            else
            {
                m_parameterTableWidget->setRowHidden(row, true);
            }

            switch(parameterItem.type)
            {
                case DataType::Bool:
                    if (m_boolComboBox.count(parameterItem.name))
                    {
                        if (isEnabledChange == true && Maintain >= parameterItem.writePrivilege)
                        {
                            m_boolComboBox[parameterItem.name]->setEnabled(true);
                        }
                        else
                        {
                            m_boolComboBox[parameterItem.name]->setEnabled(false);
                        }
                    }
                break;
                case DataType::Option:
                    if (m_optionComboBox.count(parameterItem.name))
                    {
                        if (isEnabledChange == true && Maintain >= parameterItem.writePrivilege)
                        {
                            m_optionComboBox[parameterItem.name]->setEnabled(true);
                        }
                        else
                        {
                            m_optionComboBox[parameterItem.name]->setEnabled(false);
                        }

                        if (parameterItem.name == "config.system.language")   //禁止切换语言
                        {
                            if(!IsInternationalSupported())
                            {
                                m_optionComboBox[parameterItem.name]->setEnabled(false);
                            }
                        }
                    }
                break;
                case DataType::Int:case DataType::Float:case DataType::IP:case DataType::IntArray:case DataType::Double:case DataType::String:
                {
                     QTableWidgetItem *item = m_parameterTableWidget->item(row, 2);
                    if (isEnabledChange == true && Maintain >= parameterItem.writePrivilege)
                    {
                        item->setFlags((Qt::ItemFlags)63);
                    }
                    else
                    {
                        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                        item->setFlags(Qt::ItemIsEnabled);
                    }
                }
                break;
            }
        }
    }
    else
    {
        bool isEnabledDisplay = false;
        if (m_isMeaParaml == true && IsDisplayMeaParamInGeneral() == true)
        {
            isEnabledDisplay = true;
        }
        for (map<Uint8, ParameterItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            ParameterItem &parameterItem = iter->second;

            if (parameterItem.readPrivilege == Maintain && isEnabledDisplay)
            {
                m_parameterTableWidget->setRowHidden(row, false);
            }
            else
            {
                m_parameterTableWidget->setRowHidden(row, true);
            }

            switch(parameterItem.type)
            {
                case DataType::Bool:
                    if (m_boolComboBox.count(parameterItem.name))
                    {
                        m_boolComboBox[parameterItem.name]->setEnabled(false);
                    }
                break;
                case DataType::Option:
                    if (m_optionComboBox.count(parameterItem.name))
                    {
                        m_optionComboBox[parameterItem.name]->setEnabled(false);
                    }
                break;
                case DataType::Int:case DataType::Float:case DataType::IP:case DataType::IntArray:case DataType::Double:case DataType::String:
                {
                    QTableWidgetItem *item = m_parameterTableWidget->item(row, 2);
                    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                    item->setFlags(Qt::ItemIsEnabled);
                }
                break;
            }
        }
        m_confirmButtom->setHidden(true);
        m_defaultButtom->setHidden(true);
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(m_parameterTableWidget, m_tableInfo);
}

void ParameterWidget::DoubleClickedSlot(QTableWidgetItem *item)
{
    bool isEnabledChange = true;
    if (m_isMeaParaml == true && IsSolidifyMeaParamFromUI() == true)
    {
        isEnabledChange = false;
    }
    if(item->column() == 2
        && item->row() < m_tableInfo.userRow
        && (LoginDialog::userType == Super
        || (LoginDialog::userType == Administrator && isEnabledChange)
        || (LoginDialog::userType == Maintain && isEnabledChange)))
    {
        if (m_mapSetPointRow.find(item->row()) != m_mapSetPointRow.end())
        {
            CNumberKeyboard *numKbd = CNumberKeyboard::Instance();//让键盘隐藏
            (void)numKbd;
            SetEnableDialogShow(item->row());
        }
        else if(m_items.count(item->row()))
        {
            ParameterItem parameterItem = m_items[item->row()];
            if (LoginDialog::userType >= parameterItem.writePrivilege
                    && (parameterItem.type == DataType::Int || parameterItem.type == DataType::Float || parameterItem.type == DataType::Double
                        || parameterItem.type == DataType::IP /*|| parameterItem.type == DataType::String*/))
            {
                int curX = cursor().pos().x();
                int curY = cursor().pos().y();
                int curWidth = m_parameterTableWidget->columnWidth(item->column());
                int curHeight = m_parameterTableWidget->rowHeight(item->row());

                CNumberKeyboard *numKbd = CNumberKeyboard::Instance();

                if(true == numKbd->isHidden())
                {
                    numKbd->myMove(curX, curY, curWidth, curHeight);
                    numKbd->show();
                }
            }
            else if(LoginDialog::userType >= parameterItem.writePrivilege
                    && parameterItem.type == DataType::String)
            {
                int curX = cursor().pos().x();
                int curY = cursor().pos().y();
                int curWidth = m_parameterTableWidget->columnWidth(item->column());
                int curHeight = m_parameterTableWidget->rowHeight(item->row());

                InputKeyBoard *numKbd = InputKeyBoard::Instance();

                if(true == numKbd->isHidden())
                {
                    numKbd->myMove(curX, curY, curWidth, curHeight);
                    numKbd->show();
                }
            }
        }
    }
}

void ParameterWidget::CheckValueSlot(QTableWidgetItem *item)
{
    if (!m_isConfigInit && item->column() == 2 && m_items.count(item->row()))
    {
        ParameterItem &parameterItem = m_items[item->row()];
        if (parameterItem.type == DataType::IP)
        {
            QString tmpPattern("^([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");  //检查数字是否合法
            QRegExp tmpRx(tmpPattern);
            if(false == tmpRx.exactMatch(item->text()))
            {
                String ret;
                LuaEngine* luaEngine = LuaEngine::Instance();
                lua_State * state = luaEngine->GetThreadState();

                luaEngine->GetLuaValue(state, parameterItem.name, ret);
                item->setText(ret.c_str());
            }
        }
        else if (parameterItem.isCheckValue)
        {
            String ret =  item->text().toStdString();
            m_lua->call(parameterItem.checkValueFunc, ret);
            m_lua->pull(ret);
            item->setText(ret.c_str());
        }
    }
}

void ParameterWidget::DefaultRestoreSlot()
{
    bool defaultReviseParameter = true;
    MessageDialog msg(tr("是否恢复默认设置？\n"), this, MsgStyle::OKANDCANCEL);
    if(msg.exec()==QDialog::Rejected)
    {
        return;
    }

    if(CheckSignatureTimeout())
    {
        MessageDialog msg(tr("签名已过期,请重新签名\n"), this,MsgStyle::OKANDCANCEL);
        if(msg.exec()==QDialog::Rejected)
        {
            return;
        }
        SignatureDialog signatureDialog;
        signatureDialog.FillObjectText(m_profileTableText.c_str());
        signatureDialog.FillDescriptorText(QString::fromStdString(m_profileTableText)+ "恢复默认");
        if(signatureDialog.exec()== LoginDialog::Rejected)
        {
            return;
        }
    }

    if (Super == LoginDialog::userType && isMeasureParamItems)
    {
        MessageDialog msgChoose(tr("是否将内部校正参数恢复默认设置？\n"), this, MsgStyle::OKANDCANCEL);
        if(msgChoose.exec()==QDialog::Rejected)
        {
            defaultReviseParameter = false;
        }
    }

    m_lua->call(m_defaultRestore, LoginDialog::userType,defaultReviseParameter);

    bool isRestart = false;
    m_lua->pull(isRestart);

    this->ConfigInit(false);
    if (isMeasureParamItems)
    {
        this->RefreshUnit(0,true);
    }
    UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");

    if(isRestart == true)
    {
        QString text = QObject::tr(m_profileTableText.c_str()) + tr("更改，重启仪器后生效，是否立即重启？\n");
        MessageDialog msg(text, 10, this, MsgStyle::ONLYCANCEL);
        if(msg.exec()==QDialog::Rejected)
        {
            return;
        }
    }
}

void ParameterWidget::ConfigRefreshSlot()
{
    MessageDialog msg(tr("是否确认更改设置？\n"), this,MsgStyle::OKANDCANCEL);
    if(msg.exec()==QDialog::Rejected)
    {
        return;
    }

    if(CheckSignatureTimeout())
    {
        MessageDialog msg(tr("签名已过期,请重新签名\n"), this,MsgStyle::OKANDCANCEL);
        if(msg.exec()==QDialog::Rejected)
        {
            return;
        }
        SignatureDialog signatureDialog;
        signatureDialog.FillObjectText(m_profileTableText.c_str());
        signatureDialog.FillDescriptorText(QString::fromStdString(m_profileTableText) + "修改");
        if(signatureDialog.exec()== LoginDialog::Rejected)
        {
            return;
        }
    }

    for (map<Uint8, ParameterItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
    {
        Uint8 row = iter->first;
        ParameterItem &item = iter->second;
        if (LoginDialog::userType < item.readPrivilege)
        {
            continue;
        }
        String ret = item.name + " = ";
        bool isSet = false;
        switch(item.type)
        {
            case DataType::Bool:
            {
                if (m_boolComboBox.count(item.name))
                {
                    if (m_boolComboBox[item.name]->currentIndex())
                    {
                        ret += "true";
                    }
                    else
                    {
                        ret += "false";
                    }
                    isSet = true;
                }
            }
            break;
            case DataType::Option:
            {
                if (m_optionComboBox.count(item.name))
                {
                    Byte number = m_optionComboBox[item.name]->currentIndex();
                    ret += QString::number(number).toStdString();
                    isSet = true;
                }
            }
            break;
            case DataType::Int:
            {
                int data = m_parameterTableWidget->item(row, 2)->text().toInt();
                if (item.isUnitChange)
                {
                    m_lua->call(item.unitChangeFunc, data, UnitChange::Write);
                    m_lua->pull(data);
                }

                if (item.isCurveParamCurveXYChange)
                {
                    m_lua->call(item.curveParamCurveXYChange, data, false);
                    m_lua->pull(data);
                }

                ret += QString::number(data).toStdString();
                isSet = true;
            }
            break;
            case DataType::Float:
            {
                float data = m_parameterTableWidget->item(row, 2)->text().toFloat();
                if (item.isUnitChange)
                {
                    m_lua->call(item.unitChangeFunc, data, UnitChange::Write);
                    m_lua->pull(data);
                }

                if (item.isCurveParamCurveXYChange)
                {
                    m_lua->call(item.curveParamCurveXYChange, data, false);
                    m_lua->pull(data);
                }

                ret += QString::number(data).toStdString();
                isSet = true;
            }
            break;
            case DataType::Double:
            {
                double data = m_parameterTableWidget->item(row, 2)->text().toDouble();
                if (item.isUnitChange)
                {
                    m_lua->call(item.unitChangeFunc, data, UnitChange::Write);
                    m_lua->pull(data);
                }

                if (item.isCurveParamCurveXYChange)
                {
                    m_lua->call(item.curveParamCurveXYChange, data, false);
                    m_lua->pull(data);
                }

                ret += QString::number(data, 'f', 9).toStdString();
                isSet = true;
            }
            break;
            case DataType::IP:
            {
                ret += "\"" + m_parameterTableWidget->item(row, 2)->text().toStdString() +  "\"" ;
                isSet = true;
            }
            break;
            case DataType::String:
            {
                ret += "\"" + m_parameterTableWidget->item(row, 2)->text().toStdString() +  "\"" ;
                isSet = true;
            }
            break;
            case DataType::IntArray:
            if (m_mapSetPointRow.find(row) != m_mapSetPointRow.end())
            {
                vector<bool> timedPoint(m_phaLimited, 0);
                int k = 0;
                for (vector<bool>::iterator iter = timedPoint.begin(); iter != timedPoint.end(); ++iter, ++k)
                {
                    if (k >= m_phaLimited)
                    {
                        break;
                    }
                    *iter = m_mapRowToTimePointSet[row][k];
                }
                this->SetTimedPoint(item.name, timedPoint);
            }
            break;
        }
        if (isSet)
        {
            LuaEngine::Instance()->RunChunk(ret);
        }
    }
    if (isMeasureParamItems)
    {        
        m_lua->call(m_saveFile, true,true);
        this->ConfigInit(true);
    }
    else
    {
        m_lua->call(m_saveFile, true);
    }

    bool isRestart = false;
    m_lua->pull(isRestart);
    UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");

    if(isRestart == true)
    {
        QString text = QObject::tr(m_profileTableText.c_str()) + tr("更改，重启仪器后生效，是否立即重启？\n");
        MessageDialog msg(text, 10, this, MsgStyle::ONLYCANCEL);
        if(msg.exec()==QDialog::Rejected)
        {
            return;
        }
    }
}

int ParameterWidget::GetPointSize(QString& text, int limitWidth)
{
    QFont font = m_parameterTableWidget->font();
    int size = font.pointSize();

    QFontMetrics fontWidth(font);
    int textWidth = fontWidth.width(text);

    while(size > 8 && textWidth > limitWidth)
    {
        size = size - 1;
        font.setPointSize(size);

        QFontMetrics fontWidth(font);
        textWidth = fontWidth.width(text);
    }

    if (size >PARAMRTER_LIMIT_SIZE)
    {
        size = PARAMRTER_LIMIT_SIZE;
    }
    return size;
}

void ParameterWidget::showEvent(QShowEvent *event)
{
    CNumberKeyboard *numberKey = new CNumberKeyboard(this);
    if (numberKey)
    {
        delete numberKey;
        numberKey = nullptr;
    }
    (void)event;
    this->ConfigInit(false);
}

void ParameterWidget::OnUpdateWidget(UpdateEvent event, System::String message)
{
    if (event == UpdateEvent::ChangeConfigParam || event == UpdateEvent::WqimcChangeParam)
    {
        this->ConfigInit(false);
    }
    else if (event == UpdateEvent::ChangeInterconnectionParam && m_eventName == UpdateEvent::ChangeInterconnectionParam && message != "ParameterWidget")
    {
        this->ConfigInit(false);
    }
    else if (event == UpdateEvent::ChangeSystemParam && m_eventName == UpdateEvent::ChangeSystemParam && message != "ParameterWidget")
    {
        this->ConfigInit(false);
    }
    else if (event == UpdateEvent::ChangeMeasureParam && m_eventName == UpdateEvent::ChangeMeasureParam && message == "CalibrateCurve")
    {
        this->ConfigInit(false);
    }
    else if (event == UpdateEvent::NewCalibrateCurve && m_eventName == UpdateEvent::ChangeMeasureParam)
    {
        this->ConfigInit(false);
    }
    else if (event == UpdateEvent::ModbusChangeParam && m_eventName == UpdateEvent::ChangeMeasureParam)
    {
        this->ConfigInit(false);
    }
}

void ParameterWidget::ChangeIndexSlot(int index)
{
    Uint8 num = 0;
    if (0 == index)//如果选择量程一
    {
        if (m_optionComboBox.count(m_items[RangeRowNum[0]].name))//拿量程一索引
        {
            num = m_optionComboBox[m_items[RangeRowNum[0]].name]->currentIndex();
        }
    }
    else if (1 == index)//如果选择量程二
    {
        if (m_optionComboBox.count(m_items[RangeRowNum[1]].name))//拿量程二索引
        {
            num = m_optionComboBox[m_items[RangeRowNum[1]].name]->currentIndex();
        }
    }
    else if (2 == index)//如果选择量程三
    {
        if (m_optionComboBox.count(m_items[RangeRowNum[2]].name))//拿量程三索引
        {
            num = m_optionComboBox[m_items[RangeRowNum[2]].name]->currentIndex();
        }
    }
    else
    {
        return;
    }

    if (isMeasureParamItems)
    {
        m_lua->call(m_updaterCurveParam,num,false);
    }
    this->ConfigInit(true);
    this->RefreshUnit(num,false);
    UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");
}

void ParameterWidget::CalibrateChangeIndexSlot(int index)
{
    Uint8 num = 0;
    if (0 == index)//如果选择量程一
    {
        if (m_optionComboBox.count(m_items[RangeRowNum[0]].name))//拿量程一索引
        {
            num = m_optionComboBox[m_items[RangeRowNum[0]].name]->currentIndex();
        }
    }
    else if (1 == index)//如果选择量程二
    {
        if (m_optionComboBox.count(m_items[RangeRowNum[1]].name))//拿量程二索引
        {
            num = m_optionComboBox[m_items[RangeRowNum[1]].name]->currentIndex();
        }
    }
    else if (2 == index)//如果选择量程三
    {
        if (m_optionComboBox.count(m_items[RangeRowNum[2]].name))//拿量程三索引
        {
            num = m_optionComboBox[m_items[RangeRowNum[2]].name]->currentIndex();
        }
    }
    else
    {
        return;
    }

    if (isMeasureParamItems)
    {
        m_lua->call(m_updaterCurveParam,num,false,true);
    }
    this->ConfigInit(true);
    UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");
}

/*
*  对KB值单位进行刷新
*/
void ParameterWidget::RefreshUnit(int index,bool useMeasureParamConfig)
{
    String CurveKRangeStr;
    String CurveBRangeStr;
    m_lua->call(m_getCurveKBRange,index,useMeasureParamConfig);
    m_lua->pull(CurveBRangeStr);
    m_lua->pull(CurveKRangeStr);

    QString QCurveKRangeStr  = QString::fromStdString(CurveKRangeStr);
    QString QCurveBRangeStr  = QString::fromStdString(CurveBRangeStr);

    QFont unitFont = m_parameterTableWidget->font();

    unitFont.setPointSize(GetPointSize(QCurveKRangeStr, m_parameterTableWidget->columnWidth(3)-10));
    m_parameterTableWidget->item(m_rangeParamStart, 3)->setFont(unitFont);
    m_parameterTableWidget->item(m_rangeParamStart, 3)->setText(QObject::tr(CurveKRangeStr.c_str()));

    unitFont.setPointSize(GetPointSize(QCurveBRangeStr, m_parameterTableWidget->columnWidth(3)-10));
    m_parameterTableWidget->item(m_rangeParamStart+1, 3)->setFont(unitFont);
    m_parameterTableWidget->item(m_rangeParamStart+1, 3)->setText(QObject::tr(CurveBRangeStr.c_str()));
}

void ParameterWidget::RangeNum1ChangeIndexSlot(int index)
{
    Byte Num;
    if (m_optionComboBox.count(m_items[RangeMasterRowNum].name))//拿当前量程索引
    {
        Num = m_optionComboBox[m_items[RangeMasterRowNum].name]->currentIndex();
    }

    if (Num ==  0)
    {
        if (isMeasureParamItems)
        {
            m_lua->call(m_updaterCurveParam,index,false);
        }
        this->ConfigInit(true);
        this->RefreshUnit(index,false);
        UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");
    }

    Byte CalibrateNum;
    if (m_optionComboBox.count(m_items[CalibrateMasterRowNum].name))//拿当前量程索引
    {
        CalibrateNum = m_optionComboBox[m_items[CalibrateMasterRowNum].name]->currentIndex();
    }

    if (CalibrateNum ==  0)
    {
        if (isMeasureParamItems)
        {
            m_lua->call(m_updaterCurveParam,index,false,true);
        }
        this->ConfigInit(true);
        UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");
    }
}

void ParameterWidget::RangeNum2ChangeIndexSlot(int index)
{
    Byte Num;
    if (m_optionComboBox.count(m_items[RangeMasterRowNum].name))//拿当前量程索引
    {
        Num = m_optionComboBox[m_items[RangeMasterRowNum].name]->currentIndex();
    }

    if (Num ==  1)
    {
        if (isMeasureParamItems)
        {
            m_lua->call(m_updaterCurveParam,index,false);
        }
        this->ConfigInit(true);
        this->RefreshUnit(index,false);
        UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");
    }

    Byte CalibrateNum;
    if (m_optionComboBox.count(m_items[CalibrateMasterRowNum].name))//拿当前量程索引
    {
        CalibrateNum = m_optionComboBox[m_items[CalibrateMasterRowNum].name]->currentIndex();
    }

    if (CalibrateNum ==  1)
    {
        if (isMeasureParamItems)
        {
            m_lua->call(m_updaterCurveParam,index,false,true);
        }
        this->ConfigInit(true);
        UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");
    }
}

void ParameterWidget::RangeNum3ChangeIndexSlot(int index)
{
    Byte Num;
    if (m_optionComboBox.count(m_items[RangeMasterRowNum].name))//拿当前量程索引
    {
        Num = m_optionComboBox[m_items[RangeMasterRowNum].name]->currentIndex();
    }

    if (Num ==  2)
    {
        if (isMeasureParamItems)
        {
            m_lua->call(m_updaterCurveParam,index,false);
        }
        this->ConfigInit(true);
        this->RefreshUnit(index,false);
        UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");
    }

    Byte CalibrateNum;
    if (m_optionComboBox.count(m_items[CalibrateMasterRowNum].name))//拿当前量程索引
    {
        CalibrateNum = m_optionComboBox[m_items[CalibrateMasterRowNum].name]->currentIndex();
    }

    if (CalibrateNum ==  2)
    {
        if (isMeasureParamItems)
        {
            m_lua->call(m_updaterCurveParam,index,false,true);
        }
        this->ConfigInit(true);
        UpdateWidgetManager::Instance()->SlotUpdateWidget(m_eventName, "ParameterWidget");
    }
}

void ParameterWidget::SetEnableDialogInit()
{
    QFont font;
    font.setPointSize(16);
    QPushButton * confirmButton = new QPushButton();
    confirmButton->setText(tr("确定"));
    confirmButton->setObjectName("brownButton");
    confirmButton->setFixedSize(120,45);
    confirmButton->setFont(font);

    QPushButton *cancelButton = new QPushButton();
    cancelButton->setText(tr("取消"));
    cancelButton->setObjectName("redButton");
    cancelButton->setFixedSize(120,45);
    cancelButton->setFont(font);

    m_checkAllButton = new QPushButton();
    m_checkAllButton->setText(tr("选择全部"));
    m_checkAllButton->setObjectName("brownButton");
    m_checkAllButton->setFixedSize(120,45);
    m_checkAllButton->setFont(font);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_checkAllButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(confirmButton);
    buttonLayout->setSpacing(15);

    QVBoxLayout *DialogLayout = new QVBoxLayout();

    m_setPointDialog = new QDialog(this, Qt::WindowSystemMenuHint);
    m_setPointDialog->setWindowTitle(tr("药典设置"));
    m_setPointDialog->setFont(font);
    m_setPointDialog->setFixedSize(675,375);

    m_setPointDialog->setLayout(DialogLayout);

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Table phaName;
    luaEngine->GetLuaValue(state, "setting.instrument.pharmacopoeiaName", phaName);
    m_phaLimited = 0;

    oolua_ipairs(phaName)
    {
        String ret;
        OOLUA::pull(state, ret);
        m_phaList.append(QString::fromStdString(ret));
        m_phaLimited = m_phaLimited + 1;
    }
    oolua_ipairs_end()

    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(int k = 0;k < m_phaLimited;k++)
    {
       m_check[k] = new QCheckBox();
       m_check[k]->setObjectName("checkboxtwo");
       m_check[k]->setText(m_phaList.at(k));
       m_check[k]->setFont(font);
       signalMapper->setMapping(m_check[k], k);
       connect(m_check[k], SIGNAL(clicked()), signalMapper, SLOT(map()));
    }
//    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(SetPointCheckSlot(int)));

    QHBoxLayout *checkBoxLayout[2];
    for(int k = 0;k < 2;k++)
    {
        checkBoxLayout[k] = new QHBoxLayout();
    }
    for(int i = 0;i < 2;i++)
    {
        for(int j =0;j< 4;j++)
        {
            if((i*4+j) < 7)
            {
              checkBoxLayout[i]->addWidget(m_check[i*4+j]);
            }
        }
    }
    checkBoxLayout[1]->addSpacing(165);

    for(int k = 0;k < 2;k++)
    {
        DialogLayout->addLayout(checkBoxLayout[k]);
    }
    DialogLayout->addLayout(buttonLayout);

    connect(confirmButton, SIGNAL(clicked()), m_setPointDialog, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), m_setPointDialog, SLOT(close()));
    connect(m_checkAllButton, SIGNAL(clicked()), this, SLOT(OneCheckAllSlot()));
    m_setPointDialog->hide();
}

void ParameterWidget::SetEnableDialogShow(int row)
{
    if (m_mapSetPointRow.find(row) == m_mapSetPointRow.end())
    {
        return ;
    }

    vector<bool> timedPoint(m_phaLimited, 0);
    int k = 0;
    for (vector<bool>::iterator iter = timedPoint.begin(); iter != timedPoint.end(); ++iter, ++k)
    {
        if (k >= 24)
        {
            break;
        }
        *iter = m_mapRowToTimePointSet[row][k];
    }

    m_currentSetPointRow = row;

    k = 0;
    for (vector<bool>::iterator iter = timedPoint.begin(); iter != timedPoint.end(); ++iter, ++k)
    {
        if (k >= m_phaLimited)
        {
            break;
        }
        if (*iter)
        {
            m_check[k]->setChecked(true);
        }
        else
        {
            m_check[k]->setChecked(false);
        }
    }
    if(m_setPointDialog->exec() == QDialog::Rejected)
    {
        int j = 0;
        for (vector<bool>::iterator iter = timedPoint.begin(); iter != timedPoint.end(); ++iter, ++j)
        {
            if (j >= m_phaLimited)
            {
                break;
            }
            if (*iter)
            {
                m_check[j]->setChecked(true);
            }
            else
            {
                m_check[j]->setChecked(false);
            }
        }
    }
    QString chosenPoint = "";
    for(int k = 0;k < m_phaLimited;k++)
    {
        if(m_check[k]->checkState() == Qt::Checked)
        {
            chosenPoint+= m_phaList.at(k)+",";
            m_mapRowToTimePointSet[row][k] = 1;
        }
        else
        {
            m_mapRowToTimePointSet[row][k] = 0;
        }
    }
    m_parameterTableWidget->item(row, 2)->setText(chosenPoint);

    m_currentSetPointRow = -1;
}


void ParameterWidget::OneCheckAllSlot()
{
    bool flag = false;
    for(int k = 0;k < 7;k++)
    {
        if(m_check[k]->checkState() == Qt::Unchecked)
        {
            flag = true;
            break;
        }
    }

    int setNum = 0;
    for(int k = 0;k < 7;k++)
    {
        if(!flag)
        {
            if (m_check[k]->checkState() == Qt::Checked)
            {
                m_check[k]->setChecked(false);
                setNum--;
            }
        }
        else
        {
//            if (SetPointCheck(m_currentSetPointRow, k) < 0)
            {
                m_check[k]->setChecked(true);
                setNum++;
            }
        }
    }
}

//int ParameterWidget::SetEnableCheck(int rowSetPoint, int indexCheck)
//{
//    int nRet = -1;
//    if (indexCheck < 0 || indexCheck >= 24)
//    {
//        return nRet;
//    }

//    std::map<System::Uint8,  int*>::iterator iter = m_mapRowToTimePointSet.begin();
//    for (; iter != m_mapRowToTimePointSet.end(); ++iter)
//    {
//        Uint8 row = iter->first;
//        int* timedPoint = iter->second;
//        if (row != rowSetPoint && timedPoint[indexCheck] == 1)
//        {
//            nRet = row;
//            break;
//        }
//    }

//    return nRet;
//}


//void ParameterWidget::SetPointCheckSlot(int index)
//{
//    if (m_currentSetPointRow < 0 || index < 0 || index >= 24)
//    {
//        return ;
//    }

//    if (m_check[index]->checkState() == Qt::Unchecked)
//    {
//        // 取消勾选不用检查
//        return ;
//    }

//    int nRow = SetPointCheck(m_currentSetPointRow, index);
//    if (nRow >= 0)
//    {
//        QString strInfo = QString("%1 时 已配置 %2 流程！")
//                .arg(index)
//                .arg(m_mapSetPointRow[nRow].data());
//        MessageDialog msg(strInfo, m_setPointDialog, MsgStyle::ONLYOK, true);
//        msg.exec();
//        m_check[index]->setChecked(false);
//    }
//    m_index = index;
//    m_check[index]->setEnabled(false);
//}

bool ParameterWidget::CheckSignatureTimeout()
{
    int currentTimeSec = QDateTime::currentDateTime().toTime_t();
    int lastTimeSec = UI::SignatureDialog::lastSignatureTime;
    if(!ProLicenseManager::Instance()->GetProLicense())
    {
        return false;
    }
    if(currentTimeSec - lastTimeSec > 30)
    {
        return true;
    }
    else
    {
        return false;
    }
}

}
