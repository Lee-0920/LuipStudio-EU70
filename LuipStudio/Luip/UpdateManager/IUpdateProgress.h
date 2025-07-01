#ifndef UPDATER_IUPDATEPROGRESS_H
#define UPDATER_IUPDATEPROGRESS_H

#include <QString>
#include "System/Types.h"

using System::String;

namespace Updater
{

class IUpdateProgress
{
public:
    IUpdateProgress() {}
    virtual ~IUpdateProgress() {}

public:
    virtual void OnProgressed(String status,float times) = 0;
    virtual void OnUpdaterTotalTimes(String type,float times) = 0;
};

}




#endif // UPDATER_IUPDATEPROGRESS_H
