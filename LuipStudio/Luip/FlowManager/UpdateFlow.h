#ifndef FLOWMANAGER_UPDATEFLOW_H
#define FLOWMANAGER_UPDATEFLOW_H
#include "CFlow.h"
#include "UpdateManager/UpdateManager.h"
#include "UpdateManager/MasterUpdate.h"

namespace Flow
{
class UpdateFlow:
        public CFlow
{
public:
    UpdateFlow();
    ~UpdateFlow();
    void OnStarted();
    void Run();
    void OnStopped();
    void AddStep(Updater::UpdateManager *updateManager);
    void AddStep(Updater::MasterUpdate *masterUpdate);

private:
    list<Updater::UpdateManager*> m_updateManager;
    list<Updater::MasterUpdate*> m_masterUpdate;
};
}
#endif // FLOWMANAGER_UPDATEFLOW_H
