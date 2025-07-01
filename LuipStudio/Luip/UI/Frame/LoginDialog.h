/*
 登陆界面 包括360、新浪、人人登录
 作者：╰☆奋斗ing❤孩子`
 博客地址：http://blog.sina.com.cn/liang19890820
 QQ：550755606
 Qt分享、交流群：26197884

 注：请尊重原作者劳动成果，仅供学习使用，请勿盗用，违者必究！
 */

#ifndef UI_FRAME_LOGINDIALOG_H
#define UI_FRAME_LOGINDIALOG_H

#include <QDialog>
#include <QStackedLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QKeyEvent>
#include <QGridLayout>
#include "Common.h"
#include "DropShadowWidget.h"
#include "UI/Frame/NumberKeyboard.h"
#include "DataBasePlugin/DataBaseManager.h"

using namespace DataBaseSpace;

namespace UI
{

enum RoleType
{
    None = 0,
    General = 1,
    Maintain = 2,
    Administrator = 3,
    Super = 4,
};

struct UserInfo
{
    QString userName;
    QString levelName;
    int levelInt;
};

class LoginDialog: public DropShadowWidget
{
Q_OBJECT

public:

    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

public:
    void TranslateLanguage();

public slots:
    void SlotloginButton();
protected:

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

public:
    static RoleType userType;
    static UserInfo userInfo;
    static bool GetUserPrivilege(Authority arg)
    {
        if(userInfo.levelInt & (1 << (int)arg))
        {
//            qDebug()<< "#" << QString::number((int)arg);
            return true;
        }
        else
        {
//            qDebug()<< "@" << QString::number((int)arg);
            return false;
        }
    }

private:

    QLabel *m_titleLabel;
    QLabel *m_logoLabel;
    QLabel *m_userLabel;
    QLabel *m_passwordLabel;
    QComboBox *m_userComboBox;
    QLineEdit *m_passwordLineEdit;
    QPushButton *m_loginButton;
    QPushButton *m_cancelButton;
    int m_totalClick;
    bool m_superAppear;
    CNumberKeyboard *m_loginDialogKeyboard;
};

}

#endif //UI_FRAME_LOGINDIALOG_H
