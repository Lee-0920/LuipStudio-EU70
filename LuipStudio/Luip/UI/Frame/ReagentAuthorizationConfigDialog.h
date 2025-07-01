#ifndef UI_FRAME_REAGENTAUTHORIZATIONCONFIGDIALOG_H
#define UI_FRAME_REAGENTAUTHORIZATIONCONFIGDIALOG_H

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include "DropShadowWidget.h"
#include "UI/Frame/QMyEdit.h"
#include "UI/Frame/NumberKeyboard.h"

namespace UI
{

class ReagentAuthorizationConfigDialog: public DropShadowWidget
{
Q_OBJECT

public:
    explicit ReagentAuthorizationConfigDialog(QWidget *parent = 0);
    ~ReagentAuthorizationConfigDialog();

public:
    void TranslateLanguage();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

protected:
    void ShowContent();

protected slots:
    void SlotloginButton();
    void SlotChoseDay();
    void SlotChangeDay();
    void SlotEnableChange();

private:
    QLabel *m_titleLabel;
    QLabel *m_logoLabel;
    QLabel *m_enableLabel;
    QLabel *m_dateLabel;
    QComboBox *m_authorizationComboBox;
    QMyEdit *m_dateEdit;
    QPushButton *m_loginButton;
    QPushButton *m_cancelButton;
    int m_totalClick;
    bool m_superAppear;
    CNumberKeyboard *m_dialogKeyboard;
    QCalendarWidget *m_dayCaledar;
};

}

#endif //UI_FRAME_REAGENTAUTHORIZATIONCONFIGDIALOG_H
