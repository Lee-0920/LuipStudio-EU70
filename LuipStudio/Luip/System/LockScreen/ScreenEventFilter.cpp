/**
 * @file
 * @brief 屏幕保护事件过滤器。
 * @details
 * @version 1.0.0
 * @author xingfan
 * @date 2016/10/20
 */

#include <QDebug>
#include "ScreenEventFilter.h"
#include "ScreenProtection.h"
#include "Buzzer.h"

namespace System
{
namespace Screen
{

bool ScreenEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    int  eventType = event->type();


    if(eventType== QEvent::MouseButtonPress ||
       eventType== QEvent::MouseButtonDblClick
        )//判断事件类型是否是点击触摸屏所产生类型
    {
        if(true == Screenprotection::Instance()->IsFilter())//判断屏保是否启动过滤
        {
            return true;
        }
        else
        {
            Buzzer::Instance()->Beep();
        }
    }
    else if(eventType == QEvent::MouseMove ||
            eventType == QEvent::MouseButtonRelease ||
            eventType == QEvent::FocusIn ||
            eventType == QEvent::FocusOut ||
            eventType == QEvent::FocusAboutToChange)//判断事件类型是否是点击触摸屏所产生类型
    {
        if(true == Screenprotection::Instance()->IsFilter())//判断屏保是否启动过滤
        {
            return true;
        }
    }
    return QObject::eventFilter(obj,event);
}

}
}
