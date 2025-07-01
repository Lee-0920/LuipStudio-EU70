
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VncPlugin
TEMPLATE = lib



CONFIG += -std=c++11

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
    DES.cpp \
    RFBProtol.cpp \
#    Test.cpp \
    VncClientWidget.cpp \
    VncManager.cpp \
    VncScreen.cpp


HEADERS  += \
    DES.h \
    KeysymDef.h \
    RFBProtol.h \
#    Test.h \
    VncClientWidget.h \
    VncManager.h \
    VncScreen.h
