QT += core network

contains(QT, gui): message("WARNING: building MTServer with QT GUI enabled! Are you sure that's needed?")


CONFIG += c++11

isEmpty(TPOOL_PATH) {
    error("Thread Pool path not defined! Please define the TPOOL_PATH variable before including this file.")
}

INCLUDEPATH += $$PWD
INCLUDEPATH += $$TPOOL_PATH/src/

SOURCES += \
    $$TPOOL_PATH/src/nrthreadpool.cpp \
    $$PWD/mthreadserver.cpp \
    $$PWD/mthreadserver.tpp \
    $$PWD/serverworker.cpp \
    $$PWD/sslserver.cpp

HEADERS += \
    $$TPOOL_PATH/src/nrthreadpool.h \
    $$PWD/mthreadserver.h \
    $$PWD/serverworker.h \
    $$PWD/sslserver.h \
    $$PWD/mtcommon.h

