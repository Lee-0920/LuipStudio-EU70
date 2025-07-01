#ifndef INSTRUMENTINFORMATIONWIDGET_H
#define INSTRUMENTINFORMATIONWIDGET_H
#include <QPushButton>
#include <QFont>
#include <QTableWidget>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include <iostream>
#include "SetSNDialog.h"

namespace UI
{

class InstrumentInformationWidget: public QWidget, public IUserChangeNotifiable, public IUpdateWidgetNotifiable
{
Q_OBJECT
public:
    explicit InstrumentInformationWidget(QWidget *parent = 0);
    ~InstrumentInformationWidget();
    void OnUserChange();

    void OnUpdateWidget(UpdateEvent event, System::String message);
private:
    QLabel *typeName;
    QLabel *nameValue;
    QLabel *argumentName;
    QLabel *serialnumberName;
    QLabel *typeNumberName;
    QLabel *versionName;
    QLabel *platformName;
    QLabel *firmName;
    QPushButton *m_setSNButton;
    QLabel *m_blankLabel;

    QLabel *qrencodeLabel;

    SetSNDialog* m_setSNDialog;

    void ChangeBottomStatus();
    void InfoDisplay();

protected:
    void showEvent(QShowEvent *event);

private slots:
    void SlotSetSNButton();
    void UpdateQrencode();

    //   void slotDoubleClicked(QTableWidgetItem *item);
};

}
#endif // INSTRUMENTINFORMATIONWIDGET_H
