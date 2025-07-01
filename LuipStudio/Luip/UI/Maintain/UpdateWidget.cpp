#include "UseResourceWidget.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "System/Types.h"
#include "UpdateWidget.h"
#include "System/Exception.h"
#include "Log.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include <QDebug>
#include "System/FileDetecter.h"
#include "UI/Frame/MessageDialog.h"
#include "Setting/Environment.h"
#include "LuaEngine/LuaEngine.h"
#include "StatusManager/StatusManager.h"
#include "Controller/ControllerManager.h"
#include "FlowManager/FlowManager.h"
#include "UpdateManager/ProgramFileParser.h"
#include "Communication/CommunicationException.h"
#include "AlarmManager/AlarmManager.h"
#include "UpdateManager/MasterUpdate.h"
#include "Setting/SettingManager.h"
#include "UpdateManager/MasterUpdate.h"
#include "FlowManager/UpdateFlow.h"
#include "System/Uncompress.h"
#include "System/AuthorizationManager.h"
#include "UI/ControlConfig.h"
#include "System/NFSManager.h"
#include "checkscreen.h"

using namespace Measure;
using System::Uint32;
using namespace std;
using namespace OOLUA;
using namespace Lua;
using namespace Controller;
using namespace Flow;
using namespace Configuration;
using namespace Communication;
using namespace Configuration;
using namespace Updater;
using namespace System;

namespace UI
{

UpdateWidget::UpdateWidget(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);

    Script *lua = LuaEngine::Instance()->GetEngine();
    Table table, itermsTable;
    itermsTable.bind_script(*lua);
    itermsTable.set_table("setting");
    itermsTable.at("ui", table);
    table.at("update", itermsTable);

    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("rowCount", useRowCount);
    useRowCount++;
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("administratorRow", administratorRow);

    QFont font;                           //字体
    font.setPointSize(14);                //字体大小

    this->resize(650, 430);

    updateTableWidget = new MQtableWidget();

    updateTableWidget->setColumnCount(5);

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 5;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(updateTableWidget, m_tableInfo);

    updateTableWidget->setFixedSize(550, 291);

    QStringList columnName;
    columnName << " " << tr("板卡") << tr("地址") << tr("当前版本") << tr("升级版本");
    updateTableWidget->setColumnAndSize(columnName,15);

    QFont headerFont;
    headerFont.setPointSize(14);

    updateTableWidget->horizontalHeader()->setFont(headerFont);

    updateTableWidget->horizontalHeader()->setFixedHeight(39); // 设置表头的高度
    updateTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        updateTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background:transparent; width: 30px;}");
    }

    QFont dataFont = updateTableWidget->font();
    dataFont.setPointSize(10);
    updateTableWidget->setFont(dataFont);
    updateTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(updateTableWidget);
    measureResultTableLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    refreshButton = new QPushButton();
    refreshButton->setObjectName("brownButton");
    refreshButton->setText(tr("刷新"));
    refreshButton->setFont(font);
    refreshButton->setFixedSize(120, 52);
    QHBoxLayout *refreshButtonLayout = new QHBoxLayout();
    refreshButtonLayout->addWidget(refreshButton);
    refreshButtonLayout->setContentsMargins(0, 40, 0, 0);

    oneCheck = new QPushButton();
    oneCheck->setObjectName("brownButton");
    oneCheck->setText(tr("一键升级"));
    oneCheck->setFont(font);
    oneCheck->setFixedSize(120, 52);
    QHBoxLayout *oneCheckLayout = new QHBoxLayout();
    oneCheckLayout->addWidget(oneCheck);
    oneCheckLayout->setContentsMargins(50, 40, 0, 0);

    SourceLabel = new QLabel();
    SourceLabel->setText(tr("升级包来源 ： 本地"));
    SourceLabel->setFixedWidth(200);
    SourceLabel->setFixedHeight(30);
    SourceLabel->setFont(font);
    QHBoxLayout *SourceLabelLayout = new QHBoxLayout();
    SourceLabelLayout->addWidget(SourceLabel);
    SourceLabelLayout->setContentsMargins(50, 40, 0, 0);

    bottomLayout->addLayout(refreshButtonLayout);
    bottomLayout->addStretch();
    bottomLayout->addLayout(SourceLabelLayout);
    bottomLayout->addStretch();
    bottomLayout->addLayout(oneCheckLayout);

    QVBoxLayout *leftLayout = new QVBoxLayout();

    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();
    leftLayout->addLayout(bottomLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    this->setLayout(mainLayout);

    updateTableWidget->setColumnWidth(0, 64);    //设置每行或每列的高度或宽度
    updateTableWidget->setColumnWidth(1, 88);
    updateTableWidget->setColumnWidth(2, 88);
    updateTableWidget->setColumnWidth(3, 100);
    updateTableWidget->setColumnWidth(4, 208);

    for (int i = 0; i < updateTableWidget->rowCount(); i++)
    {
        updateTableWidget->setRowHeight(i, 50);
    }

    updateTableWidget->setFont(font);
    SpaceInit();
    for(int i = 0;i < m_tableInfo.userRow; i++)
    {
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setObjectName("checkboxtwo");
        checkBox->setCheckState(Qt::Unchecked);
        updateTableWidget->setCellWidget(i, 0, checkBox);
        m_checkBox.insert(make_pair(i, checkBox));
    }

    updateTableWidget->item(0, 1)->setText(tr("主控板"));

    int type = SettingManager::Instance()->GetParamType();

    Uint8 row = 1;
    oolua_ipairs(itermsTable)
    {
        Table itermTable;
        String text;
        UpdateItem updateItem;

        lua->pull(itermTable);

        int targetPrivilege = 0;
        bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

        if (isTarget == false || (type & targetPrivilege))
        {
            itermTable.at("text", updateItem.text);
            updateTableWidget->item(row, 1)->setText(QObject::tr(updateItem.text.c_str()));

            itermTable.safe_at("fileName", updateItem.hexFileName);
            itermTable.at("name", updateItem.name);
            itermTable.at("writePrivilege", updateItem.writePrivilege);
            itermTable.at("readPrivilege", updateItem.readPrivilege);

            if (CheckScreen::Instance()->isVNC())
            {
                System::String ExtFileName;
                if (itermTable.safe_at("ExtFileName", ExtFileName))
                {
                    updateItem.hexFileName = ExtFileName;
                }
            }

            m_items.insert(make_pair(row, updateItem));
            row++;
        }
    }
    oolua_ipairs_end()

    // 二次授权登录窗口
    m_authorizationDlg = new ReagentAuthorizationDialog(this);
    m_authorizationDlg->hide();

    OnUserChange();
    m_isGetInfo = false;
    m_getInfoTimer = new QTimer(this);
    connect(m_getInfoTimer, SIGNAL(timeout()), this, SLOT(GetInfoTimerHandle()));
    connect(refreshButton,SIGNAL(clicked(bool)),this, SLOT(RefreshButtonSlot()));
    connect(oneCheck,SIGNAL(clicked(bool)),this, SLOT(oneCheckConfig()));
    connect(this, SIGNAL(UpdaterResultSignals(QString)), this, SLOT(UpdaterResultSlots(QString)));
}

void UpdateWidget::SpaceInit()
{
    for (int i = 0; i < updateTableWidget->rowCount(); i++)
    {
        for (int j = 0; j < updateTableWidget->columnCount(); j++)
        {
            updateTableWidget->setItem(i, j, new QTableWidgetItem());
            updateTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
            QTableWidgetItem *item = updateTableWidget->item(i, j);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setFlags(Qt::ItemIsEnabled);
        }
    }
}

void UpdateWidget::oneCheckConfig()
{
    if (!FlowManager::Instance()->IsFlowEnable())  // 空闲状态
    {
        MessageDialog msg(tr("请确认仪器状态是否为空闲！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()==QDialog::Accepted)
        {
            return;
        }
    }

    if (m_authorizationDlg->isHidden())
    {
        //判断使能
        int nEnable = AuthorizationManager::Instance()->IsReagentAuthorizationEnable();
        if ((int)ReagentAuthorizationEnable::Effective == nEnable) //功能打开，试剂授权码界面
        {
            //动态密码
            if (m_authorizationDlg->exec() == QDialog::Rejected)
            {
                return ;
            }
        }
    }

    bool isChecked = false;
    for (map<Uint8, QCheckBox *>::iterator iter = m_checkBox.begin(); iter != m_checkBox.end(); ++iter)
    {
        QCheckBox *checkBox = iter->second;
        if (checkBox->checkState() == Qt::Checked)
        {
            isChecked = true;
            break;
        }
    }
    if (isChecked)
    {
        if (m_isGetInfo)
        {
            MessageDialog msg(tr("正在获取版本信息，请稍后升级"), this);
            msg.exec();
            return;
        }

        if (!UpdatePacketCheck())     //检测升级软件是否缺失
        {
            logger->debug("缺少升级软件");
            return;
        }

        UpdateFlow *updateFlow = new UpdateFlow();
        for (map<Uint8, QCheckBox *>::iterator iter = m_checkBox.begin(); iter != m_checkBox.end(); ++iter)
        {
            Uint8 row = iter->first;
            QCheckBox *checkBox = iter->second;

            if (checkBox->checkState() == Qt::Checked)
            {
                if (row == 0)//主控板升级
                {
                    if (m_toolFileName.empty())
                    {
                        updateFlow->AddStep(new MasterUpdate(Environment::Instance()->GetUpdatePath() + "/" + m_masterFileName, false));
                    }
                    else
                    {
                        updateFlow->AddStep(new MasterUpdate(Environment::Instance()->GetUpdatePath() + "/" + m_toolFileName));
                    }
                }
                else if (m_items.count(row))
                {
                    UpdateItem &updateItem = m_items[row];
                    updateFlow->AddStep(new UpdateManager(updateItem.addr,
                                                          Environment::Instance()->GetUpdatePath() + "/" + updateItem.fileName,
                                                          updateItem.text, false));
                }
            }
        }
        FlowManager::Instance()->StartCFlow(updateFlow);
    }
}

void UpdateWidget::showEvent(QShowEvent *event)
{
    (void)event;
    if (false == m_isGetInfo)
    {
        m_getInfoTimer->start(50);
    }
}

void UpdateWidget::RefreshButtonSlot()
{
    if (false == m_isGetInfo)
    {
        m_getInfoTimer->start(50);
    }
}

void UpdateWidget::GetInfoTimerHandle()
{
    m_getInfoTimer->stop();
    m_isGetInfo = true;
    WidgetUpdate();
    m_isGetInfo = false;
}

void UpdateWidget::WidgetUpdate()
{
    QDir dir(Environment::Instance()->GetUpdatePath().c_str());
    QStringList allPathlist = dir.entryList();
    QStringList upgPathlist = allPathlist.filter(".upg");

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    String instrumentType;
    luaEngine->GetLuaValue(state, "config.info.instrument.type", instrumentType);
    instrumentType = instrumentType + "_";
    upgPathlist = upgPathlist.filter(instrumentType.c_str());
//    upgPathlist << allPathlist.filter("PT62-MCT");//检测是否存在主控板生产检测工装软件的升级包

    for (map<Uint8, UpdateItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
    {
        Uint8 row = iter->first;
        UpdateItem &updateItem = iter->second;

        BaseController* controller = ControllerManager::Instance()->GetController(updateItem.name);
        updateItem.addr = controller->GetAddress();

        updateTableWidget->item(row, 2)->setText(updateItem.addr.ToString().c_str());

        if (controller->GetConnectStatus())
        {
            try
            {
                updateTableWidget->item(row, 3)->setText(controller->IDeviceInfo->GetSoftwareVersion().ToCompleteVersionString().c_str());
                String type = controller->GetIDeviceInfo()->GetType();
                type = "_" + type + "_";
                QStringList pathlist = upgPathlist.filter(type.c_str());
                if (pathlist.size() > 0)
                {
                    updateItem.fileName = pathlist.at(0).toStdString();
                    String version = Uncompress::ParserUnzipVersion(updateItem.fileName);
                    updateTableWidget->item(row, 4)->setText(version.c_str());
                }
                else
                {


                    Updater::ProgramFileParser programFileParser(Environment::Instance()->GetUpdatePath() + "/" + updateItem.hexFileName);
                    if (programFileParser.ParseHeader())
                    {
                        updateTableWidget->item(row, 4)->setText(programFileParser.GetVersion().c_str());
                        updateItem.fileName = updateItem.hexFileName;
                    }
                    else
                    {
                        updateItem.fileName = "";
                        updateTableWidget->item(row, 4)->setText("");
                    }

                }
            }
            catch(CommandTimeoutException e)
            {
                updateTableWidget->item(row, 3)->setText("");
                updateTableWidget->item(row, 4)->setText("");
                e.m_name = updateTableWidget->item(row, 1)->text().toStdString();
                AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
            }
            catch (CommunicationException e)
            {
                updateTableWidget->item(row, 3)->setText("");
                updateTableWidget->item(row, 4)->setText("");
                e.m_name = updateTableWidget->item(row, 1)->text().toStdString();
                AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
            }
            catch (std::exception e)
            {
                updateTableWidget->item(row, 3)->setText("");
                updateTableWidget->item(row, 4)->setText("");
                AlarmManager::Instance()->AddExceptionAlarm(e);
            }
        }
        else
        {
            updateTableWidget->item(row, 3)->setText("");
            updateTableWidget->item(row, 4)->setText("");
        }
    }


    DscpAddress masterAddr = m_items[1].addr;
    masterAddr.a3 = 0;
    masterAddr.a4 = 0;
    updateTableWidget->item(0, 2)->setText(masterAddr.ToString().c_str());

    String version;
    luaEngine->GetLuaValue(state, "setting.version.system", version);
    updateTableWidget->item(0, 3)->setText(version.c_str());

    String mainBoardType;
    luaEngine->GetLuaValue(state, "config.info.mainBoard.type", mainBoardType);

    QStringList toolPathlist = allPathlist.filter("Tool.sh");

    if (toolPathlist.size() > 0)
    {
        m_toolFileName = toolPathlist.at(0).toStdString();
    }
    else
    {
        m_toolFileName = "";
    }

    mainBoardType = "_" + mainBoardType + "_";
    QStringList pathlist = upgPathlist.filter(mainBoardType.c_str());
    if (pathlist.size() > 0)
    {
        m_masterFileName = pathlist.at(0).toStdString();
        String version = Uncompress::ParserUnzipVersion(m_masterFileName);
        if (m_toolFileName.empty())
        {
            updateTableWidget->item(0, 4)->setText(version.c_str());
        }
        else
        {
            updateTableWidget->item(0, 4)->setText(QString("T ") + version.c_str());
        }
    }
    else
    {
        m_masterFileName = "";
        if (m_toolFileName.empty())
        {
            updateTableWidget->item(0, 4)->setText("");
        }
        else
        {
            updateTableWidget->item(0, 4)->setText("T ");
        }
    }

#ifdef    _CS_ARM_LINUX
    FileDetecter fileDetect;    //U盘检测
    bool useNFS = NFSManager::Instance()->IsNFSEnable();
    if (useNFS && NFSManager::Instance()->IsConnected())
    {
        SourceLabel->setText(tr("升级包来源 ： 远程文件"));
    }
    else if (!useNFS && fileDetect.UdiskDetect())
    {
        SourceLabel->setText(tr("升级包来源 ： U盘"));
    }
    else
    {
        SourceLabel->setText(tr("升级包来源 ： 本地"));
    }

#endif
}

bool UpdateWidget::UpdatePacketCheck()
{
    bool ret = true;
    QString result;
    WidgetUpdate();
    for (map<Uint8, QCheckBox *>::iterator iter = m_checkBox.begin(); iter != m_checkBox.end(); ++iter)
    {
        Uint8 row = iter->first;
        QCheckBox *checkBox = iter->second;
        if (checkBox->checkState() == Qt::Checked && (updateTableWidget->item(row, 4)->text().size() < 1))
        {
            ret = false;
            result += tr("缺少") + updateTableWidget->item(row, 1)->text() + tr("升级文件\n");
        }
    }
    if (false == ret)
    {
        MessageDialog msg(result, this);
        msg.exec();
    }
    return ret;
}

void UpdateWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        for (map<Uint8, UpdateItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;

            updateTableWidget->setRowHidden(row, false);
        }
    }
    else if(LoginDialog::userType == Administrator)
    {
        for (map<Uint8, UpdateItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            UpdateItem &updateItem = iter->second;
            if (Administrator >= updateItem.readPrivilege)
            {
                updateTableWidget->setRowHidden(row, false);
            }
            else
            {
                updateTableWidget->setRowHidden(row, true);
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        for (map<Uint8, UpdateItem>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            Uint8 row = iter->first;
            UpdateItem &updateItem = iter->second;
            if (Maintain >= updateItem.readPrivilege)
            {
                updateTableWidget->setRowHidden(row, false);
            }
            else
            {
                updateTableWidget->setRowHidden(row, true);
            }
        }
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(updateTableWidget, m_tableInfo);
}
UpdateWidget::~UpdateWidget()
{
    if (updateTableWidget)
        delete updateTableWidget;
}

void UpdateWidget::OnResult(QString result)
{
    emit UpdaterResultSignals(result);
}

void UpdateWidget::UpdaterResultSlots(QString result)
{
    RefreshButtonSlot();
    MessageDialog msg(result, this);
    msg.exec();
}
}
