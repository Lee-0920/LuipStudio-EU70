#include <QString>
#include "ProgramSegment.h"

namespace Updater
{

ProgramSegment::ProgramSegment(unsigned char *segmentData, int len, unsigned long addr)
{
    address = addr;
    length = len;
    data = (unsigned char*)new unsigned char[len];
    memcpy(data, segmentData, len);
}

ProgramSegment::~ProgramSegment()
{
    if (data)
        delete[] data;
}

unsigned long ProgramSegment::GetAddress(void)
{
    return address;
}

void ProgramSegment::SetAddress(unsigned long addr)
{
    address = addr;
}

unsigned char *ProgramSegment::GetData(void)
{
    return data;
}

int ProgramSegment::GetLength(void)
{
    return length;
}

}
