SSL_SERVER_VERSION=2.0.0

#----- DO NOT MODIFY BELOW HERE ------
exists($$PWD/config.pri) {
    message("Including $$PWD/config.pri as SSLServer configuration build settings")
    include($$PWD/config.pri)
}

QT += network

INCLUDEPATH += $$PWD
SOURCES += $$PWD/sslserver.cpp
HEADERS += $$PWD/sslserver.h

contains(DEFINES, ENABLE_UNQL_USAGE_IN_SSLSERVER) {
    message("Using SSLServer with UniqLogger support...");
    isEmpty(UNQLPATH): error("UNQLPATH variable cannot be empty when configuring SSLServer with UniqLogger")
    INCLUDEPATH += $$UNQLPATH/include
    LIBS += -L$$UNQLPATH -lUniqLogger
}
