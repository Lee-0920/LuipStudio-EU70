#include "outage.h"
#include <QDebug>
#include "Log.h"
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<sys/types.h>
#include<signal.h>
#include "FlowManager/IFlowNotifiable.h"
#include "oolua.h"
#include "LuaEngine/LuaEngine.h"
#include "FlowManager/FlowManager.h"
using namespace Flow;
using namespace OOLUA;
using namespace Lua;

static bool m_flag = false;
static bool m_backlight = TRUE;

Outage::Outage(QObject *parent) : QObject(parent)
{
    system("echo 134 > /sys/class/gpio/export");
    system("echo in > /sys/class/gpio/gpio134/direction");      //初始化断电检测IO口
}

void Outage::show()
{
    QTimer *timer = new QTimer();
    connect( timer, SIGNAL(timeout()), this, SLOT(time()) );
    timer->start(10);           //启动断电检测

#ifdef _CS_ARM_LINUX
    using namespace std;        //打印触摸屏校准参数
    fstream inFile;
    inFile.open("/etc/pointercal");
    string str;
    while(getline(inFile, str))
    {
        logger->debug("触摸屏校准参数：" + str);
    }
#endif
}

void Outage::time()
{
    using namespace std;
    string c;
    fstream inFile;
    inFile.open("/sys/class/gpio/gpio134/value");
    inFile >> c;

    if(c == "1")
    {
        if(m_flag == false)
        {
            m_flag = true;
            bool isCorrectStopFlow = true;
            LuaEngine* luaEngine = LuaEngine::Instance();
            lua_State * state = luaEngine->GetThreadState();
            Table itermsTable;
            luaEngine->GetLuaValue(state, "status.running", itermsTable);
            if (itermsTable.safe_at("isCorrectStopFlow", isCorrectStopFlow))
            {
                luaEngine->SetLuaValue("status.running.isCorrectStopFlow", false);
            }

            FlowManager::Instance()->StopFlow();
            system("echo 0 > /sys/class/backlight/backlight/brightness");
            logger->debug("启动断电保护：流程停止,保存数据");
            system("sync");
            m_backlight = FALSE;
        }
    }
    else if(m_backlight == FALSE)
    {
        bool isCorrectStopFlow = true;
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();
        Table itermsTable;
        luaEngine->GetLuaValue(state, "status.running", itermsTable);
        if (itermsTable.safe_at("isCorrectStopFlow", isCorrectStopFlow))
        {
            luaEngine->SetLuaValue("status.running.isCorrectStopFlow", true);
        }
        system("echo 7 > /sys/class/backlight/backlight/brightness");
        logger->debug("电压恢复，点亮屏幕");
        m_backlight = TRUE;
        m_flag = false;
    }
    else
    {
        m_flag = false;
    }
}

void Outage::stopFlow()
{

}

