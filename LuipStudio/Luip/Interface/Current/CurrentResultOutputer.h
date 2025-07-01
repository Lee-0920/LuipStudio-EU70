#ifndef CURRENTRESULTOUTPUTER_H
#define CURRENTRESULTOUTPUTER_H
#include "System/Types.h"
#include "CurrentLoop.h"

using namespace std;
using System::String;

namespace Interface
{
namespace Current
{
class CurrentResultOutputer
{
public:
    explicit CurrentResultOutputer(CurrentLoopPtr currentLoop);
    float ComputeCurrent(float c4Val, float c20Val, float result);
    void Output(float current);
    void Output(float currentA, float currentB);
private:

    CurrentLoopPtr m_currentLoop;
};
}
}
#endif // CURRENTRESULTOUTPUTER_H
