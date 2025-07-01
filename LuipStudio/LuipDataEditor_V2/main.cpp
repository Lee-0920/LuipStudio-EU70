#include "mainwindow.h"
#include <QApplication>
#include <QSystemSemaphore>
#include "Log.h"
#include "LuaEngine/LuaEngine.h"
#include "oolua_exception.h"
#include "ResultManager/ResultManager.h"
#include "ResultManager/ResultManagerProxy.h"
#include "LuaException.h"
#include <QTextCodec>
#include <QStyleFactory>
#include "Lua/App.h"
#include "Lua/AppProxy.h"
#include "Setting/Environment.h"
#include <QFile>

using namespace Lua;
using namespace OOLUA;
using namespace Result;
using namespace ResultData;
using namespace Configuration;
int main(int argc, char *argv[])
{
    QString runMode = argv[1];
    qDebug("argc %d ,runMode: %s", argc, qPrintable(runMode));

    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // 日志输出初始化
    Log::Init();
    // Lua脚本引擎初始化
    LuaEngine::Instance()->Init(logger);
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Script *lua = LuaEngine::Instance()->GetEngine();

    // Log日志向OOLua注册类
    Log::Proxy(lua);
    ResultManagerProxy::ProxyEnum(lua);
    ResultManagerProxy::ProxyClass(lua);
    AppProxy::Proxy(lua);

    lua->run_chunk("setting = {}");

    //ResultManager::Instance()->Init();
    Environment::Instance()->Init();


        String qfile = Environment::Instance()->GetAppPath() + "/SP/Setting/ResultManager/ResultFileInfo.ls";
        lua->run_file(qfile);

        qfile = Environment::Instance()->GetAppPath() + "/SP/Setting/ResultManager/MeasureFileFormat.ls";
        lua->run_file(qfile);

        qfile = Environment::Instance()->GetAppPath() + "/SP/Setting/ResultManager/CalibrateFileFormat.ls";
        lua->run_file(qfile);

        lua->run_chunk("config = {}");
        lua->run_chunk("config.info = {}");

        qfile = Environment::Instance()->GetAppPath() + "/SP/Config/Default/InstrumentInfomation.ls";
        lua->run_file(qfile);

        ResultManager::Instance()->Init();

//        String qfile = Environment::Instance()->GetAppPath() + "/SP/Common/Helper.ls";
//        lua->run_file(qfile);
//        ResultManager::Instance()->Init();
//        // 打印系统信息
//        logger->notice("*****************************************************");
//        String sn;
//        luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);

//        String model;
//        luaEngine->GetLuaValue(state, "config.info.instrument.model", model);

//        String softwareVersion;
//        luaEngine->GetLuaValue(state, "setting.version.system", softwareVersion);

//        String strInfo = model + " V" + softwareVersion + "  " + sn;
//        logger->notice(strInfo);
//        logger->notice("*****************************************************");
//        logger->debug("Main Thread ID = %d", QThread::currentThreadId());
//        logger->debug("main lua_State[%X]", luaEngine->GetMainState());

    MainWindow w;
    w.show();

    return a.exec();
}
