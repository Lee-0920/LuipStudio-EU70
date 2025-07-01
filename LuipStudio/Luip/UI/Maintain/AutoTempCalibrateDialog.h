#ifndef AUTOTEMPCALIBRATEDIALOG_H
#define AUTOTEMPCALIBRATEDIALOG_H

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

struct TempCalibrateItem
{
    QString name;
    int index;
    float value;
    QString unit;
    QPushButton* confirmButton;
    OOLUA::Lua_func_ref checkValueFunc;
    OOLUA::Lua_func_ref calibrateFunc;
};

class AutoTempCalibrateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AutoTempCalibrateDialog(QWidget *parent = 0);
    ~AutoTempCalibrateDialog();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void SpaceInit();

private slots:
    void SlotQuitButton();
    void SlotApplyButton();
    void SlotCheckValue(QTableWidgetItem *item);
    void SlotDoubleClicked(QTableWidgetItem *item);
    void SlotCalibrateButton(int row);

private:
    QVector<TempCalibrateItem> m_itemVec;
     MQtableWidget *m_tempCalibrateTable;

    QPushButton* m_okButton;
    QPushButton* m_quitButton;
    CNumberKeyboard *m_numberKey;
    QPushButton* m_applyButton;


};

}

#endif // AUTOTEMPCALIBRATEDIALOG_H
