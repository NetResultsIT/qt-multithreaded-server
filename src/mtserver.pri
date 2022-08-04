MTS_VERSION=2.0.0

#---- DO NOT MODIFY BELOW HERE------
exists($$PWD/config.pri) {
    message("Including $$PWD/config.pri as MultiThreadedServer configuration build settings")
    include($$PWD/config.pri)
}

QT += core network

contains(QT, gui): message("WARNING: building MTServer with QT GUI enabled! Are you sure that's needed?")

CONFIG += c++11

contains(DEFINES, ENABLE_UNQL_USAGE_IN_MTSERVER) {
    message("Using MTServer with UniqLogger support...");
    isEmpty(UNQLPATH): error("UNQLPATH variable cannot be empty when configuring SSLServer with UniqLogger")
    INCLUDEPATH += $$UNQLPATH/include
    LIBS += -L$$UNQLPATH -lUniqLogger
    #add UniqLogger support into SslServer
    DEFINES += ENABLE_UNQL_USAGE_IN_SSLSERVER
}

TPOOL_PATH=$$PWD/ext/tpool/src
SSLSRV_PATH=$$PWD/ssl

!exists($$TPOOL_PATH/nrThreadPool.pri) : error("NrThreadPool path not defined! Please define the TPOOL_PATH variable with the location of nrThreadPool.pri before including this file (mtserver.pri)")
include($$TPOOL_PATH/nrThreadPool.pri)

!exists($$SSLSRV_PATH/sslserver.pri) : error("NrMultithreadedServer could not find mandatory sslserver.pri... aborting")
include($$SSLSRV_PATH/sslserver.pri)

INCLUDEPATH += $$PWD


#if you compiled mtserver with unql support then uncomment the following line
#!isEmpty(UNQLPATH) {
#    message("MT SERVER configured to use UniqLogger")
#    LIBS += -L$$UNQLPATH -lUniqLogger
#}

SOURCES += \
    $$PWD/mthreadserver.cpp \
    $$PWD/mthreadserver.tpp \
    $$PWD/serverworker.cpp

HEADERS += \
    $$PWD/mthreadserver.h \
    $$PWD/serverworker.h \
    $$PWD/mtcommon.h

