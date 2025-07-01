#ifndef UPDATEMANAGER_UPDATEMANAGER_H
#define UPDATEMANAGER_UPDATEMANAGER_H

#include "IUpdateProgress.h"
#include "DeviceUpdator.h"

namespace Updater
{

class UpdateManager: public IUpdateProgress
{
public:
    UpdateManager(DscpAddress addr,String fileName, String controllerName, bool isDeleteZip);
    ~UpdateManager();
    void OnProgressed(String status,float times);
    void OnUpdaterTotalTimes(String type, float times);
    DeviceUpdater *m_deviceUpdater;
    String m_controllerName;
};

}


#endif // UPDATEMANAGER_UPDATEMANAGER_H
