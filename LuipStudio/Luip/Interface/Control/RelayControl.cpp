#include "Log.h"
#include "RelayControl.h"

#ifdef _DEV_USING_CONTROL
    #include <stdlib.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <linux/kernel.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <linux/ioctl.h>
#endif

#include "checkscreen.h"
#define DEV_NAME "/dev/Relay"

#define LABSUN_IO(n)     (_IO('z',n))
#define CMD_RELAY_ON     LABSUN_IO(5)
#define CMD_RELAY_OFF    LABSUN_IO(6)

using namespace std;

namespace Interface
{
namespace Control
{

unique_ptr<RelayControl> RelayControl::m_instance(nullptr);

RelayControl* RelayControl::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new RelayControl);
    }

    return m_instance.get();
}

bool RelayControl::Init()
{
    using namespace System;
    if (true == CheckScreen::Instance()->isVNC() || false == CheckScreen::Instance()->isRelay())
    {
        return true;
    }
#ifdef _DEV_USING_CONTROL
    int fd = open(DEV_NAME,O_RDWR);
    if(fd < 0)
    {
//        logger->warn("继电器驱动打开失败");
    }
    ::close(fd);
#endif
    return true;
}

void RelayControl::TurnOn(unsigned long  port)
{
    using namespace System;
    if (true == CheckScreen::Instance()->isVNC() || false == CheckScreen::Instance()->isRelay())
    {
        return;
    }
#ifdef _DEV_USING_CONTROL
    if (port == 2)
    {
        system("echo 27 > /sys/class/gpio/export");
        system("echo out > /sys/class/gpio/gpio27/direction");
        system("echo 1 >/sys/class/gpio/gpio27/value");
    }
    else if(port == 3)
    {
        system("echo 26 > /sys/class/gpio/export");
        system("echo out > /sys/class/gpio/gpio26/direction");
        system("echo 1 >/sys/class/gpio/gpio26/value");
    }


#else
    (void)port;
#endif
}

void RelayControl::TurnOff(unsigned long  port)
{
    using namespace System;
    if (true == CheckScreen::Instance()->isVNC() || false == CheckScreen::Instance()->isRelay())
    {
        return;
    }
#ifdef _DEV_USING_CONTROL
    if (port == 2)
    {
        system("echo 27 > /sys/class/gpio/export");
        system("echo out > /sys/class/gpio/gpio27/direction");
        system("echo 0 >/sys/class/gpio/gpio27/value");
    }
    else if(port == 3)
    {
        system("echo 26 > /sys/class/gpio/export");
        system("echo out > /sys/class/gpio/gpio26/direction");
        system("echo 0 >/sys/class/gpio/gpio26/value");
    }


#else
    (void)port;
#endif
}

}
}
