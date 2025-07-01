#include "ResultDetailManager.h"
#include "ResultDetailPlugin/ResultDetailPlugin.h"
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

namespace ResultDetail
{

unique_ptr<ResultDetailManager> ResultDetailManager::m_instance(nullptr);

ResultDetailManager::~ResultDetailManager()
{

}

ResultDetailManager* ResultDetailManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new ResultDetailManager);
    }

    return m_instance.get();
}


bool ResultDetailManager::Init()
{
    bool ret = false;

    ResultDetailPlugin* resultDetailPlugin = ResultDetailPlugin::Instance();
    resultDetailPlugin->Init(logger);
    return ret;
}

void ResultDetailManager::LoadPlugin()
{
    // 定义插件中的两个导出函数的原型
    typedef ResultDetailPlugin* (*CreatePlugin)();

    Script* lua = LuaEngine::Instance()->GetEngine();
    try
    {
        String qfile = Environment::Instance()->GetAppPath() + "/SP/Setting/ResultDetailPlugins.ls";
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
                     ResultDetailPlugin *plugin = createPlugin();
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
        logger->warn("ResultDetailManager::LoadPlugin() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ResultDetailManager::LoadPlugin() => %s", e.what());
    }
    m_measureResultDetailWidget = nullptr;//如果没有结果详情显示需要，此指针一直为空，界面上根据指针是否为空进行对应控制
    m_calibrateResultDetailWidget = nullptr;
}

void ResultDetailManager::SetMeasureResultDetailWidget(ResultDetailWidget* widget)
{
    m_measureResultDetailWidget = widget;
}

void ResultDetailManager::SetCalibrateResultDetailWidget(ResultDetailWidget* widget)
{
    m_calibrateResultDetailWidget = widget;
}

ResultDetailWidget* ResultDetailManager::GetMeasureResultDetailWidget(void)
{
    return m_measureResultDetailWidget;
}

ResultDetailWidget* ResultDetailManager::GetCalibrateResultDetailWidget(void)
{
    return m_calibrateResultDetailWidget;
}
}
