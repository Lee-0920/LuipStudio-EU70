#-------------------------------------------------
#
# Project created by QtCreator 2020-06-19T13:44:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LuipDataEditor
TEMPLATE = app
DESTDIR = ../../bin

#CONFIG += console

QMAKE_CXXFLAGS += -std=c++11


INCLUDEPATH += ../Lib/log4cpp/include \
               ../Lib/oolua/include \
               ../Lib/lua/include \
               ../

DEFINES += OOLUA_USE_EXCEPTIONS=1 OOLUA_STORE_LAST_ERROR=0

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

win32 {
    DEFINES += _CS_X86_WINDOWS
    Debug:LIBS   += -L$$PWD/../Lib/oolua/lib/windows/ -looluaD

    Release:LIBS += -L$$PWD/../Lib/oolua/lib/windows/ -loolua

    LIBS += -lws2_32 \
            -lDbgHelp \
            -L$$PWD/../Lib/log4cpp/lib/windows/ -llog4cpp \
            -L$$PWD/../Lib/lua/lib/windows/ -llua \
            -L$$PWD/../../bin/ -lLuaEngine \
            -L$$PWD/../../bin/ -lRecordDataPlugin
}
unix {
     CONFIG += arm
#    CONFIG += x86

    x86 {
        DEFINES += _CS_X86_LINUX
        LIBS += -L$$PWD/../Lib/lua/lib/linux-x86 -llua \
                -L$$PWD/../Lib/log4cpp/lib/linux-x86 -llog4cpp \
                -L$$PWD/../Lib/oolua/lib/linux-x86 -loolua \
                -L$$PWD/../../bin/ -lLuaEngine \
                -L$$PWD/../../bin/ -lRecordDataPlugin
    }
    arm {

        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_USING_MODBUS
        DEFINES += _DEV_USING_CONTROL
        DEFINES += _DEV_USING_CURRENT
        LIBS += -L$$PWD/../Lib/lua/lib/linux-arm -llua \
                -L$$PWD/../Lib/log4cpp/lib/linux-arm -llog4cpp \
                -L$$PWD/../Lib/oolua/lib/linux-arm -loolua \
                -L$$PWD/../../bin/ -lLuaEngine \
                -L$$PWD/../../bin/ -lRecordDataPlugin
    }
}
android {
        CONFIG += android

        DEFINES += _CS_ARM_LINUX
        DEFINES -= _CS_USING_MODBUS
        DEFINES -= _DEV_USING_CONTROL
        DEFINES -= _DEV_USING_CURRENT
        DEFINES += _CS_ANDROID
        DEFINES += "_CS_APP_ROOT=\"\\\"/sdcard/LuipStudio\"\""  # Default: executable file path
        DEFINES += errno=0
        LIBS += -L$$PWD/../Lib/log4cpp/lib/android -llog4cpp-larm \
                -L$$PWD/../Lib/qwt/lib/android -lqwt \
                -L$$PWD/../Lib/libmodbus/lib/android/ -lmodbus
}


SOURCES += main.cpp\
        mainwindow.cpp \
    ResultManager/CalibrateRecordFile.cpp \
    ResultManager/MeasureRecordFile.cpp \
    ResultManager/OperateRecordData.cpp \
    ResultManager/RecordFields.cpp \
    ResultManager/RecordFile.cpp \
    ResultManager/ResultManager.cpp \
    ResultManager/ResultManagerProxy.cpp \
    System/Hash/md5.cpp \
    System/IO/LuipFile.cpp \
    System/CopyFile.cpp \
    System/FileDetecter.cpp \
    Log.cpp \
    AboutDialog.cpp \
    FilterDialog.cpp \
    Lua/App.cpp \
    Lua/AppProxy.cpp \
    Setting/Environment.cpp \
    Setting/SettingManager.cpp \
    Setting/SettingManagerProxy.cpp \
    System/Version.cpp

HEADERS  += mainwindow.h \
    ResultManager/CalibrateRecordFile.h \
    ResultManager/IResultNotifiable.h \
    ResultManager/MeasureRecordFile.h \
    ResultManager/OperateRecordData.h \
    ResultManager/RecordFields.h \
    ResultManager/RecordFile.h \
    ResultManager/ResultManager.h \
    ResultManager/ResultManagerProxy.h \
    System/Hash/md5.h \
    System/IO/Exception.h \
    System/IO/LuipFile.h \
    System/CopyFile.h \
    System/Exception.h \
    System/FileDetecter.h \
    System/Types.h \
    System/Uuid.h \
    Log.h \
    AboutDialog.h \
    FilterDialog.h \
    Lua/App.h \
    Lua/AppProxy.h \
    Setting/Environment.h \
    Setting/SettingManager.h \
    Setting/SettingManagerProxy.h \
    System/Version.h

FORMS    += \
    MainWindow.ui \
    AboutDialog.ui \
    FilterDialog.ui

RESOURCES += \
    main.qrc

RC_ICONS = Resources/LuipEditor.ico
