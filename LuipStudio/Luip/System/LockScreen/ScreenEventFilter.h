/**
 * @file
 * @brief 屏幕保护事件过滤器。
 * @details
 * @version 1.0.0
 * @author xingfan
 * @date 2016/10/20
 */

#include <QObject>
#include <QEvent>

#if !defined(SYSTEM_SCREENPROTECTION_SCREENEVENTFILTER_H_)
#define SYSTEM_SCREENPROTECTION_SCREENEVENTFILTER_H_

namespace System
{
namespace Screen
{

class ScreenEventFilter:public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

}
}


#endif /* SYSTEM_SCREENPROTECTION_SCREENEVENTFILTER_H_ */
