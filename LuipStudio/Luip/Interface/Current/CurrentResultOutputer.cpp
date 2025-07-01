#include "CurrentResultOutputer.h"
#include "CurrentLoop.h"
#include<QDebug>
namespace Interface{
namespace Current{
CurrentResultOutputer::CurrentResultOutputer(CurrentLoopPtr currentLoop)
                      :m_currentLoop(currentLoop)
{
}

float CurrentResultOutputer::ComputeCurrent(float c4Val, float c20Val, float result)
{
    float currentValue = 0;

    float k = 16/(c20Val-c4Val);
    float b = 4-k*c4Val;
    currentValue = k*result + b;

    return currentValue;
}

void CurrentResultOutputer::Output(float current)
{
    m_currentLoop->OutPut(current);
}

void CurrentResultOutputer::Output(float currentA, float currentB)
{
    m_currentLoop->OutPut(currentA, currentB);
}

}
}
