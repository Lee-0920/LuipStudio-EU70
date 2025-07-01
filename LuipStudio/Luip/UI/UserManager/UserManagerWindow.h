#ifndef UI_FRAME_USERMANAGERWINDOW_H
#define UI_FRAME_USERMANAGERWINDOW_H

#include <QStackedWidget>
#include "UI/Frame/DropShadowWidget.h"
#include "UserManagerTitleWidget.h"
#include "AccessLevelWidget.h"
#include "UserInfoWidget.h"
#ifdef _CS_ARM_LINUX
#include "System/LockScreen/ScreenProtection.h"
using namespace System::Screen;
#endif

namespace UI
{

class UserManagerWindow: public QDialog
{
Q_OBJECT

public:
    UserManagerWindow(QWidget *parent = 0);
    ~UserManagerWindow();

    bool IsRunning();
    void SetupBottomWidget();
    void setupContextMenu();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

public slots:
    void ShowWidget();

private slots:
    void SetupTitleWidget();
    void SetupUIWidget();
    void SlotTurnPage(int currentPage);
    void SlotUpdaterSignalTimer();
    void ScreenUpdate();
signals:
    void remoteUpdate();
private:

    QStackedWidget *m_statkedWidget;
    UserManagerTitleWidget *m_titleWidget; //标题栏    
    UserInfoWidget *m_userInfoWidget;     //用户信息窗口
	AccessLevelWidget *m_accessLevelWidget;     //权限窗口

    QTimer *m_updaterSignalTimer;
    bool m_isRunning;
};

}

#endif // UI_FRAME_USERMANAGERWINDOW_H
