#include "currenttest.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include<sys/types.h>

#ifdef _CS_ARM_LINUX
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#endif


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


static const char *device = "/dev/spidev0.0";
static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 30000;
static uint16_t delay = 0;
#define WRITE  0
static void transfer(unsigned int data)
{
    #ifdef _CS_ARM_LINUX
    int ret = 0;
    int fd;

    int ad = data * 4096 / 5;
    printf("\n data = %d, ad = %d", data, ad);

    fd = open(device, O_RDWR);
    if (fd < 0)


    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
            printf("can't set wr spi mode");

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
            printf("can't get spi mode");

    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
            printf("can't set bits per word");

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
            printf("can't get bits per word");

    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
            printf("can't set max speed hz");

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
            printf("can't get max speed hz");



    struct spi_ioc_transfer tr ;
    uint8_t tx[2] = {0};
    uint8_t rx[2] = {0};

//    tx[0] = addr;

    tx[0] = ad / 256;
    tx[1] = ad % 256;


    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = 0;
    tr.len = 2;
    tr.delay_usecs = 0;
    tr.speed_hz = 10000;
    tr.bits_per_word = 8;

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    printf("\n ret = %d", ret);
//    close(device);
#endif
}

CurrentTest::CurrentTest()
{
    system("./spi-flash &");
}
