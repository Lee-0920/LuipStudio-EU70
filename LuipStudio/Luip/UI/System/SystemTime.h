#ifndef SYSTEMTIME_H
#define SYSTEMTIME_H

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
#include "UI/Frame/NumberKeyboard.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/QMyEdit.h"
namespace UI
{

class SystemTimeWidget: public QWidget ,  public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit SystemTimeWidget(QWidget *parent = 0);
    ~SystemTimeWidget();
    void OnUserChange();

protected:
    virtual void showEvent(QShowEvent *event);

private:

    QMyEdit *dayEdit;
    QPushButton *confirmButton;
    QMyEdit *timeEdit;

private slots:
    void ChoseMinDay();
    void ChoseMinTime();
    void SetTime();
    void CheckDaySlot();

};

}

#endif // SYSTEMTIME_H
