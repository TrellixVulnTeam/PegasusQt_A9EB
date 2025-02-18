CONFIG += testcase

SOURCES += tst_qsslsocket.cpp
win32:LIBS += -lws2_32
QT = core core-private network-private testlib

TARGET = tst_qsslsocket

win32 {
  CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
  }
}

# OpenSSL support
qtConfig(openssl)|qtConfig(openssl-linked) {
    # Add optional SSL libs
    LIBS += $$OPENSSL_LIBS
}

DEFINES += SRCDIR=\\\"$$PWD/\\\"

requires(qtConfig(private_tests))
