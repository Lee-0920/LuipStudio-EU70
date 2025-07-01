#ifndef ACCESSLEVELWIDGET_H
#define ACCESSLEVELWIDGET_H

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

class AccessLevelWidget : public QDialog
{
    Q_OBJECT
public:
    explicit AccessLevelWidget(QWidget *parent = 0);
    ~AccessLevelWidget();
     static AccessLevelWidget *Instance();
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
    QString GetAuthorityString(qint64 num);
private slots:
    void SlotQuitButton();
    void SlotAddButton();
    void ToTop();
    void ToBottom();
    void ToBack();
    void ToNext();
    void SlotExportButton();
    void SlotDelButton();
    void SlotEditButton();
    void SlotMethodSaveForModbus(System::String);
    void SlotMethodApplyForModbus(int);
    void SlotMethodDelectForModbus(int);
private:
    MQtableWidget *m_tempCalibrateTable;
    QVector<AccessLevelRecord> m_showFields;
    QPushButton* m_okButton;
    QPushButton* m_quitButton;
    CNumberKeyboard *m_numberKey;
    QPushButton* m_applyButton;
    MQtableWidget *measureResultTableWidget;
    QPushButton *toTopButton;
    QPushButton *toBackButton;
    QPushButton *toNextButton;
    QPushButton *toBottomButton;
    QPushButton *exportButton;
    QPushButton *delButton;
    QPushButton *editButton;
    QPushButton *addButton;
    int m_curPage;
    int m_totalLevel;
    QStringList m_columnName;
    static std::unique_ptr<AccessLevelWidget> m_instance;
signals:
    void MethodUpdateSignal(MethodRecord, bool, bool);
    void MethodSaveForModbusSignal(System::String);
    void MethodApplyForModbusSignal(int);
    void MethodDelectForModbusSignal(int);
};

}

#endif // AUTOTEMPCALIBRATEDIALOG_H
