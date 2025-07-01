#-------------------------------------------------
#
# Project created by QtCreator 2020-11-11T09:27:02
#
#-------------------------------------------------

QT       += core gui network serialport

TARGET = HJ212Plugin
TEMPLATE = lib
DESTDIR = ../../bin

QMAKE_CXXFLAGS += -std=c++11
INCLUDEPATH += ../Lib/log4cpp/include

DEFINES += HJ212PLUGIN_LIBRARY

INCLUDEPATH += ../Luip ../
INCLUDEPATH += ../Lib/log4cpp/include
INCLUDEPATH += ../Lib/lua/include
INCLUDEPATH += ../Lib/oolua/include
INCLUDEPATH += ../Luip ../

DEFINES += OOLUA_USE_EXCEPTIONS=1 OOLUA_STORE_LAST_ERROR=0


win32 {
    DEFINES += _CS_X86_WINDOWS
    DEFINES += BIL_EXPORT

    Debug:LIBS   += -L$$PWD/../Lib/oolua/lib/windows/ -looluaD

    Release:LIBS += -L$$PWD/../Lib/oolua/lib/windows/ -loolua

    LIBS   +=  -lws2_32 \
               -L$$PWD/../Lib/lua/lib/windows/ -llua \
               -L$$PWD/../Lib/log4cpp/lib/windows/ -llog4cpp
}

unix {
     CONFIG += arm
#    CONFIG += x86

    x86 {
        DEFINES += _CS_X86_LINUX

        LIBS   +=  \
                   -L$$PWD/../Lib/lua/lib/linux-x86 -llua \
                   -L$$PWD/../Lib/oolua/lib/linux-x86 -loolua \
                   -L$$PWD/../Lib/log4cpp/lib/linux-x86/ -llog4cpp
    }
    arm {

        DEFINES += _CS_ARM_LINUX

        LIBS   +=  \
                   -L$$PWD/../Lib/lua/lib/linux-arm -llua \
                   -L$$PWD/../Lib/oolua/lib/linux-arm -loolua \
                   -L$$PWD/../Lib/log4cpp/lib/linux-arm/ -llog4cpp
    }
}


android {
        CONFIG += android
        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_ANDROID
}


SOURCES += \
    API/HJ212Packet.cpp \
    API/HJ212Protocol.cpp \
    API/V2017DbsProtocol.cpp \
    API/V2017WryProtocol.cpp \
    HJ212Connection.cpp \
    HJ212Platform.cpp \
    HJ212SendQueue.cpp \
    HJ212SerialConnection.cpp \
    HJ212TcpConnection.cpp \
    Log.cpp \
    HJ212PluginProxy.cpp

HEADERS += \
    API/HJ212Def.h \
    API/HJ212Packet.h \
    API/HJ212Protocol.h \
    API/V2017DbsProtocol.h \
    API/V2017WryProtocol.h \
    HJ212Connection.h \
    HJ212Data.h \
    HJ212Platform.h \
    HJ212SendQueue.h \
    HJ212SerialConnection.h \
    HJ212TcpConnection.h \
    Log.h \
    LuipShare.h \
    HJ212PluginProxy.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
