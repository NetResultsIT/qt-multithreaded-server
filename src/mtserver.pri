QT += core network

contains(QT, gui): message("WARNING: building MTServer with QT GUI enabled! Are you sure that's needed?")


CONFIG += c++11

isEmpty(TPOOL_PATH) {
    error("NrThreadPool path not defined! Please define the TPOOL_PATH variable with the location of nrThreadPool.pri before including this file (mtserver.pri)")
}
include($$TPOOL_PATH/src/nrThreadPool.pri)

!exists(sslserver.pri) : error("NrMultithreadedServer could not find mandatory sslserver.pri... aborting")
include(sslserver.pri)

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/mthreadserver.cpp \
    $$PWD/mthreadserver.tpp \
    $$PWD/serverworker.cpp

HEADERS += \
    $$PWD/mthreadserver.h \
    $$PWD/serverworker.h \
    $$PWD/mtcommon.h

