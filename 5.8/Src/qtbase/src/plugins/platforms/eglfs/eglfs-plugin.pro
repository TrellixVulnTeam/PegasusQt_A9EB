TARGET = qeglfs

QT += eglfsdeviceintegration-private

SOURCES += $$PWD/qeglfsmain.cpp

OTHER_FILES += $$PWD/eglfs.json

INCLUDEPATH += $$PWD/api

PLUGIN_TYPE = platforms
PLUGIN_CLASS_NAME = QEglFSIntegrationPlugin
!equals(TARGET, $$QT_DEFAULT_QPA_PLUGIN): PLUGIN_EXTENDS = -
load(qt_plugin)
