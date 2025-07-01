#include <QThread>
#include "App.h"
#include <QUuid>
#include "System/LockScreen/ScreenProtection.h"
#include "UI/Frame/LoginDialog.h"
#include "Log.h"
#ifdef _CS_ARM_LINUX
    #include <sys/sysinfo.h>
#endif

namespace Lua
{

App::App()
{

}

void App::Sleep(int ms)
{
    QThread::msleep(ms);
}

System::String App::CreateUUID(void)
{
    return QUuid::createUuid().toString().toStdString();
}

void App::SetScreenTime(float darkTime, float closeTime, float userExitTime)
{
    System::Screen::Screenprotection::Instance()->SetScreenTime(darkTime, closeTime, userExitTime);
}

void App::SetSceenEnable(bool isEnable)
{
    System::Screen::Screenprotection::Instance()->SetSceenEnable(isEnable);
}

int App::GetCurrentRole()
{
    return (int)UI::LoginDialog::userType;
}

System::String App::GetCurrentUserName()
{
    System::String str = UI::LoginDialog::userInfo.userName.toStdString();
    return str;
}

System::String App::GetCurrentLevelName()
{
    System::String str = UI::LoginDialog::userInfo.levelName.toStdString();
    return str;
}

System::String App::GetCurrentRoleStr()
{
    System::String str;
    switch(UI::LoginDialog::userType)
    {
        case UI::General:
            str = "普通用户";
            break;
        case UI::Maintain:
            str = "运维员";
            break;
        case UI::Administrator:
            str = "管理员";
            break;
        case UI::Super:
            str = "超级管理员";
            break;
        case UI::None:
        default:
            break;
    }

    return str;
}

long App::GetOSRunTime()
{
    long runTime = -1;

#ifdef _CS_ARM_LINUX
    struct sysinfo info;

    if (sysinfo(&info))
    {
        logger->warn("Failed to get sysinfo, errno:%u, reason:%s",errno, strerror(errno));
        return -1;
    }

    runTime = info.uptime;
#endif

    return runTime;
}

}
