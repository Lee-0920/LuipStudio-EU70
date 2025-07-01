#include "UpdateFlow.h"
#include "Controller/ControllerManager.h"
#include "Log.h"
#include "AlarmManager/AlarmManager.h"
#include "System/Exception.h"
#include <QThread>
#include <QDebug>
#include "System/Uncompress.h"

using namespace Updater;
using namespace Controller;
using namespace Measure;

namespace Flow
{

UpdateFlow::UpdateFlow()
{

}

UpdateFlow::~UpdateFlow()
{
    if (!m_updateManager.empty())
    {
        list<UpdateManager*>::iterator it;
        for (it = m_updateManager.begin(); it != m_updateManager.end(); it++)
        {
            if (*it)
                delete (*it);
        }
    }
    if (!m_masterUpdate.empty())
    {
        list<MasterUpdate*>::iterator itx;
        for (itx = m_masterUpdate.begin(); itx != m_masterUpdate.end(); itx++)
        {
            if (*itx)
                delete (*itx);
        }
    }
}
void UpdateFlow::OnStarted()
{
    ControllerManager::Instance()->StopAllSignalUpload();
    logger->debug("Stop Signal Upload");
}

void UpdateFlow::Run()
{
    for (list<UpdateManager*>::iterator iter = m_updateManager.begin(); iter != m_updateManager.end(); iter++)
    {
        if (false == (*iter)->m_deviceUpdater->Update())
        {
            throw UpdateException((*iter)->m_controllerName.c_str());
        }
        else
        {
            String str = (*iter)->m_controllerName + "升级成功";
            logger->info(str);
        }
    }
    if (m_updateManager.size() > 0) //没有升级下位机时，加快升级上位机的速度
    {
        QThread::sleep(2);
    }
    list<MasterUpdate*>::iterator itx;
    for (itx = m_masterUpdate.begin(); itx != m_masterUpdate.end(); itx++)
    {
        if (false == (*itx)->Update())
        {
            throw UpdateException("主控板");
        }
    }
}

void UpdateFlow::OnStopped()
{
    for (list<UpdateManager*>::iterator iter = m_updateManager.begin(); iter != m_updateManager.end(); iter++)
    {
        Uncompress::DeleteDirectory((*iter)->m_deviceUpdater->GetUnzipDestDirectory().c_str());
    }
    list<MasterUpdate*>::iterator itx;
    for (itx = m_masterUpdate.begin(); itx != m_masterUpdate.end(); itx++)
    {
        Uncompress::DeleteDirectory((*itx)->GetUnzipDestDirectory().c_str());
    }
    ControllerManager::Instance()->StartAllSignalUpload();
    logger->debug("Start Signal Upload");
}

void UpdateFlow::AddStep(UpdateManager *updateManager)
{
    m_updateManager.push_back(updateManager);
}

void UpdateFlow::AddStep(MasterUpdate *masterUpdate)
{
    m_masterUpdate.push_back(masterUpdate);
}
}
