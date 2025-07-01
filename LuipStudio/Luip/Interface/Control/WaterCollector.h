#ifndef WATERCOLLECTOR_H
#define WATERCOLLECTOR_H

#include <memory>

using namespace std;

namespace Interface
{
namespace Control
{
class WaterCollector
{
public:
    static WaterCollector *Instance();
    void Init();
    void TurnOn();
    void TurnOff();
    bool GetStatus() const;
private:
    static std::unique_ptr<WaterCollector> m_instance;
    static bool m_status;

};

}
}
#endif // WATERCOLLECTOR_H
