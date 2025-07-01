#ifndef METHODWIDGET_H
#define METHODWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/Frame/MQtableWidget.h"
#include "oolua.h"
#include "UI/Frame/QMyEdit.h"
#include "DataBasePlugin/DataBaseManager.h"
#include "MethodViewWidget.h"
#include "System/Types.h"
#include "UI/Frame/DropShadowWidget.h"
using namespace DataBaseSpace;

namespace UI
{

class MethodWidget : public DropShadowWidget
{
    Q_OBJECT
public:
    explicit MethodWidget(QWidget *parent = 0);
    ~MethodWidget();
    static MethodWidget *Instance();   

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    void SpaceInit();
    void OffLineSetting_Show();
    void OffLineSetting_Hide();
private slots:
    void SlotQuitButton();
    void SlotApplyButton();
    void SlotSaveButton();
    void SlotCheckValue(QTableWidgetItem *item);
    void SlotDoubleClicked(QTableWidgetItem *item);
    void SlotMyEdit();
    void SlotNameEdit();
    void SlotOnlineCheckBox(int arg);
    void SlotOfflineCheckBox(int arg);
    void SlotUpdateMethod(MethodRecord record, bool isWriteToSql, bool isFromModbus);

private:    
    QPushButton* m_saveAsButton;
    QPushButton* m_saveButton;
    QPushButton* m_applyButton;
    CNumberKeyboard *m_numberKey;
    QLabel* m_nameLabel;
    QMyEdit* m_nameEdit;
    QCheckBox* m_onlineCheckBox;
    QCheckBox* m_offlineCheckBox;
    QCheckBox* m_turboCheckBox;
    QCheckBox* m_ICRCheckBox;
    QCheckBox* m_TOCCheckBox;
    QCheckBox* m_ECCheckBox;
    QCheckBox* m_autoReagentCheckBox;
    QLabel* m_reagent1Label;
    QMyEdit* m_reagent1Edit;
    QLabel* m_reagent2Label;
    QMyEdit* m_reagent2Edit;
    QLabel* m_cleanTimeLabel;
    QMyEdit* m_cleanTimeEdit;
    QLabel* m_measureTimesLabel;
    QMyEdit* m_measureTimesEdit;
    QLabel* m_rejectTimesLabel;
    QMyEdit* m_rejectTimesEdit;
    static std::unique_ptr<MethodWidget> m_instance;
signals:
    void MethodWidgetUpdateSignal(MethodRecord, bool, bool);
};

}

#endif // AUTOTEMPCALIBRATEDIALOG_H
