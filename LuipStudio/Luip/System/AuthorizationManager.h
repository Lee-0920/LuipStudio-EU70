#ifndef AuthorizationManager_H
#define AuthorizationManager_H

#include <memory>
#include <QDate>
#include "System/Types.h"

namespace System
{

enum class ReagentAuthorizationEnable
{
    Unsupported = -1,
    Invalid,
    Effective,
};

class AuthorizationManager
{
public:
    ~AuthorizationManager();

    static AuthorizationManager* Instance();
    String GetValidity();
    bool GetAuthorization();
    void SetAuthorization(Uint32 expirationDate);
    bool GetReagentAuthorization();
    void SetReagentAuthorization(Uint32 expirationDate);
    int IsReagentAuthorizationEnable();
    QDate GetReagentAuthorizationExpirationDate();

protected:
    bool Check();
    bool CheckReagentAuthorization();

private:
    static std::unique_ptr<AuthorizationManager> m_instance;
    bool m_isAlarm;

};

}

#endif // AuthorizationManager_H
