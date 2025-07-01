#include "UI/MainWindow.h"
#include "Treasure/SystemDef.h"
#include "Treasure/Control/SplashScreen.h"
#include "Treasure/Control/App.h"
#include "VncPlugin/VncManager.h"
#include "UdpPlugin/UdpManager.h"
#include "Log.h"

int main(int argc, char *argv[])
{
    Treasure::SignalApp app(argc, argv, "REMOTE");
    if (app.IsRunning())
    {
        QMessageBox::warning(NULL, "提示", "程序已经运行", QMessageBox::Yes, QMessageBox::Yes);
        return 0;
    }

    app.SetStyle("Fusion");
    app.SetFont("Microsoft YaHei", FONT_SIZE);
    app.SetQss(":/qss/UI/Style.qss");
#ifndef _WIN32
    app.RemoveCursor();
    Treasure::SplashScreen splash(QApplication::desktop()->size());
    splash.SetJpg(":/UI/svg/startLogo");
    splash.show();
    app.processEvents();
#endif


#ifdef HAVE_LOG_LIB
    Log::Instance()->Init();
#endif
    trLogger->Debug("*****************************************************");
    trLogger->Debug(QString("系统版本[%1]").arg(APP_VERSION));
    trLogger->Debug("*****************************************************");

    trLogger->ShowQDebug(true);

    VncSpace::VncManager::Instance()->Init(trLogger);

    MainWindow w;
#ifdef QT_DEBUG
    int width = 1024;
    int height = 600;
#ifdef _WIN32
    height = 763;
#endif
    w.setFixedSize(width,height);
#else
    w.setFixedSize(QApplication::desktop()->screen()->width() + 2,
                   QApplication::desktop()->screen()->height() + 2);
    w.move(-1, -1);
#endif
    w.show();

#ifndef _WIN32
    splash.hide();
#endif

#ifdef HAVE_NETMODE_FUNC
    //w.OnUdhcpdMode();
    w.OnLocalAreaMode();
#endif

    UdpSpace::UdpManager::Instance()->InitByServer(trLogger);
    QThread::sleep(1);
    UdpSpace::UdpManager::Instance()->ClearBeforeSendSerach();
    UdpSpace::UdpManager::Instance()->SendSearch();

#ifndef QT_DEBUG
#ifdef HAVE_NFS_SERVER
    UdpSpace::UdpManager::Instance()->SendNFSServerNotify(true);
#else
    UdpSpace::UdpManager::Instance()->SendNFSServerNotify(false);
#endif
#endif

    app.exec();
    w.hide();

    VncSpace::VncManager::Instance()->Uninit();
    UdpSpace::UdpManager::Instance()->Uninit();
    Style::Instance()->Uninit();

    return 0;
}
