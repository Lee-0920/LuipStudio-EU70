/*
    标题  包括：最小化、关闭等
    作者：╰☆奋斗ing❤孩子`
    博客地址：http://blog.sina.com.cn/liang19890820
    QQ：550755606
    Qt分享、交流群：26197884

    注：请尊重原作者劳动成果，仅供学习使用，请勿盗用，违者必究！
*/

#ifndef UI_FRAME_TITLEWIDGET_H
#define UI_FRAME_TITLEWIDGET_H

#include <QWidget>
#include <QtWidgets>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include "ToolButton.h"
#include "PushButton.h"
#include "UI/Frame/LoginDialog.h"
#include "System/LockScreen/ILockScreenNotifiable.h"
#include "UI/UserManager/UserManagerWindow.h"

using System::ILockScreenNotifiable;

namespace UI
{

class TitleWidget : public QWidget ,public ILockScreenNotifiable
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = 0);
    void TranslateLanguage();
    void ChangeContextMenu();
    void SetupUIWidget();
    void OnLockScreen();

signals:

    void TurnPage(int currentPage);
    void CreatContextMenu();
    void ChangeUserInterface();
    void ScreenAlreadyLock();
    void ShowMin();
    void CloseWidget();

public slots:

    void SlotTurnPage(QString currentPage);
    void SlotCreatContextMenu();
    void slotMenuTriggered(QAction *action);
    void slotUserCancellation();
    void SlotTimerout();
private:
    void ChangeBottomStatus();
    void UserChangeInfo(RoleType old);

    void ChangeUser();
    void ModifyPassword();
    void UserCancellation();
    void ScreenShot();
    void UserManager();

private:

    bool m_isMove;
    QLabel *m_softwareTitle; //标题

#ifdef _CS_ARM_LINUX
    QPushButton *m_closeButton; //关闭
#endif

#ifdef _CS_X86_WINDOWS
    PushButton *m_minButton; //最小化
    PushButton *m_closeButton; //关闭
#endif

    QList<ToolButton *> m_buttonList;

    ToolButton *m_contextMenuButton;
 //   QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;
    QHBoxLayout *titleLayout;
    QHBoxLayout *buttonLayoutTemp;
    QMenu *contextMenu;
    QTimer *screenShotTimer;

    QStringList m_menuList;
    QList<QAction *> m_actionList;
    UserManagerWindow *m_userManagerWindow;
};

}

#endif // UI_FRAME_TITLEWIDGET_H
