#ifndef UI_UI_SYSTEMINFOWINDOW_H
#define UI_SYSTEMINFOWINDOW_H
#include <QWidget>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QtWidgets/QListView>
#include <QtWidgets/QStackedWidget>
#include <QLineEdit>
#include <QtWidgets/QStackedWidget>
#include <QLabel>
#include <QTableWidget>
#include <QDebug>
#include <QStringListModel>
#include <QComboBox>
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "InstrumentInformationWidget.h"
#include "DeviceInformation.h"
//#include "AmountMannger.h"
#include "NetSet.h"
#include "SystemTime.h"
#include "FactoryTime.h"
namespace UI
{

class SystemInfoWindow: public QWidget, public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit SystemInfoWindow(QWidget *parent = 0);
    ~SystemInfoWindow();

    void OnUserChange();

private:
    void ChangeBottomStatus();

private:
    QAbstractItemModel *standardItemModel;
    QListView *listView;
    QLineEdit *textEdit;
    QStackedWidget *stackedWidget;
    QTableWidget *measureResultTableWidget;
    QTableWidget *otherwidget;

    QLineEdit *minTime;
    QLineEdit *maxTime;
    QLabel *dateModel;
    QLabel *dateType;
    QComboBox *dateTypeCombobox;
    QComboBox *dateModelCombobox;
    QAbstractItemModel *model;
    InstrumentInformationWidget *instrumentInformation;
    DeviceInformation *deviceInformation;
    NetSet *netSet;
    SystemTimeWidget *systemTime;
    FactoryTimeWidget *factoryTime;

signals:
    void UpdateNetSetView();
private slots:
    void itemClicked(QModelIndex index);
};

}
#endif // SYSTEMINFOWINDOW_H
