QT += core network

contains(QT, gui): message("WARNING: building MTServer with QT GUI enabled! Are you sure that's needed?")


CONFIG += c++11

TPOOL_PATH=$$PWD/ext/tpool/src
SSLSRV_PATH=$$PWD/ssl
isEmpty(TPOOL_PATH) {
    error("NrThreadPool path not defined! Please define the TPOOL_PATH variable with the location of nrThreadPool.pri before including this file (mtserver.pri)")
}
include($$TPOOL_PATH/nrThreadPool.pri)

!exists($$SSLSRV_PATH/sslserver.pri) : error("NrMultithreadedServer could not find mandatory sslserver.pri... aborting")
include($$SSLSRV_PATH/sslserver.pri)

INCLUDEPATH += $$PWD


#if you compiled sslserver with unql support then uncomment the following line
!isEmpty(UNQLPATH) {
message("MT SERVER configured to use UniqLogger")
LIBS += -L$$UNQLPATH -lUniqLogger
}

SOURCES += \
    $$PWD/mthreadserver.cpp \
    $$PWD/mthreadserver.tpp \
    $$PWD/serverworker.cpp

HEADERS += \
    $$PWD/mthreadserver.h \
    $$PWD/serverworker.h \
    $$PWD/mtcommon.h

