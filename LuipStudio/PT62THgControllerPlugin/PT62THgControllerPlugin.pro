#-------------------------------------------------
#
# Project created by QtCreator 2017-11-15T11:46:57
#
#-------------------------------------------------
QT       -= gui

TARGET = PT62THgControllerPlugin
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

SOURCES += PT62THgControllerPlugin.cpp \
    Log.cpp \
    PT62THgController.cpp \
    PT62THgControllerPluginProxy.cpp \
    API/MeterPoints.cpp \
    API/OpticalAcquireInterface.cpp \
    API/OpticalMeterInterface.cpp \
    API/PeristalticPumpInterface.cpp \
    API/SolenoidValveInterface.cpp \
    API/TemperatureControlInterface.cpp \
    API/ValveMap.cpp

HEADERS += PT62THgControllerPlugin.h \
    Log.h \
    LuipShare.h \
    PT62THgController.h \
    PT62THgControllerPluginProxy.h \
    API/MeterPoints.h \
    API/OpticalAcquireInterface.h \
    API/OpticalMeterInterface.h \
    API/PeristalticPumpInterface.h \
    API/SolenoidValveInterface.h \
    API/TemperatureControlInterface.h \
    API/ValveMap.h \
    API/Code/OpticalAcquireInterface.h \
    API/Code/OpticalMeterInterface.h \
    API/Code/PeristalticPumpInterface.h \
    API/Code/SolenoidValveInterface.h \
    API/Code/TemperatureControlInterface.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
