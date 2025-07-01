#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "Log.h"
#include "FactoryTime.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/System/ScreenCalibrate.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "System/Types.h"
#include "Setting/SettingManager.h"
#include "Setting/Environment.h"
#include "FlowManager/MeasureScheduler.h"
#include "System/Uncompress.h"
#include "System/AuthorizationManager.h"
#include "System/DynamicPassword.h"
#include "System/CopyFile.h"
#include "UI/Data/TextfileParser.h"
#include "Lua/App.h"
#include "UI/ControlConfig.h"
#include "checkscreen.h"
#include "DebugModeDialog.h"
#include "DataBasePlugin/DataBaseManager.h"
#include "System/ProLicenseManager.h"

#define LOGPWD  ("LuipSuper@2018")
#define SysLogFileName  ("log.txt")
#define AuditTrailTableFileName  ("AuditTrailTable.csv")

using namespace OOLUA;
using namespace Lua;
using namespace System;
using namespace Configuration;
using namespace Flow;
using namespace DataBaseSpace;

namespace UI
{

FactoryTimeWidget::FactoryTimeWidget(QWidget *parent):
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this); //向窗口更新器注册

    QFont font;                           //字体
    font.setPointSize(16);                //字体大小
    font.setWeight(60);

    int hj212Type = 0;
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.hj212Type", hj212Type);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("FactoryTimeWidget::Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("FactoryTimeWidget::Init() => %s", e.what());
    }

    this->resize(650,400);

    defaultPassword = new QPushButton();
    defaultFactory = new QPushButton();
    screenCalibrate = new QPushButton();
    activation = new QPushButton();
    clearData = new QPushButton();
    auditTrailLogExport = new QPushButton();
    clearFault = new QPushButton();
    sysLogExport = new QPushButton();
    sysLogClear = new QPushButton();
    reagentAuthorizationClear = new QPushButton();
    proLicenseActive = new QPushButton();

    defaultPassword->setObjectName("brownButton");
    defaultPassword->setText(tr("一键恢复默认密码"));
    defaultPassword->setFixedSize(250,50);

    defaultFactory->setObjectName("brownButton");
    defaultFactory->setText(tr("一键恢复出厂设置"));
    defaultFactory->setFixedSize(250,50);

    screenCalibrate->setObjectName("brownButton");
    screenCalibrate->setText(tr("触摸屏校准"));
    screenCalibrate->setFixedSize(250,50);

    clearFault->setObjectName("brownButton");
    clearFault->setText(tr("故障清除"));
    clearFault->setFixedSize(250,50);

    auditTrailLogExport->setObjectName("brownButton");
    auditTrailLogExport->setText(tr("审计日志导出"));
    auditTrailLogExport->setFixedSize(250,50);

    clearData->setObjectName("brownButton");
    clearData->setText(tr("一键清除数据"));
    clearData->setFixedSize(250,50);
    clearData->hide();

    sysLogExport->setObjectName("brownButton");
    sysLogExport->setText(tr("系统日志导出"));
    sysLogExport->setFixedSize(250,50);
    sysLogExport->hide();

    sysLogClear->setObjectName("brownButton");
    sysLogClear->setText(tr("系统日志清除"));
    sysLogClear->setFixedSize(250,50);
    sysLogClear->hide();

    reagentAuthorizationClear->setObjectName("brownButton");
    reagentAuthorizationClear->setText(tr("移除试剂授权功能"));
    reagentAuthorizationClear->setFixedSize(250,50);
    reagentAuthorizationClear->hide();

    activation->setObjectName("brownButton");
    activation->setText(tr("出厂激活"));
    activation->setFixedSize(250,50);
    activation->hide();

    proLicenseActive->setObjectName("brownButton");
    proLicenseActive->setText(tr("Pro版本激活"));
    proLicenseActive->setFixedSize(250,50);
    proLicenseActive->hide();

    QVBoxLayout *buttonLayout1 = new QVBoxLayout();
    buttonLayout1->setSpacing(20);
    buttonLayout1->addWidget(defaultFactory);
    buttonLayout1->setSpacing(20);
    if (false == CheckScreen::Instance()->isVNC())
    {
        buttonLayout1->addWidget(screenCalibrate);
        buttonLayout1->setSpacing(20);
    }
    buttonLayout1->addWidget(clearFault);
    buttonLayout1->setSpacing(20);
    buttonLayout1->addWidget(sysLogExport);
    buttonLayout1->setSpacing(20);
    buttonLayout1->addWidget(auditTrailLogExport);
    buttonLayout1->setSpacing(20);
    buttonLayout1->addWidget(proLicenseActive);
    buttonLayout1->addStretch();

    QVBoxLayout *buttonLayout2 = new QVBoxLayout();
    buttonLayout2->setSpacing(20);
    buttonLayout2->addWidget(defaultPassword);
    buttonLayout2->setSpacing(20);
    buttonLayout2->addWidget(clearData);
    buttonLayout2->setSpacing(20);
    buttonLayout2->addWidget(sysLogClear);
    buttonLayout2->setSpacing(20);
    buttonLayout2->addWidget(reagentAuthorizationClear);
    buttonLayout2->setSpacing(20);
    buttonLayout2->addWidget(activation);
    buttonLayout2->addSpacing(10);
    buttonLayout2->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addLayout(buttonLayout1);
    buttonLayout->setSpacing(20);
    buttonLayout->addLayout(buttonLayout2);
    buttonLayout->addStretch();

    defaultPassword->setFont(font);
    defaultFactory->setFont(font);
    screenCalibrate->setFont(font);
    clearData->setFont(font);
    activation->setFont(font);
    clearFault->setFont(font);
    auditTrailLogExport->setFont(font);
    sysLogExport->setFont(font);
    sysLogClear->setFont(font);
    reagentAuthorizationClear->setFont(font);
    proLicenseActive->setFont(font);

    reagentLabel = new ClickableLabel();
    reagentLabel->setFixedSize(40, 40);
    reagentLabel->setPixmap(QPixmap(":/img/reagent"));
    reagentLabel->setScaledContents(true);

    debugLabel = new ClickableLabel();
    debugLabel->setFixedSize(40, 40);
    debugLabel->setPixmap(QPixmap(":/img/img/debug.png"));
    debugLabel->setScaledContents(true);
    debugLabel->hide();

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addLayout(buttonLayout);
    leftLayout->addStretch();
    leftLayout->setContentsMargins(0,0,0,0);

    QVBoxLayout *reagentLayout = new QVBoxLayout();
    reagentLayout->addStretch();
    reagentLayout->addWidget(debugLabel);
    reagentLayout->addStretch();
    reagentLayout->addWidget(reagentLabel);
    reagentLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *maintainerLayout = new QHBoxLayout();
    maintainerLayout->addLayout(leftLayout);
    maintainerLayout->addStretch();
    maintainerLayout->addLayout(reagentLayout);
    maintainerLayout->setContentsMargins(30,10,0,30);
    this->setLayout(maintainerLayout);

    // 二次授权登录窗口
    m_authorizationDlg = new ReagentAuthorizationDialog(this);
    m_authorizationDlg->hide();

    // 二次授权配置窗口
    m_authorizationConfigDlg = new ReagentAuthorizationConfigDialog(this);
    m_authorizationConfigDlg->hide();

    // Pro版本激活窗口
    m_proLicenseDlg = new ProLicenseDialog(this);
    m_proLicenseDlg->hide();

    OnUserChange();

    connect(defaultPassword, SIGNAL(clicked(bool)), this, SLOT(SlotDefaultPassword()));
    connect(defaultFactory, SIGNAL(clicked(bool)), this, SLOT(SlotDefaultFactory()));
    connect(screenCalibrate, SIGNAL(clicked(bool)), this, SLOT(SlotScreenCalibrate()));
    connect(clearData, SIGNAL(clicked(bool)), this, SLOT(SlotClearData()));
    connect(activation, SIGNAL(clicked(bool)), this, SLOT(SlotActivation()));
    connect(clearFault, SIGNAL(clicked(bool)), this, SLOT(SlotClearFault()));
    connect(auditTrailLogExport, SIGNAL(clicked(bool)), this, SLOT(SlotAuditTrailLogExport()));
    connect(sysLogExport, SIGNAL(clicked(bool)), this, SLOT(SlotSysLogExport()));
    connect(sysLogClear, SIGNAL(clicked(bool)), this, SLOT(SlotSysLogClear()));
    connect(reagentAuthorizationClear, SIGNAL(clicked(bool)), this, SLOT(SlotReagentAuthorizationClear()));
    connect(this,SIGNAL(SignalShowReagentAuthorizationDialog()), this, SLOT(SlotShowReagentAuthorizationDialog()), Qt::QueuedConnection);
    connect(reagentLabel, SIGNAL(clicked()), this, SLOT(SlotReagentLabelClicked()));
    connect(debugLabel, SIGNAL(clicked()), this, SLOT(SlotDebugLabelClicked()));
    connect(proLicenseActive, SIGNAL(clicked(bool)), this, SLOT(SlotProLicenseActive()));
}

FactoryTimeWidget::~FactoryTimeWidget()
{

}

void FactoryTimeWidget::SlotClearFault()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Lua_function call(state);

    Lua_func_ref clearFault;
    luaEngine->GetLuaValue(state, "setting.ui.factoryTime.clearFault", clearFault);

    if(clearFault.valid())
    {
        call(clearFault);
        logger->info("清除故障");

        MeasureScheduler::Instance()->Update();
    }
}

void FactoryTimeWidget::SlotDebugLabelClicked()
{
    static int clickTime = 0;
    if(clickTime < 20)
    {
        clickTime++;
    }
    if(clickTime >= 20)
    {
        clickTime = 0;

        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        bool skipFlow = false;
        Table measureTable;
        luaEngine->GetLuaValue(state, "setting.common", measureTable);

        MessageDialog msg(tr("是否进入调试模式？"), this, MsgStyle::OKANDCANCEL, true);
        if(msg.exec()==QDialog::Accepted)
        {
            if (measureTable.safe_at("skipFlow", skipFlow))
            {
                luaEngine->SetLuaValue("setting.common.skipFlow", true);
                logger->debug("**************************** 进入调试模式 ****************************");
                DebugModeDialog dialog;
                dialog.exec();
            }
            else
            {
                MessageDialog msg(tr("该参数未集成此功能"), this, MsgStyle::ONLYOK, true);
                msg.exec();
            }
        }
        else
        {
            if (measureTable.safe_at("skipFlow", skipFlow))
            {
                luaEngine->SetLuaValue("setting.common.skipFlow", false);
                logger->debug("**************************** 退出调试模式 ****************************");
            }
        }
    }
}

void FactoryTimeWidget::SlotProLicenseActive()
{
    if(m_proLicenseDlg->isHidden())
    {
        m_proLicenseDlg->exec();
        if (ProLicenseManager::Instance()->GetProLicense())
        {
            proLicenseActive->hide();
        }
        else
        {
            proLicenseActive->show();
        }
    }
}

void FactoryTimeWidget::SlotReagentLabelClicked()
{
    int clickTime = GetRotalClickTime();
    if(clickTime < 10)
    {
        AddRotalClickTime();
    }
    if(clickTime >= 10)
    {
        ResetRotalClickTime();

        emit SignalShowReagentAuthorizationDialog();
    }
}

void FactoryTimeWidget::SlotShowReagentAuthorizationDialog()
{
    bool isShowConfigDlg = false;
    if (m_authorizationDlg->isHidden() && m_authorizationConfigDlg->isHidden())
    {
        //判断使能
        int nEnable = AuthorizationManager::Instance()->IsReagentAuthorizationEnable();
        if ((int)ReagentAuthorizationEnable::Invalid == nEnable) //功能关闭，直接进入界面
        {
            isShowConfigDlg = true;
        }
        else if ((int)ReagentAuthorizationEnable::Effective == nEnable) //功能打开，试剂授权码界面
        {
            //动态密码
            if (m_authorizationDlg->exec() == QDialog::Accepted)
            {
                isShowConfigDlg = true;
            }
            else
            {
                return ;
            }
        }

        if (isShowConfigDlg)
        {
            m_authorizationConfigDlg->exec();
        }
    }
}

QFileInfoList FactoryTimeWidget::GetSysLogFileList()
{
    QDir dir = QDir(Environment::Instance()->GetAppDataPath().c_str());

    QStringList nameFilters;
    //nameFilters<<"log.txt*";
    nameFilters<<QString(SysLogFileName).append("*");

    QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::Files);

    return fileInfoList;
}


void FactoryTimeWidget::SlotSysLogExport()
{
    QString result;
    CopyFileAction copyFileAction;
    QString strDir = copyFileAction.GetTargetDir().c_str();
    QDir dir(strDir);

    bool isFail = false;
    QString errmsg;
    if (!copyFileAction.ExMemoryDetect(errmsg)) //U盘检测
    {
        MessageDialog msg(errmsg, this);
        msg.exec();
        return;
    }

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State *state = luaEngine->GetThreadState();

    bool isValid = false;
    luaEngine->GetLuaValue(state, "config.producerAuthorization.enable", isValid);

    if(isValid == true)
    {
        if (!copyFileAction.TargetDirCheck(dir)) //拷贝目录检测
        {
            MessageDialog msg(tr("创建目录，日志数据导出失败"), this);
            msg.exec();
            return;
        }

        QString sPath = Environment::Instance()->GetAppDataPath().c_str() + QString("/") +QString(SysLogFileName);
        QString dPath = dir.filePath(SysLogFileName);
        QList<QString> dPathList;

       if(copyFileAction.CopyFileToPath(sPath.toStdString(), dPath.toStdString(), true))
       {
           dPathList.append(dPath);

           for(int index = 1; index <= 10; index++)
           {
               QString logName;

               if(index != 10)
               {
                   logName = QString(SysLogFileName) + ".0" + QString::number(index);
               }
               else
               {
                   logName = QString(SysLogFileName) + "." + QString::number(index);
               }

               sPath = Environment::Instance()->GetAppDataPath().c_str() + QString("/") +QString(logName);
               dPath = dir.filePath(logName);

               QFileInfo fileInfo(sPath);

               if (QFile::exists(sPath)&&fileInfo.isFile())
               {
                   dPathList.append(dPath);

                   if (!copyFileAction.CopyFileToPath(sPath.toStdString(), dPath.toStdString(), true))
                   {
                       isFail = true;
                   }
               }
           }
       }
       else
       {
           isFail = true;
       }

       if (!isFail)
       {
           for(QList<QString> ::iterator it = dPathList.begin(); it != dPathList.end(); it++)
           {
               if (!QFile::exists((*it)))
               {
                   logger->info("%s 系统日志导出失败", App::GetCurrentRoleStr().c_str());
                   MessageDialog msg(tr("系统日志导出失败"), this);
                   msg.exec();
                   return;
               }
           }
           result = tr("系统日志导出成功");
           logger->info("%s 系统日志导出成功", App::GetCurrentRoleStr().c_str());
       }
       else
       {
           result = tr("系统日志导出失败");
           logger->info("%s 系统日志导出失败", App::GetCurrentRoleStr().c_str());
       }
    }
    else
    {
        std::list<String> logPathList;

        QFileInfoList logFileList = this->GetSysLogFileList();
        if(!logFileList.isEmpty())
        {
            foreach (QFileInfo file, logFileList)
            {
                QString logPath = file.absoluteFilePath();
                logPathList.push_back(logPath.toStdString());
            }
        }

        String zipPath = strDir.toStdString() + ".upg";

        if(Uncompress::CompressFileList(logPathList, zipPath, LOGPWD))
        {
            result = tr("系统日志导出成功");
            logger->info("%s 系统日志导出成功", App::GetCurrentRoleStr().c_str());
        }
        else
        {
            result = tr("系统日志导出失败");
            logger->info("%s 系统日志导出失败", App::GetCurrentRoleStr().c_str());
        }
    }
    MessageDialog msg(result, this);
    msg.exec();

#ifdef    _CS_ARM_LINUX
    system("sync");
#endif

    return;
}

void FactoryTimeWidget::SlotSysLogClear()
{
    MessageDialog msg(tr("是否清除系统日志？"), this, MsgStyle::OKANDCANCEL, true);
    if(msg.exec()==QDialog::Rejected)
    {
        return;
    }

    QFileInfoList logFileList = this->GetSysLogFileList();
    if(!logFileList.isEmpty())
    {
        foreach (QFileInfo file, logFileList)
        {
            if (file.isFile() && file.fileName() != SysLogFileName)
            {
                file.dir().remove(file.fileName());
            }
        }
    }
    logFileList.clear();

    std::unique_ptr<TextfileParser> logFile;
    logFile.reset(new TextfileParser(QString(Environment::Instance()->GetAppDataPath().c_str()) + QString("/") + SysLogFileName));

    logFile->ClearRecord();
}

void FactoryTimeWidget::SlotReagentAuthorizationClear()
{
    MessageDialog msg(tr("是否移除试剂授权功能?"), this, MsgStyle::OKANDCANCEL, true);
    if(msg.exec()==QDialog::Rejected)
    {
        return;
    }

    AuthorizationManager::Instance()->SetReagentAuthorization(0);

    logger->info("%s 移除试剂授权功能", App::GetCurrentRoleStr().c_str());
}

//QFileInfoList FactoryTimeWidget::GetHj212LogFileList()
//{
//    QDir dir = QDir(Environment::Instance()->GetAppDataPath().c_str());

//    QStringList nameFilters;
//    nameFilters << QString(Hj212LogFileName).append("*");

//    QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::Files);

//    return fileInfoList;
//}

void FactoryTimeWidget::SlotClearData()
{
    MessageDialog msg(tr("是否一键清除数据？"), this, MsgStyle::OKANDCANCEL, true);
    if(msg.exec()==QDialog::Rejected)
    {
        return;
    }

    logger->info("%s 一键清除所有数据", App::GetCurrentRoleStr().c_str());

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Lua_function call(state);

    Lua_func_ref clearData;
    luaEngine->GetLuaValue(state, "setting.ui.factoryTime.clearData", clearData);

    if(clearData.valid())
    {
        call(clearData);
    }

    this->SlotSysLogClear();

    UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::OneKeyClearData, "FactoryTime");
}

void FactoryTimeWidget::SlotDefaultFactory()
{
    MessageDialog msg(tr("是否恢复出厂设置？\n"), this, MsgStyle::OKANDCANCEL);
    if(msg.exec()==QDialog::Rejected)
    {
        return;
    }

    logger->info("%s 一键恢复出厂设置", App::GetCurrentRoleStr().c_str());
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    OOLUA::Script* lua = luaEngine->GetEngine();
    Lua_function call(state);

    Lua_func_ref defaultRestore;
    luaEngine->GetLuaValue(state, "setting.ui.factoryTime.defaultRestore", defaultRestore);

    call(defaultRestore, LoginDialog::userType);

    bool isRestart = false;
    lua->pull(isRestart);

    UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::ChangeConfigParam);

    if(isRestart == true)
    {
        QString text = tr("关键参数更改，重启仪器后生效，是否立即重启？\n");
        MessageDialog msg(text, 10, this, MsgStyle::ONLYCANCEL);
        if(msg.exec()==QDialog::Rejected)
        {
            return;
        }
    }
}

void FactoryTimeWidget::showEvent(QShowEvent *event)
{
    (void)event;
    if (LoginDialog::userType == RoleType::Super)
    {
        if (SettingManager::Instance()->IsProducerStatus())
        {
            activation->show();
        }
        else
        {
            activation->hide();
        }
        defaultPassword->show();
        clearData->show();
        sysLogExport->show();
        sysLogClear->show();
        reagentAuthorizationClear->show();
        if (ProLicenseManager::Instance()->GetProLicense())
        {
            proLicenseActive->hide();
        }
        else
        {
            proLicenseActive->show();
        }
    }
    else if(LoginDialog::userType == RoleType::Administrator)
    {
        defaultPassword->hide();
        activation->hide();
        clearData->hide();
        sysLogExport->show();
        sysLogClear->hide();
        reagentAuthorizationClear->hide();
        proLicenseActive->hide();
    }
    else
    {
        defaultPassword->hide();
        activation->hide();
        clearData->hide();
        sysLogExport->hide();
        sysLogClear->hide();
        reagentAuthorizationClear->hide();
        proLicenseActive->hide();
    }

    ResetRotalClickTime();
}

void FactoryTimeWidget::SlotDefaultPassword()
{
    MessageDialog msg(tr("是否恢复默认密码？\n"), this, MsgStyle::OKANDCANCEL);
    if(msg.exec()==QDialog::Rejected)
    {
        return;
    }

    logger->info("%s 恢复默认密码", App::GetCurrentRoleStr().c_str());

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Lua_function call(state);

    Lua_func_ref LoadAuthorization;
    Table defaultAuthorization;
    luaEngine->GetLuaValue(state, "ConfigLists.LoadAuthorization", LoadAuthorization);
    call(LoadAuthorization, true);
    OOLUA::pull(state, defaultAuthorization);

    Table authorization;
    luaEngine->GetLuaValue(state, "config.authorization", authorization);

    String normalUserPassword;
    defaultAuthorization.at("normalUserPassword", normalUserPassword);
    authorization.set("normalUserPassword", normalUserPassword);

    String maintenancePassword;
    defaultAuthorization.at("maintenancePassword", maintenancePassword);
    authorization.set("maintenancePassword", maintenancePassword);

    String administratorPassword;
    defaultAuthorization.at("administratorPassword", administratorPassword);
    authorization.set("administratorPassword", administratorPassword);

    String superPassword;
    defaultAuthorization.at("superPassword", superPassword);
    authorization.set("superPassword", superPassword);

    Lua_func_ref SaveAuthorization;
    luaEngine->GetLuaValue(state, "ConfigLists.SaveAuthorization", SaveAuthorization);
    call(SaveAuthorization);

    QString text = tr("密码已重置！");
    MessageDialog msgRet(text, 10, this, MsgStyle::ONLYOK);
    msgRet.exec();
}

void FactoryTimeWidget::OnUserChange()
{
    if (LoginDialog::userType == RoleType::Super)
    {
        if (SettingManager::Instance()->IsProducerStatus())
        {
            activation->show();
        }
        else
        {
            activation->hide();
        }
        clearData->show();
        defaultPassword->show();
        sysLogExport->show();
        sysLogClear->show();
        reagentAuthorizationClear->show();
        auditTrailLogExport->show();

        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State* state = luaEngine->GetThreadState();
        bool isAuthorization = false;
        luaEngine->GetLuaValue(state, "config.producerAuthorization.enable", isAuthorization);

        if (isAuthorization)
        {
            debugLabel->show();
        }
    }
    else if(LoginDialog::userType == RoleType::Administrator)
    {
        auditTrailLogExport->hide();
        defaultPassword->hide();
        activation->hide();
        clearData->hide();
        sysLogExport->show();
        sysLogClear->hide();
        reagentAuthorizationClear->hide();
        debugLabel->hide();
    }
    else
    {
        auditTrailLogExport->hide();
        defaultPassword->hide();
        activation->hide();
        clearData->hide();
        sysLogExport->hide();
        sysLogClear->hide();
        reagentAuthorizationClear->hide();
        debugLabel->hide();
    }
}

void FactoryTimeWidget::SlotScreenCalibrate()
{
    logger->info("%s 触摸屏校准", App::GetCurrentRoleStr().c_str());
    ScreenCalibrate screenCalibrate(this);
}

void FactoryTimeWidget::SlotAuditTrailLogExport()
{
    QString result = "";
    CopyFileAction copyFileAction;
    QString strDir = copyFileAction.GetTargetDir().c_str();
    QDir dir(strDir);

    QString sPath = Environment::Instance()->GetAppDataPath().c_str() + QString("/") + QString(AuditTrailTableFileName);
    QString dPath = dir.filePath(AuditTrailTableFileName);

    qDebug() << dPath;

    bool isFail = false;
    QString errmsg;
    if (!copyFileAction.ExMemoryDetect(errmsg)) //U盘检测
    {
        MessageDialog msg(errmsg, this);
        msg.exec();
        return;
    }

    if (!copyFileAction.TargetDirCheck(dir)) //拷贝目录检测
    {
        MessageDialog msg(tr("创建目录，审计日志导出失败"), this);
        msg.exec();
        return;
    }

    if  (DataBaseManager::Instance()->GetAuditTrailTable()->ExportTableToCsv(dPath))
    {
        result = tr("审计日志导出成功");
        logger->info("审计日志导出成功");
    }
    else
    {
        result = tr("审计日志导出失败");
        logger->info("审计日志导出失败");
    }

    MessageDialog msg(result, this);
    msg.exec();

#ifdef    _CS_ARM_LINUX
    system("sync");
#endif

    return;
}


void FactoryTimeWidget::SlotActivation()
{
    MessageDialog msg(tr("是否激活出厂模式？\n"), this, MsgStyle::OKANDCANCEL);
    if(msg.exec()==QDialog::Rejected)
    {
        return;
    }

    logger->info("激活出厂模式");

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Lua_function call(state);

    Lua_func_ref SaveEncryptionFile;
    luaEngine->GetLuaValue(state, "Serialization.SaveEncryptionFile", SaveEncryptionFile);

    Table producerAuthorization;
    luaEngine->GetLuaValue(state, "config.producerAuthorization", producerAuthorization);

    producerAuthorization.set("enable", false);

    String path = Configuration::Environment::Instance()->GetAppPath() + "/SP/Config/ProducerAuthorization.ls";
    call(SaveEncryptionFile, producerAuthorization, path, "config.producerAuthorization");

    activation->hide();
}

void FactoryTimeWidget::OnUpdateWidget(UpdateEvent event, System::String message)
{
    if (event == UpdateEvent::RecoverInFactoryMode && message == "InstrumentAgentHandler")
    {
        activation->show();
    }
    else if (event == UpdateEvent::ActivateOutFactoryMode && message == "InstrumentAgentHandler")
    {
        activation->hide();
    }
    else if (event == UpdateEvent::FaultClear && message == "InstrumentAgentHandler")
    {
        emit this->clearFault->click();
    }

    this->OnUserChange();
}

void FactoryTimeWidget::ResetRotalClickTime()
{
    m_totalClick = 0;
}

void FactoryTimeWidget::AddRotalClickTime()
{
    m_totalClick++;
}

int FactoryTimeWidget::GetRotalClickTime()
{
    return m_totalClick;
}

}

