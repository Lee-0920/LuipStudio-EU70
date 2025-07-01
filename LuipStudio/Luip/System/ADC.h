#ifndef ADC_H
#define ADC_H

#include "System/Types.h"
using System::String;
using System::Uint16;

namespace System
{
class ADC
{
public:
    explicit ADC(String driveFile);
    ~ADC();
    Uint16 ReadValue();

private:
    int m_fd;
};
}



#endif // ADC_H
