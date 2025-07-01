#if !defined(SIGNALMANAGER_IUPDATESIGNALNOTIFIABLE_H_)
#define SIGNALMANAGER_IUPDATESIGNALNOTIFIABLE_H_

#include <QString>

/**
 * @brief 结果上报接口。
 * @details 观察者。
 */
class IUpdateSignalNotifiable
{
public:
    IUpdateSignalNotifiable() {}
    virtual ~IUpdateSignalNotifiable() {}

public:
    virtual void OnUpdateSignal(void) = 0;
};


#endif  //SIGNALMANAGER_IUPDATESIGNALNOTIFIABLE_H_
