#ifndef UI_REMOTEMAINTAINWINDOW_H
#define UI_REMOTEMAINTAINWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QStackedWidget>
#include <QStringListModel>
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "MaintenanceWidget.h"
#include "ReagentManagerWidget.h"
#include "MaintenanceCombineWidget.h"
#include "UseResourceWidget.h"
#include "PipeManngerWidget.h"
#include "CombineOpsManagerWidget.h"
#include "HardwareCheckWidget.h"
#include "HardwareTestWidget.h"
#include "IntellectCheckWidget.h"
#include "CommunicationTestWidget.h"
#include "UpdateWidget.h"

namespace UI
{

class RemoteMainTainWindow: public QWidget, public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit RemoteMainTainWindow(QWidget *parent = 0);
    ~RemoteMainTainWindow();

    void OnUserChange();
    bool HasCombineOperation(void);
    void ConfigInit(int showListNum);
private:
    void ChangeBottomStatus();

private:
    QListView *listView;
    QWidget *autoWidget;
    MaintenanceWidget *Maintenance;
//    MaintenanceCombineWidget *MaintenanceCombine;
    QAbstractItemModel *model;
    QStackedWidget *stackedWidget;
    ReagentManagerWidget *reagentManager;
    UseResourceWidget *userResource;
    PipeManngerWidget *pipeMannger;
    CombineOpsManagerWidget *combineOpsManager;
    HardwareCheckWidget *hardwareCheck;
    HardwareTestWidget *hardwareTest;
    IntellectCheck *intellectCheck;
    CommunicationTestWidget *communicationTest;
    UpdateWidget *updateWidget;

    QList<RoleType> m_privilegeList;
    QStringList m_strList;
    bool m_hasCombineOperation;

signals:
    void SignalOneKeyChangeReagentWindow();
private slots:
    void itemClicked(QModelIndex index);
};

}

#endif // REMOTEMAINTAINWINDOW_H
