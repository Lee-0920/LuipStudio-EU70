#-------------------------------------------------
#
# Project created by QtCreator 2017-04-19T17:25:01
#
#-------------------------------------------------

QT       -= gui

TARGET = NP66DriveControllerPlugin
TEMPLATE = lib
DESTDIR = ../../bin

INCLUDEPATH += ../Luip ../

INCLUDEPATH += ../Lib/log4cpp/include ../Lib/oolua/include ../Lib/lua/include

DEFINES += OOLUA_USE_EXCEPTIONS=1 OOLUA_STORE_LAST_ERROR=0

win32 {
    DEFINES += _CS_X86_WINDOWS
    DEFINES += BIL_EXPORT

Debug:LIBS   += -L$$PWD/../Lib/oolua/lib/windows/ -looluaD

    Release:LIBS += -L$$PWD/../Lib/oolua/lib/windows/ -loolua


    LIBS   += -L$$PWD/../Lib/log4cpp/lib/windows/ -llog4cpp \
              -L$$PWD/../Lib/lua/lib/windows/ -llua \
              -L$$PWD/../../bin/ -lLuaEngine \
              -L$$PWD/../../bin/ -lCommunication \
              -L$$PWD/../../bin/ -lControllerPlugin
}
unix {
    DESTDIR = ../../bin/Plugin/lib
     CONFIG += arm
#    CONFIG += x86

    x86 {
        DEFINES += _CS_X86_LINUX
        LIBS += -L$$PWD/../Lib/log4cpp/lib/linux-x86 -llog4cpp \
                -L$$PWD/../Lib/lua/lib/linux-x86 -llua \
                -L$$PWD/../Lib/oolua/lib/linux-x86 -loolua \
                -L$$PWD/../../bin/ -lLuaEngine \
                -L$$PWD/../../bin -lCommunication \
                -L$$PWD/../../bin/ -lControllerPlugin
    }
    arm {
        DEFINES += _CS_ARM_LINUX
        LIBS += -L$$PWD/../Lib/log4cpp/lib/linux-arm -llog4cpp \
                -L$$PWD/../Lib/lua/lib/linux-arm -llua \
                -L$$PWD/../Lib/oolua/lib/linux-arm -loolua \
                -L$$PWD/../../bin/ -lLuaEngine \
                -L$$PWD/../../bin -lCommunication \
                -L$$PWD/../../bin/ -lControllerPlugin
    }
}
android {
        CONFIG += android

        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_ANDROID
        LIBS += -L$$PWD/../Lib/log4cpp/lib/android -llog4cpp
}

SOURCES += NP66DriveControllerPlugin.cpp \
    API/NP66MeterPoints.cpp \
    API/NP66OpticalMeterInterface.cpp \
    API/NP66PeristalticPumpInterface.cpp \
    API/NP66SolenoidValveInterface.cpp \
    API/NP66ValveMap.cpp \
    NP66DriveController.cpp \
    Log.cpp \
    NP66DriveControllerPluginProxy.cpp \
    API/NP66TemperatureControlInterface.cpp \
    API/NP66OpticalAcquireInterface.cpp

HEADERS += NP66DriveControllerPlugin.h \
    API/Code/NP66OpticalMeterInterface.h \
    API/Code/NP66PeristalticPumpInterface.h \
    API/Code/NP66SolenoidValveInterface.h \
    API/NP66MeterPoints.h \
    API/NP66OpticalMeterInterface.h \
    API/NP66PeristalticPumpInterface.h \
    API/NP66SolenoidValveInterface.h \
    API/NP66ValveMap.h \
    Log.h \
    LuipShare.h \
    NP66DriveController.h \
    NP66DriveControllerPluginProxy.h \
    API/NP66TemperatureControlInterface.h \
    API/Code/NP66TemperatureControlInterface.h \
    API/Code/NP66OpticalAcquireInterface.h \
    API/NP66OpticalAcquireInterface.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

