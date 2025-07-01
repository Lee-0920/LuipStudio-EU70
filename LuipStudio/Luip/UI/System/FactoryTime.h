#ifndef FACTORYTIME_H
#define FACTORYTIME_H

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
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include <QFileInfo>
#include "UI/Frame/ReagentAuthorizationDialog.h"
#include "UI/Frame/ReagentAuthorizationConfigDialog.h"
#include "UI/Frame/MessageDialog.h"
#include "UI/Frame/QclickLabel.h"
#include <QFileInfo>
#include "UI/Frame/ProLicenseDialog.h"

namespace UI
{

class FactoryTimeWidget: public QWidget,  public IUserChangeNotifiable, public IUpdateWidgetNotifiable
{
    Q_OBJECT
public:
    explicit FactoryTimeWidget(QWidget *parent = 0);
    ~FactoryTimeWidget();
    void OnUserChange();
    void OnUpdateWidget(UpdateEvent event, System::String message);

signals:
    void SignalShowReagentAuthorizationDialog();

protected:
    void ResetRotalClickTime();
    void AddRotalClickTime();
    int GetRotalClickTime();

protected:
    void showEvent(QShowEvent *event);

private:
    QFileInfoList GetSysLogFileList();
//    QFileInfoList GetHj212LogFileList();

private:
    QAbstractItemModel *standardItemModel;
    QLineEdit *textEdit;
    QStackedWidget *stackedWidget;
    QTableWidget *measureResultTableWidget;
    QTableWidget *otherwidget;
    QPushButton *defaultPassword;
    QPushButton *defaultFactory;
    QPushButton *screenCalibrate;
    QPushButton *clearFault;
    QPushButton *activation;
    QPushButton *clearData;
    QPushButton *auditTrailLogExport;
    QPushButton *sysLogExport;
    QPushButton *sysLogClear;
    QPushButton *reagentAuthorizationClear;
    QPushButton *proLicenseActive;
    QLineEdit *minTime;
    QLineEdit *maxTime;
    QLabel *dateModel;
    QLabel *dateType;
    QComboBox *dateTypeCombobox;
    QComboBox *dateModelCombobox;
    QAbstractItemModel *model;
    ClickableLabel *reagentLabel;
    ClickableLabel *debugLabel;

    int m_totalClick;
    ReagentAuthorizationDialog* m_authorizationDlg;
    ReagentAuthorizationConfigDialog* m_authorizationConfigDlg;
    ProLicenseDialog* m_proLicenseDlg;


private slots:
    void SlotDefaultPassword();
    void SlotDefaultFactory();
    void SlotScreenCalibrate();
    void SlotSysLogExport();
    void SlotSysLogClear();
    void SlotReagentAuthorizationClear();
    void SlotAuditTrailLogExport();
    void SlotClearData();
    void SlotActivation();
    void SlotClearFault();
    void SlotReagentLabelClicked();
    void SlotDebugLabelClicked();
    void SlotShowReagentAuthorizationDialog();
    void SlotProLicenseActive();
};

}

#endif // FACTORYTIME_H
