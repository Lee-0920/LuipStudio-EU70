#ifndef UI_FRAME_CHANGEPASSWORDDIALOG_H
#define UI_FRAME_CHANGEPASSWORDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QStackedLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QKeyEvent>
#include <QListWidget>
#include "Common.h"
#include "DropShadowWidget.h"
#include "UI/Frame/NumberKeyboard.h"


namespace UI
{


class ChangePasswordDialog : public DropShadowWidget
{
Q_OBJECT

public:
    explicit ChangePasswordDialog(QWidget *parent = 0);
    ~ChangePasswordDialog();

public:
    void TranslateLanguage();

public slots:
    void SlotPasswordChangeConfirmButton();
    void SlotnewPasswordChangeCancelButton();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    QLabel *m_titleLabel;
    QLabel *m_userLabel;
    QLabel *m_oldPassword;
    QLabel *m_newPassword;
    QLabel *m_newPasswordConfirm;

    QComboBox *m_userComboBox; //选择用户
    QLineEdit *m_oldPasswordLineEdit; //旧密码编辑框
    QLineEdit *m_newPasswordLineEdit; //新密码编辑框
    QLineEdit *m_newPasswordConfirmLineEdit; //新密码确认编辑框

    QPushButton *m_passwordChangeConfirmButton; //确认修改按钮
    QPushButton *m_newPasswordChangeCancelButton; //取消修改按钮
    int m_totalClick;
    bool m_superAppear;
    CNumberKeyboard *m_umberKeyboard;
};

}
#endif // UI_FRAME_CHANGEPASSWORDDIALOG_H
