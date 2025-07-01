/**
 * @file
 * @brief AD信号上报接口。
 * @version 1.0.0
 * @author ludijun
 * @date 2016/07/22
 */

#ifndef CONTROLLER_IACQUIRESIGNALNOTIFIABLE_H_
#define CONTROLLER_IACQUIRESIGNALNOTIFIABLE_H_

#include "LuipShare.h"

namespace Controller
{

/**
 * @brief AD信号上报接口。
 * @details 观察者。
 */
class LUIP_SHARE IAcquireSignalNotifiable
{
public:
    IAcquireSignalNotifiable() {}
    virtual ~IAcquireSignalNotifiable() {}

public:
    virtual void OnAcquireADChanged(unsigned long ref,unsigned long mea) = 0;
};


}

#endif  //CONTROLLER_IACQUIRESIGNALNOTIFIABLE_H_
