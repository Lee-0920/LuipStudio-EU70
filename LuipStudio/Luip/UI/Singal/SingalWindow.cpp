#include "SingalWindow.h"
#include "UI/ControlConfig.h"
#include "System/ProLicenseManager.h"

using namespace System;

namespace UI
{

SingalWindow::SingalWindow(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);
    listView = new QListView();
    listView->setFixedHeight(400);
    listView->setFixedWidth(101);
    listView->setObjectName(QStringLiteral("commonListView"));
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QVBoxLayout *listViewLayout = new QVBoxLayout();
    listViewLayout->addWidget(listView);
    listViewLayout->setContentsMargins(0, 11, 0, 0);
    listViewLayout->addStretch();

    stackedWidget = new QStackedWidget();
    stackedWidget->setFixedHeight(450);
    stackedWidget->setFixedWidth(650);
    stackedWidget->setObjectName(QStringLiteral("stackedWidget"));

    QHBoxLayout *stackedWidgetLayout = new QHBoxLayout();
    stackedWidgetLayout->addWidget(stackedWidget);

    stackedWidgetLayout->addStretch();
    stackedWidgetLayout->setContentsMargins(10, 0, 0, 0);

    QVBoxLayout *vstackedWidgetLayout = new QVBoxLayout();
    vstackedWidgetLayout->addLayout(stackedWidgetLayout);
    vstackedWidgetLayout->addStretch();
    vstackedWidgetLayout->setContentsMargins(0, 3, 0, 0);

    QStringList num;
    num<<tr("基本信号")<< tr("趋势图");
    model = new QStringListModel(num);
    listView->setGridSize(QSize(50,50));
    listView->setModel(model);

    QFont listFont = listView->font();
    listFont.setPointSize(16);
    listFont.setWeight(60);
    listView->setFont(listFont);

    basicSignal = new SignalWidget();
    stackedWidget->addWidget(basicSignal);

    //*********
    peakShapeWidget = new PeakShape();
    stackedWidget->addWidget(peakShapeWidget);    
    //*********

    connect(listView, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));

    QHBoxLayout *mainlayout = new QHBoxLayout();

    mainlayout->addLayout(listViewLayout);
    mainlayout->addLayout(vstackedWidgetLayout);
    mainlayout->setContentsMargins(10, 5, 0, 0);

    stackedWidget->setCurrentWidget(basicSignal);
     this->setLayout(mainlayout);
}

void SingalWindow::itemClicked(QModelIndex index)
{
    QStringList strList;
    strList.append(tr("基本信号"));    
    strList.append(tr("趋势图"));

    if(strList.at(0) == index.data().toString())
    {
        stackedWidget->setCurrentWidget(basicSignal);
    }    
    else
    {
        stackedWidget->setCurrentWidget(peakShapeWidget);
    }
}

void SingalWindow::OnUserChange()
{
    ChangeBottomStatus();
}

void SingalWindow::ChangeBottomStatus()
{
    stackedWidget->setCurrentWidget(basicSignal);
    QModelIndex index = model->index(0,0);
    listView->setCurrentIndex(index);

    if(ProLicenseManager::Instance()->GetProLicense())
    {
        if(LoginDialog::GetUserPrivilege(Authority::BasicSignal))
        {
            listView->setRowHidden(0, false);
        }else{
            listView->setRowHidden(0, true);
        }        
        if(LoginDialog::GetUserPrivilege(Authority::TrendDiagram))
        {
            listView->setRowHidden(1, false);
        }else{
            listView->setRowHidden(1, true);
        }
    }
    else
    {
        if(LoginDialog::userType == Super)
        {
            listView->setRowHidden(0, false);
            listView->setRowHidden(1, false);
        }
        else if(LoginDialog::userType == Administrator)
        {
            listView->setRowHidden(0, false);
            listView->setRowHidden(1, false);          
        }
        else if(LoginDialog::userType == Maintain)
        {
            listView->setRowHidden(0, true);
            listView->setRowHidden(1, true);            
        }
        else if(LoginDialog::userType == General)
        {
            listView->setRowHidden(0, true);
            listView->setRowHidden(1, true);
        }
    }
}

SingalWindow::~SingalWindow()
{
    if (basicSignal)
        delete basicSignal;
    if (peakShapeWidget)
        delete peakShapeWidget;

}

}
