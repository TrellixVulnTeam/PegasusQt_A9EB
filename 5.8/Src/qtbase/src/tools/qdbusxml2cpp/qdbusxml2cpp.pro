option(host_build)
!force_bootstrap:!qtConfig(commandlineparser): \
    CONFIG += force_bootstrap
QT = core-private
force_bootstrap: QT += bootstrap_dbus-private
else: QT += dbus-private
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_FOREACH
QMAKE_CXXFLAGS += $$QT_HOST_CFLAGS_DBUS

SOURCES = qdbusxml2cpp.cpp

load(qt_tool)
