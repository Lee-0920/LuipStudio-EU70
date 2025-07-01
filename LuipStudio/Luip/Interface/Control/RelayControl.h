#if !defined(INTERFACE_RELAYCONTROL_H)
#define INTERFACE_RELAYCONTROL_H

#include <memory>
#include <list>

using std::list;

namespace Interface
{
namespace Control
{
class RelayControl
{
public:
    static RelayControl *Instance();
    bool Init();
    void TurnOn(unsigned long  port);
    void TurnOff(unsigned long port);

private:
    static std::unique_ptr<RelayControl> m_instance;

};

}
}


#endif // INTERFACE_RELAYCONTROL_H
