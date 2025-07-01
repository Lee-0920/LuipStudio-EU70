/**
 * @file
 * @brief 运行状态。
 * @details 
 * @version 1.0.0
 * @author ludijun
 * @date 2016/7/27
 */
#if !defined(STATUSMANAGER_RUNSTATUS_H_)
#define STATUSMANAGER_RUNSTATUS_H_

#include <QDateTime>
#include "System/Types.h"

using namespace System;

namespace Measure
{

/**
 * @brief 运行状态。
 * @details
 */
class RunStatus
{
public:
    RunStatus() {}
    RunStatus(String name, String content, int time = 0):
       time(time)
    {
        this->name = name;
        this->content = content;
        createTime = QDateTime::currentDateTime();
    }

public:
    String name;
    String content;
    int time;
    QDateTime createTime;
};

}

#endif  //STATUSMANAGER_RUNSTATUS_H_
