
#include <QHBoxLayout>
#include <QStringListModel>
#include "UI/Frame/LoginDialog.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "Setting/SettingManager.h"
#include "MeasureDataWindow.h"
#include "UI/ControlConfig.h"

using namespace Configuration;

using namespace OOLUA;

namespace UI
{

MeasureDataWindow::MeasureDataWindow(QWidget *parent) :
    QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);
    initWindow();
}

MeasureDataWindow::~MeasureDataWindow()
{

}

void MeasureDataWindow::initWindow()
{   
    listView = new QListView();
    listView->setFixedHeight(400);
    listView->setFixedWidth(101);
    listView->setObjectName(QStringLiteral("commonListView"));
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 20px;}");

    stackedWidget = new QStackedWidget();
    stackedWidget->setFixedHeight(425);
    stackedWidget->setFixedWidth(693);
    stackedWidget->setObjectName(QStringLiteral("stackedWidget"));

    Table itermsTable, measureDataTable, waveDataTable, calibrateCurveTable;

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "setting.ui.measureDataWindow", itermsTable);

    bool isExistMeasureData = itermsTable.safe_at("measureData", measureDataTable);
    bool isExistWaveData = itermsTable.safe_at("waveData", waveDataTable);
    bool isExistCalibrateCurve = itermsTable.safe_at("calibrateCurve", calibrateCurveTable);

    int type = SettingManager::Instance()->GetParamType();

    if (isExistMeasureData)
    {
        oolua_ipairs(measureDataTable)
        {
            Table itermTable;
            lua->pull(itermTable);
            int targetPrivilege = 0;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

            if (isTarget == false || (type & targetPrivilege))
            {
                String name, resultDataname;
                MeasureData *measureDataWidget;

                itermTable.at("name", name);
                m_strList << QObject::tr(name.c_str());

                itermTable.at("resultDataname", resultDataname);
                measureDataWidget = new MeasureData(resultDataname);
                stackedWidget->addWidget(measureDataWidget);

                m_measureDataWidget.push_back(measureDataWidget);
            }
        }
        oolua_ipairs_end()
        m_measureDataEndIndex = m_strList.size() - 1;
    }
    else
    {
        m_measureDataEndIndex = 0;
    }

    if (isExistWaveData)
    {
        oolua_ipairs(waveDataTable)
        {
            Table itermTable;
            lua->pull(itermTable);

            int targetPrivilege = 0;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

            if (isTarget == false || (type & targetPrivilege))
            {
                String name, resultDataname, profileTableName, rangeTableName;
                WaveData *waveDataWidget;

                itermTable.at("name", name);
                m_strList << QObject::tr(name.c_str());

                itermTable.at("resultDataname", resultDataname);

                itermTable.at("profileTableName", profileTableName);

                itermTable.at("rangeTableName", rangeTableName);

                waveDataWidget = new WaveData(resultDataname, profileTableName, rangeTableName);
                stackedWidget->addWidget(waveDataWidget);

                m_waveDataWidget.push_back(waveDataWidget);
            }
        }
        oolua_ipairs_end()
        m_waveDataEndIndex = m_strList.size() - 1;
    }
    else
    {
        m_waveDataEndIndex = 0;
    }

    if (isExistCalibrateCurve)
    {
        oolua_ipairs(calibrateCurveTable)
        {
            Table itermTable;
            lua->pull(itermTable);

            int targetPrivilege = 0;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

            if (isTarget == false || (type & targetPrivilege))
            {
                String name, resultDataname, profileTableName, profileFileName, curveParamName;
                CalibrateCurve *calibrateCurveWidget;

                itermTable.at("name", name);
                m_strList << QObject::tr(name.c_str());

                itermTable.at("resultDataname", resultDataname);

                itermTable.at("profileTableName", profileTableName);

                itermTable.at("profileFileName", profileFileName);

                itermTable.at("curveParamName", curveParamName);

                calibrateCurveWidget = new CalibrateCurve(resultDataname, profileTableName, profileFileName, curveParamName);
                stackedWidget->addWidget(calibrateCurveWidget);

                m_calibrateCurveWidget.push_back(calibrateCurveWidget);
            }
        }
        oolua_ipairs_end()
        m_calibrateCurveEndIndex = m_strList.size() - 1;
    }
    else
    {
        m_calibrateCurveEndIndex = 0;
    }
//    m_strList <<tr("量程信息");
//    curveWidget = new CurveTableWidget();
//    stackedWidget->addWidget(curveWidget);

    if (m_strList.size() == 0)
    {
        warnManagerIndex = 0;
    }
    else
    {
        warnManagerIndex = m_strList.size();
    }
    runLogIndex = warnManagerIndex + 1;

    m_strList<<tr("告警管理")<<tr("运行日志");

    warnManagerWidget = new WarnManager();
    stackedWidget->addWidget(warnManagerWidget);

    runLogWidget = new RunLog();
    stackedWidget->addWidget(runLogWidget);

    QVBoxLayout *listViewLayout = new QVBoxLayout();
    listViewLayout->addWidget(listView);
    listViewLayout->setContentsMargins(0, 11, 0, 0);
    listViewLayout->addStretch();

    QHBoxLayout *stackedWidgetLayout = new QHBoxLayout();
    stackedWidgetLayout->addWidget(stackedWidget);
    stackedWidgetLayout->addStretch();
    stackedWidgetLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *vstackedWidgetLayout = new QVBoxLayout();
    vstackedWidgetLayout->addLayout(stackedWidgetLayout);
    vstackedWidgetLayout->addStretch();
    vstackedWidgetLayout->setContentsMargins(0, 3, 0, 0);

    model = new QStringListModel(m_strList);
    //listView->setSpacing(5);
    listView->setGridSize(QSize(50,50));
    listView->setModel(model);

    QFont listFont = listView->font();
    listFont.setPointSize(16);
    listFont.setWeight(65);
    listView->setFont(listFont);

    connect(listView,SIGNAL(clicked(QModelIndex)), this,SLOT(itemClicked(QModelIndex)));
    QHBoxLayout *mainlayout = new QHBoxLayout();

    mainlayout->addLayout(listViewLayout);
    mainlayout->addLayout(vstackedWidgetLayout);

    mainlayout->setContentsMargins(2, 5, 0, 0);
    this->setLayout(mainlayout);
    ChangeBottomStatus();

    connect(warnManagerWidget, SIGNAL(SignalAlarmClear()), this, SIGNAL(SignalWindowAlarmClear()));
}

void MeasureDataWindow::itemClicked(QModelIndex index)
{
//    if(m_strList.at(3) == index.data().toString())
//    {
//        stackedWidget->setCurrentWidget(curveWidget);
//    }
    if(m_strList.at(warnManagerIndex) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(warnManagerWidget);
    }

    else if(m_strList.at(runLogIndex) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(runLogWidget);
    }
    else
    {
        int startIndex = 0;
        if (m_measureDataWidget.size() != 0)
        {
            for (int i = startIndex; i <= m_measureDataEndIndex; i++)
            {
                if (m_strList.at(i) == index.data().toString())
                {
                     stackedWidget->setCurrentWidget(m_measureDataWidget.at(i));
                     break;
                }
            }
            startIndex = m_measureDataEndIndex + 1;
        }
        if (m_waveDataWidget.size() != 0)
        {
            for (int i = startIndex; i <= m_waveDataEndIndex; i++)
            {
                if (m_strList.at(i) == index.data().toString())
                {
                     stackedWidget->setCurrentWidget(m_waveDataWidget.at(i - startIndex));
                     break;
                }
            }
            startIndex = m_waveDataEndIndex + 1;
        }
        if (m_calibrateCurveWidget.size() != 0)
        {
            for (int i = startIndex; i <= m_calibrateCurveEndIndex; i++)
            {
                if (m_strList.at(i) == index.data().toString())
                {
                     stackedWidget->setCurrentWidget(m_calibrateCurveWidget.at(i - startIndex));
                     break;
                }
            }
        }
    }
}

void MeasureDataWindow::OnUserChange()
{
    ChangeBottomStatus();
}

void MeasureDataWindow::ChangeBottomStatus()
{
    if (m_measureDataWidget.size() != 0)
    {
        stackedWidget->setCurrentWidget(m_measureDataWidget.at(0));
    }
    else if (m_waveDataWidget.size() != 0)
    {
        stackedWidget->setCurrentWidget(m_waveDataWidget.at(0));
    }
    else if (m_calibrateCurveWidget.size() != 0)
    {
        stackedWidget->setCurrentWidget(m_calibrateCurveWidget.at(0));
    }
    else
    {
        stackedWidget->setCurrentWidget(warnManagerWidget);
    }
    QModelIndex index = model->index(0,0);
    listView->setCurrentIndex(index);

    if(LoginDialog::userType == Super)
    {
        listView->setRowHidden(0, false);
        listView->setRowHidden(1, false);
        listView->setRowHidden(2, false);
        listView->setRowHidden(3, false);
        listView->setRowHidden(4, false);
        listView->setRowHidden(5, false);
    }
    else if(LoginDialog::userType == Administrator)
    {
        listView->setRowHidden(0, false);
        listView->setRowHidden(1, false);
        listView->setRowHidden(2, false);
        listView->setRowHidden(3, true);
        listView->setRowHidden(4, false);
        listView->setRowHidden(5, false);

    }
    else if(LoginDialog::userType == Maintain)
    {
        listView->setRowHidden(0, false);
        listView->setRowHidden(1, false);
        listView->setRowHidden(2, false);
        listView->setRowHidden(3, true);
        listView->setRowHidden(4, false);
        listView->setRowHidden(5, false);

    }
    else if(LoginDialog::userType == General)
    {
        listView->setRowHidden(0, false);
        listView->setRowHidden(1, false);
        listView->setRowHidden(2, false);
        listView->setRowHidden(3, true);
        listView->setRowHidden(4, false);
        listView->setRowHidden(5, false);
    }
}

}


