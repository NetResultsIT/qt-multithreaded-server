QT += core network
QT -= gui

CONFIG += c++11

TARGET = echo_server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app


#!exists($$PWD/depspath.pri) : error("Cannot find depspath.pri file to choose echo_server deps: UniqLogger and Threadpool")
#include($$PWD/depspath.pri)

#Set the path for the multi-threaded server
MTS_PATH = ../../../src
!exists($$MTS_PATH/mtserver.pri) : error("Cannot find Multi-threaded server .pri file")
include($$MTS_PATH/mtserver.pri)

OBJECTS_DIR = build
MOC_DIR = build

SOURCES += main.cpp \
    echoservertest.cpp \
    echoworker.cpp

HEADERS += \
    echoservertest.h \
    echoworker.h

#if you compiled sslserver with unql support then uncomment the following line
LIBS += -L$$UNQLPATH
