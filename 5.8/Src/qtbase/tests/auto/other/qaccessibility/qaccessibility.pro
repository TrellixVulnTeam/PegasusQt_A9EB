CONFIG += testcase
TARGET = tst_qaccessibility
requires(qtConfig(accessibility))
QT += testlib core-private gui-private widgets-private
SOURCES += tst_qaccessibility.cpp
HEADERS += accessiblewidgets.h

unix:!darwin:!haiku:!integity: LIBS += -lm

win32 {
    !*g++:!winrt {
        include(../../../../src/3rdparty/iaccessible2/iaccessible2.pri)
        DEFINES += QT_SUPPORTS_IACCESSIBLE2
    }
    LIBS += -luuid
    !winphone: LIBS += -loleacc -loleaut32 -lole32
}
