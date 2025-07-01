#include <QApplication>
#include <QSystemSemaphore>
#include "Log.h"
#include "Communication/CommunicationManager.h"
#include "Controller/ControllerManager.h"
#include "SignalManager/SignalManager.h"
#include "SignalManager/SignalManagerProxy.h"
#include "Setting/Environment.h"
#include "LuaEngine/LuaEngine.h"
#include "oolua_exception.h"
#include "Setting/SettingManager.h"
#include "Setting/SettingManagerProxy.h"
#include "ResultManager/ResultManager.h"
#include "ResultManager/ResultManagerProxy.h"
#include "AlarmManager/AlarmManager.h"
#include "AlarmManager/AlarmManagerProxy.h"
#include "StatusManager/StatusManagerProxy.h"
#include "StatusManager/StatusManager.h"
#include "LuaException.h"
#include "UI/Frame/LoginDialog.h"
#include <QTextCodec>
#include <QStyleFactory>
#include "UI/Frame/MainWindow.h"
#include "UI/UIProxy.h"
#include "Lua/AppProxy.h"
#include "System/Watchdogclient.h"
#include "FlowManager/FlowManager.h"
#include "FlowManager/MeasureScheduler.h"
#include "Interface/Control/RelayControl.h"
#include "Interface/Control/WaterCollector.h"
#include "Interface/Current/CurrentResultManager.h"
#include "Interface/Current/CurrentResultOutputer.h"
#include "Interface/Current/CurrentLoop.h"
#include "Interface/Current/CurrentResultManager.h"
#include "Interface/CCEP/CCEPInterface.h"
#include "Interface/HJ212/HJ212Interface.h"
#include "Interface/Modbus/ModbusManager.h"
#include "Interface/Wqimc/WqimcManager.h"
#include "Lua/LoadScript.h"
#include "Controller/ControllerManagerProxy.h"
#include "FlowManager/FlowManagerProxy.h"
#include "Interface/ExternalInterfaceProxy.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "RecordDataPlugin/RecordDataProxy.h"
#include "ResultDetailManager/ResultDetailManager.h"
#include "ResultDetailManager/ResultDetailManagerProxy.h"
#include "PrinterManager/PrinterManager.h"
#include "PrinterManager/PrinterManagerProxy.h"
#include "System/ADCManager.h"
#include <QFile>
#include "Lua/App.h"
#include "outage.h"
#include "Interface/Current/currenttest.h"
#include "UdpPlugin/UdpManager.h"
#include "System/NFSManager.h"
#include "System/PlatformInfo.h"
#include "checkscreen.h"
#include<fstream>
#include "HJ212Manager/HJ212Manager.h"
#include "HJ212Plugin/HJ212PluginProxy.h"
#include "DataBasePlugin/DataBaseManager.h"
#include "ConnectionManager/Server.h"
#include "ConnectionManager/Client.h"
#include "System/ProLicenseManager.h"

using namespace System;

#ifdef _CS_ARM_LINUX
#include "System/LockScreen/ScreenEventFilter.h"
using namespace System::Screen;
#endif

using namespace Lua;
using namespace UI;
using namespace Communication;
using namespace Controller;
using namespace Configuration;
using namespace OOLUA;
using namespace Measure;
using namespace Result;
using namespace Flow;
using namespace Interface::Current;
using namespace Interface::Control;
using namespace Interface::Modbus;
using namespace Interface::CCEP;
using namespace Interface::HJ212;
using namespace ResultData;
using namespace ResultDetail;
using namespace PrinterSpace;
using namespace Interface::Wqimc;
using namespace HJ212;
using namespace DataBaseSpace;

int main(int argc, char *argv[])
{
    bool  linuxDebugMode = false;
    QString runMode = argv[1];
#ifdef _CS_ARM_LINUX
    CheckScreen::Instance()->Init();
    if (runMode == "-s")
    {
        linuxDebugMode = true;
    }

    if (CheckScreen::Instance()->isVNC())
    {
        argc = 3;
        argv[1] = "-platform";
        argv[2] = "vnc:size=800x600:mmsize=203x152";
        qDebug("argc %d", argc);
        for(int i = 0; i < argc; i++)
        {
            qDebug("argv-%d:%s", i, argv[i]);
        }
    }
    else
    {
        argc = 4;
        argv[1] = "-qws";
        argv[2] = "-font";
        argv[3] = "unifont";
    }

    system("hwclock -s");
#endif
    runMode = argv[1];
    qDebug("argc %d ,runMode: %s", argc, qPrintable(runMode));
//    system("wr /opt/Calibrate &");
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    Environment::Instance()->Init();
    QString appDataPath = QString::fromStdString(Environment::Instance()->GetAppDataPath());

    if (argc == 2 && runMode == "-v") //获取版本信息，并输出
    {
        Script *lua = new Script;;

        String qfile = Environment::Instance()->GetAppPath() + "/SP/Setting/Version.ls";
        lua->run_file(qfile);

        String software;
        lua->run_chunk("return setting.version.system");
        lua->pull(software);
        printf("%s", software.c_str());
        return 0;
    }
    else
    {
        Environment::Instance()->CreateFolder();
        // 加载QSS样式表
        QApplication::setStyle("Fusion");
        QFile qss(":/qss/wqimc.qss");
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
        qss.close();

#ifdef _CS_ANDROID
        QFont font("Droid Sans Fallback");
        font.setStretch(50);
        a.setFont(font);
#else

#ifdef _CS_X86_WINDOWS
        QFont font("SimHei");
        a.setFont(font);
#else
        QFont font("Droid Sans Fallback");
        a.setFont(font);
#endif

        // 只运行一个进程
        QSystemSemaphore semaphore("Luip",1, QSystemSemaphore::Open);
        QSharedMemory sharedMemory;
        sharedMemory.setKey(QString("LuipStudio"));
        if (sharedMemory.attach())
        {
                qDebug("Shared memory attach fail!");
                semaphore.release();
                return 0;
        }


        if (sharedMemory.create(1))
#endif
        {
            // 日志输出初始化
            Log::Init();

#ifdef _CS_ARM_LINUX
            logger->debug("Linux操作系统运行时长:  %ld",  App::GetOSRunTime());


            // 主控板软件升级结果信息
            using namespace std;
            fstream inFile;
            inFile.open("/opt/Updaterlog.txt");
            string str;
            getline(inFile, str);
            if (str == "Succeed")
            {
                logger->info("主控板软件升级成功");
                system("echo log > /opt/Updaterlog.txt");
            }
            else if (str == "log")
            {

            }
            else
            {
                logger->info("主控板软件升级失败");
                system("echo log > /opt/Updaterlog.txt");
            }
#endif

            // Lua脚本引擎初始化
            LuaEngine::Instance()->Init(logger);
            LuaEngine* luaEngine = LuaEngine::Instance();
            lua_State* state = luaEngine->GetThreadState();

            try
            {
                Script *lua = LuaEngine::Instance()->GetEngine();

                // Log日志向OOLua注册类
                Log::Proxy(lua);
                // Lua系统应用相关的类向OOLua注册
                AppProxy::Proxy(lua);
                // 通信管理器向OOLua注册类
                CommunicationManager::Instance()->Proxy();
                // 加载通信地址配置脚本文件，初始化通信地址
                CommunicationManager::Instance()->LoadAddrScript();
                // 控制器管理器向OOLua注册类,初始化
                ControllerManager::Instance()->Init();
                // 加载各个驱动控制插件（动态库）
                ControllerManager::Instance()->LoadPlugin();
                // 详情插件管理器初始化
                ResultDetailManager::Instance()->Init();
                // 加载详情插件
                ResultDetailManager::Instance()->LoadPlugin();
                // 打印机插件管理器初始化
                PrinterManager::Instance()->Init();
                // 加载打印机插件
                PrinterManager::Instance()->LoadPlugin();

                SettingManagerProxy::Proxy(lua);
                ResultManagerProxy::ProxyEnum(lua);
                AlarmManagerProxy::Proxy(lua);
                SignalManagerProxy::Proxy(lua);
                StatusManagerProxy::Proxy(lua);
                StatusManagerProxy::ProxyEnum(lua);
                UIProxy::Proxy(lua);
                LuaExceptionProxy::Init(lua);

                ControllerManagerProxy::Proxy(lua);
                FlowManagerProxy::Proxy(lua);
                ExternalInterfaceProxy::Proxy(lua);
                HJ212PluginProxy::Proxy(lua);

                // 加载Lua初始化的Init.ls脚本文件
                LoadScript::Instance()->LoadInitScript();

                String qfile = Environment::Instance()->GetAppPath() + "/SP/Common/Helper.ls";
                lua->run_file(qfile);

                SettingManager::Instance()->Init();
                StatusManager::Instance()->Init();
                ResultManager::Instance()->Init();
                ResultManagerProxy::ProxyClass(lua);
                RecordDataProxy::Proxy();
                ResultDetailManagerProxy::Proxy(lua);
                PrinterManagerProxy::Proxy(lua);
                // 打印系统信息
                logger->notice("*****************************************************");
                String sn;
                luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);

                String model;
                luaEngine->GetLuaValue(state, "config.info.instrument.model", model);

                String softwareVersion;
                luaEngine->GetLuaValue(state, "setting.version.system", softwareVersion);

                String strInfo = model + " V" + softwareVersion + "  " + sn + " " + System::PlatformVersion;
                logger->notice(strInfo);
                logger->notice("*****************************************************");
                logger->debug("Main Thread ID = %d", QThread::currentThreadId());
                logger->debug("main lua_State[%X]", luaEngine->GetMainState());

                // 加载驱动控制相关脚本
                LoadScript::Instance()->LoadPluginScript();
                // 加载仪器相关的业务脚本
                LoadScript::Instance()->LoadInstrumentScript();
                // 内部通信管理器初始化通信层的线程及通信处理业务，
                CommunicationManager::Instance()->Init();

                // 特殊升级标识处理
                if (argc == 2 && runMode == "-R")
                {
                    String lsFile = Environment::Instance()->GetUpdatePath() + "/LS";
                    String oemFile = Environment::Instance()->GetUpdatePath() + "/OEM";
                    String diyFile = Environment::Instance()->GetUpdatePath() + "/DIY";
                    if(QFile::exists(QString::fromStdString(lsFile)))
                    {
                        logger->debug("Handle LS Update Flag");
                        luaEngine->SetLuaValue("config.info.instrument.manufactureType", (int)ManufactureType::LS);
                        SettingManager::Instance()->InstrumentInfoSave();
                    }
                    else if(QFile::exists(QString::fromStdString(oemFile)))
                    {
                        logger->debug("Handle OEM Update Flag");
                        luaEngine->SetLuaValue("config.info.instrument.manufactureType", (int)ManufactureType::OEM);
                        SettingManager::Instance()->InstrumentInfoSave();
                    }
                    else if(QFile::exists(QString::fromStdString(diyFile)))
                    {
                        logger->debug("Handle DIY Update Flag");
                        luaEngine->SetLuaValue("config.info.instrument.manufactureType", (int)ManufactureType::DIY);
                        SettingManager::Instance()->InstrumentInfoSave();
                    }
                }
            }
            catch(OOLUA::LuaException e)
            {
                logger->warn("main() => %s", e.What().c_str());
            }
            catch(OOLUA::Exception e)
            {
                logger->warn("main() => %s", e.what());
            }
            catch (std::exception e)
            {
                logger->warn("main() => %s", e.what());
            }
            // 各个驱动器复位
//            ControllerManager::Instance()->DeviceReset();
            // 4-20管理器初始化
            CurrentResultManager::Instance()->Init();
            // 多功能继电器初始化
            RelayControl::Instance()->Init();
            // 采水继电器初始化
            WaterCollector::Instance()->Init();
            // 后台运行Lua解析器
            LuaEngine::Instance()->RunParser(argc, argv);
            // 初始化界面中用户自定义的配置
            LoadScript::Instance()->InitWidgetUserConfig();
            //用户权限初始化
            bool debugMode;
            luaEngine->GetLuaValue(state, "config.system.debugMode", debugMode);
            if (linuxDebugMode || (debugMode == true  && SettingManager::Instance()->IsProducerStatus()))
            {
                LoginDialog::userType = Super;
            }
            else if (CheckScreen::Instance()->isVNC())
            {
                LoginDialog::userType = Administrator;
            }
            else
            {
                LoginDialog::userType = General;
            }
            if(ProLicenseManager::Instance()->GetProLicense())
            {
                LoginDialog::userType == Super;
            }

#ifdef _CS_X86_WINDOWS
            LoginDialog::userType = RoleType::Super;
#endif
            //数据库初始化
            DataBaseManager::Instance()->Init(appDataPath, trLogger);
            //启动后台监控软件进程间通信线程。
            WatchdogClient::Instance()->Init();
            // 更新界面管理器初始化
            UpdateWidgetManager::Instance()->Init();
            QTranslator translator;
            Language language = SettingManager::Instance()->GetAppLanguage();
            if(language == Language::TraditionalChinese)
            {
                translator.load("Translate/Luip_CT", a.applicationDirPath());
                a.installTranslator(&translator);
            }
            else if(language == Language::English)
            {
                translator.load("Translate/Luip_EN", a.applicationDirPath());
                a.installTranslator(&translator);

            #ifdef _CS_X86_WINDOWS
                QFont en_font("Cambria", 12);
            #else
                QFont en_font("Luxi Serif", 12);
            #endif

                a.setFont(en_font);
            }
#ifdef _CS_ARM_LINUX

#endif
            // UI界面显示
            MainWindow w;
            w.show();

            // 读取当前状态
            StatusManager::Instance()->ResetDeviceStatus();
            // 下位机通信连接检查
            ControllerManager::Instance()->DeviceConnectCheck();
            //清空事件池
            EventHandler::Instance()->EmptyEvents();
            // 信号管理器初始化，对各个控制插件信号上报进行初始化
            SignalManager::Instance()->Init();
            //设置控制器信号上报周期
            ControllerManager::Instance()->SetResetNotice();
            ControllerManager::Instance()->StartAllSignalUpload();
            // 初始化仪器相关的业务脚本
            LoadScript::Instance()->InitInstrument();
            // 测量流程管理器初始化
            FlowManager::Instance()->Init();
            //WQIMC服务器连接初始化，上传信息
            WqimcManager::Instance()->Init();
            QThread::msleep(200);
            //Modbus初始化，启动RS48、RS232的Modbus线程
            ModbusManager::Instance()->Init();
            QThread::msleep(200);
            //CCEP串口初始化，启动串口线程
            //CCEPInterface::Instance()->Init();
            //QThread::msleep(200);
            //HJ212初始化
            HJ212Manager::Instance()->Init();
            // ADC检测初始化
            ADCManager::Instance()->Init();
            //启动测量排期线程
            MeasureScheduler::Instance()->Init();
            //启动服务器连接
//            qDebug() << "Current thread ID:" << QThread::currentThreadId();
            ConnectionSpace::ServerThread server;
            server.start();

#ifdef _CS_ARM_LINUX
            // UDP广播
            if (CheckScreen::Instance()->isVNC())
            {
                {
                    String type;
                    luaEngine->GetLuaValue(state, "config.info.instrument.type", type);

                    UdpSpace::DeviceInfo device;
                    device.port = 5900;
                    device.password = "";
                    device.name = type.c_str();
                    UdpSpace::UdpManager::Instance()->InitByClient(trLogger, device);
                }
                NFSManager::Instance()->Init();
            }
#endif

            //设置初始化完成标志
            ControllerManager::Instance()->SetInitOverFlag(true);
            Outage t;
            t.show();
#ifdef _CS_ARM_LINUX
            //安装事件过滤器
            ScreenEventFilter* screenEventFilter = new ScreenEventFilter;
            a.installEventFilter(screenEventFilter);
#endif
            int status = a.exec();
            CommunicationManager::Instance()->Uninit();
            LuaEngine::Instance()->Uninit();
            if (CheckScreen::Instance()->isVNC())
            {
                UdpSpace::UdpManager::Instance()->Uninit();
                NFSManager::Instance()->Uninit();
            }
            DataBaseManager::Instance()->Uninit();


            sharedMemory.detach();
            semaphore.release();

            logger->notice("应用程序退出的状态： %d", status);
            return status;

        }
    }
}
