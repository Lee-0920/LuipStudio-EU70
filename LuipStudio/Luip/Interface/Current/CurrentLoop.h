#ifndef CURRENTLOOP_H
#define CURRENTLOOP_H

#include<memory>
#include "System/Types.h"
using System::String;
namespace Interface{namespace Current{
enum class ChipType
{
     DAC8750 = 0,
     LTC1446 = 1,
};

class CurrentLoop
{
public:
    explicit CurrentLoop(String driveFile);
    ~CurrentLoop();

    ChipType GetChipType();

    void OutPut(float current);
    void OutPut(float currentA, float currentB);
private:
    uint16_t Transfer(uint8_t addr, uint16_t data);    void Transfer(uint32_t data);
    uint16_t Read(uint8_t readaddr);

    ChipType m_chip;

    int m_fd;
};

typedef std::shared_ptr<CurrentLoop> CurrentLoopPtr;
}
}


#endif // CURRENTLOOP_H
