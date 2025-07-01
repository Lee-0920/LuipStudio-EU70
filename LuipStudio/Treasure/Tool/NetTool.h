#ifndef __TREASURE_TOOL_NETTOOL_H__
#define __TREASURE_TOOL_NETTOOL_H__

#include "Requests.h"
#include <QtNetwork/QNetworkInterface>
#ifdef  _CS_ARM_LINUX
    #include <stdio.h>
    #include <stdlib.h>
    #define TREASURE_NTH_ERNET "eth0"
#else
    #define TREASURE_NTH_ERNET "ethernet"
#endif
#define TREASURE_WLAN "wireless"

namespace Treasure
{

class NetTool
{

public:
    /**
     * @brief 检查网线
     */
    static bool CheckCableStatus()
    {
        QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();

        foreach(auto item, list)
        {
            auto name = item.name();

            #ifdef  _CS_ARM_LINUX
            if (name == TREASURE_NTH_ERNET)
            {
                return item.flags().testFlag(QNetworkInterface::IsRunning);
            }
            #else
            if (name.split("_")[0] == TREASURE_NTH_ERNET)
            {
                if(item.flags().testFlag(QNetworkInterface::IsRunning))
                    return true;
            }
            #endif
        }
        return false;
    }

public:
    class NetEntry
    {
    public:
        enum Type
        {
            None,
            Ethernet,   //以太网
            Wlan,       //无线网
            Virtual,    //虚拟机网
        };

        QString ip;             //IP
        QString netmask;        //掩码
        QString broadcast;      //网关

        Type type;              //类型
        QString name;           //名称 ethernet
        QString desc;           //描述 以太网
        QString hardwareAddress;//物理地址
    };


    /**
     * @brief 获取网络接口
     */
    static QList<NetEntry> GetNetEntrys(NetEntry::Type filter = NetEntry::Type::None)
    {
        QList<NetEntry> rets;
        QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
        foreach(auto intf, list)
        {
            if (!intf.flags().testFlag(QNetworkInterface::IsRunning) ||
                 intf.flags().testFlag(QNetworkInterface::IsLoopBack))
                continue;

            auto name = intf.name();                        //名称
            auto desc = intf.humanReadableName();           //显示描述
            auto hardwareAddress = intf.hardwareAddress();  //物理地址

            foreach(auto entry, intf.addressEntries())
            {
                //过滤无效的IP
                if (entry.broadcast() == QHostAddress::Null ||
                    entry.ip() == QHostAddress::LocalHost ||
                    entry.ip().protocol() != QAbstractSocket::IPv4Protocol)
                    continue;

                NetEntry item;
                item.ip = entry.ip().toString();
                item.netmask = entry.netmask().toString();
                item.broadcast = entry.broadcast().toString();
                item.name = name;
                item.desc = desc;
                item.hardwareAddress = hardwareAddress;
                item.type = NetEntry::Type::None;

                //以太网（虚拟机网络也有这个标识）
                if (name.contains(TREASURE_NTH_ERNET))
                    item.type = NetEntry::Type::Ethernet;

                //无线与虚拟机
                QString cmpstr = desc.toLower();
                if (cmpstr == "wlan" || name.contains(TREASURE_WLAN))
                    item.type = NetEntry::Type::Wlan;
                else if (cmpstr.contains("virtual") ||
                         cmpstr.contains("vmware") ||
                         cmpstr.contains("vethernet"))
                    item.type = NetEntry::Type::Virtual;

                rets.append(item);
            }
        }

        //过滤
        for (int i = rets.size() - 1; i >= 0 && filter != NetEntry::Type::None; i--)
        {
            if (rets[i].type != filter)
                rets.removeAt(i);
        }

        return rets;
    }


public:
    static QString GetSystemTime(QString format = "yyyy-MM-dd hh:mm:ss", int time = 2000)
    {
        auto req = trRequests->Get("http://www.baidu.com", time);
        auto dateString = req->rawHeader("Date");   //Thu, 03 Mar 2022 02:16:06 GMT
        if (!dateString.contains("GMT"))
            return "";

        QLocale locale = QLocale::English;
        QDateTime dateTime = locale.toDateTime(dateString.replace(" GMT", ""), "ddd, dd MMM yyyy hh:mm:ss");
        if (dateTime.isValid())
        {
            dateTime = dateTime.addSecs(28800 - 1);
            return dateTime.toString(format);
        }
        return "";
    }
};


}

#endif // NETTOOL_H
