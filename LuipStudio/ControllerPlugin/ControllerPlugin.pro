#-------------------------------------------------
#
# Project created by QtCreator 2017-04-18T22:53:53
#
#-------------------------------------------------

QT       -= gui

TARGET = ControllerPlugin
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
              -L$$PWD/../../bin/ -lCommunication
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
                -L$$PWD/../../bin -lCommunication
    }
    arm {
        DEFINES += _CS_ARM_LINUX
        LIBS += -L$$PWD/../Lib/log4cpp/lib/linux-arm -llog4cpp \
                -L$$PWD/../Lib/lua/lib/linux-arm -llua \
                -L$$PWD/../Lib/oolua/lib/linux-arm -loolua \
                -L$$PWD/../../bin/ -lLuaEngine \
                -L$$PWD/../../bin -lCommunication
    }
}
android {
        CONFIG += android

        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_ANDROID
        LIBS += -L$$PWD/../Lib/log4cpp/lib/android -llog4cpp
}



SOURCES += \
    API/DeviceInfoInterface.cpp \
    API/DeviceStatusInterface.cpp \
    API/DeviceUpdateInterface.cpp \
    API/DscpSystemInterface.cpp \
    API/DeviceInterface.cpp \
    API/Version.cpp \
    API/UpdatableInfo.cpp \
    API/ManufDate.cpp \
    Log.cpp \
    ControllerPlugin.cpp \
    BaseController.cpp \
    API/InstrumentInfoInterface.cpp \
    ControllerPluginProxy.cpp

HEADERS +=\
    API/DeviceInfoInterface.h \
    API/DeviceStatusInterface.h \
    API/DeviceUpdateInterface.h \
    API/DscpSystemInterface.h \
    API/Code/DeviceInfoInterface.h \
    API/Code/DeviceStatusInterface.h \
    API/Code/DeviceUpdateInterface.h \
    API/DeviceInterface.h \
    API/Version.h \
    API/UpdatableInfo.h \
    API/ManufDate.h \
    ControllerPlugin.h \
    LuipShare.h \
    ISignalNotifiable.h \
    BaseController.h \
    API/InstrumentInfoInterface.h \
    API/Code/InstrumentInfoInterface.h \
    ControllerPluginProxy.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
