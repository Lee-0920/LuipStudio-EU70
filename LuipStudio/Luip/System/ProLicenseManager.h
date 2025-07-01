#ifndef ProLicenseManager_H
#define ProLicenseManager_H

#include <memory>
#include <QDate>
#include "System/Types.h"

namespace System
{

enum class ProLicenseEnable
{
    Unsupported = -1,
    Invalid,
    Effective,
};

class ProLicenseManager
{
public:
    ~ProLicenseManager();

    static ProLicenseManager* Instance();
    bool GetProLicense();
    void SetProLicense(bool);
    int IsProLicenseEnable();

protected:
    bool Check();

private:
    static std::unique_ptr<ProLicenseManager> m_instance;
    bool m_isAlarm;

};

}

#endif // ProLicenseManager_H
