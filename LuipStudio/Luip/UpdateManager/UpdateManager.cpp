#include <QObject>
#include "StatusManager/StatusManager.h"
#include "UpdateManager.h"

using namespace Measure;

namespace Updater
{
UpdateManager::UpdateManager(DscpAddress addr, String fileName, String controllerName, bool isDeleteZip):
    m_controllerName(controllerName)
{
    m_deviceUpdater = new DeviceUpdater(addr, fileName, this, isDeleteZip);
}

UpdateManager::~UpdateManager()
{
    if (m_deviceUpdater)
        delete m_deviceUpdater;
}

void UpdateManager::OnProgressed(String status,float times)
{
   StatusManager *statusManager = StatusManager::Instance();

   if("Erase" == status)
   {
       RunAction action(QObject::tr("Erase").toStdString(), QObject::tr("擦除").toStdString(), (int)times);
       statusManager->SetAction(action);
   }
   else if("WriteProgram" == status)
   {
       RunAction action(QObject::tr("WriteProgram").toStdString(), QObject::tr("烧写").toStdString(), (int)times);
       statusManager->SetAction(action);
   }
}

void UpdateManager::OnUpdaterTotalTimes(String type, float times)
{
    String updateStr = m_controllerName + "升级";
    RunStatus status(type, updateStr, (int)times);
    StatusManager::Instance()->SetStatus(status);
}
}
