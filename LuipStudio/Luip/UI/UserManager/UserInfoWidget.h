#ifndef USERINFOWIDGET_H
#define USERINFOWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/Frame/MQtableWidget.h"
#include "oolua.h"
#include "DataBasePlugin/DataBaseManager.h"
#include "System/Types.h"
using namespace DataBaseSpace;

namespace UI
{

class UserInfoWidget : public QDialog
{
    Q_OBJECT
public:
    explicit UserInfoWidget(QWidget *parent = 0);
    ~UserInfoWidget();
     static UserInfoWidget *Instance();
     void MethodSaveForModbus(System::String);
     void MethodApplyForModbus(int);
     void MethodDelectForModbus(int);

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void SpaceInit();
    void TableSpaceInit();
    void ViewRefresh();
    void ShowTable();
private slots:
    void SlotQuitButton();      
    void ToTop();
    void ToBottom();
    void ToBack();
    void ToNext();
    void SlotAddButton();
    void SlotDelButton();
    void SlotEditButton();
    void SlotMethodSaveForModbus(System::String);
    void SlotMethodApplyForModbus(int);
    void SlotMethodDelectForModbus(int);
private:
    MQtableWidget *m_tempCalibrateTable;
    QVector<UserRecord> m_showFields;
    QPushButton* m_okButton;
    QPushButton* m_quitButton;
    CNumberKeyboard *m_numberKey;
    QPushButton* m_applyButton;
    MQtableWidget *measureResultTableWidget;
    QPushButton *toTopButton;
    QPushButton *toBackButton;
    QPushButton *toNextButton;
    QPushButton *toBottomButton;
    QPushButton *delButton;
    QPushButton *editButton;
    QPushButton *addButton;
    int m_curPage;
    int m_totalUsers;
    QStringList m_columnName;
    static std::unique_ptr<UserInfoWidget> m_instance;
signals:
    void MethodUpdateSignal(MethodRecord, bool, bool);
    void MethodSaveForModbusSignal(System::String);
    void MethodApplyForModbusSignal(int);
    void MethodDelectForModbusSignal(int);
};

}

#endif // AUTOTEMPCALIBRATEDIALOG_H
