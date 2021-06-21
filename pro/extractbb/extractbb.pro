TARGET = extractbb
TEMPLATE = app

QT += network

include(../dir.prj)

HEADERS += $${DVIDIR}/XWImageBoundingBox.h

SOURCES += $${DVIDIR}/XWImageBoundingBox.cpp \
           $${DVIDIR}/extractbb.cpp

MOC_DIR = $${TMPDIR}/moc/extractbb
OBJECTS_DIR = $${TMPDIR}/obj/extractbb

CONFIG += console

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG	
    TARGET = extractbbd
    LIBS += -lXWCored
} else {
    TARGET = extractbb
    LIBS += -lXWCore
}
