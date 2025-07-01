/**
 * @file
 * @brief 屏幕保护控制进程。
 * @details
 * @version 1.0.0
 * @author xingfan
 * @date 2016/10/20
 */
#include <QThread>
#include <QFile>
#include <QTextStream>
#include "UI/Frame/LoginDialog.h"
#include "ScreenProtection.h"
#include "LuaEngine/LuaEngine.h"

#ifdef    _CS_ARM_LINUX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#endif


using namespace std;
using namespace UI;
using namespace Lua;

namespace System
{
namespace Screen
{

#define MAX(a,b) (a)>(b)?(a):(b)
#define SYSFS_BRIGHTNESS            "/sys/class/backlight/backlight/brightness"
#define SYSFS_DUTYPERCENT           "/sys/class/pwm/ecap.2/duty_percent"
#define SYSFS_BACKLIGHT_FB_BLANK    "/sys/class/backlight/pwm-backlight/fb_blank"
#define SYSFS_BACKLIGHT_POWER       "/sys/class/backlight/backlight/bl_power"//设置屏幕电源开关的文件

unique_ptr<Screenprotection> Screenprotection::m_instance(nullptr);

/**
 * @brief 获取全局唯一的 Screenprotection 对象（单件）。
 */
Screenprotection* Screenprotection::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new Screenprotection);
    }

    return m_instance.get();
}

void Screenprotection::Init(void)
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();


    bool enable = true;
    luaEngine->GetLuaValue(state, "config.system.screenSaver.enable", enable);
    float darkTime;
    luaEngine->GetLuaValue(state, "config.system.screenSaver.darkTime", darkTime);
    float powerOffTime;
    luaEngine->GetLuaValue(state, "config.system.screenSaver.offTime", powerOffTime);
    float loginOffTime;
    luaEngine->GetLuaValue(state, "config.system.screenSaver.loginKeepingTime", loginOffTime);

    bool isAuthorization = false;
    luaEngine->GetLuaValue(state, "config.producerAuthorization.enable", isAuthorization);

    m_banklightControl.SetPower(0);//开启电源
//    if( 0 == access(SYSFS_BACKLIGHT_FB_BLANK,0) )
//    {
//        m_banklightControl.SetFbBlank(0);
//    }
    m_banklightControl.SetBrightness(7);

    m_oldPoint = QCursor::pos();
    m_screenStatus = SCREEN_BRIGHT;
    m_isUserExit = false;
    m_noOperationTime = 0;
    m_isFilter = false;
    m_dynamicKeepingTime = 1800*1000; //动态登录保持时间
    m_isAuthorization = isAuthorization;
    SetSceenEnable(enable);
    SetScreenTime(darkTime,
                  powerOffTime,
                  loginOffTime);
    this->start();//启动屏保线程

}


/**
 * @brief 获取是否启动事件滤波，启动则实现锁屏效果
 * @return
 */
bool Screenprotection::IsFilter(void)
{
    return m_isFilter;
}

void Screenprotection::Register(ILockScreenNotifiable *handle)
{
    m_notifise.push_back(handle);
}

void Screenprotection::run(void)
{
    QPoint currentPoint;
    unsigned int maxWaitTime = 0;
    while(1)
    {   
        try
        {
            msleep(TIMER_TIME);
#ifdef    _CS_ARM_LINUX
            currentPoint = QCursor::pos();
            maxWaitTime = MAX(m_loginOffTime,m_powerOffTime);
            if( currentPoint == m_oldPoint )    //屏幕未点击
            {   
                if( m_noOperationTime <=  maxWaitTime ) //超出范围不再自加，防止越界
                {
                    m_noOperationTime += TIMER_TIME;
                }
            }
            else    //屏幕被点击
            {
                m_noOperationTime = 0;    
            }
            m_oldPoint = currentPoint;

            if(0 == m_noOperationTime)    //屏幕被点击，如果屏幕未亮则点亮屏幕，解开锁屏,用户退出标志复位
            {
                if(SCREEN_BRIGHT != m_screenStatus)
                {
                    if(true == m_banklightControl.SetBrightness(7))
                    {
                        if(true == m_banklightControl.SetBrightness(7))
                        {
                            m_screenStatus = SCREEN_BRIGHT;
                        }
                    }
                }
                if(true == m_isFilter)
                {
                    m_isFilter = false;
                }
                m_isUserExit = false; //用户登录复位
            }
            else //屏幕未点击
            {
                if(false == m_isUserExit)
                {
                    if(false == m_isAuthorization && LoginDialog::userType == Super)
                    {
                        if( m_noOperationTime >= m_loginOffTime || m_noOperationTime >= m_dynamicKeepingTime )
                        {
                            m_isUserExit = true;
                            for(std::list<ILockScreenNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
                            {
                               if (*it != NULL)
                               {
                                    (*it)->OnLockScreen();
                               }
                            }
                        }
                    }
                    else
                    {
                        if( m_noOperationTime >= m_loginOffTime && LoginDialog::userType > General)
                        {
                            m_isUserExit = true;
                            for(std::list<ILockScreenNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
                            {
                               if (*it != NULL)
                               {
                                    (*it)->OnLockScreen();
                               }
                            }
                        }
                    }
                }

                if(true == m_isScreenProtectionEnable)
                {
                    switch(m_screenStatus)
                    {
                        case SCREEN_BRIGHT: // 如果亮屏未操作超出暗屏时间，打暗屏幕
                            if(m_noOperationTime > m_darkTime)
                            {
                                if(true == m_banklightControl.SetBrightness(5))
                                {
                                    if(true == m_banklightControl.SetBrightness(5))
                                    {
                                        m_isFilter = true;
                                        m_screenStatus = SCREEN_DARK;
                                    }
                                }
                            }
                            break;
                        case SCREEN_DARK: // 如果暗屏未操作超出关屏时间，关闭屏幕
                            if(m_noOperationTime > m_powerOffTime)
                            {
                                if(true == m_banklightControl.SetBrightness(0))
                                {
                                    if(true == m_banklightControl.SetBrightness(0))
                                    {
                                        m_isFilter = true;
                                        m_screenStatus = SCREEN_POWEROFF;
                                    }
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }  
            }
#endif
        }
        catch(std::exception e)
        {
            logger->notice("Screenprotection::run()-> %s",e.what());
        }
    }
}


void Screenprotection::SetScreenTime(float darkTime, float powerOffTime, float loginOffTime)
{
    m_darkTime = darkTime * 1000;
    m_powerOffTime = powerOffTime * 1000;
    m_loginOffTime = loginOffTime * 1000;
    if(m_powerOffTime <= m_darkTime) //如果屏幕变暗大于全黑时间，则全黑时间改为变暗时间加TIMER_TIME
    {
        m_powerOffTime = m_darkTime + TIMER_TIME;
    }
}

void Screenprotection::SetSceenEnable(bool isEnable)
{
    if (isEnable != m_isScreenProtectionEnable)
    {
        m_isScreenProtectionEnable = isEnable;
    }
}

//通过写brightness属性以及fb_blank属性来控制背光
bool Screenprotection::SetBacklightByFbBlank(int backlight)
{
#ifdef    _CS_ARM_LINUX
    char readBuf[10] = {0};
    char cmpBuf[10]= {0};
    char fb_blank[] = "0";
    char brightness[] = "100";
    char duty_percent[] = "100";
    memset(fb_blank,0,sizeof(fb_blank));
    memset(brightness,0,sizeof(brightness));
    memset(duty_percent,0,sizeof(duty_percent));
    switch(backlight)
    {
    case SCREEN_BRIGHT:
	    emit ScreenUpdateRequest();
        strcpy(fb_blank,"0");
        strcpy(brightness,"100");
        strcpy(duty_percent,"100");
        break;
    case SCREEN_DARK:
        strcpy(fb_blank,"0");
        strcpy(brightness,"1");
        strcpy(duty_percent,"21");
        break;
    case SCREEN_POWEROFF:
        strcpy(fb_blank,"1");
        strcpy(brightness,"0");
        strcpy(duty_percent,"0");
        break;
    default:
        break;

    }
    strcpy(cmpBuf,duty_percent);       
    strcat(cmpBuf,"\n");

    //写属性fb_blank或者bl_power
    int fd = 0;
    if( 0 == access(SYSFS_BACKLIGHT_FB_BLANK,0) )
    {
        fd = open(SYSFS_BACKLIGHT_FB_BLANK, O_RDWR|O_APPEND|O_NONBLOCK|O_SYNC);
    }
    else
    {
    	fd = open(SYSFS_BACKLIGHT_POWER, O_RDWR|O_APPEND|O_NONBLOCK|O_SYNC);
    }
    if(-1 == fd)
    {
        logger->debug("%s open failed.",SYSFS_BACKLIGHT_FB_BLANK);
        return false;
    }
    int ret = write(fd,fb_blank,strlen(fb_blank));
    if(-1 == ret)
    {
        logger->debug("%s write failed.",SYSFS_BACKLIGHT_FB_BLANK);
        close(fd);
        return false;
    }
    close(fd);

    //写属性brightness
    fd = open(SYSFS_BRIGHTNESS, O_RDWR|O_APPEND|O_NONBLOCK|O_SYNC);
    if(-1 == fd)
    {
        logger->debug("%s open failed.",SYSFS_BRIGHTNESS);
        return false;
    }
    ret = write(fd,brightness,strlen(brightness));
    if(-1 == ret)
    {
        logger->debug("%s write failed.",SYSFS_BRIGHTNESS);
        close(fd);
        return false;
    }
    close(fd);
    msleep(TIMER_TIME);

    //读属性duty_percent，确定写入生效了
    fd = open(SYSFS_DUTYPERCENT,O_RDONLY|O_NONBLOCK);
    if(-1  == fd)
    {
        logger->debug("%s open failed.",SYSFS_DUTYPERCENT);
        return false;
    }
    ret = read(fd,readBuf,sizeof(readBuf));
    if(-1 == ret)
    {
        logger->debug("%s read failed.",SYSFS_DUTYPERCENT);
        close(fd);
        return false;
    }
    close(fd);
    if(0 != strcmp(cmpBuf,readBuf))
    {
        logger->debug("%s %s write not useful.",SYSFS_BRIGHTNESS,backlight);
        return false;
    }
#endif
    return true;
}

}
}
