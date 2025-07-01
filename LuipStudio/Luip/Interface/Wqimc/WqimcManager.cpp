
#include <iostream>
#include <sstream>
#include <map>
#include <QUuid>
#include "LuaEngine/LuaEngine.h"
#include "Communication/CommunicationException.h"
#include "Controller/ControllerManager.h"
#include "CloudConnectionService.h"
#include "LuaException.h"
#include "Log.h"
#include "oolua.h"
#include "WqimcManager.h"
#include "UI/System/DeviceInformation.h"
#include "SignalManager/SignalManager.h"
#include <float.h>
#include <QApplication>
#include <QDateTime>
#include "Setting/SettingManager.h"
#include "StatusManager/StatusManager.h"
#include "System/SetSystemTime.h"
#include "System/Uncompress.h"
#include "FlowManager/FlowManager.h"
#include "UpdateManager/MasterUpdate.h"
#include "FlowManager/UpdateFlow.h"
#include "System/PlatformInfo.h"
#include "Setting/Environment.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "WqimcManager.h"
#include "UI/Setting/ParameterWidget.h"

#ifdef  _CS_ARM_LINUX
#include <stdio.h>
#include <stdlib.h>
#define NETWORKNAME "eth0"
#else
#define NETWORKNAME "ethernet"
#endif

#define UPLOADEXCEPTIONMAXCNT 10

using namespace std;
using namespace OOLUA;
using namespace Controller;
using namespace Communication;
using namespace Measure;
using namespace Lua;
using namespace Interface::Wqimc::Api::Manager;
using namespace Result;
using namespace Configuration;
using namespace Flow;

//Q_DECLARE_METATYPE(System::String)

namespace Interface
{
namespace Wqimc
{

WqimcManager::WqimcManager()
    :m_isConnectedFlag(false),
     m_isUpgrading(false),
     m_upgradeFile(nullptr),
     m_isUploading(false),
     m_isDownloading(false),
     m_downloadFile(nullptr),
     m_isCalibration(false),
     m_UploadExceptionCnt(0),
     m_isEnableConnet(false),
     m_isValidSN(false),
     m_cableConnected(false),
     m_connectStep(ConnectStep::InitConnect)
{
    m_newState = LuaEngine::Instance()->CreateThread();
    workThread = new QThread();
    this->moveToThread(workThread);
    workThread->start();
    m_timer = new QTimer();
    m_timer->moveToThread(workThread);

    m_reconnectTimer = new QTimer();
    m_reconnectTimer->moveToThread(workThread);

    qRegisterMetaType<std::vector<Interface::Wqimc::Api::Manager::Config>>("std::vector<Interface::Wqimc::Api::Manager::Config>");
    qRegisterMetaType<Interface::Wqimc::Api::Manager::MeasureData>("Interface::Wqimc::Api::Manager::MeasureData");
    qRegisterMetaType<Interface::Wqimc::Api::Manager::MeasureCurve>("Interface::Wqimc::Api::Manager::MeasureCurve");
    qRegisterMetaType<std::vector<Interface::Wqimc::Api::Manager::ReagentRemain>>("std::vector<Interface::Wqimc::Api::Manager::ReagentRemain>");
    qRegisterMetaType<System::String>("System::String");
    qRegisterMetaType<Interface::Wqimc::Api::Manager::Alarm>("Interface::Wqimc::Api::Manager::Alarm");
    qRegisterMetaType<Interface::Wqimc::Api::Manager::Status>("Interface::Wqimc::Api::Manager::Status");
    qRegisterMetaType<Interface::Wqimc::Api::Manager::DiagnosisResult>("Interface::Wqimc::Api::Manager::DiagnosisResult");
    qRegisterMetaType<Interface::Wqimc::Api::Agent::Config>("Interface::Wqimc::Api::Agent::Config");

    connect(this, SIGNAL(UpdateConfigsSignals(std::vector<Interface::Wqimc::Api::Manager::Config>)), this, SLOT(UpdateConfigsSlots(std::vector<Interface::Wqimc::Api::Manager::Config>)));
    connect(this, SIGNAL(UploadMeasureDataSignals(Interface::Wqimc::Api::Manager::MeasureData)), this, SLOT(UploadMeasureDataSlots(Interface::Wqimc::Api::Manager::MeasureData)));
    connect(this, SIGNAL(UploadMeasureCurveSignals(Interface::Wqimc::Api::Manager::MeasureCurve)), this, SLOT(UploadMeasureCurveSlots(Interface::Wqimc::Api::Manager::MeasureCurve)));
    connect(this, SIGNAL(UploadReagentProfileSignals()), this, SLOT(UploadReagentProfileSlots()));
    connect(this, SIGNAL(UpdateReagentRemainSignals(std::vector<Interface::Wqimc::Api::Manager::ReagentRemain>)),
            this, SLOT(UpdateReagentRemainSlots(std::vector<Interface::Wqimc::Api::Manager::ReagentRemain>)));
    connect(this, SIGNAL(UploadConsumableProfileSignals()), this, SLOT(UploadConsumableProfileSlots()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(UpdaterSignalTimerSlots()));
    connect(this, SIGNAL(SetSignalUpdateCycleSignals(double)), this, SLOT(SetSignalUpdateCycleSlots(double)));
    connect(this, SIGNAL(RebootSignals()), this, SLOT(RebootSlots()));
    connect(this, SIGNAL(UpgradeSignals(System::String)), this, SLOT(UpgradeSlots(System::String)));
    connect(this, SIGNAL(AlarmSignals(Interface::Wqimc::Api::Manager::Alarm)),
            this, SLOT(AlarmSlots(Interface::Wqimc::Api::Manager::Alarm)));
    connect(this, SIGNAL(UpdateStatusSignals(Interface::Wqimc::Api::Manager::Status)),
            this, SLOT(UpdateStatusSlots(Interface::Wqimc::Api::Manager::Status)));
    connect(this, SIGNAL(UpdateActionSignals(Interface::Wqimc::Api::Manager::Status)),
            this, SLOT(UpdateActionSlots(Interface::Wqimc::Api::Manager::Status)));
    connect(this, SIGNAL(NotifyDiagnosisResultSignals(const Interface::Wqimc::Api::Manager::DiagnosisResult)),
            this, SLOT(NotifyDiagnosisResultSlots(const Interface::Wqimc::Api::Manager::DiagnosisResult)));
    connect(this, SIGNAL(ChangeHardwareParamSignals(const Interface::Wqimc::Api::Agent::Config)),
            this, SLOT(ChangeHardwareParamSlot(const Interface::Wqimc::Api::Agent::Config)));

    connect(this, SIGNAL(CalibrationTouchScreenSignals()), this, SLOT(CalibrationTouchScreenSlot()));
    connect(this, SIGNAL(UpLoadFileSignal()), this, SLOT(UpLoadFileSlot()));
    connect(this, SIGNAL(DownloadFileSignal()), this, SLOT(DownloadFileSlot()));
    connect(this, SIGNAL(InitSignal()), this, SLOT(InitSlot()));
    connect(m_reconnectTimer, SIGNAL(timeout()), this, SLOT(ConnectTimerSlot()));
    connect(this, SIGNAL(DisconnectSignal()), this, SLOT(DisconnectSlot()));
}

unique_ptr<WqimcManager> WqimcManager::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 WqimcManager 对象（单件）。
 */
WqimcManager* WqimcManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new WqimcManager);
    }

    return m_instance.get();
}

bool WqimcManager::Init()
{
    bool ret = false;
    ChangeWqimcStatus(WqimcStatus::Disconnected);

    Measure::StatusManager::Instance()->Register(this);

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State *  state = LuaEngine::Instance()->GetThreadState();

    try
    {
        Table fieldTable;
        luaEngine->GetLuaValue(state, "setting.ui.measureData", fieldTable);
        oolua_ipairs(fieldTable)
        {
            Table table;
            String name;

            OOLUA::pull(state, table);
            table.at("name", name);
            if (name == "consistency")
            {
                table.at("format", measureRusultFormat);
            }
        }
        oolua_ipairs_end()
        luaEngine->GetLuaValue(state, "setting.ui.calibrateCurve", fieldTable);
        oolua_ipairs(fieldTable)
        {
            Table table;
            String name;

            OOLUA::pull(state, table);
            table.at("name", name);
            if (name == "curve")
            {
                table.at("format", curveFormat);
            }
        }
        oolua_ipairs_end()

        emit InitSignal();
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("WqimcManager::Init() => %s",e.what());
    }
    catch (exception e)
    {
        logger->warn("WqimcManager::Init() => Exception %s", e.what());
    }

    return ret;
}

void WqimcManager::InitSlot()
{
    logger->debug("WqimcManager Thread ID = %d", QThread::currentThreadId());

    httpClient = new HttpClient();
    httpClient->moveToThread(workThread);

    connect(httpClient, SIGNAL(PostMethodFinishedSignal()), this, SLOT(UpLoadFinishedSlot()));
    connect(httpClient, SIGNAL(PostMethodErrorSignal(QString)), this, SLOT(UpLoadErrorSlot(QString)));

    LuaEngine::Instance()->AttachCurrentThread(m_newState);

    this->CheckEnableConnet();
    this->CheckCableStatus();
    this->CheckSNValid();

    if (m_isEnableConnet)
    {
        this->EnableDisconnectNotify(false);
        m_reconnectTimer->start(10);
    }
}

void WqimcManager::CheckEnableConnet()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State *  state = LuaEngine::Instance()->GetThreadState();

    //检测是否打开云服务开关
    bool serverMode = false;

    try
    {
        luaEngine->GetLuaValue(state, "config.system.serverMode", serverMode);

        if (serverMode)
        {
            m_isEnableConnet = true;
        }
        else
        {
            logger->debug("WQIMC function not open!!!");
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("WqimcManager::IsEnableConnet() => [OOLUA::Exception] %s",e.what());
    }
    catch (exception e)
    {
        logger->warn("WqimcManager::IsEnableConnet() => [std::exception]Exception %s", e.what());
    }
}

void WqimcManager::CheckSNValid()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State *  state = LuaEngine::Instance()->GetThreadState();

    try
    {
        //检测SN是否为全0
        String sn;
        luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);

        bool isValid = false;
        int snSize = (int)sn.size();

        for (int i = 1; i < 9; i++) //取SN后8位数字，判断是否全都是0。
        {
            if (sn[snSize - i] != '0')
            {
               isValid = true;
               break;
            }
        }

        if (isValid)
        {
            m_isValidSN = true;
        }
        else
        {
            m_isValidSN = false;
            logger->debug("The SN is 0!!!");
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("WqimcManager::IsValidSN() => [OOLUA::Exception] %s",e.what());
    }
    catch (exception e)
    {
        logger->warn("WqimcManager::IsValidSN() => [std::exception]Exception %s", e.what());
    }
}

void WqimcManager::CheckCableStatus()
{
    bool ret = false;

    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();

    for(QList<QNetworkInterface>::iterator it = list.begin(); it != list.end(); it++)
    {
        QString netName =(*it).name();

        #ifdef  _CS_ARM_LINUX
        if(netName == NETWORKNAME)
        {
            ret = (*it).flags().testFlag(QNetworkInterface::IsRunning);
            break;
        }
        #else
            QStringList list = netName.split("_");//QString字符串分割函数

            if(list.at(0) == NETWORKNAME)
            {
                ret = (*it).flags().testFlag(QNetworkInterface::IsRunning);

                if (ret)
                {
                   break;
                }
            }

        #endif
    }

    if (ret)
    {
        m_cableConnected = true;
    }
    else
    {
        m_cableConnected = false;
    }
}

void WqimcManager::SetCableStatus(bool status)
{
    m_cableConnected = status;
}

void WqimcManager::ChangeSN()
{
    this->CheckSNValid();
}

bool WqimcManager::GetConnectedFlag()
{
    std::unique_lock<std::mutex> lock(m_connectLock);

    return m_isConnectedFlag;
}

void WqimcManager::SetConnectedFlag(bool flag)
{
    std::unique_lock<std::mutex> lock(m_connectLock);

    if (flag)
    {
        ChangeWqimcStatus(WqimcStatus::Connected);
        m_isConnectedFlag = true;
    }
    else
    {
        ChangeWqimcStatus(WqimcStatus::Disconnected);
        m_isConnectedFlag = false;
    }
}

void WqimcManager::ConnectTimerSlot()
{
    m_reconnectTimer->stop();

    switch ((int)m_connectStep)
    {
        case (int)ConnectStep::InitConnect://初始连接
        {
//            logger->debug("WqimcManager::ReconnectTimerSlot() => @@@@@@@ [ InitConnect ]");

            this->CheckCableStatus();   //检查网线是否连接

            if (m_isEnableConnet && m_isValidSN && m_cableConnected)
            {
//                logger->debug("WqimcManager::ReconnectTimerSlot() => @@@@@@@ Initial Connect Service");
                bool ret = this->InitConnectService();
                if (ret)
                {
                    m_connectStep = ConnectStep::CheckConnectStatus;
                }
                else
                {
                    m_connectStep = ConnectStep::ReConnect;
                }
            }
        }
            break;

        case (int)ConnectStep::ReConnect://掉线重连
        {
//            logger->debug("WqimcManager::ReconnectTimerSlot() => @@@@@@@  [ ReConnect ]");

            this->CheckCableStatus();   //检查网线是否连接

            if (m_cableConnected)
            {
                bool ret = this->ReconnectService();
                if (ret)
                {
                    m_connectStep = ConnectStep::CheckConnectStatus;
                }
            }
//            else
//            {
//                logger->debug("WqimcManager::ReconnectTimerSlot() => @@@@@@@ Cable is not connected!!!");
//            }
        }
            break;

        case (int)ConnectStep::CheckConnectStatus: //检查连接
        {
//            logger->debug("WqimcManager::ReconnectTimerSlot() => @@@@@@@ [ CheckConnectStatus ]");

            m_connectStep = ConnectStep::ReConnect;

//            if (this->upload())               //上传UUID及仪器的信息，
//            {
                this->upload();                 //上传UUID及仪器的信息
                if (this->UploadConnectInfo())  //检测上次连接是否成功
                {
                    m_connectStep = ConnectStep::Nomal;
                    this->EnableDisconnectNotify(true);
                    this->SetConnectedFlag(true);
                    ClearExceptioFlag();
                    logger->debug("WqimcManager::ReconnectTimerSlot() => @@@@@@@ ConnectStep set to Nomal");
                }
//            }
        }
            break;

        case (int)ConnectStep::Nomal://正常
        {

        }
            break;
        default:
            break;
    }

    // 设置下次定时器超时进入的状态时间
    if (m_connectStep == ConnectStep::CheckConnectStatus)
    {
        m_reconnectTimer->start(5000);
    }
    else
    {
        m_reconnectTimer->start(60000);
    }
}

bool WqimcManager::InitConnectService()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State *  state = LuaEngine::Instance()->GetThreadState();

    try
    {
        //初始TCP连接，启动服务线程
        CloudConnectionService* service = CloudConnectionService::Instance();

        String serverIP;
        int port = 9090;

        luaEngine->GetLuaValue(state, "config.system.serverIP", serverIP);

        luaEngine->GetLuaValue(state, "config.system.serverPort", port);
        service->SetServer(serverIP, port);
        service->SetConnectionNotifier(this);

        return service->Start();
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("WqimcManager::ConnectService() => %s",e.what());
    }
    catch (TTransportException e)
    {
        logger->warn("WqimcManager::InitConnectService() => TTransportException %s", e.what());
    }
    catch (TProtocolException e)
    {
        logger->warn("WqimcManager::InitConnectService() => TProtocolException %s", e.what());
    }
    catch (TException e)
    {
        logger->warn("WqimcManager::InitConnectService() => TException %s", e.what());
    }
    catch (exception e)
    {
        logger->warn("WqimcManager::InitConnectService() => Exception %s", e.what());
    }
}

void WqimcManager::onDisconnected()
{
    std::unique_lock<std::mutex> lock(m_disconnectLock);

    if (m_connectStep == ConnectStep::Nomal)
    {
//        logger->debug("WqimcManager::onDisconnected() => ####### Trigger reconnect Signal");
        emit DisconnectSignal();
    }
}

void WqimcManager::DisconnectSlot()
{
    this->SetConnectedFlag(false);
    CloudConnectionService::Instance()->OnDisconnectStatus();

    if (m_connectStep == ConnectStep::Nomal)
    {
        m_connectStep = ConnectStep::ReConnect;
//        logger->debug("WqimcManager::DisconnectSlot() => $$$$$$$ ConnectStep set to ReConnect");
    }
    else
    {
//        logger->debug("WqimcManager::DisconnectSlot() => $$$$$$$ ConnectStep is %d", (int)m_connectStep);
    }
}

bool WqimcManager::ReconnectService()
{
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        return service->Reconnect();
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("WqimcManager::ReconnectService() => %s",e.what());
    }
    catch (TTransportException e)
    {
        logger->warn("WqimcManager::ReconnectService() => TTransportException %s", e.what());
    }
    catch (TProtocolException e)
    {
        logger->warn("WqimcManager::ReconnectService() => TProtocolException %s", e.what());
    }
    catch (TException e)
    {
        logger->warn("WqimcManager::ReconnectService() => TException %s", e.what());
    }
    catch (exception e)
    {
        logger->warn("WqimcManager::ReconnectService() => Exception %s", e.what());
    }

    return false;
}

bool WqimcManager::UploadConnectInfo()
{
    echo();

    int permissionLevel = (int)UI::RoleType::Super;
    uploadInstrumentPermissionLevel(permissionLevel);
    uploadSettingProfiles();
    UploadReagentProfileSlots();
    UploadConsumableProfileSlots();
    uploadSignalProfile();
    uploadOperations();
    uploadDiagnoses();

    int64_t sendTime = QDateTime::currentMSecsSinceEpoch();
    int64_t time = getSystemTime();
    int64_t receiveTime = QDateTime::currentMSecsSinceEpoch();

    if (time != 0)
    {
        time += (receiveTime - sendTime) / 2;
        QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(time);
        System::SetSystemTime::SetTime(dateTime);
        cout << "cur " << sendTime << std::endl;
        cout << "rev " << receiveTime << endl;
        cout << "time " << time << endl;
    }
    RunStatus status = StatusManager::Instance()->GetStatus();
    Interface::Wqimc::Api::Manager::Status sendStatus;
    sendStatus.__set_time(status.createTime.toMSecsSinceEpoch());
    sendStatus.__set_name(status.name);
    sendStatus.__set_text(status.content);
    sendStatus.__set_duration(status.time);
    UpdateStatusSlots(sendStatus);

    RunAction action = StatusManager::Instance()->GetAction();
    Interface::Wqimc::Api::Manager::Status sendAction;
    sendAction.__set_time(action.createTime.toMSecsSinceEpoch());
    sendAction.__set_name(action.name);
    sendAction.__set_text(action.content);
    sendAction.__set_duration(action.time);

    return UpdateAction(sendAction);
}

void WqimcManager::EnableDisconnectNotify(bool enable)
{
    CloudConnectionService::Instance()->EnableDisconnectNotify(enable);
}

bool WqimcManager::echo()
{
    bool ret = false;

    CloudConnectionService* service = CloudConnectionService::Instance();

    // Echo 测试
    stringstream echoData;
    try
    {
        string reply;

        LuaEngine* luaEngine = LuaEngine::Instance();
        String QUUIDStr;
        luaEngine->GetLuaValue(m_newState, "config.info.instrument.uuid", QUUIDStr);

        QUuid id(QUUIDStr.c_str());

        echoData.str("");
        echoData << "L[" << id.toString().toStdString() << "]";
        service->echo(reply, echoData.str());

        if (reply == (echoData.str() + "-REPLY"))
        {
            ret = true;
            cout << "CALL: echo(" << echoData.str() << ")" << endl;
            cout << "      RET: " << reply << endl;

            logger->debug("[thrift] CALL: echo( << %s << )   RET[%s]",echoData.str().c_str(), reply.c_str());
        }

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::echo()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::echo()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::echo()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::echo()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::echo()", e);
    }

    return ret;
}

bool WqimcManager::uploadInstrumentPermissionLevel(const int32_t level)
{
    bool ret = false;
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        ret = service->uploadInstrumentPermissionLevel(level);
        cout << "uploadInstrumentPermissionLevel " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadInstrumentPermissionLevel()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::uploadInstrumentPermissionLevel()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::uploadInstrumentPermissionLevel()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::uploadInstrumentPermissionLevel()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadInstrumentPermissionLevel()", e);
    }
    return ret;
}

bool WqimcManager::upload()
{
    bool status = false;
    CloudConnectionService* service = CloudConnectionService::Instance();
    // 仪器注册
    Instrument ins;
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();

        luaEngine->GetLuaValue(m_newState, "setting.instrument.name", ins.name);
        luaEngine->GetLuaValue(m_newState, "setting.version.system", ins.version);

        Table argus;
        luaEngine->GetLuaValue(m_newState, "setting.instrument.argus", argus);
        int paramType = SettingManager::Instance()->GetParamType();
        int mask = 1;
        oolua_ipairs(argus)
        {
            String ret;
            OOLUA::pull(m_newState, ret);
            if (mask & paramType)
            {
                ins.targets.push_back(ret);
            }
            mask = mask << 1;
        }
        oolua_ipairs_end()

        String QUUIDStr;
        luaEngine->GetLuaValue(m_newState, "config.info.instrument.uuid", QUUIDStr);
        ins.uuid = QUuid(QUUIDStr.c_str()).toRfc4122().toStdString();

        luaEngine->GetLuaValue(m_newState, "config.info.instrument.model", ins.model);
        luaEngine->GetLuaValue(m_newState, "config.info.instrument.type", ins.type);
        luaEngine->GetLuaValue(m_newState, "config.info.instrument.sn", ins.sn);
        luaEngine->GetLuaValue(m_newState, "config.info.instrument.manuFacturer", ins.manufacturer);
        ins.platformVersion = System::PlatformVersion;

        ins.authorization.name = ins.name;
        luaEngine->GetLuaValue(m_newState, "config.info.instrument.expirationDate", ins.authorization.expirationDate);
        ins.authorization.expirationDate = QDateTime::fromTime_t(ins.authorization.expirationDate).toMSecsSinceEpoch();

        Table systemTable;
        luaEngine->GetLuaValue(m_newState, "config.system", systemTable);
        int unitIndex = 0;
        if (systemTable.safe_at("unitIndex", unitIndex))
        {
            unitIndex++;
            std::string unit = "";
            double proportion = 0;
            luaEngine->GetLuaValue(m_newState, QString("setting.unit[%1].text").arg(unitIndex).toStdString(), unit);
            luaEngine->GetLuaValue(m_newState, QString("setting.unit[%1].multiple").arg(unitIndex).toStdString(), proportion);
            ins.__set_unit(unit);
            ins.__set_proportion(proportion);
        }
        else
        {
            //ins.unit = "mg/L";
            //ins.proportion = 1;
            ins.__set_unit("mg/L");
            ins.__set_proportion(1);
        }

        Table waveTable;
        luaEngine->GetLuaValue(m_newState, "setting.ui.wareData", waveTable);
        QStringList waveList;
        oolua_ipairs(waveTable)
        {
            Table itermTable;
            OOLUA::pull(m_newState, itermTable);
            String waveStr;
            itermTable.safe_at("text", waveStr);
            waveList.append(QString::fromStdString(waveStr));
        }
        oolua_ipairs_end()
        QString wave = waveList.join(",");
        //ins.waveType = wave.toStdString();
        ins.__set_waveType(wave.toStdString());

        CircuitBoard ircuitBoard;
        ircuitBoard.name = "主控板";

        luaEngine->GetLuaValue(m_newState, "config.info.mainBoard.type", ircuitBoard.type);
        luaEngine->GetLuaValue(m_newState, "config.info.mainBoard.model", ircuitBoard.model);
        luaEngine->GetLuaValue(m_newState, "config.info.mainBoard.sn", ircuitBoard.sn);

        int year = 1970, month = 1, day = 1;
        luaEngine->GetLuaValue(m_newState, "config.info.mainBoard.manuDate.year", year);
        luaEngine->GetLuaValue(m_newState, "config.info.mainBoard.manuDate.month", month);
        luaEngine->GetLuaValue(m_newState, "config.info.mainBoard.manuDate.day", day);
        ircuitBoard.manufDate = ManufDate(year, month, day).ToString();

        luaEngine->GetLuaValue(m_newState, "config.info.mainBoard.manuFacturer", ircuitBoard.manufacturer);
        luaEngine->GetLuaValue(m_newState, "setting.version.software", ircuitBoard.softwareVersion);
        luaEngine->GetLuaValue(m_newState, "setting.version.hardware", ircuitBoard.hardwareVersion);

        ircuitBoard.fileSystemVersion = DeviceInformation::GetFileSystemVersion().toStdString();
        QString version = QSysInfo::kernelVersion();
        version.replace(QString("-EmbedSky"), QString(""));
        ircuitBoard.kernelVersion = QString(QSysInfo::kernelType() + "-" + version).toStdString();
        ins.circuitBoards.push_back(ircuitBoard);

        std::map<String,BaseController*> *controllerMap = ControllerManager::Instance()->GetControllerMap();
        for(std::map<String,BaseController*>::iterator it = controllerMap->begin();
         it != controllerMap->end(); it++)
        {
            BaseController *controller = (*it).second;

            CircuitBoard ircuitBoard;

            if (controller->GetConnectStatus())
            {
                try
                {
                    ircuitBoard.name = ControllerManager::Instance()->GetControllerText(controller->GetAddress());
                    ircuitBoard.type = controller->GetIDeviceInfo()->GetType();
                    ircuitBoard.model = controller->GetIDeviceInfo()->GetModel();
                    ircuitBoard.sn = controller->GetIDeviceInfo()->GetSerialNumber();
                    ircuitBoard.manufacturer = controller->GetIDeviceInfo()->GetManufacturer();
                    ircuitBoard.manufDate = controller->GetIDeviceInfo()->GetManufDate().ToString();
                    ircuitBoard.softwareVersion = controller->GetIDeviceInfo()->GetSoftwareVersion().ToString();
                    ircuitBoard.hardwareVersion = controller->GetIDeviceInfo()->GetHardwareVersion().ToString();

                    ins.circuitBoards.push_back(ircuitBoard);
                }
                catch(CommandTimeoutException e)
                {
                    AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                }
                catch (CommunicationException e)
                {
                    AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                }
                catch (std::exception e)
                {
                    AlarmManager::Instance()->AddExceptionAlarm(e);
                }
            }
        }

        status = service->upload(ins);
        logger->debug("Calling upload(), Return: %d", status);
        cout << "Calling upload(), Return: " << status << endl;
        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::upload()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::upload()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::upload()", e);
    }
    catch(TException e)
    {
        ExceptionLogOutput("WqimcManager::upload()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::upload()", e);
    }

    return status;
}

bool WqimcManager::UploadMeasureDataSlots(const MeasureData& data)
{
    bool ret = false;
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        ret = service->uploadMeasureData(data);
        cout << "uploadMeasureData " << ret << endl;
        logger->debug("uploadMeasureData  = %d",  (int)ret);
        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureData()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureData()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureData()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureData()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureData()", e);
    }
    return ret;
}

bool WqimcManager::UploadMeasureCurveSlots(const MeasureCurve& curve)
{
    bool ret = false;
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        ret = service->uploadMeasureCurve(curve);
        cout << "uploadMeasureCurve " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureCurve()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureCurve()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureCurve()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureCurve()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadMeasureCurve()", e);
    }
    return ret;
}

MeasureItem WqimcManager::MakeMeasureItem(OperateRecordData* operateRecordData, ResultData::RecordData recordData, String fiedName)
{
    MeasureItem measureItem;
    measureItem.name = operateRecordData->GetRecordFieldText(fiedName);
    measureItem.unit = operateRecordData->GetRecordFieldUnit(fiedName);
    switch(operateRecordData->GetRecordFieldType(fiedName))
    {
     case FieldType::Bool:
         {
             bool ret;
             if (operateRecordData->GetRecordDataFieldValue(fiedName, recordData, ret))
             {
                if (ret)
                {
                    measureItem.value = "是";
                }
                else
                {
                    measureItem.value = "否";
                }
             }
         }
         break;
     case FieldType::Byte:
         {
             Byte ret;
             if (operateRecordData->GetRecordDataFieldValue(fiedName, recordData, ret))
             {
                measureItem.value = QString::number(ret).toStdString();
             }
         }
         break;
     case FieldType::Int:
         {
             int ret;
             if (operateRecordData->GetRecordDataFieldValue(fiedName, recordData, ret))
             {
                measureItem.value = QString::number(ret).toStdString();
             }
         }
         break;
     case FieldType::Float:
         {
             float ret;
             if (operateRecordData->GetRecordDataFieldValue(fiedName, recordData, ret))
             {
                measureItem.value = QString::number(ret).toStdString();
             }
         }
         break;
     case FieldType::Double:
         {
             double ret;
             if (operateRecordData->GetRecordDataFieldValue(fiedName, recordData, ret))
             {
                measureItem.value = QString::number(ret,'f',9).toStdString();
             }
         }
         break;
     case FieldType::Time:
        {
            int ret;
            if (operateRecordData->GetRecordDataFieldValue(fiedName, recordData, ret))
            {
               measureItem.value = QDateTime::fromTime_t(ret).toString("yyyy-MM-dd hh:mm:ss").toStdString();
            }
        }
        break;
    case FieldType::Enum:
        {
            operateRecordData->GetRecordDataFieldEnumString(fiedName, recordData, measureItem.value);
        }
        break;
     case FieldType::IntArray:
         {
             Uint32 size = operateRecordData->GetRecordFieldSize(fiedName);
             ResultData::IntArray array(size / sizeof(int));
             if (operateRecordData->GetRecordDataFieldValue<ResultData::IntArray>(fiedName, recordData, array))
             {
                String text;
                for (System::Uint32 i = 0; i < array.GetSize(); i++)
                {
                    if (i != 0)
                    {
                        text += ",";
                    }
                    int ret;
                    array.GetInt(i, ret);
                    text += QString::number(ret).toStdString();
                }
                measureItem.value = text;
             }
         }
         break;
    }
    return measureItem;
}

void WqimcManager::uploadMeasureData(String name, ResultData::RecordData recordData)
{
    if (GetConnectedFlag())
    {
        OperateRecordData* operateRecordData = ResultManager::Instance()->GetMeasureOperateRecordData(name);
        if(operateRecordData != nullptr)
        {
            MeasureData data;
            data.__set_target(name);

            int time = 0;
            float consistency = 0;
            String resultType;
            Byte mode;

            operateRecordData->GetRecordDataFieldValue("dateTime", recordData, time);
            operateRecordData->GetRecordDataFieldValue("consistency", recordData, consistency);
            operateRecordData->GetRecordDataFieldValue("mode", recordData, mode);
            operateRecordData->GetRecordDataFieldEnumString("resultType", recordData, resultType);

            data.__set_time(QDateTime::fromTime_t(time).toMSecsSinceEpoch());//转化成毫秒
            data.__set_result(consistency);
            data.__set_mode(mode);
            data.__set_type(resultType);
            data.__set_resultFormat(measureRusultFormat);

            vector<MeasureItem> measureItems;
            vector<String> fieldNames;
            fieldNames = operateRecordData->GetRecordAllFieldName();
            for (vector<String>::iterator iter = fieldNames.begin(); iter != fieldNames.end(); ++iter)
            {
                if (*iter == "dateTime" || *iter == "consistency" ||  *iter == "mode" ||  *iter == "resultType")
                {
                    continue;
                }
                measureItems.push_back(MakeMeasureItem(operateRecordData, recordData, *iter));
            }
            data.__set_optionals(measureItems);
            emit UploadMeasureDataSignals(data);
        }
    }
}

void WqimcManager::uploadMeasureCurve(String name, ResultData::RecordData recordData)
{
    if (GetConnectedFlag())
    {
        OperateRecordData* operateRecordData = ResultManager::Instance()->GetCalibraOperateRecordData(name);
        if(operateRecordData != nullptr)
        {
            MeasureCurve data;
            data.__set_target(name);

            int time = 0;

            operateRecordData->GetRecordDataFieldValue("dateTime", recordData, time);
            data.__set_time(QDateTime::fromTime_t(time).toMSecsSinceEpoch());

            operateRecordData->GetRecordDataFieldValue("curveK", recordData, data.k);
            operateRecordData->GetRecordDataFieldValue("curveB", recordData, data.b);

            data.__set_resultFormat(curveFormat);

            vector<MeasureItem> measureItems;
            vector<String> fieldNames;
            fieldNames = operateRecordData->GetRecordAllFieldName();
            for (vector<String>::iterator iter = fieldNames.begin(); iter != fieldNames.end(); ++iter)
            {
                if (*iter == "dateTime" || *iter == "curveK" ||  *iter == "curveB")
                {
                    continue;
                }
                measureItems.push_back(MakeMeasureItem(operateRecordData, recordData, *iter));
            }
            data.__set_optionals(measureItems);
            emit UploadMeasureCurveSignals(data);
        }
    }
}

bool WqimcManager::uploadSettingProfiles()
{
    std::vector<Profile> profilesList;
    std::map<int, Profile> profilesMap;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");

    bool ret = false;
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        int paramType = SettingManager::Instance()->GetParamType();//此函数不获取Lua数据
        LuaEngine* luaEngine = LuaEngine::Instance();

        Table itermsTable;
        luaEngine->GetLuaValue(m_newState, "setting.ui.profile", itermsTable);
        oolua_pairs(itermsTable)//界面
        {
            Table  viewTable;
            OOLUA::pull(m_newState, viewTable);

            int targetPrivilege = 0;
            bool isTarget = viewTable.safe_at("targetPrivilege", targetPrivilege);
            if (isTarget == false || (paramType & targetPrivilege))
            {
                Profile profile;
                int index;

                viewTable.at("index", index);
                viewTable.at("name", profile.name);
                viewTable.at("text", profile.text);
                viewTable.at("writePrivilege", profile.writePrivilege);
                viewTable.at("readPrivilege", profile.readPrivilege);
                if (profile.name != "hardwareParamIterms")
                {
                    oolua_ipairs(viewTable)//类别
                    {
                        Table typeTable;
                        OOLUA::pull(m_newState, typeTable);

                        ConfigGrop configGrop;
                        typeTable.at("text", configGrop.text);
                        typeTable.at("name", configGrop.name);

                        oolua_ipairs(typeTable)//子项
                        {
                            ConfigItem configItem;
                            Table itermTable;

                            OOLUA::pull(m_newState, itermTable);
                            int targetPrivilege = 0;
                            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
                            if (isTarget == false || (paramType & targetPrivilege))
                            {
                                int nType = Interface::Wqimc::Api::Manager::DataType::type::Float;
                                itermTable.at("name", configItem.name);
                                itermTable.at("text", configItem.text);
                                itermTable.at("type", nType);    //type的类型是UI::DataType
                                itermTable.safe_at("unit", configItem.unit);
                                itermTable.at("writePrivilege", configItem.writePrivilege);
                                itermTable.at("readPrivilege", configItem.readPrivilege);

                                if (static_cast<int>(UI::DataType::String) == nType)
                                {
                                    nType = Interface::Wqimc::Api::Manager::DataType::type::String;
                                }
                                configItem.type = static_cast<Interface::Wqimc::Api::Manager::DataType::type>(nType);

                                if (configItem.type == Interface::Wqimc::Api::Manager::DataType::type::Option)
                                {
                                    Table optionsTable;
                                    itermTable.at("options", optionsTable);
                                    oolua_ipairs(optionsTable)
                                    {
                                        String str;
                                        OOLUA::pull(m_newState, str);
                                        configItem.options.push_back(str);
                                    }
                                    oolua_ipairs_end()
                                }
                                String valueName = "config." + profile.name + "." + configItem.name;
                                switch(configItem.type)
                                {
                                    case Interface::Wqimc::Api::Manager::DataType::type::Bool:
                                    {
                                        bool ret;
                                        luaEngine->GetLuaValue(m_newState, valueName, ret);
                                        if(ret)
                                        {
                                            configItem.value = "true";
                                        }
                                        else
                                        {
                                            configItem.value = "false";
                                        }
                                    }
                                    break;
                                    case Interface::Wqimc::Api::Manager::DataType::type::Option:
                                    {
                                        Byte ret;
                                        luaEngine->GetLuaValue(m_newState, valueName, ret);
                                        configItem.value = QString::number(ret).toStdString();
                                    }
                                    break;
                                    case Interface::Wqimc::Api::Manager::DataType::type::Int:
                                    {
                                        int ret;
                                        luaEngine->GetLuaValue(m_newState, valueName, ret);
                                        configItem.value = QString::number(ret).toStdString();
                                    }
                                    break;
                                    case Interface::Wqimc::Api::Manager::DataType::type::Float:
                                    {
                                        float ret;
                                        luaEngine->GetLuaValue(m_newState, valueName, ret);
                                        configItem.value = QString::number(ret).toStdString();
                                    }
                                    break;
                                    case Interface::Wqimc::Api::Manager::DataType::type::Double:
                                    {
                                        double ret;
                                        luaEngine->GetLuaValue(m_newState, valueName, ret);
                                        configItem.value = QString("%1").arg(ret, 0, 'f', 6).replace(rx,"").toStdString();
                                    }
                                    break;
                                    case Interface::Wqimc::Api::Manager::DataType::type::String:
                                        luaEngine->GetLuaValue(m_newState, valueName, configItem.value);
                                    break;
                                    case Interface::Wqimc::Api::Manager::DataType::type::IntArray:
//                                        if (configItem.name == "measure.timedPoint")
                                        {
                                            Table timedPoinTtable;
                                            luaEngine->GetLuaValue(m_newState, valueName, timedPoinTtable);
                                            int index =0;
                                            oolua_ipairs(timedPoinTtable)
                                            {
                                                bool result;
                                                OOLUA::pull(m_newState, result);
                                                if (result)
                                                {
                                                    configItem.value += QString::number(index).toStdString() + ",";
                                                }
                                                index++;
                                            }
                                            oolua_ipairs_end()
                                        }
                                    break;
                                    default:
                                        break;
                                }
                                configGrop.configs.push_back(configItem);
                            }
                        }
                        oolua_ipairs_end()
                        if (configGrop.configs.size() > 0)
                        {
                            profile.groups.push_back(configGrop);
                        }
                    }
                    oolua_ipairs_end()
                    if (profile.groups.size() > 0)
                    {
                        profilesMap.insert(make_pair(index, profile));
                    }

                }
                else
                {
                    Lua_function call(m_newState);
                    oolua_ipairs(viewTable)
                    {
                        OOLUA::Lua_func_ref hardwareCategoryget;

                        Table typeTable;
                        OOLUA::pull(m_newState, typeTable);

                        ConfigGrop configGrop;
                        typeTable.at("text", configGrop.text);
                        typeTable.at("name", configGrop.name);
                        typeTable.at("get", hardwareCategoryget);
                        call(hardwareCategoryget);
                        oolua_ipairs(typeTable)
                        {
                            Table itermTable;
                            OOLUA::pull(m_newState, itermTable);

                            int targetPrivilege = 0;
                            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
                            if (isTarget == false || (paramType & targetPrivilege))
                            {
                                ConfigItem configItem;
                                OOLUA::Lua_func_ref hardwareItemget;
                                itermTable.at("name", configItem.name);
                                itermTable.at("text", configItem.text);
                                configItem.type = Interface::Wqimc::Api::Manager::DataType::type::Float;
                                itermTable.safe_at("unit", configItem.unit);
                                itermTable.at("writePrivilege", configItem.writePrivilege);
                                itermTable.at("readPrivilege", configItem.readPrivilege);
                                itermTable.at("get", hardwareItemget);

                                float result;
                                call(hardwareItemget);
                                OOLUA::pull(m_newState, result);
                                configItem.value = QString::number(result).toStdString();

                                configGrop.configs.push_back(configItem);
                            }
                        }
                        oolua_ipairs_end()
                        if (configGrop.configs.size() > 0)
                        {
                            profile.groups.push_back(configGrop);
                        }
                    }
                    oolua_ipairs_end()
                    if (profile.groups.size() > 0)
                    {
                        profilesMap.insert(make_pair(index, profile));
                    }
                }
            }
        }
        oolua_pairs_end()
        for (std::map<int, Profile>::iterator iter = profilesMap.begin(); iter != profilesMap.end(); iter++)
        {
            profilesList.push_back(iter->second);
        }
        ret = service->uploadSettingProfiles(profilesList);
        cout << "uploadSettingProfiles " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadSettingProfiles()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::uploadSettingProfiles()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::uploadSettingProfiles()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::uploadSettingProfiles()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadSettingProfiles()", e);
    }
    return ret;
}

void WqimcManager::UpdateConfigsSlots(const std::vector<Config> &config)
{
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        bool ret = service->updateConfigs(config);
        cout << "updateConfigs " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdateConfigsSlots()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateConfigsSlots()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateConfigsSlots()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateConfigsSlots()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdateConfigsSlots()", e);
    }
}

bool WqimcManager::updateConfigs(ConfigVector configVector)
{
    bool ret = false;
    if (GetConnectedFlag())
    {
        emit UpdateConfigsSignals(configVector.GetConfigList());
        ret = true;
    }
    return ret;
}

bool WqimcManager::UploadReagentProfileSlots()
{
    vector<ReagentConfigItem> reagentConfigItemList;
    bool ret = false;
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        int paramType = SettingManager::Instance()->GetParamType();

        LuaEngine* luaEngine = LuaEngine::Instance();
        Table itermsTable;
        luaEngine->GetLuaValue(m_newState, "setting.ui.reagentManager", itermsTable);
        oolua_ipairs(itermsTable)
        {
            Table itermTable;
            OOLUA::pull(m_newState, itermTable);

            int targetPrivilege = 0;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
            if (isTarget == false || (paramType & targetPrivilege))
            {
                ReagentConfigItem reagentConfigItem;

                itermTable.at("name", reagentConfigItem.name);
                itermTable.at("text", reagentConfigItem.text);

                float total;
                luaEngine->GetLuaValue(m_newState, String("config.remain." + reagentConfigItem.name + ".total"), total);
                reagentConfigItem.totalVolume = total;

                float cycle;
                luaEngine->GetLuaValue(m_newState, String("config.remain." + reagentConfigItem.name + ".cycle"), cycle);
                reagentConfigItem.cycle = cycle;

                int lastTime;
                luaEngine->GetLuaValue(m_newState, String("config.remain." + reagentConfigItem.name + ".lastTime"), lastTime);
                reagentConfigItem.lastTime = QDateTime::fromTime_t(lastTime).toMSecsSinceEpoch();

                float remainVolume;
                luaEngine->GetLuaValue(m_newState, String("status.remain." + reagentConfigItem.name), remainVolume);
                reagentConfigItem.remainVolume = remainVolume;

                reagentConfigItemList.push_back(reagentConfigItem);
            }
        }
        oolua_ipairs_end()
        ret = service->uploadReagentProfile(reagentConfigItemList);
        cout << "uploadReagentProfile " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadReagentProfile()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::uploadReagentProfile()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::uploadReagentProfile()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::uploadReagentProfile()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadReagentProfile()", e);
    }
    return ret;
}

bool WqimcManager::uploadReagentProfile()
{
    bool ret = false;
    if (GetConnectedFlag())
    {
        emit UploadReagentProfileSignals();
        ret = true;
    }
    return ret;
}

bool WqimcManager::updateReagentRemain(ReagentRemainVector reagentRemainVector)
{
    bool ret = false;
    if (GetConnectedFlag())
    {
        emit UpdateReagentRemainSignals(reagentRemainVector.GetReagentRemainList());
        ret = true;
    }
    return ret;
}

void WqimcManager::UpdateReagentRemainSlots(const std::vector<Interface::Wqimc::Api::Manager::ReagentRemain> &reagentRemainList)
{
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        bool ret = service->updateReagentRemain(reagentRemainList);
        cout << "UpdateReagentRemainSlots " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdateReagentRemainSlots()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateReagentRemainSlots()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateReagentRemainSlots()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateReagentRemainSlots()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdateReagentRemainSlots()", e);
    }
}

void WqimcManager::UploadConsumableProfileSlots()
{
    vector<ConsumableConfigItem> configs;
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        int paramType = SettingManager::Instance()->GetParamType();
        LuaEngine* luaEngine = LuaEngine::Instance();
        Table itermsTable;
        luaEngine->GetLuaValue(m_newState, "setting.ui.useResourceWidget", itermsTable);
        oolua_ipairs(itermsTable)
        {
            Table itermTable;
            OOLUA::pull(m_newState, itermTable);

            int targetPrivilege = 0;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
            if (isTarget == false || (paramType & targetPrivilege))
            {
                ConsumableConfigItem consumableConfigItem;

                itermTable.at("name", consumableConfigItem.name);
                itermTable.at("text", consumableConfigItem.text);

                float cycle;
                luaEngine->GetLuaValue(m_newState, String("config.consumable." + consumableConfigItem.name + ".cycle"), cycle);
                consumableConfigItem.cycle = cycle;

                int lastTime;
                luaEngine->GetLuaValue(m_newState, String("config.consumable." + consumableConfigItem.name + ".lastTime"), lastTime);
                consumableConfigItem.lastTime = QDateTime::fromTime_t(lastTime).toMSecsSinceEpoch();

                configs.push_back(consumableConfigItem);
            }
        }
        oolua_ipairs_end()
        bool ret = service->uploadConsumableProfile(configs);
        cout << "UploadConsumableProfileSlots "<< ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::UploadConsumableProfileSlots()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::UploadConsumableProfileSlots()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::UploadConsumableProfileSlots()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::UploadConsumableProfileSlots()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::UploadConsumableProfileSlots()", e);
    }
}

bool WqimcManager::uploadConsumableProfile()
{
    bool ret = false;
    if (GetConnectedFlag())
    {
        emit UploadConsumableProfileSignals();
        ret = true;
    }
    return ret;
}

std::string WqimcManager::GetSignalFormat(std::string name)
{
    std::string format;
    if (m_signalFormat.count(name))
    {
        format = m_signalFormat.at(name);
    }
    return format;
}

bool WqimcManager::uploadSignalProfile()
{
    bool ret = false;
    CloudConnectionService* service = CloudConnectionService::Instance();
    SignalManager *signalManager = SignalManager::Instance();
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        Profile profile;

        profile.name = "signal";
        profile.text = "信号";
        profile.writePrivilege = Api::Manager::RoleType::type::Engineer;
        profile.readPrivilege = Api::Manager::RoleType::type::Engineer;

        Table viewTable;
        luaEngine->GetLuaValue(m_newState, "setting.ui.signalIterms", viewTable);

        oolua_ipairs(viewTable)//类别
        {
            Table typeTable;
            OOLUA::pull(m_newState, typeTable);
            ConfigGrop configGrop;

            typeTable.at("text", configGrop.text);
            typeTable.at("name", configGrop.name);
            oolua_ipairs(typeTable)//子项
            {
                ConfigItem configItem;
                Table itermTable;

                OOLUA::pull(m_newState, itermTable);

                itermTable.at("name", configItem.name);
                itermTable.at("text", configItem.text);
                configItem.type = Interface::Wqimc::Api::Manager::DataType::type::Float;
                itermTable.safe_at("unit", configItem.unit);
                configItem.writePrivilege = Api::Manager::RoleType::type::Engineer;
                configItem.readPrivilege = Api::Manager::RoleType::type::Engineer;

                itermTable.at("format", configItem.format);

                float value = signalManager->GetSignal(configItem.name);
                if (value != DBL_MIN)
                {
                    QString str;
                    str.sprintf(configItem.format.c_str(), value);
                    configItem.value = str.toStdString();
                }
                m_signalFormat.insert(make_pair(configItem.name, configItem.format));
                configGrop.configs.push_back(configItem);
            }
            oolua_ipairs_end()
            profile.groups.push_back(configGrop);
        }
        oolua_ipairs_end()

        ret = service->uploadSignalProfile(profile);
        cout << "uploadSignalProfile " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadSignalProfile()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::uploadSignalProfile()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::uploadSignalProfile()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::uploadSignalProfile()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadSignalProfile()", e);
    }
    return ret;
}

void WqimcManager::UpdaterSignalTimerSlots()
{
    std::map<String, double>signalMap = SignalManager::Instance()->GetAllSignal();
    std::vector<Interface::Wqimc::Api::Manager::Signal> signalsList;

    for (std::map<String, double>::iterator iter = signalMap.begin(); iter != signalMap.end(); ++iter)
    {
        Signal signal;
        signal.__set_name(iter->first);
        signal.__set_value(iter->second);
        signalsList.push_back(signal);
    }

    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        service->updateSignals(signalsList);
//        cout << "UpdateSignalsSlots " << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdaterSignalTimerSlots()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::UpdaterSignalTimerSlots()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::UpdaterSignalTimerSlots()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::UpdaterSignalTimerSlots()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdaterSignalTimerSlots()", e);
    }
}

bool WqimcManager::setSignalUpdateCycle(float cycleSec)
{
    emit SetSignalUpdateCycleSignals(cycleSec);
    return true;
}

void WqimcManager::SetSignalUpdateCycleSlots(const double cycleSec)
{
    if (cycleSec == 0)
    {
        m_timer->stop();
    }
    else
    {
        m_timer->setInterval(cycleSec * 1000);
        if (!m_timer->isActive())
        {
            m_timer->start();
        }
    }
}

void WqimcManager::reboot()
{
    emit RebootSignals();
}

void WqimcManager::RebootSlots()
{
    QThread::sleep(5);
    QProcess::execute("reboot");
}

bool WqimcManager::upgrade(const System::String& type, const System::String& newVersion, const System::String& url)
{
    (void)newVersion;
    if (m_isUpgrading == false)
    {
        m_isUpgrading = true;
        m_upgradeType = type;
        emit UpgradeSignals(url);
        return true;
    }
    else
    {
        logger->debug("云平台:下载器正在忙");
        return false;
    }
}

QFile *WqimcManager::OpenFileForWrite(const QString &fileName)
{
    QScopedPointer<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly))
    {
       logger->debug("不能创建 %s 文件：%s",
                  fileName.toStdString().c_str(),
                  file->errorString().toStdString().c_str());
        return Q_NULLPTR;
    }
    return file.take();
}

void WqimcManager::UpgradeSlots(const System::String& url)
{
    const QUrl newUrl = QUrl::fromUserInput(url.c_str());

    QString fileName = newUrl.fileName();
    QString downloadDirectory = Configuration::Environment::Instance()->GetUnzipPath().c_str();
    QDir dir(downloadDirectory);
    if (!dir.exists())
    {
        dir.mkpath(downloadDirectory);
    }
    fileName.prepend(downloadDirectory + '/');
    if (QFile::exists(fileName))//删除已存在的同名升级包
    {
        QFile::remove(fileName);
    }
    if (m_upgradeFile)
    {
        if (m_upgradeFile->isOpen())
        {
            m_upgradeFile->close();
        }
        delete m_upgradeFile;
        m_upgradeFile = Q_NULLPTR;
    }
    m_upgradeFile = OpenFileForWrite(fileName);
    if (!m_upgradeFile)
    {
        m_isUpgrading = false;
        return;
    }

    if (false == httpClient->UpgradeGetRequest(url.c_str(), this, SLOT(UpgradeReadyReadSlot(QByteArray)),
                                        SLOT(UpgradeFinishedSlot()), SLOT(UpgradeErrorSlot(QString))))
    {
        m_isUpgrading = false;
    }
    else
    {
        RunStatus status("DownloadFile", "下载", 0);
        StatusManager::Instance()->SetStatus(status);
    }
}

void WqimcManager::UpgradeReadyReadSlot(const QByteArray &data)
{
    if (m_upgradeFile)
    {
        m_upgradeFile->write(data);
    }
}

void WqimcManager::UpgradeFinishedSlot()
{
    QFileInfo fi;
    if (m_upgradeFile)
    {
        fi.setFile(m_upgradeFile->fileName());
        m_upgradeFile->close();
        delete m_upgradeFile;
        m_upgradeFile = Q_NULLPTR;
    }
    QFileInfo qFileInfo(fi.absoluteFilePath());
    logger->debug("云平台:下载 %s 文件 %lld 字节",
            fi.absoluteFilePath().toStdString().c_str(), qFileInfo.size());

    QString fileName = fi.absoluteFilePath();

    StatusManager::Instance()->ResetDeviceStatus();
    if (!FlowManager::Instance()->IsFlowEnable())  // 空闲状态
    {
        return;
    }

    LuaEngine* luaEngine = LuaEngine::Instance();
    String mainBoardType;
    luaEngine->GetLuaValue(m_newState, "config.info.mainBoard.type", mainBoardType);
    if (m_upgradeType == mainBoardType)
    {
        UpdateFlow *updateFlow = new UpdateFlow();
        updateFlow->AddStep(new Updater::MasterUpdate(fileName.toStdString(), true));
        FlowManager::Instance()->StartCFlow(updateFlow);
    }
    else
    {
        std::map<String,BaseController*> *controllerMap = ControllerManager::Instance()->GetControllerMap();
        for(std::map<String,BaseController*>::iterator it = controllerMap->begin();
         it != controllerMap->end(); it++)
        {
            BaseController *controller = (*it).second;
            if (controller->GetConnectStatus())
            {
                try
                {
                    String type =  controller->GetIDeviceInfo()->GetType();
                    logger->debug("Update type = %s",type.c_str());
                    if(0 == QString::compare(QString::fromStdString(m_upgradeType), QString::fromStdString(type), Qt::CaseInsensitive))
                    {
                        UpdateFlow *updateFlow = new UpdateFlow();
                        updateFlow->AddStep(new Updater::UpdateManager(controller->GetAddress(),
                                                              fileName.toStdString(),
                                                              ControllerManager::Instance()->GetControllerText(controller->GetAddress()),
                                                              true));
                        FlowManager::Instance()->StartCFlow(updateFlow);
                    }
                }
                catch(CommandTimeoutException e)
                {
                    AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                }
                catch (CommunicationException e)
                {
                    AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                }
                catch (std::exception e)
                {
                    AlarmManager::Instance()->AddExceptionAlarm(e);
                }
            }
        }
    }
    m_isUpgrading = false;
}

void WqimcManager::UpgradeErrorSlot(const QString message)
{
    QFileInfo fi;
    if (m_upgradeFile)
    {
        fi.setFile(m_upgradeFile->fileName());
        m_upgradeFile->close();
        delete m_upgradeFile;
        m_upgradeFile = Q_NULLPTR;
    }
    QFile::remove(fi.absoluteFilePath());
    logger->debug("云平台:升级文件下载失败%s：%s",
               fi.absoluteFilePath().toStdString().c_str(),
               message.toStdString().c_str());
    logger->info("下载失败");

    StatusManager::Instance()->ResetDeviceStatus();
    m_isUpgrading = false;
}

void WqimcManager::Register(IWqimcStatusNotifiable *handle)
{
    m_notifise.push_back(handle);
}

void WqimcManager::ChangeWqimcStatus(WqimcStatus wqimcStatus)
{
    for(std::list<IWqimcStatusNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnChangeStatus(wqimcStatus);
        }
    }
}

bool WqimcManager::alarm(const Measure::Alarm& alarm)
{
    bool ret = false;
    if (GetConnectedFlag())
    {
        Interface::Wqimc::Api::Manager::Alarm sendAlarm;
        sendAlarm.__set_cause(alarm.cause);
        sendAlarm.__set_level(alarm.level);
        sendAlarm.__set_time(alarm.time.toMSecsSinceEpoch());
        sendAlarm.__set_type(alarm.type);
        sendAlarm.__set_name(alarm.name);
//        qDebug("alarm time %s name : %s, type : %s, cause :%s level: %s", alarm.time.toString("yyyy-MM-dd hh:mm:ss").toLatin1().data(),
//                alarm.name.c_str(), alarm.type.c_str(), alarm.cause.c_str(), alarm.level.c_str());
        emit AlarmSignals(sendAlarm);
        ret = true;
    }
    return ret;
}

void WqimcManager::AlarmSlots(const Interface::Wqimc::Api::Manager::Alarm& alarm)
{
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
       bool ret = service->alarm(alarm);
       cout << "alarm " << ret << endl;
       logger->debug("alarm  = %d",  (int)ret);

       ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::AlarmSlots()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::AlarmSlots()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::AlarmSlots()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::AlarmSlots()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::AlarmSlots()", e);
    }
}

void WqimcManager::UpdateStatusSlots(const Interface::Wqimc::Api::Manager::Status& status)
{
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        bool ret = service->updateStatus(status);
        cout << "updateStatus " << ret << endl;
        logger->debug("updateStatus  = %d",  (int)ret);
        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdateStatusSlots()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateStatusSlots()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateStatusSlots()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateStatusSlots()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdateStatusSlots()", e);
    }
}

bool WqimcManager::UpdateAction(const Interface::Wqimc::Api::Manager::Status& action)
{
    bool ret = false;

    CloudConnectionService* service = CloudConnectionService::Instance();

    try
    {
        ret = service->updateAction(action);
        cout << "updateAction " << ret << endl;
        logger->debug("updateAction  = %d",  (int)ret);
        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdateActionSlots()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateActionSlots()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateActionSlots()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::UpdateActionSlots()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::UpdateActionSlots()", e);
    }

    return ret;
}

void WqimcManager::UpdateActionSlots(const Interface::Wqimc::Api::Manager::Status& action)
{
    this->UpdateAction(action);
}

void WqimcManager::OnStattusChanged(RunStatus status)
{
    if (GetConnectedFlag())
    {
        Interface::Wqimc::Api::Manager::Status sendStatus;
        sendStatus.__set_time(QDateTime::currentMSecsSinceEpoch());
        sendStatus.__set_name(status.name);
        sendStatus.__set_text(status.content);
        sendStatus.__set_duration(status.time);
//        qDebug("OnStattusChanged time %s name : %s, text : %s, duration :%d ", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLatin1().data(),
//                status.name.c_str(), status.content.c_str(), status.time);
        emit UpdateStatusSignals(sendStatus);
    }
}

void WqimcManager::OnStattusDelay(int time)
{
    (void)time;
}

void WqimcManager::OnActionChanged(RunAction action)
{
    if (GetConnectedFlag())
    {
        Interface::Wqimc::Api::Manager::Status sendAction;
        sendAction.__set_time(QDateTime::currentMSecsSinceEpoch());
        sendAction.__set_name(action.name);
        sendAction.__set_text(action.content);
        sendAction.__set_duration(action.time);

//        qDebug("OnActionChanged time %s name : %s, text : %s, duration :%d ", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLatin1().data(),
//                action.name.c_str(), action.content.c_str(), action.time);
        emit UpdateActionSignals(sendAction);
    }
}

void WqimcManager::OnActionDelay(int time)
{
    (void)time;
}

int64_t WqimcManager::getSystemTime()
{
    int64_t time = 0;
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
       time = service->getSystemTime();
       cout << "getSystemTime " << time << endl;

       ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::getSystemTime()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::getSystemTime()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::getSystemTime()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::getSystemTime()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::getSystemTime()", e);
    }
    return time;
}

bool WqimcManager::uploadOperations()
{
    bool ret;
    std::vector<OperationSuit> ops;
    std::map<int, OperationSuit> opsMap;

    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        int paramType = SettingManager::Instance()->GetParamType();
        LuaEngine* luaEngine = LuaEngine::Instance();

        Table itermsTable;
        luaEngine->GetLuaValue(m_newState, "setting.ui.operation", itermsTable);
        oolua_pairs(itermsTable)//界面
        {
            Table  viewTable;
            OOLUA::pull(m_newState, viewTable);
            OperationSuit operationSuit;
            int index = 0;
            bool isCorrect = true;
            viewTable.at("name", operationSuit.name);
            viewTable.at("text", operationSuit.text);

            if (operationSuit.name == "maintain")
            {
                index = 1;

                viewTable.at("readPrivilege", operationSuit.readPrivilege);
                viewTable.at("writePrivilege", operationSuit.writePrivilege);

                OperationGroup operationGroup;
                operationGroup.name = operationSuit.name;
                operationGroup.text = operationSuit.text;

                oolua_ipairs(viewTable)//子项
                {
                    Table itermTable;
                    OOLUA::pull(m_newState, itermTable);

                    int targetPrivilege = 0;
                    bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
                    if (isTarget == false || (paramType & targetPrivilege))
                    {
                        OperationItem operationItem;
                        itermTable.at("name", operationItem.name);
                        itermTable.at("text", operationItem.text);
                        itermTable.at("writePrivilege", operationItem.writePrivilege);
                        itermTable.at("readPrivilege", operationItem.readPrivilege);
                        operationGroup.operations.push_back(operationItem);
                    }
                }
                oolua_ipairs_end()
                if (operationGroup.operations.size() > 0)
                {
                    operationSuit.groups.push_back(operationGroup);
                }
            }
            else if (operationSuit.name == "hardwareTest")
            {
                index = 3;

                viewTable.at("readPrivilege", operationSuit.readPrivilege);
                viewTable.at("writePrivilege", operationSuit.writePrivilege);

                oolua_ipairs(viewTable)//类别
                {
                    Table typeTable;
                    OOLUA::pull(m_newState, typeTable);

                    OperationGroup operationGroup;
                    typeTable.at("text", operationGroup.text);
                    typeTable.at("name", operationGroup.name);

                    oolua_ipairs(typeTable)//子项
                    {
                        Table itermTable;
                        OOLUA::pull(m_newState, itermTable);

                        int targetPrivilege = 0;
                        bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
                        if (isTarget == false || (paramType & targetPrivilege))
                        {
                            OperationItem operationItem;
                            itermTable.at("name", operationItem.name);
                            itermTable.at("text", operationItem.text);

                            operationItem.params.push_back("0");
                            itermTable.at("readPrivilege", operationItem.readPrivilege);
                            itermTable.at("writePrivilege", operationItem.writePrivilege);

                            operationGroup.operations.push_back(operationItem);
                        }
                    }
                    oolua_ipairs_end()
                    if (operationGroup.operations.size() > 0)
                    {
                        operationSuit.groups.push_back(operationGroup);
                    }
                }
                oolua_ipairs_end()

                OperationParam operationParam;
                operationParam.__set_name("operation");
                operationParam.__set_text("操作方式");
                operationParam.__set_type(Interface::Wqimc::Api::Manager::DataType::type::Option);
                operationParam.options.push_back("关闭");
                operationParam.options.push_back("打开");
                operationSuit.params.push_back(operationParam);
            }
            else if (operationSuit.name == "liquidOperator")
            {
                index = 2;

                viewTable.at("readPrivilege", operationSuit.readPrivilege);
                viewTable.at("writePrivilege", operationSuit.writePrivilege);

                OperationGroup operationGroup;
                operationGroup.name = operationSuit.name;
                operationGroup.text = operationSuit.text;

                oolua_ipairs(viewTable)//子项
                {
                    Table itermTable;
                    OOLUA::pull(m_newState, itermTable);

                    int targetPrivilege = 0;
                    bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
                    if (isTarget == false || (paramType & targetPrivilege))
                    {
                        OperationItem operationItem;
                        float data = 0;

                        itermTable.at("name", operationItem.name);
                        itermTable.at("text", operationItem.text);
                        itermTable.at("data", data);

                        operationItem.params.push_back("0");
                        operationItem.params.push_back(QString::number(data).toStdString());

                        itermTable.at("readPrivilege", operationItem.readPrivilege);
                        itermTable.at("writePrivilege", operationItem.writePrivilege);

                        operationGroup.operations.push_back(operationItem);
                    }
                }
                oolua_ipairs_end()
                operationSuit.groups.push_back(operationGroup);

                OperationParam operationParam;
                operationParam.__set_name("operation");
                operationParam.__set_text("操作方式");
                operationParam.__set_type(Interface::Wqimc::Api::Manager::DataType::type::Option);
                operationParam.options.push_back("泵计步");
                operationParam.options.push_back("智能定量");
                operationParam.options.push_back("精确定量");
                operationSuit.params.push_back(operationParam);

                OperationParam volumeParam;
                volumeParam.__set_name("volume");
                volumeParam.__set_text("体积");
                volumeParam.__set_type(Interface::Wqimc::Api::Manager::DataType::type::Float);
                volumeParam.__set_unit("ml");
                operationSuit.params.push_back(volumeParam);
            }
            else if (operationSuit.name == "combineOperator")
            {
                index = 4;

                viewTable.at("readPrivilege", operationSuit.readPrivilege);
                viewTable.at("writePrivilege", operationSuit.writePrivilege);

                OperationGroup operationGroup;
                operationGroup.name = operationSuit.name;
                operationGroup.text = operationSuit.text;

                oolua_ipairs(viewTable)//子项
                {
                    Table itermTable;
                    OOLUA::pull(m_newState, itermTable);

                    int targetPrivilege = 0;
                    bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
                    if (isTarget == false || (paramType & targetPrivilege))
                    {
                        OperationItem operationItem;
                        float data = 0;

                        itermTable.at("name", operationItem.name);
                        itermTable.at("text", operationItem.text);
                        itermTable.at("data", data);

//                        operationItem.params.push_back("0");
                        operationItem.params.push_back(QString::number(data).toStdString());

                        itermTable.at("readPrivilege", operationItem.readPrivilege);
                        itermTable.at("writePrivilege", operationItem.writePrivilege);

                        operationGroup.operations.push_back(operationItem);
                    }
                }
                oolua_ipairs_end()
                operationSuit.groups.push_back(operationGroup);

                OperationParam volumeParam;
                volumeParam.__set_name("volume");
                volumeParam.__set_text("体积");
                volumeParam.__set_type(Interface::Wqimc::Api::Manager::DataType::type::Float);
                volumeParam.__set_unit("ml");
                operationSuit.params.push_back(volumeParam);
            }
            else
            {
                isCorrect = false;
            }
            if (isCorrect) opsMap.insert(make_pair(index, operationSuit));
        }
        oolua_pairs_end()
        for (std::map<int, OperationSuit>::iterator iter = opsMap.begin(); iter != opsMap.end(); iter++)
        {
            ops.push_back(iter->second);
        }
        ret = service->uploadOperations(ops);
        cout << "uploadOperations " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::LuaException e)
    {
        logger->warn("WqimcManager::uploadOperations() => %s", e.What().c_str());
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::uploadOperations()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::uploadOperations()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::uploadOperations()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadOperations()", e);
    }
    return ret;
}

bool WqimcManager::uploadDiagnoses()
{
    bool ret;
    std::vector<DiagnosisSuit> ops;
    std::map<int, DiagnosisSuit> opsMap;

    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        int paramType = SettingManager::Instance()->GetParamType();
        LuaEngine* luaEngine = LuaEngine::Instance();

        Table itermsTable;
        luaEngine->GetLuaValue(m_newState, "setting.ui.diagnosis", itermsTable);
        oolua_pairs(itermsTable)//界面
        {
            Table  viewTable;
            OOLUA::pull(m_newState, viewTable);
            DiagnosisSuit diagnosisSuit;
            int index = 0;
            viewTable.at("name", diagnosisSuit.name);
            viewTable.at("text", diagnosisSuit.text);
            if (diagnosisSuit.name == "smartDetect")
            {
                index = 1;

                viewTable.at("readPrivilege", diagnosisSuit.readPrivilege);
                viewTable.at("writePrivilege", diagnosisSuit.writePrivilege);

                DiagnosisGroup diagnosisGroup;
                diagnosisGroup.name = diagnosisSuit.name;
                diagnosisGroup.text = diagnosisSuit.text;

                oolua_ipairs(viewTable)//子项
                {
                    Table itermTable;
                    OOLUA::pull(m_newState, itermTable);
                    DiagnosisItem diagnosisItem;

                    int targetPrivilege = 0;
                    bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
                    if (isTarget == false || (paramType & targetPrivilege))
                    {
                        itermTable.at("name", diagnosisItem.name);
                        itermTable.at("text", diagnosisItem.text);

                        itermTable.at("readPrivilege", diagnosisItem.readPrivilege);
                        itermTable.at("writePrivilege", diagnosisItem.writePrivilege);

                        diagnosisGroup.diagnoses.push_back(diagnosisItem);
                    }
                }
                oolua_ipairs_end()
                if (diagnosisGroup.diagnoses.size() > 0)
                {
                    diagnosisSuit.groups.push_back(diagnosisGroup);
                }
            }
            else if (diagnosisSuit.name == "communicationCheck")
            {
                index = 2;

                viewTable.at("readPrivilege", diagnosisSuit.readPrivilege);
                viewTable.at("writePrivilege", diagnosisSuit.writePrivilege);

                oolua_ipairs(viewTable)//类别
                {
                    Table typeTable;
                    OOLUA::pull(m_newState, typeTable);

                    DiagnosisGroup diagnosisGroup;
                    typeTable.at("text", diagnosisGroup.text);
                    typeTable.at("name", diagnosisGroup.name);
                    oolua_ipairs(typeTable)//子项
                    {
                        Table itermTable;
                        OOLUA::pull(m_newState, itermTable);
                        DiagnosisItem diagnosisItem;

                        int targetPrivilege = 0;
                        bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);
                        if (isTarget == false || (paramType & targetPrivilege))
                        {

                            itermTable.at("name", diagnosisItem.name);
                            itermTable.at("text", diagnosisItem.text);
                            itermTable.at("readPrivilege", diagnosisItem.readPrivilege);
                            itermTable.at("writePrivilege", diagnosisItem.writePrivilege);

                            diagnosisGroup.diagnoses.push_back(diagnosisItem);
                        }
                    }
                    oolua_ipairs_end()
                    if (diagnosisGroup.diagnoses.size() > 0)
                    {
                        diagnosisSuit.groups.push_back(diagnosisGroup);
                    }
                }
                oolua_ipairs_end()
            }
            opsMap.insert(make_pair(index, diagnosisSuit));
        }
        oolua_pairs_end()
        for (std::map<int, DiagnosisSuit>::iterator iter = opsMap.begin(); iter != opsMap.end(); iter++)
        {
            ops.push_back(iter->second);
        }

        ret = service->uploadDiagnoses(ops);
        cout << "uploadDiagnoses " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadDiagnoses()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::uploadDiagnoses()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::uploadDiagnoses()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::uploadDiagnoses()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::uploadDiagnoses()", e);
    }
    return ret;
}

bool WqimcManager::notifyDiagnosisResult(const Interface::Wqimc::Api::Manager::DiagnosisResult& result)
{
    bool ret = false;
    if (GetConnectedFlag())
    {
        emit NotifyDiagnosisResultSignals(result);
        ret = true;
    }
    return ret;
}

void WqimcManager::NotifyDiagnosisResultSlots(const Interface::Wqimc::Api::Manager::DiagnosisResult& result)
{
    CloudConnectionService* service = CloudConnectionService::Instance();
    try
    {
        bool ret = service->notifyDiagnosisResult(result);
        cout << "notifyDiagnosisResult " << ret << endl;

        ClearExceptioFlag();
    }
    catch(OOLUA::Exception e)
    {
        ExceptionLogOutput("WqimcManager::NotifyDiagnosisResultSlots()", e);
    }
    catch (TTransportException e)
    {
        ExceptionLogOutput("WqimcManager::NotifyDiagnosisResultSlots()", e);
    }
    catch (TProtocolException e)
    {
        ExceptionLogOutput("WqimcManager::NotifyDiagnosisResultSlots()", e);
    }
    catch (TException e)
    {
        ExceptionLogOutput("WqimcManager::NotifyDiagnosisResultSlots()", e);
    }
    catch (exception e)
    {
        ExceptionLogOutput("WqimcManager::NotifyDiagnosisResultSlots()", e);
    }
}


bool WqimcManager::calibrationTouchScreen()
{
    if (m_isCalibration == false)
    {
        m_isCalibration = true;
        emit CalibrationTouchScreenSignals();
        return true;
    }
    else
    {
        return false;
    }
}

void WqimcManager::CalibrationTouchScreenSlot()
{
    if (false == QProcess::startDetached(Environment::Instance()->GetAppPath().c_str() + QString("/Calibrate.sh")))//启动触摸屏校准启动进程
    {
        m_isCalibration = false;
        logger->debug("触摸屏校准启动失败");
        logger->debug("工作目录 %s", QDir::currentPath().toLatin1().data());
    }
}

bool WqimcManager::catchLogFile(const std::string& urlSpec, const std::string& logFileName, const bool isZip)
{
    bool isExist = false;
    std::unique_lock<std::mutex> lck(m_lockUpLoadFileQueue);
    for (std::list<UpLoadFile>::iterator iter = m_upLoadFileQueue.begin(); iter != m_upLoadFileQueue.end(); iter++)
    {
        UpLoadFile &upLoadFile = *iter;
        if (logFileName == upLoadFile.filePath)
        {
            isExist = true;
            break;
        }
    }
    if (false == isExist)//文件已存在上传队列里，不能上传
    {
        UpLoadFile upLoadFile;
        upLoadFile.filePath = logFileName;
        upLoadFile.isZip = isZip;
        upLoadFile.url = urlSpec;
        m_upLoadFileQueue.push_back(upLoadFile);
        lck.unlock();
        emit UpLoadFileSignal();
        return true;
    }
    else//上传队列里存在文件，会持续上传文件直到队列空，不需要重新发送信号上传文件。
    {
        logger->debug("%s 已在上传队列，不再重复上传", logFileName.c_str());
        return true;
    }
}

void WqimcManager::UpLoadFileSlot()
{
    if (false == m_isUploading)
    {
        m_isUploading = true;
        bool isPostFileRequest = false;

        while (!m_upLoadFileQueue.empty())
        {
            std::unique_lock<std::mutex> lck(m_lockUpLoadFileQueue);
            UpLoadFile &upLoadFile = m_upLoadFileQueue.front();
            lck.unlock();
            QString upLoadFilePath = upLoadFile.filePath.c_str();
            QString urlSpec = upLoadFile.url.c_str();

            QFileInfo fi(upLoadFilePath);

            String QUUIDStr;
            LuaEngine::Instance()->GetLuaValue(m_newState, "config.info.instrument.uuid", QUUIDStr);
            QString str(QUUIDStr.c_str());
            str.remove("{");
            str.remove("}");
            QString upLoadFileName;

            if (fi.suffix() == "zip" || false == upLoadFile.isZip)
            {
                upLoadFileName = str + QString("_") + fi.fileName();
                if (true == httpClient->PostFileRequest(urlSpec, upLoadFileName, upLoadFilePath))
                {
                    isPostFileRequest = true;
                    break;
                }
                else
                {
                    lck.lock();
                    m_upLoadFileQueue.pop_front();
                    lck.unlock();
                    logger->debug("%s 启动上传失败", upLoadFilePath.toStdString().c_str());
                }
            }
            else
            {
                upLoadFileName = str + QString("_") + fi.completeBaseName() + ".zip";
                QString zipFileName = fi.absolutePath() + "/" + fi.completeBaseName() + ".zip";

                if (Uncompress::Compress(upLoadFilePath.toStdString(), zipFileName.toStdString()))
                {
                    if (true == httpClient->PostFileRequest(urlSpec, upLoadFileName, zipFileName, true))
                    {
                        isPostFileRequest = true;
                        break;
                    }
                    else
                    {
                        lck.lock();
                        m_upLoadFileQueue.pop_front();
                        lck.unlock();
                        logger->debug("%s 启动上传失败", upLoadFilePath.toStdString().c_str());
                    }
                }
                else
                {
                    lck.lock();
                    m_upLoadFileQueue.pop_front();
                    lck.unlock();
                    logger->debug("%s 压缩失败，不上传此文件", upLoadFilePath.toStdString().c_str());
                }
            }
        }

        if (false == isPostFileRequest)
        {
            m_isUploading = false;
        }
    }
}

void WqimcManager::UpLoadFinishedSlot()
{
    std::unique_lock<std::mutex> lck(m_lockUpLoadFileQueue);
    QString upLoadFilePath = m_upLoadFileQueue.front().filePath.c_str();
    m_upLoadFileQueue.pop_front();
    lck.unlock();
    m_isUploading = false;
    logger->debug("%s 上传成功", upLoadFilePath.toStdString().c_str());
    UpLoadFileSlot();
}

void WqimcManager::UpLoadErrorSlot(const QString message)
{
    std::unique_lock<std::mutex> lck(m_lockUpLoadFileQueue);
    QString upLoadFilePath = m_upLoadFileQueue.front().filePath.c_str();
    m_upLoadFileQueue.pop_front();
    lck.unlock();
    m_isUploading = false;
    logger->debug("%s 上传失败， 原因：%s", upLoadFilePath.toStdString().c_str(),
                   message.toStdString().c_str());
    UpLoadFileSlot();
}

bool WqimcManager::Download(const System::String& url, const System::String& saveFilePath)
{
    bool isExist = false;
    std::unique_lock<std::mutex> lck(m_lockDownloadFileQueue);
    for (std::list<DownloadFile>::iterator iter = m_downloadFileQueue.begin(); iter != m_downloadFileQueue.end(); iter++)
    {
        DownloadFile &downloadFile = *iter;
        if (saveFilePath == downloadFile.saveFilePath)
        {
            isExist = true;
            break;
        }
    }
    if (false == isExist)
    {
        DownloadFile downloadFile;
        downloadFile.saveFilePath = saveFilePath;
        downloadFile.url = url;
        m_downloadFileQueue.push_back(downloadFile);
        lck.unlock();
        emit DownloadFileSignal();
        return true;
    }
    else
    {
        logger->debug("%s 已在下载队列，不再重复下载", saveFilePath.c_str());
        return true;
    }
}

void WqimcManager::DownloadFileSlot()
{
    if (false == m_isDownloading)
    {
        m_isDownloading = true;
        bool isGetRequest = false;

        while (!m_downloadFileQueue.empty())
        {
            std::unique_lock<std::mutex> lck(m_lockDownloadFileQueue);
            DownloadFile &downloadFile = m_downloadFileQueue.front();
            lck.unlock();

            QString fileName = downloadFile.saveFilePath.c_str();

            if (QFile::exists(fileName))
            {
                if (false == QFile::remove(fileName))
                {
                    lck.lock();
                    m_downloadFileQueue.pop_front();
                    lck.unlock();
                    logger->debug("%s 原文件删除失败，不下载此文件", fileName.toStdString().c_str());
                    continue;
                }
            }
            if (m_downloadFile)
            {
                m_downloadFile->close();
                delete m_downloadFile;
                m_downloadFile = Q_NULLPTR;
            }
            m_downloadFile = OpenFileForWrite(fileName);
            if (!m_downloadFile)
            {
                lck.lock();
                m_downloadFileQueue.pop_front();
                lck.unlock();
                continue;
            }
            if (true == httpClient->DownloadGetRequest(downloadFile.url.c_str(), this, SLOT(DownloadReadyReadSlot(QByteArray)),
                                                        SLOT(DownloadFinishedSlot()), SLOT(DownloadErrorSlot(QString))))
            {
                isGetRequest = true;
                break;
            }
            else
            {
                lck.lock();
                m_downloadFileQueue.pop_front();
                lck.unlock();
                logger->debug("%s 启动下载失败", fileName.toStdString().c_str());
            }
        }

        if (false == isGetRequest)
        {
            m_isDownloading = false;
        }
    }
}

void WqimcManager::DownloadFinishedSlot()
{
    std::unique_lock<std::mutex> lck(m_lockDownloadFileQueue);
    m_downloadFileQueue.pop_front();
    lck.unlock();

    QFileInfo fi;
    if (m_downloadFile)
    {
        fi.setFile(m_downloadFile->fileName());
        m_downloadFile->close();
        delete m_downloadFile;
        m_downloadFile = Q_NULLPTR;
    }
    QFileInfo qFileInfo(fi.absoluteFilePath());
    logger->debug("云平台:下载 %s 文件 %lld 字节",
            fi.absoluteFilePath().toStdString().c_str(), qFileInfo.size());
    m_isDownloading = false;
    DownloadFileSlot();
}

void WqimcManager::DownloadErrorSlot(const QString message)
{
    std::unique_lock<std::mutex> lck(m_lockDownloadFileQueue);
    m_downloadFileQueue.pop_front();
    lck.unlock();

    QFileInfo fi;
    if (m_downloadFile)
    {
        fi.setFile(m_downloadFile->fileName());
        m_downloadFile->close();
        delete m_downloadFile;
        m_downloadFile = Q_NULLPTR;
    }
    QFile::remove(fi.absoluteFilePath());
    logger->debug("云平台:文件下载失败%s：%s",
               fi.absoluteFilePath().toStdString().c_str(),
               message.toStdString().c_str());

    m_isDownloading = false;
    DownloadFileSlot();
}

void WqimcManager::DownloadReadyReadSlot(const QByteArray &data)
{
    if (m_downloadFile)
    {
        m_downloadFile->write(data);
    }
}

void WqimcManager::ChangeHardwareParam(const Interface::Wqimc::Api::Agent::Config config)
{
    emit ChangeHardwareParamSignals(config);
}

void WqimcManager::ChangeHardwareParamSlot(const Interface::Wqimc::Api::Agent::Config config)
{
   LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State *lua = luaEngine->GetThreadState();
    Lua_function call(lua);
    ConfigVector configVector;

    Table viewTable;
    luaEngine->GetLuaValue(lua, "setting.ui.profile.hardwareParamIterms", viewTable);
    bool isUpdater = false;
    oolua_ipairs(viewTable)//类别
    {
        Table typeTable;
        OOLUA::pull(lua, typeTable);
        OOLUA::Lua_func_ref hardwareCategorySet;
        OOLUA::Lua_func_ref hardwareCategoryGet;

        typeTable.at("set", hardwareCategorySet);
        typeTable.at("get", hardwareCategoryGet);

        oolua_ipairs(typeTable)//子项
        {
            Table itermTable;
            OOLUA::Lua_func_ref hardwareItemSet;
            OOLUA::Lua_func_ref hardwareItemGet;
            OOLUA::Lua_func_ref checkValueFunc;

            OOLUA::pull(lua, itermTable);
            itermTable.at("set", hardwareItemSet);
            itermTable.at("get", hardwareItemGet);
            itermTable.at("checkValue", checkValueFunc);

            String name;
            itermTable.at("name", name);
            if (name == config.config)//判断是不是该子项
            {
                String checkValueResult;
                call(checkValueFunc, config.value);//检查值是否符合规则
                OOLUA::pull(lua, checkValueResult);
                if (checkValueResult != config.value)//检查失败退出
                {
                    logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] value格式错误。", config.profile.c_str()
                                  , config.config.c_str(), config.value.c_str());
                    UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                    if (configVector.Size() > 0)
                    {
                        WqimcManager::Instance()->updateConfigs(configVector);
                    }
                    return;
                }

                float value =  QString(config.value.c_str()).toFloat();//设置该值到下位机
                call(hardwareItemSet, value);
                call(hardwareCategorySet);
                String str;
                bool ret = false;
                OOLUA::pull(lua, str);
                OOLUA::pull(lua, ret);
                if (ret == false)//设置失败退出
                {
                    logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] 通信异常。", config.profile.c_str()
                                  , config.config.c_str(), config.value.c_str());
                    UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                    if (configVector.Size() > 0)
                    {
                        WqimcManager::Instance()->updateConfigs(configVector);
                    }
                    return;
                }
                QThread::msleep(250);
                call(hardwareCategoryGet);//检查下位机的值是否等于设置的值
                call(hardwareItemGet);
                float result;
                OOLUA::pull(lua, result);
                if (qAbs(result - value) > 0.000001)//检查失败退出
                {
                    logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] 无法设置到下位机。", config.profile.c_str()
                                  , config.config.c_str(), config.value.c_str());
                    UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                    if (configVector.Size() > 0)
                    {
                        WqimcManager::Instance()->updateConfigs(configVector);
                    }
                    return;
                }
                else
                {
                    Api::Manager::Config  setConfig;
                    setConfig.__set_profile(config.profile);
                    setConfig.__set_config(config.config);
                    setConfig.__set_value(config.value);
                    configVector.Push(setConfig);
                    logger->debug("云平台:修改设置参数profile：%s， config：%s， value：%s 成功", config.profile.c_str()
                                  , config.config.c_str(), config.value.c_str());
                }
                isUpdater = true;
                break;
            }
        }
        oolua_ipairs_end()
        if (isUpdater)
        {
            break;
        }
    }
    oolua_ipairs_end()

    UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
}

void WqimcManager::ExceptionLogOutput(String path, exception &e)
{
    if (OOLUA::Exception* ex = dynamic_cast<OOLUA::Exception* >(&e))
    {
        logger->warn((String("[thrift] ") + path + " => [OOLUA::Exception] %s").c_str(),ex->what());
    }
    else if (TTransportException* ex = dynamic_cast<TTransportException* >(&e))
    {
        m_UploadExceptionCnt++;

        if (m_UploadExceptionCnt <= UPLOADEXCEPTIONMAXCNT)
        {
            logger->warn((String("[thrift] ") + path + " => [TTransportException] %s").c_str(), ex->what());
        }
    }
    else if (TProtocolException* ex = dynamic_cast<TProtocolException* >(&e))
    {
        m_UploadExceptionCnt++;

        if (m_UploadExceptionCnt <= UPLOADEXCEPTIONMAXCNT)
        {
            logger->warn((String("[thrift] ") + path + " => [TProtocolException] %s").c_str(), ex->what());
        }
    }
    else if (TException* ex = dynamic_cast<TException* >(&e))
    {
        m_UploadExceptionCnt++;

        if (m_UploadExceptionCnt <= UPLOADEXCEPTIONMAXCNT)
        {
            logger->warn((String("[thrift] ") + path + " => [TException] %s").c_str(), ex->what());
        }
    }
    else if (std::exception* ex = dynamic_cast<std::exception* >(&e))
    {
        m_UploadExceptionCnt++;

        if (m_UploadExceptionCnt <= UPLOADEXCEPTIONMAXCNT)
        {
            logger->warn((String("[thrift] ") + path + " => [std::exception] %s").c_str(), ex->what());
        }
    }

//    if (m_UploadExceptionCnt >= UPLOADEXCEPTIONMAXCNT)
//    {
//        logger->debug("WqimcManager::ExceptionLogOutput() => Upload exception tirgger Reconnection");
//        this->onDisconnected();
//    }
}

void WqimcManager::ClearExceptioFlag()
{
    m_UploadExceptionCnt = 0;
}


}

}
