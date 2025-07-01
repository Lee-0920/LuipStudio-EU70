#ifndef ADCMANAGER_H
#define ADCMANAGER_H

#include <memory>
#include <map>
#include "ADCDetecter.h"

namespace System
{

class ADCManager
{
public:
    static ADCManager *Instance();
    ~ADCManager();

    bool Init();

    bool Start();
    void Stop();

private:
    static std::unique_ptr<ADCManager> m_instance;

    std::map<int, ADCDetecter*> m_adcDetecters;
};
}

#endif // ADCMANAGER_H
