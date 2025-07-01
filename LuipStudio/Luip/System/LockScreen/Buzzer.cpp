/*
 * BeepControl.cpp
 *
 *  Created on: 2016年11月15日
 *      Author: liangliwen
 */

#include "Buzzer.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <QDebug>

#ifdef _DEV_USING_CURRENT
#include <sys/ioctl.h>
#define DEV_NAME "/dev/labsun"
#define LABSUN_IO(n)     (_IO('z',n))
#define CMD_BUZZER_SWITCH_ON       LABSUN_IO(3)
#define CMD_BUZZER_SWITCH_OFF      LABSUN_IO(4)
#define CMD_BUZZER_BEEP            LABSUN_IO(5)
#endif

namespace System
{
namespace Screen
{
std::unique_ptr<Buzzer> Buzzer::m_instance(nullptr);

Buzzer *Buzzer::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new Buzzer);
    }
    return m_instance.get();
}


Buzzer::Buzzer(QWidget *parent):
    QWidget(parent)
{
    m_QProcess = 0;
#ifdef _DEV_USING_CURRENT
    m_QProcess = new QProcess();
#endif
}

Buzzer::~Buzzer()
{
#ifdef _DEV_USING_CURRENT
    delete m_QProcess;
#endif
}

void Buzzer::Beep(void)
{
#ifdef _DEV_USING_CURRENT
    if(QProcess::NotRunning == m_QProcess->state())
    {
        m_QProcess->start("./beep.sh");
    }
#endif
}

}

}


