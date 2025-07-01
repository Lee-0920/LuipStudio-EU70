#include "WaterCollector.h"
#include "Log.h"
#include "checkscreen.h"

#ifdef _DEV_USING_CONTROL
    #include <stdlib.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <linux/kernel.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <linux/ioctl.h>
#endif

#define DEV_NAME "/dev/Relay"
#define SWITCH_PORT 1

#define LABSUN_IO(n)     (_IO('z',n))
#define CMD_RELAY_ON     LABSUN_IO(5)
#define CMD_RELAY_OFF    LABSUN_IO(6)

using namespace std;

namespace Interface
{
namespace Control
{

unique_ptr<WaterCollector> WaterCollector::m_instance(nullptr);
bool WaterCollector::m_status = false;

WaterCollector *WaterCollector::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new WaterCollector);
    }
    return m_instance.get();
}

void WaterCollector::Init()
{
    #ifdef _DEV_USING_CONTROL
    #endif
}

void WaterCollector::TurnOn()
{
    using namespace System;
    if (true == CheckScreen::Instance()->isVNC() || false == CheckScreen::Instance()->isRelay())
    {
        return;
    }
    #ifdef _DEV_USING_CONTROL

    system("echo 22 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio22/direction");
    system("echo 1 >/sys/class/gpio/gpio22/value");

    m_status = true;
    #endif
}

void WaterCollector::TurnOff()
{
    using namespace System;
    if (true == CheckScreen::Instance()->isVNC() || false == CheckScreen::Instance()->isRelay())
    {
        return;
    }
    #ifdef _DEV_USING_CONTROL

    system("echo 22 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio22/direction");
    system("echo 0 >/sys/class/gpio/gpio22/value");

    m_status = false;
    #endif
}

bool WaterCollector::GetStatus() const
{
    return m_status;
}

}
}
