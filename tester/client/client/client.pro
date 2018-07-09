QT += core network
QT -= gui

CONFIG += c++11

TARGET = client
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    clienttester.cpp \
    echoclient.cpp

HEADERS += \
    clienttester.h \
    echoclient.h
