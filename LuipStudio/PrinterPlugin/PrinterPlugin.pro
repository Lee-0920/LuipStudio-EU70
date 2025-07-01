#-------------------------------------------------
#
# Project created by QtCreator 2020-04-20T13:35:31
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = PrinterPlugin
TEMPLATE = lib
DESTDIR = ../../bin

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += ../Luip ../
INCLUDEPATH += ../Lib/log4cpp/include
INCLUDEPATH += ../Lib/lua/include
INCLUDEPATH += ../Lib/oolua/include

DEFINES += OOLUA_USE_EXCEPTIONS=1 OOLUA_STORE_LAST_ERROR=0

win32 {
    DEFINES += _CS_X86_WINDOWS
    DEFINES += BIL_EXPORT

    Debug:LIBS   += -L$$PWD/../Lib/oolua/lib/windows/ -looluaD

    Release:LIBS += -L$$PWD/../Lib/oolua/lib/windows/ -loolua

    LIBS   +=  -L$$PWD/../Lib/lua/lib/windows/ -llua \
               -L$$PWD/../Lib/log4cpp/lib/windows/ -llog4cpp \
               -L$$PWD/../../bin/ -lLuaEngine
}

unix {
     CONFIG += arm
#    CONFIG += x86

    x86 {
        DEFINES += _CS_X86_LINUX
        LIBS += \
                -L$$PWD/../Lib/lua/lib/linux-x86 -llua \
                -L$$PWD/../Lib/oolua/lib/linux-x86 -loolua \
                -L$$PWD/../Lib/log4cpp/lib/linux-x86 -llog4cpp \
                -L$$PWD/../../bin/ -lLuaEngine
    }
    arm {

        DEFINES += _CS_ARM_LINUX
        LIBS +=  \
                -L$$PWD/../Lib/lua/lib/linux-arm -llua \
                -L$$PWD/../Lib/oolua/lib/linux-arm -loolua \
                -L$$PWD/../Lib/log4cpp/lib/linux-arm -llog4cpp \
                -L$$PWD/../../bin/ -lLuaEngine
    }
}


android {
        CONFIG += android
        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_ANDROID
}

SOURCES += PrinterPlugin.cpp \
    PrinterPluginProxy.cpp \
    Log.cpp \
    Printer.cpp

HEADERS += PrinterPlugin.h \
    PrinterPluginProxy.h \
    Log.h \
    LuipShare.h \
    Printer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

