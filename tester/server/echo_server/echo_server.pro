QT += core network
QT -= gui

CONFIG += c++11

TARGET = echo_server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

UNQLPATH=/home/franci/prog/uniqlogger/lib
MTSPATH = ../../../src

INCLUDEPATH += $$MTSPATH $$UNQLPATH/src

!exists($$MTSPATH/mtserver.pri) : error("Cannot find Multi-threaded server .pri file")

include($$MTSPATH/mtserver.pri)

OBJECTS_DIR = build
MOC_DIR = build

SOURCES += main.cpp \
    echoservertest.cpp \
    echoworker.cpp

HEADERS += \
    echoservertest.h \
    echoworker.h

LIBS += -L$$UNQLPATH/last_build -lUniqLogger_d
