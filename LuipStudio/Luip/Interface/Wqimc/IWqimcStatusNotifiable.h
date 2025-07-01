#if !defined(WQIMC_WQIMCSTATUSNOTIFIABLE_H_)
#define WQIMC_WQIMCSTATUSNOTIFIABLE_H_

namespace Interface
{
namespace Wqimc
{
enum  class WqimcStatus
{
    Connected = 0,
    Disconnected = 1,
};

/**
 * @brief 结果上报接口。
 * @details 观察者。
 */
class IWqimcStatusNotifiable
{
public:
    IWqimcStatusNotifiable() {}
    virtual ~IWqimcStatusNotifiable() {}

public:
    virtual void OnChangeStatus(WqimcStatus wqimcStatus) = 0;
};

}
}

#endif  //WQIMC_WQIMCSTATUSNOTIFIABLE_H_
