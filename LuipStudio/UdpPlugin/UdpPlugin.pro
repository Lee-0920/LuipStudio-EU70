
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UdpPlugin
TEMPLATE = lib

DEFINES += MORE_DEBUG_MSG

CONFIG += -std=c++11 -mcx16

INCLUDEPATH += ../

win32{
    Debug:DESTDIR = ../../bin
    Release:DESTDIR = ../../bin
    DEFINES += _CS_X86_WINDOWS
    DEFINES += BIL_EXPORT
    LIBS += -lws2_32 -lz
}

unix {
    DESTDIR = ../../bin
    CONFIG += arm

    arm {

        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_USING_MODBUS
        DEFINES += _DEV_USING_CONTROL
        DEFINES += _DEV_USING_CURRENT

    }
}


SOURCES += \
    Test.cpp \
    UdpBase.cpp \
    UdpClient.cpp \
    UdpManager.cpp \
    UdpServer.cpp


HEADERS  += \
    Test.h \
    UdpBase.h \
    UdpClient.h \
    UdpDef.h \
    UdpManager.h \
    UdpServer.h
