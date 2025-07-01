#include <QFile>
#include <QDataStream>
#include "UserChangeManager.h"
using namespace std;
namespace UI {

unique_ptr<MaintainUserChange> MaintainUserChange::m_instance(nullptr);

MaintainUserChange* MaintainUserChange::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new MaintainUserChange);
    }

    return m_instance.get();
}

void MaintainUserChange::Register(IUserChangeNotifiable *handle)
{
    m_notifise.push_back(handle);
}

void MaintainUserChange::UpdateUserInterfaceResult()
{
    for(std::list<IUserChangeNotifiable* >::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnUserChange();
        }
    }
}


}

