#ifndef UDPDEF_H
#define UDPDEF_H

#include "Treasure/SystemDef.h"

namespace UdpSpace
{

#define UDP_MCAST_PORT 37102        //组播端口
#define UDP_MCAST_ADDR "224.0.1.88" //组播地址

#define UDP_NFSSERVER_NOTIFY "@@LABSUN_NFSSERVER_NOTIFY@@"   //NFS服务通知
#define UDP_SEARCH_BUFF      "@@LABSUN_SEARCH_REQUEST@@"     //搜索的请求标识
#define UDP_DEVICE_INFO      "@@LABSUN_DEVICE_RESPOND@@"     //设备响应标识

struct DeviceInfo
{
    int     port;
    QString password;
    QString name;

    //不需要配置,自动赋值
    QString ip;
    int     cononectTime;
};

};

#endif // UDPDEF_H
