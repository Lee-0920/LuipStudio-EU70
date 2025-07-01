#include "PrinterManager.h"
#include "PrinterPlugin/PrinterPlugin.h"
#include "Log.h"
#include "LuaEngine/LuaEngine.h"
#include "oolua.h"
#include "Setting/Environment.h"
#include <QLibrary>

using namespace std;
using namespace OOLUA;
using namespace Lua;
using namespace System;
using namespace Configuration;

namespace PrinterSpace
{

std::unique_ptr<PrinterManager> PrinterManager::m_instance(nullptr);

PrinterManager::~PrinterManager()
{

}

PrinterManager* PrinterManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new PrinterManager);
    }

    return m_instance.get();
}


bool PrinterManager::Init()
{
    bool ret = false;

    PrinterPlugin* printerPlugin = PrinterPlugin::Instance();
    printerPlugin->Init(logger);

    return ret;
}

void PrinterManager::LoadPlugin()
{
    // 定义插件中的两个导出函数的原型
    typedef PrinterPlugin* (*CreatePlugin)();

    Script* lua = LuaEngine::Instance()->GetEngine();
    try
    {
        String qfile = Environment::Instance()->GetAppPath() + "/SP/Setting/PrinterPlugins.ls";
        lua->run_file(qfile);

        Table table;
        lua->pull(table);

        String pluginName;
        table.at("name", pluginName);

        if (!pluginName.empty())//当产品不使用结果详情时，加载的名字为空
        {
            QString libName;

            #ifdef _CS_X86_WINDOWS
                libName = QString(pluginName.c_str());
            #elif _CS_ARM_LINUX
            libName = QString((Environment::Instance()->GetAppPath() + "/Plugin/lib/" + pluginName).c_str());
            #endif

            QLibrary lib(libName);
            if (lib.load())
            {
                // 解析导出函数
                CreatePlugin createPlugin = (CreatePlugin) lib.resolve("CreatePlugin");
                if (createPlugin)
                {
                     PrinterPlugin *plugin = createPlugin();
                    if (plugin)
                    {
                        plugin->Init(logger);
                    }
                }
            }
            else
            {
                logger->warn("%s DLL load error", libName.toLatin1().data());
            }
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("PrinterManager::LoadPlugin() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("PrinterManager::LoadPlugin() => %s", e.what());
    }
    m_measureDataPrinter = nullptr;
    m_calibrateDataPrinter = nullptr;
}

void PrinterManager::SetMeasureDataPrinter(Printer* printer)
{
    m_measureDataPrinter = printer;
}

void PrinterManager::SetCalibrateDataPrinter(Printer* printer)
{
    m_calibrateDataPrinter = printer;
}

Printer* PrinterManager::GetMeasureDataPrinter(void)
{
    return m_measureDataPrinter;
}

Printer* PrinterManager::GetCalibrateDataPrinter(void)
{
    return m_calibrateDataPrinter;
}
}
