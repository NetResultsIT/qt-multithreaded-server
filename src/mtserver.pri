QT += core network

contains(QT, gui): message("WARNING: building MTServer with QT GUI enabled! Are you sure that's needed?")


CONFIG += c++11

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/ext/tpool/src/

SOURCES += \
    $$PWD/ext/tpool/src/nrthreadpool.cpp \
    $$PWD/mthreadserver.cpp \
    $$PWD/mthreadserver.tpp \
    $$PWD/serverworker.cpp \
    $$PWD/sslserver.cpp

HEADERS += \
    $$PWD/ext/tpool/src/nrthreadpool.h \
    $$PWD/mthreadserver.h \
    $$PWD/serverworker.h \
    $$PWD/sslserver.h \
    $$PWD/mtcommon.h

