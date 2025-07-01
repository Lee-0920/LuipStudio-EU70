#ifndef UI_MACHINEPARAMWINDOW_H
#define UI_MACHINEPARAMWINDOW_H

#include <QWidget>
#include <QListView>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QtWidgets/QListView>
#include <QtWidgets/QStackedWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QDebug>
#include <QStringListModel>
#include <QComboBox>
#include <QGroupBox>
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "AutoMeasureWidget.h"
#include "ParameterWidget.h"

namespace UI
{

struct MachineParamWidget
{
    ParameterWidget* parameterWidget;
    bool isMeaParaml;
    System::String text;
    System::Uint8 index;
    RoleType privilege;
};

class MachineParamWindow: public QWidget, public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit MachineParamWindow(QWidget *parent = 0);
    ~MachineParamWindow();

    void OnUserChange();

private:
    void ChangeBottomStatus();

private:
    QListView *listView;
    QWidget *autoWidget;
    QAbstractItemModel *model;
    QStackedWidget *stackedWidget;
    AutoMeasureWidget *autoMeasure;
    QStringList m_strList;
    QList<RoleType> m_privilegeList;
    std::vector<MachineParamWidget> m_machineParamWidget;

private slots:
    void itemClicked(QModelIndex index);
};

}

#endif // MACHINEPARAMWINDOW_H
