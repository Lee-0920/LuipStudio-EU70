/**
 * @file
 * @brief AD信号上报接口。
 * @version 1.0.0
 * @author ludijun
 * @date 2016/07/22
 */

#ifndef CONTROLLER_ISIGNALNOTIFIABLE_H_
#define CONTROLLER_ISIGNALNOTIFIABLE_H_

#include "System/Types.h"
#include "LuipShare.h"

using System::String;

namespace Controller
{

/**
 * @brief AD信号上报接口。
 * @details 观察者。
 */
class LUIP_SHARE ISignalNotifiable
{
public:
    ISignalNotifiable() {}
    virtual ~ISignalNotifiable() {}

public:
    virtual void OnSignalChanged(String name, double value) = 0;
};


}

#endif  //CONTROLLER_ISIGNALNOTIFIABLE_H_
