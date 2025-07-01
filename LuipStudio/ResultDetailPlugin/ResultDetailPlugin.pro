#-------------------------------------------------
#
# Project created by QtCreator 2017-07-24T09:05:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = ResultDetailPlugin
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
               -L$$PWD/../../bin/ -lLuaEngine \
               -L$$PWD/../../bin/ -lRecordDataPlugin
}

unix {
     CONFIG += arm
#    CONFIG += x86

    x86 {
        DEFINES += _CS_X86_LINUX
        LIBS += \
                -L$$PWD/../Lib/lua/lib/linux-x86 -llua \
                -L$$PWD/../Lib/oolua/lib/linux-x86 -loolua \
                -L$$PWD/../../bin/ -lLuaEngine \
                -L$$PWD/../../bin/ -lRecordDataPlugin
    }
    arm {

        DEFINES += _CS_ARM_LINUX
        LIBS +=  \
                -L$$PWD/../Lib/lua/lib/linux-arm -llua \
                -L$$PWD/../Lib/oolua/lib/linux-arm -loolua \
                -L$$PWD/../../bin/ -lLuaEngine \
                -L$$PWD/../../bin/ -lRecordDataPlugin
    }
}


android {
        CONFIG += android
        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_ANDROID
}

SOURCES += \
    ResultDetailWidget.cpp \
    MeasureResultDetailWidget.cpp \
    CalibrateResultDetailWidget.cpp \
    ResultDetailPlugin.cpp \
    Log.cpp \
    ResultDetailPluginProxy.cpp

HEADERS += \
    LuipShare.h \
    ResultDetailWidget.h \
    MeasureResultDetailWidget.h \
    CalibrateResultDetailWidget.h \
    ResultDetailPlugin.h \
    Log.h \
    ResultDetailPluginProxy.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
