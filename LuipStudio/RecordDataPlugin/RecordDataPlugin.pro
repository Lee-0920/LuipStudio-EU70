#-------------------------------------------------
#
# Project created by QtCreator 2017-07-21T13:59:32
#
#-------------------------------------------------

QT       -= gui
 
TARGET = RecordDataPlugin
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
              -L$$PWD/../../bin/ -lLuaEngine
}

unix {
     CONFIG += arm
#    CONFIG += x86

    x86 {
        DEFINES += _CS_X86_LINUX
        LIBS +=  \
                -L$$PWD/../Lib/lua/lib/linux-x86 -llua \
                -L$$PWD/../Lib/oolua/lib/linux-x86 -loolua \
                -L$$PWD/../../bin/ -lLuaEngine
    }
    arm {

        DEFINES += _CS_ARM_LINUX
        LIBS += \
                -L$$PWD/../Lib/lua/lib/linux-arm -llua \
                -L$$PWD/../Lib/oolua/lib/linux-arm -loolua \
                -L$$PWD/../../bin/ -lLuaEngine
    }
}
android {
        CONFIG += android
        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_ANDROID
}

SOURCES += \
    RecordData.cpp \
    RecordDataProxy.cpp

HEADERS += \
    RecordData.h \
    LuipShare.h \
    RecordDataProxy.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
