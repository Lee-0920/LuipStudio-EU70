#-------------------------------------------------
#
# Project created by QtCreator 2017-04-19T17:25:01
#
#-------------------------------------------------

QT       -= gui

TARGET = CM66DriveControllerPlugin
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

SOURCES += CM66DriveControllerplugin.cpp \
    API/MeterPoints.cpp \
    API/OpticalMeterInterface.cpp \
    API/PeristalticPumpInterface.cpp \
    API/SolenoidValveInterface.cpp \
    API/TemperatureControlInterface.cpp \
    API/TitrationOperateInterface.cpp \
    API/ValveMap.cpp \
    Log.cpp \
    CM66DriveController.cpp \
    CM66DriveControllerPluginProxy.cpp

HEADERS += CM66DriveControllerplugin.h \
    API/Code/OpticalMeterInterface.h \
    API/Code/PeristalticPumpInterface.h \
    API/Code/SolenoidValveInterface.h \
    API/Code/TemperatureControlInterface.h \
    API/Code/TitrationOperateInterface.h \
    API/MeterPoints.h \
    API/OpticalMeterInterface.h \
    API/PeristalticPumpInterface.h \
    API/SolenoidValveInterface.h \
    API/TemperatureControlInterface.h \
    API/TitrationOperateInterface.h \
    API/ValveMap.h \
    Log.h \
    LuipShare.h \
    CM66DriveController.h \
    CM66DriveControllerPluginProxy.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

