#ifndef NETSET_H
#define NETSET_H

#include <QList>
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
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "Setting/SettingManager.h"
#include <QProcess>
#include <QTimer>
#include "UI/Frame/UpdateWidgetManager.h"

using Configuration::SettingIPMode;

namespace UI
{

struct DNSInfo
{
    QString name;
    QString addr;
};
class NetSet: public QWidget ,  public IUserChangeNotifiable, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    explicit NetSet(QWidget *parent = 0);
    ~NetSet();
    void OnUserChange();

    void OnUpdateWidget(UpdateEvent event, System::String message);

protected:
    virtual void showEvent(QShowEvent *event);

private:

    QLineEdit *textEdit;
    QTableWidget *measureResultTableWidget;
    QPushButton *useButton;

    QRadioButton *DHCPRadio;
    QRadioButton *staticRadio;
    QButtonGroup *radioButtonGroup;

    QLabel *netAddrLabel;
    QLabel *subnetLabel;
    QLabel *gateWayLabel;
    QLabel *dns1Label;
    QLabel *dns2Label;

    QLineEdit *staticNetAddressEdit;
    QLineEdit *staticSubnetEdit;
    QLineEdit *staticGateWayEdit;
    QLineEdit *staticDNS1Edit;
    QLineEdit *staticDNS2Edit;

    QProcess *process;
    QProcess *DHCPprocess;
    QTimer *m_timer;
    bool isDHCPStop;
    bool isDHCPOK;
    bool isLinkDown;
    void UpdateSetIPMode(SettingIPMode mode);
    bool CheckValue();
    bool SaveIPToScript();
    bool IsSupportDHCP(bool isWarning);
    bool StartProcess(const QString &command, int msecs, bool isTimeOutClose);

    void GetDNS(QList<DNSInfo> &dnsList);
    bool SaveDNS();
    void UpdateDNSInfo(SettingIPMode mode);
    void SetDNS();
public slots:
    void UpdateCurrentIPInfo();//获取当前IP信息

private slots:
    void NetApply();
    void OnRadioClick();
    void TimerHandler();
    void DHCPFinish(int exitCode);
};

}

#endif // NETSET_H
