#-------------------------------------------------
#
# Project created by QtCreator 2021-01-04T09:50:35
#
#-------------------------------------------------

QT       += core sql

TARGET = DataBasePlugin
TEMPLATE = lib
DESTDIR = ../../bin
CONFIG += c++14

win32 {
    DEFINES += BIL_EXPORT
}

INCLUDEPATH += ../

SOURCES +=\
        DataBaseManager.cpp \
        Table/DBTable.cpp \
        Table/DBConnectionPool.cpp \
        Table/DataTable.cpp \
    Table/MeasureTable.cpp \
    Table/CalibrateTable.cpp \
    Table/MethodTable.cpp \
    Table/WarningTable.cpp \
    Table/AuditTrailTable.cpp \
    Table/UserTable.cpp \
    Table/AccessLevelTable.cpp

HEADERS  += \
            DataBaseDef.h \
            DataBaseManager.h \
            Table/DBTable.h \
            Table/DBConnectionPool.h \
            Table/DataTable.h \
    Table/MeasureTable.h \
    Table/CalibrateTable.h \
    Table/MethodTable.h \
    Table/WarningTable.h \
    Table/AuditTrailTable.h \
    Table/UserTable.h \
    Table/AccessLevelTable.h
