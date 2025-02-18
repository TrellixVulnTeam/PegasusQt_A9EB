TARGET = qcocoa

OBJECTIVE_SOURCES += main.mm \
    qcocoaintegration.mm \
    qcocoatheme.mm \
    qcocoabackingstore.mm \
    qcocoawindow.mm \
    qnsview.mm \
    qnsviewaccessibility.mm \
    qnswindowdelegate.mm \
    qcocoanativeinterface.mm \
    qcocoaeventdispatcher.mm \
    qcocoaapplicationdelegate.mm \
    qcocoaapplication.mm \
    qcocoamenu.mm \
    qcocoamenuitem.mm \
    qcocoamenubar.mm \
    qcocoamenuloader.mm \
    qcocoahelpers.mm \
    qmultitouch_mac.mm \
    qcocoaaccessibilityelement.mm \
    qcocoaaccessibility.mm \
    qcocoacolordialoghelper.mm \
    qcocoafiledialoghelper.mm \
    qcocoafontdialoghelper.mm \
    qcocoacursor.mm \
    qcocoaclipboard.mm \
    qcocoadrag.mm \
    qmacclipboard.mm \
    qcocoasystemsettings.mm \
    qcocoainputcontext.mm \
    qcocoaservices.mm \
    qcocoasystemtrayicon.mm \
    qcocoaintrospection.mm \
    qcocoakeymapper.mm \
    qcocoamimetypes.mm

SOURCES += messages.cpp

HEADERS += qcocoaintegration.h \
    qcocoatheme.h \
    qcocoabackingstore.h \
    qcocoawindow.h \
    qnsview.h \
    qnswindowdelegate.h \
    qcocoanativeinterface.h \
    qcocoaeventdispatcher.h \
    qcocoaapplicationdelegate.h \
    qcocoaapplication.h \
    qcocoamenu.h \
    qcocoamenuitem.h \
    qcocoamenubar.h \
    qcocoamenuloader.h \
    qcocoahelpers.h \
    qmultitouch_mac_p.h \
    qcocoaaccessibilityelement.h \
    qcocoaaccessibility.h \
    qcocoacolordialoghelper.h \
    qcocoafiledialoghelper.h \
    qcocoafontdialoghelper.h \
    qcocoacursor.h \
    qcocoaclipboard.h \
    qcocoadrag.h \
    qmacclipboard.h \
    qcocoasystemsettings.h \
    qcocoainputcontext.h \
    qcocoaservices.h \
    qcocoasystemtrayicon.h \
    qcocoaintrospection.h \
    qcocoakeymapper.h \
    messages.h \
    qcocoamimetypes.h

qtConfig(opengl.*) {
    OBJECTIVE_SOURCES += qcocoaglcontext.mm

    HEADERS += qcocoaglcontext.h
}

RESOURCES += qcocoaresources.qrc

LIBS += -framework AppKit -framework Carbon -framework IOKit -lcups

QT += \
    core-private gui-private \
    accessibility_support-private clipboard_support-private theme_support-private \
    fontdatabase_support-private graphics_support-private cgl_support-private

qtHaveModule(widgets) {
    OBJECTIVE_SOURCES += \
        qpaintengine_mac.mm \
        qprintengine_mac.mm \
        qcocoaprintersupport.mm \
        qcocoaprintdevice.mm \

    HEADERS += \
        qpaintengine_mac_p.h \
        qprintengine_mac_p.h \
        qcocoaprintersupport.h \
        qcocoaprintdevice.h \

    QT += widgets-private printsupport-private
}

OTHER_FILES += cocoa.json

# Acccessibility debug support
# DEFINES += QT_COCOA_ENABLE_ACCESSIBILITY_INSPECTOR
# include ($$PWD/../../../../util/accessibilityinspector/accessibilityinspector.pri)


PLUGIN_TYPE = platforms
PLUGIN_CLASS_NAME = QCocoaIntegrationPlugin
!equals(TARGET, $$QT_DEFAULT_QPA_PLUGIN): PLUGIN_EXTENDS = -
load(qt_plugin)
