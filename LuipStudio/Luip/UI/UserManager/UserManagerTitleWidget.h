#ifndef UI_FRAME_USER_MANAGER_TITLE_WIDGET_H
#define UI_FRAME_USER_MANAGER_TITLE_WIDGET_H

#include <QWidget>
#include <QtWidgets>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include "UI/Frame/ToolButton.h"
#include "UI/Frame/PushButton.h"
#include "UI/Frame/LoginDialog.h"
#include "System/LockScreen/ILockScreenNotifiable.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"

using System::ILockScreenNotifiable;

namespace UI
{

class UserManagerTitleWidget : public QWidget ,public ILockScreenNotifiable, public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit UserManagerTitleWidget(QWidget *parent = 0);
    void TranslateLanguage();
    void ChangeContextMenu();
    void SetupUIWidget();
    void OnLockScreen();
    void OnUserChange();
signals:

    void MethodTurnPage(int currentPage);   
    void ScreenAlreadyLock();
    void ShowMin();
    void CloseWidget();
    void UserManagerQuit();

public slots:

    void SlotTurnPage(QString currentPage);
    void slotUserCancellation();  
private:
    void ChangeBottomStatus();    
private:
    QLabel *m_softwareTitle; //标题
    ToolButton *m_closeButton; //关闭
    QList<ToolButton *> m_buttonList;
    QVBoxLayout *mainLayout;
    QHBoxLayout *titleLayout;
    QHBoxLayout *buttonLayoutTemp;
};

}

#endif // UI_FRAME_TITLEWIDGET_H
