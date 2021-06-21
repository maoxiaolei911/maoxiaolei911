TARGET = xwtray
TEMPLATE = app

QT += network

include(../dir.prj)

XWTRAYSRCDIR = ../../src/xwtray

INCLUDEPATH += $${XWTRAYSRCDIR}

HEADERS += $${XWTRAYSRCDIR}/XWTrayDialog.h 

SOURCES += $${XWTRAYSRCDIR}/XWTrayDialog.cpp \
           $${XWTRAYSRCDIR}/xwtray.cpp 

RESOURCES    = $${XWRESDIR}/xw_tray.qrc

MOC_DIR = $${TMPDIR}/moc/xwtray
OBJECTS_DIR = $${TMPDIR}/obj/xwtray

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = xwtrayed
} else {
    TARGET = xwtray
}
