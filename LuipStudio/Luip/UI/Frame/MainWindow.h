#ifndef UI_FRAME_MAINWINDOW_H
#define UI_FRAME_MAINWINDOW_H

#include <QStackedWidget>
#include "DropShadowWidget.h"
#include "TitleWidget.h"
#include "BottomWidget.h"
#include "UI/Home/RunStatusWindow.h"
#include "UI/Data/MeasureDataWindow.h"
#include "UI/Singal/SingalWindow.h"
#include "UI/Setting/MachineParamWindow.h"
#include "UI/Maintain/RemoteMaintainWindow.h"
#include "UI/System/SystemInfoWindow.h"
#ifdef _CS_ARM_LINUX
#include "System/LockScreen/ScreenProtection.h"
using namespace System::Screen;
#endif
//#include "ToolButton.h"
//#include "UI/Home/widget.h"

namespace UI
{

class MainWindow: public DropShadowWidget
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
    void SlotChangeUserInterface();
    void SlotUpdaterSignalTimer();
    void ScreenUpdate();
signals:
    void remoteUpdate();
private:

    QStackedWidget *m_statkedWidget;
    TitleWidget *m_titleWidget; //标题栏
    BottomWidget *m_bottomWidget; //底层栏

    RunStatusWindow *m_runStatusWindow;          //运行状态界面
    MeasureDataWindow *m_measureDataWindow;      //测量数据界面
    SingalWindow *m_singalWindow;             //故障日志界面
    MachineParamWindow *m_machineParamWindow;     //仪器参数界面
    RemoteMainTainWindow *m_remoteMainTainWindow; //远程维护界面
    SystemInfoWindow *m_systemInfoWindow;         //系统信息界面
    QTimer *m_updaterSignalTimer;
    bool m_isRunning;
#ifdef _CS_ARM_LINUX
    Screenprotection* m_screenProtection;
#endif
};

}

#endif // UI_FRAME_MAINWINDOW_H
