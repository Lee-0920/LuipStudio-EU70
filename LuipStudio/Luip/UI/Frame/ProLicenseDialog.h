#ifndef UI_FRAME_PROLICENSEDIALOG_H
#define UI_FRAME_PROLICENSEDIALOG_H

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include "DropShadowWidget.h"
#include "UI/Frame/QMyEdit.h"
#include "UI/Frame/NumberKeyboard.h"

namespace UI
{

class ProLicenseDialog: public DropShadowWidget
{
Q_OBJECT

public:
    explicit ProLicenseDialog(QWidget *parent = 0);
    ~ProLicenseDialog();

public:
    void TranslateLanguage();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);

protected:
    void ShowContent();

protected slots:
    void SlotloginButton();

private:
    QLabel *m_titleLabel;
    QLabel *m_logoLabel;
    QLabel *m_codeLabel;
    QLineEdit *m_codeEdit;
    QPushButton *m_loginButton;
    QPushButton *m_cancelButton;
    int m_totalClick;
    bool m_superAppear;
    CNumberKeyboard *m_dialogKeyboard;
    QCalendarWidget *m_dayCaledar;
};

}

#endif //UI_FRAME_PROLICENSEDIALOG_H
