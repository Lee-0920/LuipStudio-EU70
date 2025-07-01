#ifndef UPDATER_PROGRAMSEGMENT_H
#define UPDATER_PROGRAMSEGMENT_H

namespace Updater
{
class ProgramSegment
{
public:
    ProgramSegment(unsigned char *segmentData, int len, unsigned long addr);
    ~ProgramSegment();
    unsigned long GetAddress(void);
    void SetAddress(unsigned long addr);
    unsigned char *GetData(void);
    int GetLength(void);
private:
    unsigned long address;
    int length;
    unsigned char *data;
};

}

#endif // UPDATER_PROGRAMSEGMENT_H
