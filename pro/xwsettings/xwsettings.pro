TARGET = xwsettings
TEMPLATE = app

QT += network

include(../dir.prj)

XWSETTINGSSRCDIR = ../../src/settingeditor

INCLUDEPATH += $${XWSETTINGSSRCDIR}

HEADERS += $${XWSETTINGSSRCDIR}/XWSettingEditor.h

SOURCES += $${XWSETTINGSSRCDIR}/XWSettingEditor.cpp \
           $${XWSETTINGSSRCDIR}/xwsettings.cpp

RESOURCES    = $${XWRESDIR}/xw_settings.qrc

MOC_DIR = $${TMPDIR}/moc/xwsettings
OBJECTS_DIR = $${TMPDIR}/obj/xwsettings

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = xwsettingsd
} else {
    TARGET = xwsettings
}
