#ifndef EXAUTOTEMPCALIBRATEDIALOG_H
#define EXAUTOTEMPCALIBRATEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/Frame/MQtableWidget.h"
#include "oolua.h"

namespace UI
{

struct ExTempCalibrateItem
{
    QString name;
    int index;
    float value;
    QString unit;
    QPushButton* confirmButton;
    OOLUA::Lua_func_ref checkValueFunc;
    OOLUA::Lua_func_ref calibrateFunc;
};

class ExAutoTempCalibrateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ExAutoTempCalibrateDialog(QWidget *parent = 0);
    ~ExAutoTempCalibrateDialog();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void SpaceInit();

private slots:
    void SlotQuitButton();
    void SlotCheckValue(QTableWidgetItem *item);
    void SlotDoubleClicked(QTableWidgetItem *item);
    void SlotCalibrateButton(int row);

private:
    QVector<ExTempCalibrateItem> m_itemVec;
     MQtableWidget *m_tempCalibrateTable;

    QPushButton* m_okButton;
    QPushButton* m_quitButton;
    CNumberKeyboard *m_numberKey;


};

}

#endif // AUTOTEMPCALIBRATEDIALOG_H
