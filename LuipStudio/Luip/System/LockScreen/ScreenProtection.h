/**
 * @file
 * @brief 屏幕保护控制进程。
 * @details
 * @version 1.0.0
 * @author xingfan
 * @date 2016/10/20
 */

#if !defined(SYSTEM_SCREENPROTECTION_SCREENPROTECTION_H_)
#define SYSTEM_SCREENPROTECTION_SCREENPROTECTION_H_

#include <memory>
#include <QThread>
#include <list>
#include <QWidget>
#include "ILockScreenNotifiable.h"
#include "BanklightControl.h"
#include <QDebug>


using std::list;

namespace System
{
namespace Screen
{

#define TIMER_TIME    200

//屏幕状态枚举值
#define SCREEN_BRIGHT       0
#define SCREEN_DARK         1
#define SCREEN_POWEROFF     2

class Screenprotection:public QThread
{
    Q_OBJECT
public:
    static Screenprotection *Instance();
    void Init(void);
    bool IsFilter(void);
    void Register(ILockScreenNotifiable *handle);

    void SetScreenTime(float darkTime, float closeTime, float loginOffTime);
    void SetSceenEnable(bool isEnable);
    bool SetBacklightByFbBlank(int backlight);

protected :
    void run(void);
signals:
    void ScreenUpdateRequest();
private:

    QPoint m_oldPoint;

    bool m_isFilter;
    bool m_isUserExit;
    unsigned int m_darkTime;
    unsigned int m_powerOffTime;
    unsigned int m_loginOffTime;
    unsigned int m_screenStatus;
    bool m_isScreenProtectionEnable;
    unsigned int m_noOperationTime;

    int m_dynamicKeepingTime;
    bool m_isAuthorization;

    BanklightControl m_banklightControl;
    list<ILockScreenNotifiable*> m_notifise;
    static std::unique_ptr<Screenprotection> m_instance;
};

}
}

#endif /* SYSTEM_SCREENPROTECTION_SCREENPROTECTION_H_ */
