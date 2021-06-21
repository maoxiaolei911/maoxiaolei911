TARGET = xwreader
TEMPLATE = app

QT += network
QT += phonon

include(../dir.prj)

XWREADERSRCDIR = ../../src/reader

INCLUDEPATH += $${XWREADERSRCDIR}

HEADERS += $${XWREADERSRCDIR}/XWReaderCore.h \
           $${XWREADERSRCDIR}/XWReaderMainWindow.h \
           $${XWREADERSRCDIR}/XWReader.h

SOURCES += $${XWREADERSRCDIR}/XWReaderCore.cpp \
           $${XWREADERSRCDIR}/XWReaderMainWindow.cpp \
           $${XWREADERSRCDIR}/XWReader.cpp \
           $${XWREADERSRCDIR}/reader.cpp

RESOURCES    = $${XWRESDIR}/xw_reader.qrc

MOC_DIR = $${TMPDIR}/moc/xwreader
OBJECTS_DIR = $${TMPDIR}/obj/xwreader

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = xwreaderd
    LIBS += -lXWCored -lXWGuiCored
} else {
    TARGET = xwreader
    LIBS += -lXWCore -lXWGuiCore
}
