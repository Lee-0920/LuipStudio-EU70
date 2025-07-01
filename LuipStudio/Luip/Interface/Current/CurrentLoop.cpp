#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <QString>
#include "CurrentLoop.h"
#include <QThread>
#ifdef _DEV_USING_CURRENT
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#endif
#include "Log.h"
#include <QProcess>
#include "Log.h"
#include "Common.h"
#include "CurrentResultManager.h"
#include "System/Types.h"
#include "LuaEngine/LuaEngine.h"



using namespace System;

namespace Interface
{
namespace Current
{
#ifdef _DEV_USING_CURRENT
    #define SPI_MODE   SPI_MODE_0
    #define SPI_BITS   8
    #define SPI_SPEED  100000
    #define SPI_DELAY  0

    #define DAC_NOP                         0
    #define DAC_DATA_REGISTER               0x01
    #define READ_REGISTER                   0x02
    #define CONTROL_REGISTER                0x55
    #define RESET_REGISTER                  0x56
    #define CONFIGURATION_REGISTER          0x57

    #define READ_STATUS_REGISTER            0x0
    #define READ_DACDATA_REGISTER           0X01
    #define READ_CONTROL_REGISTER           0x02
    #define READ_CONFIGURATION_REGISTER     0x0B

    //使能输出,SRCLK 6.9K,SRSTEP 128LSB,使能转换频率 电流输出范围4mA-20mA
    #define DAC8750_MODE                    0x1CF5
#endif

CurrentLoop::CurrentLoop(String driveFile)
    :m_chip(ChipType::DAC8750)
{
#ifdef _DEV_USING_CURRENT
    int ret = 0;
    uint8_t mode = SPI_MODE;
    uint8_t bits = SPI_BITS;
    uint32_t speed = SPI_SPEED;

    const char *device = driveFile.c_str();

    m_fd = open(device, O_RDWR);
    if(m_fd < 0)
    {
        qDebug(" %s can't open device", device);
        logger->debug(" %s can't open device", device);
    }
    else
    {
        qDebug(" %s open device.fd %d", device, m_fd);
        /*
         * spi mode
         */
        ret = ioctl(m_fd, SPI_IOC_WR_MODE, &mode);
        if (ret == -1)
        {
            qDebug("can't set spi mode");
            logger->debug("can't set spi mode");
        }

        ret = ioctl(m_fd, SPI_IOC_RD_MODE, &mode);
        if (ret == -1)
        {
            qDebug("can't get spi mode");
            logger->debug("can't get spi mode");
        }

        /*
         * bits per word
         */
        ret = ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1)
        {
            qDebug("can't set bits per word");
            logger->debug("can't set bits per word");
        }

        ret = ioctl(m_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1)
        {
            qDebug("can't get bits per word");
            logger->debug("can't get bits per word");
        }

        /*
         * max speed hz
         */
        ret = ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1)
        {
            qDebug("can't set max speed hz");
            logger->debug("can't set max speed hz");
        }

        ret = ioctl(m_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        if (ret == -1)
        {
            qDebug("can't get max speed hz");
            logger->debug("can't get max speed hz");
        }

//        qDebug("spi mode: %d\n", mode);
//        qDebug("bits per word: %d\n", bits);
//        qDebug("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
        if(DAC8750_MODE == Read(READ_CONTROL_REGISTER))
        {
            m_chip = ChipType::DAC8750;
            qDebug("Select DAC8750 Chip");
            logger->debug("Select DAC8750 Chip");
        }
        else
        {
            Transfer(CONTROL_REGISTER, DAC8750_MODE);
            if(DAC8750_MODE != Read(READ_CONTROL_REGISTER))
            {
                m_chip = ChipType::LTC1446;
                qDebug("Select LTC1446 Chip");
                logger->debug("Select LTC1446 Chip");
            }
        }
    }
#else
    (void)driveFile;
#endif
}

CurrentLoop::~CurrentLoop()
{
#ifdef _DEV_USING_CURRENT
    qDebug(" fd %d close",m_fd);
    ::close(m_fd);
#endif
}

ChipType CurrentLoop::GetChipType()
{
        return m_chip;
}

uint16_t CurrentLoop::Transfer(uint8_t addr, uint16_t data)
{
#ifdef _DEV_USING_CURRENT
    struct spi_ioc_transfer tr ;
    int ret;
    uint8_t tx[3] = {0};
    uint8_t rx[3] = {0};
    uint16_t rxData;

    tx[0] = addr;
    tx[1] = data / 256;
    tx[2] = data % 256;

    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = 3;
    tr.delay_usecs = SPI_DELAY;
    tr.speed_hz = SPI_SPEED;
    tr.bits_per_word = SPI_BITS;

    ret = ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        qDebug("can't send spi message");
    }
    rxData = (uint16_t)rx[1] * 256 + rx[2];
//    for (ret = 0; ret < 3; ret++)
//    {
//        qDebug("0x%.2X ", rx[ret]);
//    }
    return rxData;
#else
    (void)addr;
    (void)data;
    return 0;
#endif
}

void CurrentLoop::Transfer(uint32_t data)
{
#ifdef _DEV_USING_CURRENT
    struct spi_ioc_transfer tr ;
    int ret;
    uint8_t tx[3] = {0};
    uint8_t rx[3] = {0};

    uint8_t dataBuff[4] = {0};
    memcpy(dataBuff, &data, sizeof(uint32_t));

    tx[0] = dataBuff[2];
    tx[1] = dataBuff[1];
    tx[2] = dataBuff[0];

    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = 3;
    tr.delay_usecs = SPI_DELAY;
    tr.speed_hz = SPI_SPEED;
    tr.bits_per_word = SPI_BITS;

    ret = ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        qDebug("can't send spi message");
    }

#else
    (void)data;
#endif
}

uint16_t CurrentLoop::Read(uint8_t readaddr)
{
#ifdef _DEV_USING_CURRENT
    Transfer(READ_REGISTER, readaddr);
    return Transfer(DAC_NOP, DAC_NOP);
#else
    (void)readaddr;
    return 0;
#endif
}

void CurrentLoop::OutPut(float current)
{
#ifdef _DEV_USING_CURRENT
    int ret = 0;
    if(current < 4)
    {
        current = 4;
    }
    else if(current > 20)
    {
        current = 20;
    }

        ostringstream oss;

        oss<<current;

        string str(oss.str());

        QProcess::startDetached(QString("wr /opt/luip/spi-flash ")   + str.c_str()/*+ QString(strCurrent)*/);
//    uint16_t adValue = (current- 4) * 1.0  / 16 * 65535;
//    qDebug("current = %f, AD = %d", current, adValue);
//    while(ret < 3)
//    {
//        Transfer(DAC_DATA_REGISTER, adValue);
//        QThread::msleep(100);
//        if(adValue == Read(READ_DACDATA_REGISTER))
//        {
//            qDebug("Output\n");
//            break;
//        }
//        ret++;
//    }
#else
    (void)current;
#endif
}

void CurrentLoop::OutPut(float currentA, float currentB)
{
#ifdef _DEV_USING_CURRENT
    if(currentA < 4)
    {
        currentA = 4;
    }
    else if(currentA > 20)
    {
        currentA = 20;
    }

    if(currentB < 4)
    {
        currentB = 4;
    }
    else if(currentB > 20)
    {
        currentB = 20;
    }

    uint32_t adA = ((uint32_t)((currentA*0.2)*(4095/4.095))) & 0x00000fff;
    uint32_t adB = ((uint32_t)((currentB*0.2)*(4095/4.095))) & 0x00000fff;

    uint32_t adValue = adA << 12 | adB;
    Transfer(adValue);

    qDebug("currentA = %f, currentB = %f, adA = %d, adB = %d, AD = %d", currentA, currentB, adA, adB, adValue);
#else
    (void)currentA;
    (void)currentB;
#endif
}

}
}
