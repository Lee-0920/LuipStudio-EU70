#ifndef UPDATER_IUPDATERESULT_H
#define UPDATER_IUPDATERESULT_H

#include <QString>
namespace ControlNet
{
namespace Updater
{


class IUpdateResult
{
public:
    IUpdateResult() {}
    virtual ~IUpdateResult() {}

public:
    virtual void OnResult(QString result) = 0;
};


}
}



#endif // UPDATER_IUPDATEPROGRESS_H
