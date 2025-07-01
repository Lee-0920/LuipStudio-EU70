#ifndef UI_USERCHANGEMANAGER_USERCHANGEMANAGER_H
#define UI_USERCHANGEMANAGER_USERCHANGEMANAGER_H
#include <memory>
#include <list>
#include "IUserChangeNotifiable.h"

using std::list;

namespace UI {

/**
 * @brief 运维人员UI更新管理器
 */
class MaintainUserChange
{
public:
    static MaintainUserChange *Instance();
    void Register(IUserChangeNotifiable *handle);
    void UpdateUserInterfaceResult();

private:
   list<IUserChangeNotifiable* > m_notifise;

   static std::unique_ptr<MaintainUserChange> m_instance;
};
}


#endif // UI_USERCHANGEMANAGER_USERCHANGEMANAGER_H
