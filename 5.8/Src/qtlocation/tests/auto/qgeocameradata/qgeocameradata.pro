TEMPLATE = app
CONFIG += testcase
TARGET = tst_qgeocameradata

INCLUDEPATH += ../../../src/location/maps

SOURCES += tst_qgeocameradata.cpp

QT += location positioning-private testlib
