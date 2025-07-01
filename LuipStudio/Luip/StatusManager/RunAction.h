/**
 * @file
 * @brief 运行动作。
 * @details 
 * @version 1.0.0
 * @author ludijun
 * @date 2016/7/27
 */
#if !defined(STATUSMANAGER_RUNACTION_H_)
#define STATUSMANAGER_RUNACTION_H_

#include <QDateTime>
#include "System/Types.h"

using namespace System;

namespace Measure
{

/**
 * @brief 运行动作。
 * @details
 */
class RunAction
{
public:
    RunAction() {}
    RunAction(String name, String content, int time = 0):
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

#endif  //STATUSMANAGER_RUNACTION_H_
