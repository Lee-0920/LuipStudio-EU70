#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "NetSet.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include <QList>
#include <QNetworkInterface>
#include "UI/Frame/MessageDialog.h"
#include "LuaEngine/LuaEngine.h"
#include "Interface/Wqimc/WqimcManager.h"
#include<iostream>
#include<fstream>

#ifdef  _CS_ARM_LINUX
#include <stdio.h>
#include <stdlib.h>
#define NETWORKNAME "eth0"
#else
#define NETWORKNAME "ethernet_10"
#endif

#define NDSFile "/etc/resolv.conf"

using namespace Configuration;
using namespace Lua;

namespace UI
{

NetSet::NetSet(QWidget *parent):
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this);

    QFont font;                           //字体
    font.setPointSize(12);                //字体大小

    this->resize(650,400);

    DHCPRadio = new QRadioButton(this);
    DHCPRadio->setObjectName("radiobuttonone");

    QLabel *dhcpLabel = new QLabel();
    dhcpLabel->setText(tr("DHCP自动分配"));
    dhcpLabel->setFixedWidth(200);
    dhcpLabel->setFont(font);
    QHBoxLayout *dhcpLayout = new QHBoxLayout();
    dhcpLayout->addWidget(DHCPRadio);
    dhcpLayout->addWidget(dhcpLabel);
    dhcpLayout->addStretch();

    netAddrLabel = new QLabel();
    netAddrLabel->setText(tr("网络地址 ："));
    netAddrLabel->setFixedWidth(240);
    netAddrLabel->setFixedHeight(30);
    netAddrLabel->setFont(font);

    subnetLabel = new QLabel();
    subnetLabel->setText(tr("子网掩码 ："));
    subnetLabel->setFixedWidth(240);
    subnetLabel->setFixedHeight(30);
    subnetLabel->setFont(font);

    gateWayLabel = new QLabel();
    gateWayLabel->setText(tr("默认网关 ："));
    gateWayLabel->setFixedWidth(240);
    gateWayLabel->setFixedHeight(30);
    gateWayLabel->setFont(font);

    dns1Label = new QLabel();
    dns1Label->setText(tr("DNS1 ："));
    dns1Label->setFixedWidth(240);
    dns1Label->setFixedHeight(30);
    dns1Label->setFont(font);

    dns2Label = new QLabel();
    dns2Label->setText(tr("DNS2 ："));
    dns2Label->setFixedWidth(240);
    dns2Label->setFixedHeight(30);
    dns2Label->setFont(font);

    QHBoxLayout *netAddressLayout = new QHBoxLayout();
    netAddressLayout->addWidget(netAddrLabel);
    netAddressLayout->addStretch();
    netAddressLayout->addWidget(dns1Label);
    netAddressLayout->addStretch();
    netAddressLayout->addSpacing(30);

    QHBoxLayout *subnetLabelLayout = new QHBoxLayout();
    subnetLabelLayout->addWidget(subnetLabel);
    subnetLabelLayout->addStretch();
    subnetLabelLayout->addWidget(dns2Label);
    subnetLabelLayout->addStretch();
    subnetLabelLayout->addSpacing(30);

    QHBoxLayout *gateWayLabelLayout = new QHBoxLayout();
    gateWayLabelLayout->addWidget(gateWayLabel);
    gateWayLabelLayout->addStretch();

    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addLayout(dhcpLayout);
    netAddressLayout->setContentsMargins(30,20,0,0);
    topLayout->addLayout(netAddressLayout);
    subnetLabelLayout->setContentsMargins(30,20,0,0);
    topLayout->addLayout(subnetLabelLayout);
    gateWayLabelLayout->setContentsMargins(30,20,0,0);
    topLayout->addLayout(gateWayLabelLayout);
    topLayout->addStretch();
    topLayout->setContentsMargins(10,25,0,0);

    staticRadio = new QRadioButton(this);
    staticRadio->setObjectName("radiobuttonone");

    QLabel *staticSetLabel = new QLabel();
    staticSetLabel->setText(tr("静态指定"));
    staticSetLabel->setFixedWidth(200);
    staticSetLabel->setFont(font);
    QHBoxLayout *staticSetLayout = new QHBoxLayout();
    staticSetLayout->addWidget(staticRadio);
    staticSetLayout->addWidget(staticSetLabel);
    staticSetLayout->addStretch();

    radioButtonGroup = new QButtonGroup(this);
    radioButtonGroup->addButton(DHCPRadio, 0);
    radioButtonGroup->addButton(staticRadio, 1);

    QLabel *staticNetAddress = new QLabel();
    staticNetAddress->setText(tr("网络地址"));
    staticNetAddress->setFixedWidth(70);
    staticNetAddress->setFixedHeight(30);
    staticNetAddress->setFont(font);
    staticNetAddressEdit = new QLineEdit();
    staticNetAddressEdit->setText("");
    staticNetAddressEdit->setFixedWidth(140);
    staticNetAddressEdit->setFixedHeight(30);
    staticNetAddressEdit->setFont(font);

    QHBoxLayout *staticNetAddressLayout = new QHBoxLayout();
    staticNetAddressLayout->addWidget(staticNetAddress);
    staticNetAddressLayout->addWidget(staticNetAddressEdit);

    QLabel *staticSubnetLabel = new QLabel();
    staticSubnetLabel->setText(tr("子网掩码"));
    staticSubnetLabel->setFixedWidth(70);
    staticSubnetLabel->setFixedHeight(30);
    staticSubnetLabel->setFont(font);
    staticSubnetEdit = new QLineEdit();
    staticSubnetEdit->setText("");
    staticSubnetEdit->setFixedWidth(140);
    staticSubnetEdit->setFixedHeight(30);
    staticSubnetEdit->setFont(font);

    QHBoxLayout *staticSubnetLayout = new QHBoxLayout();
    staticSubnetLayout->addWidget(staticSubnetLabel);
    staticSubnetLayout->addWidget(staticSubnetEdit);

    QLabel *staticGateWayLabel = new QLabel();
    staticGateWayLabel->setText(tr("默认网关"));
    staticGateWayLabel->setFixedWidth(70);
    staticGateWayLabel->setFixedHeight(30);
    staticGateWayLabel->setFont(font);
    staticGateWayEdit = new QLineEdit();
    staticGateWayEdit->setText("");
    staticGateWayEdit->setFixedWidth(140);
    staticGateWayEdit->setFixedHeight(30);
    staticGateWayEdit->setFont(font);


    QHBoxLayout *staticGateWayLabelLayout = new QHBoxLayout();
    staticGateWayLabelLayout->addWidget(staticGateWayLabel);
    staticGateWayLabelLayout->addWidget(staticGateWayEdit);
    staticGateWayLabelLayout->addStretch();

    QLabel *staticDNS1 = new QLabel();
    staticDNS1->setText(tr("DNS1"));
    staticDNS1->setFixedWidth(45);
    staticDNS1->setFixedHeight(30);
    staticDNS1->setFont(font);
    staticDNS1Edit = new QLineEdit();
    staticDNS1Edit->setText("");
    staticDNS1Edit->setFixedWidth(140);
    staticDNS1Edit->setFixedHeight(30);
    staticDNS1Edit->setFont(font);

    QHBoxLayout *staticDNS1Layout = new QHBoxLayout();
    staticDNS1Layout->addWidget(staticDNS1);
    staticDNS1Layout->addWidget(staticDNS1Edit);
    staticDNS1Layout->addStretch();

    QLabel *staticDNS2 = new QLabel();
    staticDNS2->setText(tr("DNS2"));
    staticDNS2->setFixedWidth(45);
    staticDNS2->setFixedHeight(30);
    staticDNS2->setFont(font);
    staticDNS2Edit = new QLineEdit();
    staticDNS2Edit->setText("");
    staticDNS2Edit->setFixedWidth(140);
    staticDNS2Edit->setFixedHeight(30);
    staticDNS2Edit->setFont(font);

    QHBoxLayout *staticDNS2Layout = new QHBoxLayout();
    staticDNS2Layout->addWidget(staticDNS2);
    staticDNS2Layout->addWidget(staticDNS2Edit);
    staticDNS2Layout->addStretch();

    QHBoxLayout *ipAndDNS1Layout = new QHBoxLayout();
    ipAndDNS1Layout->addLayout(staticNetAddressLayout);
    ipAndDNS1Layout->addStretch();
    ipAndDNS1Layout->addLayout(staticDNS1Layout);
    ipAndDNS1Layout->addStretch();

    QHBoxLayout *ipAndDNS2Layout = new QHBoxLayout();
    ipAndDNS2Layout->addLayout(staticSubnetLayout);
    ipAndDNS2Layout->addStretch();
    ipAndDNS2Layout->addLayout(staticDNS2Layout);
    ipAndDNS2Layout->addStretch();

    QVBoxLayout *middleLayout = new QVBoxLayout();
    middleLayout->addLayout(staticSetLayout);
    ipAndDNS1Layout->setContentsMargins(30,20,65,0);
    middleLayout->addLayout(ipAndDNS1Layout);
    ipAndDNS2Layout->setContentsMargins(30,20,65,0);
    middleLayout->addLayout(ipAndDNS2Layout);
    staticGateWayLabelLayout->setContentsMargins(30,20,0,0);
    middleLayout->addLayout(staticGateWayLabelLayout);
    middleLayout->setContentsMargins(10,30,0,0);

    QFont useButtonFont;                           //字体
    useButtonFont.setPointSize(18);                //字体大小

    useButton = new QPushButton();
    useButton->setObjectName("brownButton");
    useButton->setText(tr("保存"));
    useButton->setFixedSize(120,50);
    useButton->setFont(useButtonFont);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(useButton);
    bottomLayout->setContentsMargins(0,0,30,0);

    QVBoxLayout *vbottomLayout = new QVBoxLayout();
    vbottomLayout->addLayout(bottomLayout);
    vbottomLayout->addStretch();

    QVBoxLayout *mainlayout = new QVBoxLayout();

    mainlayout->addLayout(topLayout);
    mainlayout->addLayout(middleLayout);
    mainlayout->addLayout(vbottomLayout);
    OnUserChange();

    connect(useButton, SIGNAL(clicked(bool)), this, SLOT(NetApply()));
    connect(DHCPRadio, SIGNAL(clicked()), this, SLOT(OnRadioClick()));
    connect(staticRadio, SIGNAL(clicked()), this, SLOT(OnRadioClick()));


    process = new QProcess(this);
    isDHCPStop = true;
    isLinkDown = false;
    isDHCPOK = false;
    DHCPprocess = new QProcess(this);
    connect(DHCPprocess, SIGNAL(finished(int)), this, SLOT(DHCPFinish(int)));

    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(TimerHandler()));
    m_timer->start(5000);

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    SettingIPMode mode;
    luaEngine->GetLuaValue(state, "config.interconnection.settingIPMode", mode);
    if (mode == SettingIPMode::DHCP)
    {
        if(true == IsSupportDHCP(false))
        {
            isDHCPStop = true;
#ifdef _CS_ARM_LINUX
            StartProcess(QString("killall -9 udhcpc"), 1000, true);
#endif
            netAddrLabel->setText("");
            subnetLabel->setText(tr("正在自动获取IP..."));
            gateWayLabel->setText("");
            dns1Label->setText("");
            dns2Label->setText("");
            isDHCPStop = false;

            DHCPprocess->start(QString("/sbin/udhcpc"));
        }
        else
        {
            luaEngine->SetLuaValue("config.interconnection.settingIPMode", (int)SettingIPMode::STATIC);
            SettingManager::Instance()->InterconnectionSave();
        }
    }
    else
    {
#ifdef _CS_ARM_LINUX
        system("killall -9 udhcpc");

        SetDNS();
        system("/etc/init.d/netset.sh");

        SetDNS();
#endif
    }
    UpdateSetIPMode(mode);
    this->setLayout(mainlayout);
}

void NetSet::OnUserChange()
{
//    if((LoginDialog::userType == Administrator))
//    {
//        useButton->setEnabled(true);
//        staticNetAddressEdit->setEnabled(true);
//        staticSubnetEdit->setEnabled(true);
//        staticGateWayEdit->setEnabled(true);
//    }
//    else
//    {
//        useButton->setEnabled(false);
//        staticNetAddressEdit->setEnabled(false);
//        staticSubnetEdit->setEnabled(false);
//        staticGateWayEdit->setEnabled(false);
//    }
}

NetSet::~NetSet()
{
    if(DHCPprocess != nullptr)
    {
        delete DHCPprocess;
    }
    if(process != nullptr)
    {
        delete process;
    }
    if( nullptr != m_timer)
    {
        m_timer->stop();
        delete m_timer;
    }
}

/*
 * @brief 获取当前IP信息，每次进入网络设置界面和点击应用后都需要更新
 */
void NetSet::UpdateCurrentIPInfo()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    SettingIPMode mode;
    luaEngine->GetLuaValue(state, "config.interconnection.settingIPMode", mode);

    staticNetAddressEdit->clear();
    staticSubnetEdit->clear();
    staticGateWayEdit->clear();
    staticDNS1Edit->clear();
    staticDNS2Edit->clear();

    QHostAddress address;
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    QList<QNetworkInterface>::iterator it;
    for(it = list.begin(); it != list.end(); it++)
    {
        if((*it).name() == NETWORKNAME)
        {
            QProcess GetGate;
            QString gateStr = "0.0.0.0";
#ifdef _CS_ARM_LINUX
            GetGate.start("ip route");
            if(GetGate.waitForFinished(3000))
            {
                QString outStr = QString::fromUtf8(GetGate.readAll().data());
                QList<QString> outList = outStr.split(" ",QString::SkipEmptyParts);
                QList<QString>::iterator p;
                for (p = outList.begin(); p != outList.end(); ++p)
                {
                    QString str = *p;
                    if(str.contains("via"))
                    {
                        QString nextStr = *(p+1);
                        QString ipPattern("^([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");
                        QRegExp rx(ipPattern);
                        if(rx.exactMatch(nextStr))
                        {
                            gateStr = nextStr;
                            break;
                        }
                    }
                }
            }
#elif _CS_X86_WINDOWS
            GetGate.start("ipconfig");
            if(GetGate.waitForFinished(3000))
            {
                const char *sub = "默认网关";
                QTextCodec *codec = QTextCodec::codecForName("UTF-8");
                QString subStr = codec->toUnicode(sub);

                const char *out = GetGate.readAll().data();
                QTextCodec *codewin = QTextCodec::codecForName("GBK");
                QString outStr = codewin->toUnicode(out);

                QStringList outList = outStr.split("\r\n");
                QString str;
                foreach(str, outList)
                {
                    if(str.contains(subStr))
                    {
                        QStringList gateList = str.split(": ");
                        QString backStr = gateList.back();
                        QString ipPattern("^([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");
                        QRegExp rx(ipPattern);
                        if(rx.exactMatch(backStr))
                        {
                            gateStr = backStr;
                            break;
                        }
                    }
                }
            }
#endif

            QNetworkInterface::InterfaceFlags flags = (*it).flags();
            QList<QNetworkAddressEntry> entryList = (*it).addressEntries();
            QList<QNetworkAddressEntry>::iterator entryit;
            for(entryit = entryList.begin(); entryit != entryList.end(); entryit++)
            {
                address = (*entryit).ip();
                if(address.protocol() == QAbstractSocket::IPv4Protocol)
                {
                    if (mode == SettingIPMode::STATIC)
                    {
                        staticNetAddressEdit->setText((*entryit).ip().toString());
                        staticSubnetEdit->setText((*entryit).netmask().toString());
                        staticGateWayEdit->setText(gateStr);
                        UpdateDNSInfo(SettingIPMode::STATIC);
                    }
                    else if(flags.testFlag(QNetworkInterface::IsRunning))
                    {
                        if(true == isDHCPOK)
                        {
                            netAddrLabel->setText(tr("网络地址 ：") + (*entryit).ip().toString());
                            subnetLabel->setText(tr("子网掩码 ：") + (*entryit).netmask().toString());
                            gateWayLabel->setText(tr("默认网关 ：") + gateStr);
                            UpdateDNSInfo(SettingIPMode::DHCP);
                        }
                        else
                        {
                            netAddrLabel->setText("");
                            subnetLabel->setText(tr("正在自动获取IP..."));
                            gateWayLabel->setText("");
                            dns1Label->setText("");
                            dns2Label->setText("");
                        }
                    }
                    break;
                }
            }
        }
    }
    UpdateSetIPMode(mode);
}

void NetSet::UpdateSetIPMode(SettingIPMode mode)
{
    if (mode == SettingIPMode::DHCP)
    {
        DHCPRadio->setChecked(true);
        staticNetAddressEdit->setReadOnly(true);
        staticSubnetEdit->setReadOnly(true);
        staticGateWayEdit->setReadOnly(true);
        staticDNS1Edit->setReadOnly(true);
        staticDNS2Edit->setReadOnly(true);
        staticNetAddressEdit->removeEventFilter(CNumberKeyboard::Instance());
        staticSubnetEdit->removeEventFilter(CNumberKeyboard::Instance());
        staticGateWayEdit->removeEventFilter(CNumberKeyboard::Instance());
        staticDNS1Edit->removeEventFilter(CNumberKeyboard::Instance());
        staticDNS2Edit->removeEventFilter(CNumberKeyboard::Instance());
    }
    else
    {
        staticRadio->setChecked(true);
        staticNetAddressEdit->setReadOnly(false);
        staticSubnetEdit->setReadOnly(false);
        staticGateWayEdit->setReadOnly(false);
        staticDNS1Edit->setReadOnly(false);
        staticDNS2Edit->setReadOnly(false);
        staticNetAddressEdit->installEventFilter(CNumberKeyboard::Instance());
        staticSubnetEdit->installEventFilter(CNumberKeyboard::Instance());
        staticGateWayEdit->installEventFilter(CNumberKeyboard::Instance());
        staticDNS1Edit->installEventFilter(CNumberKeyboard::Instance());
        staticDNS2Edit->installEventFilter(CNumberKeyboard::Instance());
    }
}

void NetSet::OnRadioClick()
{
    switch(radioButtonGroup->checkedId())
    {
    case 0:
        UpdateSetIPMode(SettingIPMode::DHCP);
        break;
    case 1:
        UpdateSetIPMode(SettingIPMode::STATIC);
        break;
    }
}

bool NetSet::CheckValue()
{
    QString errorMessage;
    QString tmpPattern("^([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");  //检查数字是否合法
    QRegExp tmpRx(tmpPattern);
    if(false == tmpRx.exactMatch(staticNetAddressEdit->text()))
    {
        errorMessage += tr("网络地址格式错误\n");
    }
    if(false == tmpRx.exactMatch(staticSubnetEdit->text()))
    {
        errorMessage += tr("子网掩码格式错误\n");
    }
    if(false == tmpRx.exactMatch(staticGateWayEdit->text()))
    {
        errorMessage += tr("默认网关格式错误\n");
    }
    if(false == tmpRx.exactMatch(staticDNS1Edit->text()))
    {
        errorMessage += tr("DNS1格式错误\n");
    }
    if(false == tmpRx.exactMatch(staticDNS2Edit->text()))
    {
        errorMessage += tr("DNS2格式错误\n");
    }

    if (!errorMessage.isEmpty())
    {
        MessageDialog msg(errorMessage, this);
        msg.exec();
        return false;
    }
    return true;
}

bool NetSet::SaveIPToScript()
{
    QFile file("/etc/init.d/netset.sh");
    if (!file.exists())
    {
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        file.setPermissions(QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther
                | QFile::WriteOwner
                | QFile::ExeOwner | QFile::ExeGroup | QFile::ExeOther);

        file.close();
    }
    QString newNet = "ifconfig eth0 " + staticNetAddressEdit->text()
            +" netmask "+ staticSubnetEdit->text()
            +"\n"+ "route add default gw " + staticGateWayEdit->text();

    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << newNet;
        file.close();
    }
    else
    {
        MessageDialog msg(tr("脚本不能打开\n"), this);
        msg.exec();
        return false;
    }
    return true;
}

bool NetSet::IsSupportDHCP(bool isWarning)
{
    QFile file("/sbin/udhcpc");
    if (file.exists())
    {
        return true;
    }
    else
    {
        if(isWarning)
        {
            MessageDialog msg(tr("不支持DHCP,请选择静态指定\n"), this);
            msg.exec();
        }
        return false;
    }
}

void NetSet::TimerHandler()
{
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    QList<QNetworkInterface>::iterator it;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    for(it = list.begin(); it != list.end(); it++)
    {
        if((*it).name() == NETWORKNAME)
        {
            QNetworkInterface::InterfaceFlags flags = (*it).flags();
            if(!flags.testFlag(QNetworkInterface::IsRunning))
            {
                if(false == isLinkDown)
                {
                    netAddrLabel->setText("");
                    subnetLabel->setText(tr("网络电缆被拔出..."));
                    gateWayLabel->setText("");
                    dns1Label->setText("");
                    dns2Label->setText("");

                    SettingIPMode mode;
                    luaEngine->GetLuaValue(state, "config.interconnection.settingIPMode", mode);
                    if(mode == SettingIPMode::DHCP)
                    {
                        isDHCPStop = true;
                        DHCPprocess->close();
#ifdef _CS_ARM_LINUX
                        QProcess::startDetached(QString("killall -9 udhcpc"));
#endif
                        StartProcess(QString("umount /home"), 1000, true);
                    }
                    isLinkDown = true;
                    Interface::Wqimc::WqimcManager::Instance()->SetCableStatus(false);
                    logger->debug("网线断开！！！");
                }
            }
            else
            {
                if(true == isLinkDown)
                {
                    SettingIPMode mode;
                    luaEngine->GetLuaValue(state, "config.interconnection.settingIPMode", mode);
                    if(mode == SettingIPMode::DHCP)
                    {
                        if(true == isDHCPStop)
                        {
                            if (IsSupportDHCP(true))
                            {
                                netAddrLabel->setText("");
                                subnetLabel->setText(tr("正在自动获取IP..."));
                                gateWayLabel->setText("");
                                dns1Label->setText("");
                                dns2Label->setText("");

                                if(true == isDHCPStop)
                                {
                                    isDHCPStop = false;
                                    isDHCPOK = false;
                                    DHCPprocess->start(QString("/sbin/udhcpc"));
                                }
                            }
                        }
                    }
                    else
                    {
                        netAddrLabel->setText(tr("网络地址 ："));
                        subnetLabel->setText(tr("子网掩码 ："));
                        gateWayLabel->setText(tr("默认网关 ："));
                        dns1Label->setText("DNS1 ：");
                        dns2Label->setText("DNS2 ：");
                    }
                    isLinkDown = false;
                    Interface::Wqimc::WqimcManager::Instance()->SetCableStatus(true);
                    logger->debug("网线连接！！！");
                }
            }
        }
    }
}

void NetSet::DHCPFinish(int exitCode)
{
//    qDebug("exitCode: %d", exitCode);
    if(isDHCPStop == false && exitCode == 0)
    {
        isDHCPOK = true;
        UpdateCurrentIPInfo();
    }
}

bool NetSet::StartProcess(const QString &command, int msecs, bool isTimeOutClose)
{
    bool ret = true;
    process->start(command);
//    qDebug("command: %s", qPrintable(command));
    if(msecs != 0)
    {
        ret = process->waitForFinished(msecs);
        if(false == ret)
        {
//            qDebug("command: %s no finished", qPrintable(command));
            if(isTimeOutClose)
            {
//                qDebug("command: %s close", qPrintable(command));
                process->close();
            }
        }
    }
    return ret;
}

void NetSet::GetDNS(QList<DNSInfo> &dnsList)
{
    QFile file(NDSFile);
    if (!file.exists())
    {
        qDebug() << " /etc/resolv.conf  not exists ";
        return;
    }

    if (file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);

        QString line = in.readLine();
        QStringList fdnsStrList=line.split(" ");
        DNSInfo fdns;
        fdns.name = fdnsStrList.at(0);
        fdns.addr = fdnsStrList.at(1);
        dnsList.push_back(fdns);

        int i = 10;
        while(i--)
        {
            line = in.readLine();
            if (line.isNull())
            {
                break;
            }
            QStringList dnsStrList=line.split(" ");

            DNSInfo dns;
            dns.name = dnsStrList.at(0);
            dns.addr = dnsStrList.at(1);

            dnsList.push_back(dns);
        }

        file.close();

//        for (int i = 0;  i < dnsList.size(); i++ )
//        {
//            qDebug() << " dns.name " << dnsList.at(i).name;
//            qDebug() << " dns.addr " << dnsList.at(i).addr;
//        }
    }
    else
    {
        MessageDialog msg(tr("/etc/resolv.conf不能打开\n"), this);
        msg.exec();
    }
}

bool NetSet::SaveDNS()
{
    QList<DNSInfo> dnsLists;
    GetDNS(dnsLists);

    QFile file("/opt/luip/resolv.conf");
    if (!file.exists())
    {
        return false;
    }

    QString dnsStr;

    if (dnsLists.isEmpty())
    {
        QString str = QString("nameserver") + " " + staticDNS1Edit->text() + "\n";
        dnsStr += str;

        str = QString("nameserver") + " " + staticDNS2Edit->text() + "\n";
        dnsStr += str;
    }
    else
    {
        if (dnsLists.size() >= 2)
        {

            QString str = dnsLists.at(0).name + " " + staticDNS1Edit->text() + "\n";
            dnsStr += str;

            str = dnsLists.at(1).name + " " + staticDNS2Edit->text() + "\n";
            dnsStr += str;

            for (int i = 2;  i < dnsLists.size(); i++ )
            {
                QString str = dnsLists.at(i).name + " " + dnsLists.at(1).addr + "\n";
                dnsStr += str;
            }
        }
        else if (dnsLists.size() == 1)
        {
            QString str = dnsLists.at(0).name + " " + staticDNS1Edit->text() + "\n";
            dnsStr += str;

            str = QString("nameserver") + " " + staticDNS2Edit->text() + "\n";
            dnsStr += str;
        }
    }

    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << dnsStr;
        file.close();
    }
    else
    {
        MessageDialog msg(tr("resolv.conf 不能打开\n"), this);
        msg.exec();
        return false;
    }

    SetDNS();
    return true;
}

void NetSet::UpdateDNSInfo(SettingIPMode mode)
{
    if (mode == SettingIPMode::DHCP)
    {
        #ifdef _CS_ARM_LINUX

            QList<DNSInfo> dnsLists;
            GetDNS(dnsLists);
            if (dnsLists.isEmpty())
            {
                dns1Label->setText("DNS1 ：");
                dns2Label->setText("DNS2 ：");
            }
            else
            {
                if (dnsLists.size() >= 2)
                {
                    dns1Label->setText("DNS1 ：" + dnsLists.at(0).addr);
                    dns2Label->setText("DNS2 ：" + dnsLists.at(1).addr);
                }
                else if (dnsLists.size() == 1)
                {
                    dns1Label->setText("DNS1 ：" + dnsLists.at(0).addr);
                    dns2Label->setText("DNS2 ：");
                }
            }
        #elif _CS_X86_WINDOWS
            dns1Label->setText("DNS1 ：");
            dns2Label->setText("DNS2 ：");
        #endif
    }
    else if (mode == SettingIPMode::STATIC)
    {
        #ifdef _CS_ARM_LINUX

            QList<DNSInfo> dnsLists;
            GetDNS(dnsLists);
            if (dnsLists.isEmpty())
            {
                staticDNS1Edit->clear();
                staticDNS2Edit->clear();
            }
            else
            {
                if (dnsLists.size() >= 2)
                {
                    staticDNS1Edit->setText(dnsLists.at(0).addr);
                    staticDNS2Edit->setText(dnsLists.at(1).addr);
                }
                else if (dnsLists.size() == 1)
                {
                    staticDNS1Edit->setText(dnsLists.at(0).addr);
                    staticDNS2Edit->clear();
                }
            }
        #elif _CS_X86_WINDOWS
            staticDNS1Edit->clear();
            staticDNS2Edit->clear();
        #endif
    }
}

void NetSet::SetDNS()
{
    using namespace std;
    fstream inFile;
    inFile.open("/opt/luip/resolv.conf");

    ofstream outFile;
    outFile.open("/etc/resolv.conf");

    string str;
    while(getline(inFile, str))
    {
        outFile << str << endl;
    }

    inFile.close();
    outFile.close();
}

void NetSet::NetApply()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    switch(radioButtonGroup->checkedId())
    {
    case 0://动态
        staticNetAddressEdit->clear();
        staticSubnetEdit->clear();
        staticGateWayEdit->clear();
        luaEngine->SetLuaValue("config.interconnection.settingIPMode", (int)SettingIPMode::DHCP);
        SettingManager::Instance()->InterconnectionSave();

        //StartProcess(QString("umount /home"), 1000, true);

        if(true == isDHCPStop && false == isLinkDown)
        {
            if (IsSupportDHCP(true))
            {
                netAddrLabel->setText("");
                subnetLabel->setText(tr("正在自动获取IP..."));
                gateWayLabel->setText("");
                dns1Label->setText("");
                dns2Label->setText("");

                if(true == isDHCPStop)
                {
                    isDHCPStop = false;
                    isDHCPOK = false;
                    DHCPprocess->start(QString("/sbin/udhcpc"));
                }
            }
        }
        break;
    case 1:
        if(CheckValue())
        {
            if(false == isLinkDown)
            {
                netAddrLabel->setText(tr("网络地址 ："));
                subnetLabel->setText(tr("子网掩码 ："));
                gateWayLabel->setText(tr("默认网关 ："));
                dns1Label->setText("DNS1 ：");
                dns2Label->setText("DNS2 ：");
            }
            luaEngine->SetLuaValue("config.interconnection.settingIPMode", (int)SettingIPMode::STATIC);
            SettingManager::Instance()->InterconnectionSave();

            isDHCPStop = true;
            DHCPprocess->close();
#ifdef _CS_ARM_LINUX
            QProcess::startDetached(QString("killall -9 udhcpc"));
#endif

            StartProcess(QString("umount /home"), 1000, true);

            if(SaveIPToScript())
            {
                SaveDNS();
                StartProcess(QString("/etc/init.d/netset.sh"), 2000, true);
            }
        }

        UpdateCurrentIPInfo();
        break;
    }
}

void NetSet::showEvent(QShowEvent *event)
{
    (void)event;
    this->setFocus();
}

void NetSet::OnUpdateWidget(UpdateEvent event, System::String message)
{
#ifdef _CS_ARM_LINUX
    if (event == UpdateEvent::ChangeConfigParam ||
        event == UpdateEvent::WqimcChangeParam  ||
        event == UpdateEvent::ChangeInterconnectionParam)//一键恢复出厂设置
    {
        UpdateCurrentIPInfo();
        NetApply();
    }
#endif
}

}

