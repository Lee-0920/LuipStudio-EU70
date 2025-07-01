#include "UI/Frame/LoginDialog.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "SystemInfoWindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "UI/ControlConfig.h"
#include "System/ProLicenseManager.h"

using namespace System;

namespace UI
{

SystemInfoWindow::SystemInfoWindow(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);

    listView = new QListView();
    listView->setFixedHeight(400);
    listView->setFixedWidth(101);
    listView->setObjectName(QStringLiteral("commonListView"));
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    stackedWidget = new QStackedWidget();
    stackedWidget->setFixedHeight(425);
    stackedWidget->setFixedWidth(663);
    stackedWidget->setObjectName(QStringLiteral("stackedWidget"));

    instrumentInformation = new InstrumentInformationWidget();
    stackedWidget->addWidget(instrumentInformation);

    deviceInformation = new DeviceInformation();
    stackedWidget->addWidget(deviceInformation);

    netSet = new NetSet();
    stackedWidget->addWidget(netSet);

    systemTime = new SystemTimeWidget();
    stackedWidget->addWidget(systemTime);

    factoryTime = new FactoryTimeWidget();
    stackedWidget->addWidget(factoryTime);

    QVBoxLayout *listViewLayout = new QVBoxLayout();
    listViewLayout->addWidget(listView);
    listViewLayout->setContentsMargins(0, 11, 0, 0);
    listViewLayout->addStretch();

    QHBoxLayout *stackedWidgetLayout = new QHBoxLayout();
    stackedWidgetLayout->addWidget(stackedWidget);
    stackedWidgetLayout->addStretch();
    stackedWidgetLayout->setContentsMargins(10, 0, 0, 0);

    stackedWidget->setCurrentWidget(instrumentInformation);

    QVBoxLayout *vstackedWidgetLayout = new QVBoxLayout();
    vstackedWidgetLayout->addLayout(stackedWidgetLayout);
    vstackedWidgetLayout->addStretch();
    vstackedWidgetLayout->setContentsMargins(0, 3, 0, 0);

    QStringList num;
    num<<tr("仪器信息")<<tr("板卡信息")<<tr("网络设置")<<tr("系统时间")<<tr("系统设置");
    model = new QStringListModel(num);
    //listView->setSpacing(5);
    listView->setGridSize(QSize(50,50));
    listView->setModel(model);

    QFont listFont = listView->font();
    listFont.setPointSize(16);
    listFont.setWeight(65);
    listView->setFont(listFont);

    connect(listView,SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
    QHBoxLayout *mainlayout = new QHBoxLayout();

    mainlayout->addLayout(listViewLayout);
    mainlayout->addLayout(vstackedWidgetLayout);

    mainlayout->setContentsMargins(10, 5, 0, 0);
    this->setLayout(mainlayout);

    ChangeBottomStatus();

    connect(this, SIGNAL(UpdateNetSetView()),netSet, SLOT(UpdateCurrentIPInfo()));
}


void SystemInfoWindow::itemClicked(QModelIndex index)
{
    QStringList strList;
    strList.append(tr("仪器信息"));
    strList.append(tr("板卡信息"));
    strList.append(tr("网络设置"));
    strList.append(tr("系统时间"));
    strList.append(tr("系统设置"));

    if(strList.at(0) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(instrumentInformation);
    }
    else if(strList.at(1) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(deviceInformation);
    }
    else if(strList.at(2) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(netSet);
        emit UpdateNetSetView();
    }
    else if(strList.at(3) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(systemTime);
    }
    else if(strList.at(4) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(factoryTime);
    }
}


SystemInfoWindow::~SystemInfoWindow()
{
    if (instrumentInformation)
        delete instrumentInformation;
    if (deviceInformation)
        delete deviceInformation;
    if (netSet)
        delete netSet;
    if (systemTime)
        delete systemTime;
    if (factoryTime)
        delete factoryTime;
}

void SystemInfoWindow::OnUserChange()
{
    ChangeBottomStatus();
}

void SystemInfoWindow::ChangeBottomStatus()
{
    stackedWidget->setCurrentWidget(instrumentInformation);
    QModelIndex index = model->index(0,0);
    listView->setCurrentIndex(index);

    if(ProLicenseManager::Instance()->GetProLicense())
    {
        if(LoginDialog::GetUserPrivilege(Authority::InstrumentInformation))
        {
            listView->setRowHidden(0, false);
        }else{
            listView->setRowHidden(0, true);
        }
        if(LoginDialog::GetUserPrivilege(Authority::DeviceInformation))
        {
            listView->setRowHidden(1, false);
        }else{
            listView->setRowHidden(1, true);
        }
        if(LoginDialog::GetUserPrivilege(Authority::NetSet))
        {
            listView->setRowHidden(2, false);
        }else{
            listView->setRowHidden(2, true);
        }
        if(LoginDialog::GetUserPrivilege(Authority::SystemTime))
        {
            listView->setRowHidden(3, false);
        }else{
            listView->setRowHidden(3, true);
        }
        if(LoginDialog::GetUserPrivilege(Authority::FactoryTime))
        {
            listView->setRowHidden(4, false);
        }else{
            listView->setRowHidden(4, true);
        }
    }
    else
    {
        if(LoginDialog::userType == Super)
        {
            listView->setRowHidden(0, false);
            listView->setRowHidden(1, false);
            listView->setRowHidden(2, false);
            listView->setRowHidden(3, false);
            listView->setRowHidden(4, false);
        }
        else if(LoginDialog::userType == Administrator)
        {
            listView->setRowHidden(0, false);
            listView->setRowHidden(1, false);
            listView->setRowHidden(2, false);
            listView->setRowHidden(3, false);
            listView->setRowHidden(4, false);
        }
        else if(LoginDialog::userType == Maintain)
        {
            listView->setRowHidden(0, false);
            listView->setRowHidden(1, true);
            listView->setRowHidden(2, true);
            listView->setRowHidden(3, true);
            listView->setRowHidden(4, true);
        }
        else if(LoginDialog::userType == General)
        {
            listView->setRowHidden(0, false);
            listView->setRowHidden(1, true);
            listView->setRowHidden(2, true);
            listView->setRowHidden(3, true);
            listView->setRowHidden(4, true);
        }
    }
}

}
