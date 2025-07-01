#include "RunStatusWindow.h"
#include "ResultManager/ResultManager.h"
#include "FlowManager/FlowManager.h"
#include "Setting/SettingManager.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "StatusManager/StatusManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MessageDialog.h"
#include <QDebug>
#include "Log.h"
#include "UI/System/ScreenCalibrate.h"
#include "LuaEngine/LuaEngine.h"
#include "FlowManager/FlowManager.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "FlowManager/MeasureScheduler.h"
#include "System/Translate.h"
#include "Lua/App.h"
#include "Common.h"
#include "System/ProLicenseManager.h"

using namespace System;

using namespace Measure;
using namespace Result;
using namespace std;
using namespace Configuration;
using namespace UI;
using namespace OOLUA;
using namespace Flow;
using namespace Lua;
using namespace ResultData;

RunStatusWindow::RunStatusWindow(QWidget *parent) :
        QWidget(parent) ,
        IScheduleNotifiable() ,
        IUserChangeNotifiable()

{
    MaintainUserChange::Instance()->Register(this);

    MeasureScheduler::Instance()->Register(this);
    ResultManager::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this);

    //注册状态观察
    statusManager = StatusManager::Instance();
    statusManager->Register(this);

    mainSplitter = new QSplitter();
    mainSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainSplitter->setOrientation(Qt::Horizontal);//水平分割
    mainSplitter->setHandleWidth(1);

    this->initLeft();
    this->initLeftTop();
    this->initLeftBottom();
    leftSplitter->addWidget(leftTopWidget);
    mainSplitter->addWidget(leftSplitter);

    for(int i = 0; i < mainSplitter->count();i++)
    {
        QSplitterHandle *handle = mainSplitter->handle(i);
        handle->setEnabled(false);
    }

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(mainSplitter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    OnUserChange();

    this->setLayout(mainLayout);
    this->resize(800,450);
    m_isMousePress = false;
}

RunStatusWindow::~RunStatusWindow()
{

}

void RunStatusWindow::OnScheduler(QString content)
{
    emit SignalUpdateExpectInfo(content);
}

void RunStatusWindow::SlotUpdateExpectInfo(QString str)
{
    m_resultPanel->estimateResultTimeLable->setText(Translate::ComplexTranslate(str));
}

void RunStatusWindow::initLeft()
{
    leftWidget = new QWidget();

    leftSplitter = new QSplitter();

    leftSplitter->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    leftSplitter->setOrientation(Qt::Vertical);//垂直分割
    leftSplitter->setHandleWidth(0);
    leftWidget->resize(800,450);
}

void RunStatusWindow::initLeftTop()
{
    leftTopWidget = new QWidget();
    leftTopWidget->resize(800, 300);

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    String showModel;
    luaEngine->GetLuaValue(state, "config.info.instrument.showModel", showModel);

    // 测量结果显示框
    m_lua = luaEngine->GetEngine();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.runStatus", itermsTable);
    m_isUnitChange = itermsTable.safe_at("unitChange", m_unitChangeFunc);

    OOLUA::Lua_func_ref useExpandName;
    bool isUseExpandName = false;
    if (itermsTable.safe_at("UseExpandName", useExpandName))
    {

        m_lua->call(useExpandName);
        m_lua->pull(isUseExpandName);
    }

    Table targetTable;
    itermsTable.at("targets", targetTable);

    int type = SettingManager::Instance()->GetParamType();

    QStringList targetList;

    oolua_ipairs(targetTable)
    {
        Table itermTable;
        m_lua->pull(itermTable);

         int targetPrivilege = 0;
        bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
        if (isTarget == false || (type & targetPrivilege))
        {
            RunStatusIterm iterm;

            if (isUseExpandName)
            {
                itermTable.safe_at("nameExpand", iterm.name);
            }
            else
            {
                itermTable.safe_at("name", iterm.name);
            }

            itermTable.at("getData", iterm.getData);
            itermTable.at("getDataTC", iterm.getDataTC);
            itermTable.at("getDataIC", iterm.getDataIC);
            iterm.isNeedProformaData = itermTable.safe_at("getProformaData", iterm.getProformaData);
            iterm.isNeedResultType = itermTable.safe_at("getResultType", iterm.getResultType);
            m_items.push_back(iterm);

            targetList.append(QString(iterm.name.c_str()));
        }
    }
    oolua_ipairs_end()

    String unit = "mg/L";
    if (m_isUnitChange)
    {
        m_lua->call(m_unitChangeFunc, unit, UnitChange::Read);
        m_lua->pull(unit);
    }

    m_resultPanel = new MeasureResultPanel(targetList, tr(""), QString::fromStdString(unit), this);
    QGraphicsDropShadowEffect *blackEffect = new QGraphicsDropShadowEffect(this);
    blackEffect->setOffset(10);              //阴影的偏移量
    blackEffect->setColor(QColor(43, 43, 43)); //阴影的颜色
    blackEffect->setBlurRadius(25);             // 阴影圆角的大小
    m_resultPanel->setGraphicsEffect(blackEffect);
    m_resultPanel->SetProformaResultInfo(QString());
//    m_resultPanel->setFixedSize(800, 400);

    QHBoxLayout * measureResultPanelLayout = new QHBoxLayout();
    measureResultPanelLayout->addWidget(m_resultPanel);
    measureResultPanelLayout->setContentsMargins(0,0,0,0);

    stopButton = new QPushButton();
    stopButton->setObjectName("mainBlueButton");
    stopButton->setFixedSize(100,100);
    stopButton->setText(tr("停止"));
    QFont measureFont = stopButton->font();
    measureFont.setPointSize(20);
    measureFont.setWeight(QFont::DemiBold);
    stopButton->setFont(measureFont);
    QGraphicsDropShadowEffect *redEffect = new QGraphicsDropShadowEffect(this);
    redEffect->setOffset(3);              //阴影的偏移量
    redEffect->setColor(QColor(30, 30, 150)); //阴影的颜色
    redEffect->setBlurRadius(25);             // 阴影圆角的大小
    stopButton->setGraphicsEffect(redEffect);

    oneKeyButton = new QPushButton();
    oneKeyButton->setObjectName("mainBlueButton");
    oneKeyButton->setFixedSize(100,100);
    oneKeyButton->setText(tr("开始"));
    oneKeyButton->setFont(measureFont);
    QGraphicsDropShadowEffect *blueEeffect = new QGraphicsDropShadowEffect(this);
    blueEeffect->setOffset(3);              //阴影的偏移量
    blueEeffect->setColor(QColor(30, 30, 150)); //阴影的颜色
    blueEeffect->setBlurRadius(25);             // 阴影圆角的大小
    oneKeyButton->setGraphicsEffect(blueEeffect);
    connect(oneKeyButton, SIGNAL(clicked(bool)), SLOT(SlotOneKeyButton()));

    methodButton = new QPushButton();
    methodButton->setObjectName("mainBlueButton");
    methodButton->setFixedSize(100,100);
    methodButton->setText(tr("设置"));
    methodButton->setFont(measureFont);
//    QGraphicsDropShadowEffect *rangeCheckEffect = new QGraphicsDropShadowEffect(this);
//    rangeCheckEffect->setOffset(3);              //阴影的偏移量
//    rangeCheckEffect->setColor(QColor(30, 30, 150)); //阴影的颜色
//    rangeCheckEffect->setBlurRadius(25);             // 阴影圆角的大小
//    rangeCheckButton->setGraphicsEffect(rangeCheckEffect);

    bool isFatalError = false;
    bool isExistFatalError = false;
    Table measureTable;
    luaEngine->GetLuaValue(state, "status.measure", measureTable);
    isExistFatalError = measureTable.safe_at("isFatalError", isFatalError);

    if(isExistFatalError && isFatalError)
    {
        ShowFatalWarning();
    }
    else
    {
        m_resultPanel->fatalButton->setVisible(false);
    }

    Table maintainTable;
    luaEngine->GetLuaValue(state, "setting.ui.operation.maintain", maintainTable);
    oolua_ipairs(maintainTable)
    {
        Table typeTable;
        String typeName;

        m_lua->pull(typeTable);
        typeTable.at("name", typeName);

        if ("Calibrate" == typeName)
        {
            typeTable.at("createFlow", creatCalibrateFunc);
        }
        else if ("MeasureRangeCheck" == typeName)
        {
            typeTable.at("createFlow", creatRangeCheckFunc);
        }
    }oolua_ipairs_end()

    //    measureModeComBox = new QComboBox();
    //    measureModeComBox->setObjectName("runStatusComboBox");
    //    measureModeComBox->setView(new QListView());
    //    measureModeComBox->setFont(measureFont);
    //    QGraphicsDropShadowEffect *measureEeffect = new QGraphicsDropShadowEffect(this);
    //    measureEeffect->setOffset(3);              //阴影的偏移量
    //    measureEeffect->setColor(QColor(30, 30, 150)); //阴影的颜色
    //    measureEeffect->setBlurRadius(25);             // 阴影圆角的大小
    //    measureModeComBox->setGraphicsEffect(measureEeffect);


    //    Table schedulerTable;
    //    luaEngine->GetLuaValue(state, "setting.ui.profile.scheduler", schedulerTable);

    //    QStringList modeList;
    //    oolua_ipairs(schedulerTable)
    //    {
    //        Table typeTable;
    //        String typeName;

    //        m_lua->pull(typeTable);
    //        typeTable.safe_at("name", typeName);

    //        oolua_ipairs(typeTable)
    //        {
    //          Table measureTable;
    //          String measureName;

    //          m_lua->pull(measureTable);
    //          measureTable.safe_at("name", measureName);
    //          if(measureName == "measure.mode")
    //          {
    //              Table optTable;
    //              measureTable.safe_at("options", optTable);
    //              oolua_ipairs(optTable)
    //              {
    //                  String modeStr;
    //                  m_lua->pull(modeStr);
    //                  modeList.append(QObject::tr(modeStr.c_str()));
    //              }oolua_ipairs_end()
    //          }

    //        }oolua_ipairs_end()

    //    }oolua_ipairs_end()

    //    measureModeComBox->setFont(measureFont);
    //    measureModeComBox->insertItem(0,QString(" 外部\n 触发"));
    //    measureModeComBox->insertItem(1,QString(" 周期\n 测量"));
    //    measureModeComBox->insertItem(2,QString(" 整点\n 测量"));
    //    measureModeComBox->insertItem(3,QString(" 连续\n 测量"));
    //    MeasureMode theMode;
    //    luaEngine->GetLuaValue(state, "config.scheduler.measure.mode", theMode);
    //    measureModeComBox->setCurrentIndex((int)theMode);
    //    measureModeComBox->setFixedSize(100,100);

    //    reportModeComBox = new QComboBox();
    //    reportModeComBox->setObjectName("runStatusComboBox");
    //    reportModeComBox->setView(new QListView());

    //    QStringList reportModeList;
    //    Table reportModeTable;
    //    luaEngine->GetLuaValue(state, "setting.ui.runStatus.reportModeList", reportModeTable);
    //    oolua_ipairs(reportModeTable)
    //    {
    //        String text;
    //        m_lua->pull(text);
    //        QString testTmp = QString::fromStdString(text);
    //        testTmp.remove(QRegExp("\\s"));             //去除所有空格
    ////        testTmp.remove(QRegExp("^ +\\s*"));     //去除字符前面的空格
    ////        testTmp.remove(QRegExp("\\s* +$"));     //去除字符后面的空格
    //        testTmp = " " + testTmp;
    //        reportModeList << testTmp;
    //    }
    //    oolua_ipairs_end()
    //    reportModeComBox->insertItems(0, reportModeList);

    //    ReportMode reportMode;
    //    luaEngine->GetLuaValue(state, "config.interconnection.reportMode", reportMode);
    //    reportModeComBox->setCurrentIndex((int)reportMode);
    //    reportModeComBox->setFixedSize(100,100);
    //    reportModeComBox->setFont(measureFont);
    //    QGraphicsDropShadowEffect *reportEeffect = new QGraphicsDropShadowEffect(this);
    //    reportEeffect->setOffset(3);              //阴影的偏移量
    //    reportEeffect->setColor(QColor(30, 30, 150)); //阴影的颜色
    //    reportEeffect->setBlurRadius(25);             // 阴影圆角的大小
    //    reportModeComBox->setGraphicsEffect(reportEeffect);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(stopButton);
    buttonLayout->addSpacing(120);
    buttonLayout->addWidget(oneKeyButton);
    buttonLayout->addSpacing(120);
    buttonLayout->addWidget(methodButton);
    buttonLayout->addStretch();
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *buttonVLayout = new QVBoxLayout();
    buttonVLayout->addStretch();
    buttonVLayout->addLayout(buttonLayout);;
    buttonVLayout->setSpacing(0);
    buttonVLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *erLayout = new QVBoxLayout();
    erLayout->addStretch();
    erLayout->addLayout(measureResultPanelLayout);
//    erLayout->addLayout(estimateResultLayout);
//    erLayout->addWidget(fatalButton,0, Qt::AlignCenter);
    erLayout->addStretch();
    erLayout->setSpacing(8); //20
    erLayout->setContentsMargins(0, 0, 0, 0);

    measureLayout = new QVBoxLayout();
    measureLayout->addStretch();
    measureLayout->addLayout(erLayout);
    measureLayout->addSpacing(20);
    measureLayout->addLayout(buttonVLayout);
    measureLayout->setSpacing(0);
    measureLayout->setContentsMargins(20, 0, 20, 0);

    QVBoxLayout *vLayout = new QVBoxLayout();
//    vLayout->addLayout(productTypeLayout);
    vLayout->addLayout(measureLayout);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 10, 0, 10);//整个窗口
    leftTopWidget->setLayout(vLayout);

    ChangeBottomStatus();

    m_mehtodWindow = new MethodWindow();
    m_mehtodWindow->hide();

    connect(stopButton, SIGNAL(clicked(bool)), this, SLOT(StopFlow()));
    connect(methodButton, SIGNAL(clicked(bool)), this, SLOT(SlotMethodSetting()));
//    connect(m_mehtodWindow, SIGNAL(MethodQuit()), this, SLOT(SlotMethodQuit()));
//    connect(measureModeComBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotChangeAutoMeasure()));
//    connect(reportModeComBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotChangeReportMode()));
    connect(this, SIGNAL(SignalUpdateExpectInfo(QString)), this, SLOT(SlotUpdateExpectInfo(QString)));

    MeasureResultPanelDisplayData();
}


void RunStatusWindow::MeasureResultPanelDisplayData()
{
    try
    {
        if (!m_items.empty())
        {
            lua_State * newState = LuaEngine::Instance()->GetThreadState();
            OOLUA::Lua_function call(newState);

            for(int i = 0; i < (int)m_items.size(); ++i)
            {
                RunStatusIterm iterm = m_items.at(i);

                int dateTime;
                float consistency;
                float consistencyTC;
                float consistencyIC;

                String strResultType = "";

                call(iterm.getData);
                OOLUA::pull(newState, consistency);
                OOLUA::pull(newState, dateTime);

                call(iterm.getDataTC);
                OOLUA::pull(newState, consistencyTC);
                OOLUA::pull(newState, dateTime);

                call(iterm.getDataIC);
                OOLUA::pull(newState, consistencyIC);
                OOLUA::pull(newState, dateTime);

                if (m_isUnitChange)
                {
                    m_lua->call(m_unitChangeFunc, consistency, UnitChange::Read);
                    m_lua->pull(consistency);
                    m_lua->call(m_unitChangeFunc, consistencyTC, UnitChange::Read);
                    m_lua->pull(consistencyTC);
                    m_lua->call(m_unitChangeFunc, consistencyIC, UnitChange::Read);
                    m_lua->pull(consistencyIC);
                }
                QString unitStr = "ppb";
                QString strConsistency = ResultManager::Instance()->DisplayResult(consistency, unitStr);
                QString strConsistencyTC = ResultManager::Instance()->DisplayResult(consistencyTC, unitStr);
                QString strConsistencyIC = ResultManager::Instance()->DisplayResult(consistencyIC, unitStr);
                QString strTime = QDateTime::fromTime_t(dateTime).toString("yyyy-MM-dd hh:mm:ss");

                if(dateTime == -1)
                {
                    strTime = " ";
                }

                if (iterm.isNeedResultType)
                {
                    Byte resultType;
                    call(iterm.getResultType);
                    OOLUA::pull(newState, resultType);

                    if ((MeasureType)resultType != MeasureType::None)
                    {
                        LuaEngine* luaEngine = LuaEngine::Instance();
                        lua_State* state = luaEngine->GetThreadState();

                        Table measureTable;
                        luaEngine->GetLuaValue(state, "setting.fileFormat.measure", measureTable);

                        int currentId = 0;
                        Table formatTable;

                        measureTable.at("current", currentId);
                        measureTable.at(currentId, formatTable);

                        Table optionTable;

                        oolua_ipairs(formatTable)
                        {
                            Table itermTable;
                            m_lua->pull(itermTable);

                            String name;
                            itermTable.at("name", name);
                            if (name == "resultType")
                            {
                                itermTable.at("option", optionTable);
                            }
                        }
                        oolua_ipairs_end()

                        optionTable.safe_at((int)resultType + 1, strResultType);
                    }
                }

                m_resultPanel->SetResultInfo(QObject::tr(iterm.name.c_str()), strConsistency, strTime, QString(strResultType.c_str()));
                m_resultPanel->SetResultInfoTC(QObject::tr("TC"), strConsistencyTC);
                m_resultPanel->SetResultInfoIC(QObject::tr("IC"), strConsistencyIC);
            }
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("RunStatusWindow::MeasureResultPanelDisplayData() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("RunStatusWindow::MeasureResultPanelDisplayData() => %s", e.what());
    }
}

void RunStatusWindow::StopFlow()
{
    MessageDialog msg(tr("是否停止当前动作？\n"), this, MsgStyle::OKANDCANCEL);
    if(msg.exec()==QDialog::Accepted)
    {
        /*停止组合维护*/
        FlowManager::Instance()->UpdateFlowMessage("MaintainCombine","Stop");

        /*停止维护界面和组合维护的所有动作*/
        LuaEngine* luaEngine = LuaEngine::Instance();
        Script* lua = luaEngine->GetEngine();
        lua_State * state = luaEngine->GetThreadState();

        MeasureMode curScheduler = MeasureMode::Trigger;
        luaEngine->GetLuaValue(state, "config.scheduler.measure.mode", curScheduler);

        if (MeasureMode::Continous == curScheduler)
        {
            luaEngine->SetLuaValue("config.scheduler.measure.mode", (int)MeasureMode::Trigger);
            SettingManager::Instance()->SchedulerSave();
        }

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
        MeasureScheduler::Instance()->Update();
    }
}

void RunStatusWindow::SlotMethodSetting()
{
    if(m_mehtodWindow)
    {
        m_mehtodWindow->show();
    }
}

void RunStatusWindow::initLeftBottom()
{

}

void RunStatusWindow::ChangeBottomStatus()
{
    if(ProLicenseManager::Instance()->GetProLicense())
    {
        bool ret = false;
        for (int i = (int)Authority::Maintain; i < ((int)Authority::InstrumentInformation); i++)
        {
            if(LoginDialog::GetUserPrivilege((Authority)i))
            {
                ret = true;
            }
        }
        if(ret)
        {
            stopButton->show();
            methodButton->show();
            oneKeyButton->show();
        }
        else
        {
            stopButton->hide();
            methodButton->hide();
            oneKeyButton->hide();
        }
    }
    else
    {
        if(LoginDialog::userType == Super)
        {
            stopButton->show();
            methodButton->show();
            oneKeyButton->show();
        }
        else if(LoginDialog::userType == Administrator)
        {
            stopButton->show();
            methodButton->show();
            oneKeyButton->show();
        }
        else if(LoginDialog::userType == Maintain)
        {
            stopButton->show();
            methodButton->show();
            oneKeyButton->show();
        }
        else if(LoginDialog::userType == General)
        {
            stopButton->hide();
            methodButton->hide();
            oneKeyButton->hide();
        }
    }
}

void RunStatusWindow::ShowFatalWarning()
{
    m_resultPanel->fatalButton->setVisible(true);
    m_resultPanel->estimateResultTimeLable->setVisible(false);
}

void RunStatusWindow::SlotOneKeyButton()
{
//    QString str = QString("试剂更新并校准<font color=red>所有量程<font color=black>曲线");
//    QString str2 = QString("校准<font color=red>当前量程<font color=black>曲线");
//    MessageDialog msg(str2,str, this);
//    int result = msg.exec();
//    if(result==1)
//    {
//        qDebug("creatCalibrateFunc");
//        StatusManager *statusManager = StatusManager::Instance();
//        Script *lua = LuaEngine::Instance()->GetEngine();
//        if (statusManager->IsIdleStatus() && statusManager->IsIdleAction())
//        {

//            if (lua->call(creatCalibrateFunc))
//            {
//                FlowManager::Instance()->StartFlow();
//            }
//            else
//            {
//                logger->warn("RunStatusWindow call the function Fail on lua");
//            }
//        }
//        else
//        {
//            MessageDialog msg(tr("请确认仪器状态是否为空闲！\n"), this,MsgStyle::ONLYOK);
//            if(msg.exec()== QDialog::Accepted)
//            {
//                return;
//            }
//        }
//    }
//    else if (result==2)
//    {
//        qDebug("oneKeyMaintain");
//        StatusManager *statusManager = StatusManager::Instance();
//        Script *lua = LuaEngine::Instance()->GetEngine();
//        Table table, itermsTable;
//        itermsTable.bind_script(*lua);
//        itermsTable.set_table("setting");
//        itermsTable.at("ui", table);
//        table.at("runStatus", itermsTable);

//        Lua_func_ref func;
//        itermsTable.at("oneKeyMaintain", func);

//        if (statusManager->IsIdleStatus() && statusManager->IsIdleAction())
//        {
//            emit SignalOneKeyChangeReagent();

//            if (lua->call(func))
//            {
//                FlowManager::Instance()->StartFlow();
//            }
//            else
//            {
//                logger->warn("RunStatusWindow call the function Fail on lua");
//            }
//        }
//        else
//        {
//            MessageDialog msg1(tr("请确认仪器状态是否为空闲！\n"), this,MsgStyle::ONLYOK);
//            if(msg1.exec()== QDialog::Accepted)
//            {
//                return;
//            }
//        }
//    }

    LuaEngine* luaEngine = LuaEngine::Instance();
    luaEngine->SetLuaValue("config.scheduler.measure.mode", 3);
    SettingManager::Instance()->SchedulerSave();
    UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::ChangeAutoMeasure, "RunStatusWindow");

    MeasureScheduler::Instance()->Update();
}

void RunStatusWindow::OnMeasureResultAdded(String name, RecordData recordData)
{
    (void)name;
    (void)recordData;
    MeasureResultPanelDisplayData();
}

//void RunStatusWindow::SlotChangeAutoMeasure()
//{
//    LuaEngine* luaEngine = LuaEngine::Instance();
//    luaEngine->SetLuaValue("config.scheduler.measure.mode", measureModeComBox->currentIndex());
//    SettingManager::Instance()->SchedulerSave();
//    UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::ChangeAutoMeasure, "RunStatusWindow");

//    MeasureScheduler::Instance()->Update();
//}

void RunStatusWindow::RefreshAutoMeasure()
{
//    LuaEngine* luaEngine = LuaEngine::Instance();
//    lua_State* state = luaEngine->GetThreadState();
//    MeasureMode autoMode;
//    luaEngine->GetLuaValue(state, "config.scheduler.measure.mode", autoMode);
//    measureModeComBox->setCurrentIndex((int)autoMode);
}

//void RunStatusWindow::SlotChangeReportMode()
//{
//    LuaEngine* luaEngine = LuaEngine::Instance();
//    luaEngine->SetLuaValue("config.interconnection.reportMode", reportModeComBox->currentIndex());
//    SettingManager::Instance()->InterconnectionSave();
//    UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::ChangeInterconnectionParam, "RunStatusWindow");
//}

void RunStatusWindow::RefreshReportMode()
{
//    LuaEngine* luaEngine = LuaEngine::Instance();
//    lua_State* state = luaEngine->GetThreadState();
//    ReportMode reportMode;
//    luaEngine->GetLuaValue(state, "config.interconnection.reportMode", reportMode);
//    reportModeComBox->setCurrentIndex((int)reportMode);
}

void RunStatusWindow::RefreshProformaResult(String message)
{
    if (message.empty())
    {
        m_resultPanel->SetProformaResultInfo(QString());
        return ;
    }

    try
    {
        if (!m_items.empty())
        {
            lua_State * newState = LuaEngine::Instance()->GetThreadState();
            OOLUA::Lua_function call(newState);

            for(int i = 0; i < (int)m_items.size(); ++i)
            {
                RunStatusIterm iterm = m_items.at(i);

                if (false == iterm.isNeedProformaData)
                {
                    continue ;
                }

                if (iterm.name.empty() == false && message.compare(iterm.name) != 0)
                {
                    continue ;
                }

                float consistency;

                call(iterm.getProformaData);
                OOLUA::pull(newState, consistency);

                if (m_isUnitChange)
                {
                    m_lua->call(m_unitChangeFunc, consistency, UnitChange::Read);
                    m_lua->pull(consistency);
                }

                QString unitStr = "ppb";
                QString strConsistency = ResultManager::Instance()->DisplayResult(consistency, unitStr);
                m_resultPanel->SetProformaResultInfo(strConsistency);

            }
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("RunStatusWindow::MeasureResultPanelDisplayData() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("RunStatusWindow::MeasureResultPanelDisplayData() => %s", e.what());
    }
}

void RunStatusWindow::OnUserChange()
{
    ChangeBottomStatus();
}
void RunStatusWindow::OnCalibrateResultAdded(String name, RecordData result)
{
    (void)name;
    (void)result;
}

void RunStatusWindow::mousePressEvent(QMouseEvent * event)
{
    m_isMousePress = true;
    m_pressPoint = event->pos();
    m_pressTime = QDateTime::currentDateTime();
}

void RunStatusWindow::mouseReleaseEvent(QMouseEvent *event)
{
//  符合条件:是否被按下过（有些区域识别不到按下），是否按下的位置和放开的位置X或Y相差50，是否按下的时间超过3秒
//    if(true == m_isMousePress &&
//            (abs(m_pressPoint.x() - event->x()) > 50
//            || abs(m_pressPoint.y() - event->y()) > 50)
//            && m_pressTime.addSecs(30) <  QDateTime::currentDateTime())
//    {
//        logger->notice("主界面启动触摸屏校准 按下坐标：(%d, %d) 松开坐标：(%d, %d)",
//                       m_pressPoint.x(), m_pressPoint.y(), event->x(), event->y());
//        ScreenCalibrate screenCalibrate(this);
//    }
//    m_isMousePress = false;
}

void RunStatusWindow::OnStattusChanged(RunStatus status)
{
    (void)status;
}

void RunStatusWindow::OnStattusDelay(int time)
{
    (void)time;
}

void RunStatusWindow::OnActionChanged(RunAction action)
{
    (void)action;
}

void RunStatusWindow::OnActionDelay(int time)
{
    (void)time;
}

void RunStatusWindow::OnUpdateWidget(UpdateEvent event, System::String message)
{
    if (event == UpdateEvent::ChangeConfigParam || event == UpdateEvent::WqimcChangeParam)//一键恢复出厂设置
    {
//        LuaEngine* luaEngine = LuaEngine::Instance();
//        lua_State* state = luaEngine->GetThreadState();

//        MeasureMode autoMode;
//        luaEngine->GetLuaValue(state, "config.scheduler.measure.mode", autoMode);
//        measureModeComBox->setCurrentIndex((int)autoMode);

//        ReportMode reportMode;
//        luaEngine->GetLuaValue(state, "config.interconnection.reportMode", reportMode);
//        reportModeComBox->setCurrentIndex((int)reportMode);
    }
    if(event == UpdateEvent::ShowNewProformaData)
    {
        RefreshProformaResult(message);
    }
    else if (event == UpdateEvent::ChangeInterconnectionParam && message != "RunStatusWindow")
    {
        RefreshReportMode();
    }
    else if (event == UpdateEvent::ChangeMeaModeOnHome)    // 刷新主界面的测量模式
    {
        RefreshAutoMeasure();
    }
    else if (event == UpdateEvent::ChangeAutoMeasure && message != "RunStatusWindow")
    {
        RefreshAutoMeasure();
    }
    else if (event == UpdateEvent::OneKeyClearData)
    {
        MeasureResultPanelDisplayData();
        emit m_resultPanel->fatalButton->click();
    }
    else if (event == UpdateEvent::FatalErrorOccurrence)
    {
        ShowFatalWarning();
    }
    else if (event == UpdateEvent::FatalErrorClear && message == "InstrumentAgentHandler")
    {
        emit m_resultPanel->fatalButton->click();
    }
    else if(event == UpdateEvent::SystemInfoChanged)
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State* state = luaEngine->GetThreadState();
        String showModel;
        luaEngine->GetLuaValue(state, "config.info.instrument.showModel", showModel);
//        if(SettingManager::Instance()->GetManufactureType() != ManufactureType::OEM)
//        {
//            productTypeLabel->setText(showModel.c_str());
//        }
    }
    else if(event == UpdateEvent::MulCalibrationTip)
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State* state = luaEngine->GetThreadState();
        String modbusStr = "";
        luaEngine->SetLuaValue("status.measure.messageDialogStr", message);
        m_msgRemindStr = new MessageDialog(QString(message.c_str()) ,  this,MsgStyle::ONLYOK);
        if(m_msgRemindStr->exec()==QDialog::Rejected)
        {
            luaEngine->SetLuaValue("status.measure.messageDialogStr", modbusStr);
            return;
        }
    }
    else if (event == UpdateEvent::ModbusChangeParam && message == "Modbus")
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State* state = luaEngine->GetThreadState();
        String modbusStr = "";
        luaEngine->SetLuaValue("status.measure.messageDialogStr", modbusStr);
        if(m_msgRemindStr != nullptr){
            m_msgRemindStr->setResult(QDialog::Rejected);
            m_msgRemindStr->done(QDialog::Rejected);
        }
    }
}

