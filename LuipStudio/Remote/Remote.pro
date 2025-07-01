
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Remote
TEMPLATE = app

VERSION = 2.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG += -std=c++11


#DEFINES += HAVE_LOG_LIB        #添加log日志库
DEFINES += HAVE_NETMODE_FUNC   #网络模式切换功能：直连和局域网
#DEFINES += HAVE_NFS_SERVER     #NFS服务功能
#DEFINES += HAVE_EXIT_BUTTON     #退出按钮

INCLUDEPATH += ../
if (contains(DEFINES, HAVE_LOG_LIB)){
    INCLUDEPATH += ../Lib/log4cpp/include
    win32{
        LIBS += -L$$PWD/../Lib/log4cpp/lib/windows/ -llog4cpp
    }
    unix {
        LIBS += -L$$PWD/../Lib/log4cpp/lib/linux-arm -llog4cpp
    }
}



win32{
    DEFINES += FONT_SIZE=12         #字体大小
    Debug:DESTDIR = ../../bin
    Release:DESTDIR = ../../bin
    DEFINES += _CS_X86_WINDOWS
    LIBS += -lws2_32
    LIBS += -L$$PWD/$$DESTDIR/ -lVncPlugin
    LIBS += -L$$PWD/$$DESTDIR/ -lUdpPlugin
}

unix {
     DEFINES += FONT_SIZE=12         #字体大小
     DESTDIR = ../../bin
     CONFIG += arm

    arm {

        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_USING_MODBUS
        DEFINES += _DEV_USING_CONTROL
        DEFINES += _DEV_USING_CURRENT
        LIBS += -L$$PWD/$$DESTDIR/ -lVncPlugin
        LIBS += -L$$PWD/$$DESTDIR/ -lUdpPlugin
    }
}



SOURCES += main.cpp \
    Log.cpp \
    UI/AddWidget.cpp \
    UI/ExplorerWidget.cpp \
    UI/MainWindow.cpp \
    UI/MessageWidget.cpp \
    UI/NFSManager.cpp \
    UI/Style.cpp


HEADERS  += \
    Log.h \
    UI/AddWidget.h \
    UI/ExplorerWidget.h \
    UI/MainWindow.h \
    UI/MessageWidget.h \
    UI/NFSManager.h \
    UI/Style.h

DISTFILES +=

RESOURCES += \
    rc.qrc

RC_ICONS += UI/svg/logo.ico
