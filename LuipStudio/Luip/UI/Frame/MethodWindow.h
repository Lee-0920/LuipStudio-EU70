#ifndef UI_FRAME_METHDOWINDOW_H
#define UI_FRAME_METHDOWINDOW_H

#include <QStackedWidget>
#include "DropShadowWidget.h"
#include "MethodTitleWidget.h"
#include "MethodViewWidget.h"
#include "MethodWidget.h"
#ifdef _CS_ARM_LINUX
#include "System/LockScreen/ScreenProtection.h"
using namespace System::Screen;
#endif
//#include "ToolButton.h"
//#include "UI/Home/widget.h"

namespace UI
{

class MethodWindow: public QDialog
{
Q_OBJECT

public:
    MethodWindow(QWidget *parent = 0);
    ~MethodWindow();

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
    MethodTitleWidget *m_titleWidget; //标题栏
    MethodWidget *m_methodWidget;     //方法设置窗口
    MethodViewWidget *m_methodViewWidget;     //方法查看窗口

    QTimer *m_updaterSignalTimer;
    bool m_isRunning;
};

}

#endif // UI_FRAME_MethodWindow_H
