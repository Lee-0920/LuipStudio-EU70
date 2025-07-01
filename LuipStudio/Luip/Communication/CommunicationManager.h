/**
 * @file
 * @brief 控制网络。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */


#if !defined(COMMUNICATION_MANAGER_H_)
#define COMMUNICATION_MANAGER_H_

#include <memory>
#include <QObject>
#include "Communication/eCek/DNCP/Lai/IConnectStatusNotifiable.h"

namespace Communication
{

/**
 * @brief 控制网络。
 * @details
 */
class CommunicationManager : public IConnectStatusNotifiable
{
public:
    ~CommunicationManager();
    static CommunicationManager* Instance();
    void Proxy();
    bool LoadAddrScript();
    bool Init();
    void Uninit();
    void OnConnectStatusChanged(unsigned char pollingAddr, bool status);

private:
    static std::unique_ptr<CommunicationManager> m_instance;
};

}

#endif  //COMMUNICATION_MANAGER_H_
