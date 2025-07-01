#ifndef DEVICEINFORMATION_H
#define DEVICEINFORMATION_H
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
#include <QPushButton>
#include <QFont>
#include <QTimer>
#include <QCalendarWidget>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/Types.h"
#include "oolua.h"

namespace UI
{

class DeviceInformation: public QWidget
{
Q_OBJECT
public:
    explicit DeviceInformation(QWidget *parent = 0);
    ~DeviceInformation();
    static QString GetFileSystemVersion();

private:
    MQtableWidget *deviceInformationTableWidget;
    QTimer *timer;

    void InitFormDeviceOne();
    void InitFormDeviceTwo();
    void InitFormDeviceThree();
    void SpaceInit();
    void TableInit(System::Uint8 deviceNum);
    std::map<System::Uint8, System::String>m_items;

protected:
    void showEvent(QShowEvent *event);

private slots:
    void SlotUpdateData();
};

}
#endif // DEVICEINFORMATION_H
