#ifndef UI_UPDATEWIDGT_H
#define UI_UPDATEWIDGT_H

#include <QPushButton>
#include <QFont>
#include <QTableWidget>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/MQtableWidget.h"
#include "UI/Frame/ReagentAuthorizationDialog.h"
#include <QTimer>
#include "System/Types.h"
#include "oolua.h"
#include "Communication/Dscp/DscpAddress.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/RowHiddenHandler.h"

namespace UI
{
struct UpdateItem
{
    String name;
    String text;
    System::String hexFileName;
    Communication::Dscp::DscpAddress addr;
    String fileName;
    RoleType writePrivilege;
    RoleType readPrivilege;
};

class UpdateWidget: public QWidget , public IUserChangeNotifiable
       // ,public IUpdateResult
{
    Q_OBJECT
public:
    explicit UpdateWidget(QWidget *parent = 0);
    ~UpdateWidget();
    void OnUserChange();
    void OnResult(QString result);

private:
    MQtableWidget *updateTableWidget;
    QWidget *rightWidget;
    QAbstractItemModel *model;
    QPushButton *oneCheck;
    QPushButton *refreshButton;
    QLabel *SourceLabel;
    QTimer *m_getInfoTimer;
    std::map<System::Uint8, QCheckBox *>m_checkBox;
    std::map<System::Uint8, UpdateItem> m_items;
    String m_masterFileName;
    String m_toolFileName;
    TableWidgetInfo m_tableInfo;

    ReagentAuthorizationDialog* m_authorizationDlg;

    bool m_isGetInfo;
    void SpaceInit();
    bool UpdatePacketCheck();
    void WidgetUpdate();

protected:
    void showEvent(QShowEvent *event);

signals:
    void UpdaterResultSignals(QString result);

private slots:
    void oneCheckConfig();
    void UpdaterResultSlots(QString result);
    void GetInfoTimerHandle();
    void RefreshButtonSlot();
};
}
#endif // UI_UPDATEWIDGT_H
