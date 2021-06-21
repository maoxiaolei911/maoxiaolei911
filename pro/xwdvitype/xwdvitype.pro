TARGET = xwdvitype
TEMPLATE = app

QT += network

include(../dir.prj)

XWDVITYPESRCDIR = ../../src/dvitype

INCLUDEPATH += $${XWDVITYPESRCDIR}

HEADERS += $${XWDVITYPESRCDIR}/XWDVITypeWindow.h \
           $${XWDVITYPESRCDIR}/XWDVITypeMainWindow.h \
           $${XWDVITYPESRCDIR}/XWDVIShow.h

SOURCES += $${XWDVITYPESRCDIR}/XWDVITypeWindow.cpp \
           $${XWDVITYPESRCDIR}/XWDVITypeMainWindow.cpp \
           $${XWDVITYPESRCDIR}/XWDVIShow.cpp \
           $${XWDVITYPESRCDIR}/xwdvitype.cpp

RESOURCES    = $${XWRESDIR}/xw_dvitype.qrc

MOC_DIR = $${TMPDIR}/moc/xwdvitype
OBJECTS_DIR = $${TMPDIR}/obj/xwdvitype

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = xwdvityped
    LIBS += -lXWCored  -lXWGuiCored
} else {
    TARGET = xwdvitype
    LIBS += -lXWCore -lXWGuiCore
}
