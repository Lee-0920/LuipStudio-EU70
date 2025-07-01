#ifndef UPDATER_DEVICEUPDATOR_H
#define UPDATER_DEVICEUPDATOR_H

#include "Communication/Dscp/DscpAddress.h"
#include <QString>
#include "ControllerPlugin/API/DeviceUpdateInterface.h"
#include "IUpdateProgress.h"
#include "System/Types.h"

using Controller::API::DeviceUpdateInterface;
using Communication::Dscp::DscpAddress;


namespace Updater
{

class DeviceUpdater
{
public:
    DeviceUpdater(DscpAddress addr, System::String fileName, IUpdateProgress *progress, bool isDeleteZip);
    ~DeviceUpdater();
    bool Update(void);
    System::String GetFileName(void);
    String GetUnzipDestDirectory(void);

private:
    DeviceUpdateInterface *m_updateInterface;
    System::String m_fileName;
    IUpdateProgress *m_progress;
    bool m_isDeleteZip;
    String m_destDirectory;
};

}




#endif // UPDATER_DEVICEUPDATOR_H
