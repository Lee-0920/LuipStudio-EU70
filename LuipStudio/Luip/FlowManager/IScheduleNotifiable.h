/**
 * @file
 * @brief 测量排期上报接口。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#ifndef MEASURE_ISCHEDULENOTIFIABLE_H_
#define MEASURE_ISCHEDULENOTIFIABLE_H_

#include <QString>
#include <QDateTime>

namespace Flow
{

/**
 * @brief 测量排期上报接口。
 * @details 观察者。
 */
class IScheduleNotifiable
{
public:
    IScheduleNotifiable() {}
    virtual ~IScheduleNotifiable() {}

public:
    virtual void OnScheduler(QString content) = 0;
};


}

#endif  //MEASURE_ISCHEDULENOTIFIABLE_H_
