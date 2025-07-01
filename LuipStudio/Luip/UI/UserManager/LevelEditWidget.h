#ifndef LEVELEDITWIDGET_H
#define LEVELEDITWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/Frame/MQtableWidget.h"
#include "oolua.h"
#include "DataBasePlugin/DataBaseManager.h"
#include "UI/Frame/QMyEdit.h"
#include "UI/Frame/DropShadowWidget.h"
#include "UI/Frame/MultiSelectComboBox.h"

using namespace DataBaseSpace;

namespace UI
{

class LevelEditWidget : public DropShadowWidget
{
    Q_OBJECT
public:
    explicit LevelEditWidget(AccessLevelRecord record, QWidget *parent);
    ~LevelEditWidget();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    void SpaceInit();
    void EditSpaceInit();
    int GetLevelMap();
    void AuditTrail(AccessLevelRecord oldRecord, AccessLevelRecord newRecord);
private slots:
    void SlotQuitButton();
    void SlotOkButton();
    void SlotNameEdit();
    void SlotCheckAll();
private:
    MultiSelectComboBox* signalComboBox;
    MultiSelectComboBox* settingComboBox;
    MultiSelectComboBox* maintainComboBox;
    MultiSelectComboBox* systemComboBox;
    QPushButton* okButton;
    QPushButton* quitButton;
    QPushButton* checkAllButton;
    QLabel* m_titleLabel;
    QLabel* m_nameLabel;
    QMyEdit* m_nameEdit;
    QList<QCheckBox*> m_checkBoxes;
    MQtableWidget *measureResultTableWidget;    
    QStringList m_rowName;
    AccessLevelRecord m_accessLevelRecord;
signals:

};

}

#endif // AUTOTEMPCALIBRATEDIALOG_H
