TARGET = makeindex
TEMPLATE = app

QT += network

include(../dir.prj)

XWMAKRINDEXSRCDIR = ../../src/makeindex

INCLUDEPATH += $${XWMAKRINDEXSRCDIR}

HEADERS += $${XWMAKRINDEXSRCDIR}/XWMakeIndex.h

SOURCES += $${XWMAKRINDEXSRCDIR}/XWMakeIndex.cpp \
           $${XWMAKRINDEXSRCDIR}/makeindex.cpp 

INCLUDEPATH += $${XWMAKRINDEXSRCDIR}

MOC_DIR = $${TMPDIR}/moc/makeindex
OBJECTS_DIR = $${TMPDIR}/obj/makeindex

CONFIG += console

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
    TARGET = makeindexd
    LIBS += -lXWCored
} else {
    TARGET = makeindex
    LIBS += -lXWCore
}
