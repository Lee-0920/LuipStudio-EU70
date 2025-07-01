/**
 * @file
 * @brief 锁屏上报接口。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#ifndef SYSTEM_ILOCKSCREENNOTIFIABLE_H_
#define SYSTEM_ILOCKSCREENNOTIFIABLE_H_


namespace System
{

/**
 * @brief 锁屏上报接口。
 * @details 观察者。
 */
class ILockScreenNotifiable
{
public:
    ILockScreenNotifiable() {}
    virtual ~ILockScreenNotifiable() {}

public:
    virtual void OnLockScreen() = 0;
};


}

#endif  //SYSTEM_ILOCKSCREENNOTIFIABLE_H_
