#ifndef USEREDITWIDGET_H
#define USEREDITWIDGET_H

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

using namespace DataBaseSpace;

namespace UI
{

class UserEditWidget : public DropShadowWidget
{
    Q_OBJECT
public:
    explicit UserEditWidget(UserRecord record, QWidget *parent);
    ~UserEditWidget();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    void SpaceInit();   
    void EditSpaceInit();
    void AuditTrail(UserRecord oldRecord, UserRecord newRecord);
private slots:
    void SlotQuitButton();
    void SlotOkButton();
    void SlotNameEdit();
private:
    QPushButton* okButton;
    QPushButton* quitButton;
    QLabel* m_titleLabel;
    QLabel* m_nameLabel;
    QMyEdit* m_nameEdit;
    QLabel* m_passwordLabel;
    QMyEdit* m_passwordEdit;
    QLabel* m_passwordConfirmLabel;
    QMyEdit* m_passwordConfirmEdit;
    QLabel* m_userLevelLabel;
    QComboBox* m_userLevelCombox;
    QLabel* m_userStatusLabel;
    QComboBox* m_userStatusCombox;

    UserRecord m_userRecord;
signals:

};

}

#endif // AUTOTEMPCALIBRATEDIALOG_H
