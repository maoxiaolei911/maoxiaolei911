TARGET = bibtex
TEMPLATE = app

QT += network

include(../dir.prj)

XWBIBTEXSRCDIR = ../../src/bibtex

INCLUDEPATH += $${XWBIBTEXSRCDIR}

HEADERS += $${XWBIBTEXSRCDIR}/XWBibTeX.h

SOURCES += $${XWBIBTEXSRCDIR}/XWBibTeX.cpp \
           $${XWBIBTEXSRCDIR}/bibtex.cpp 

MOC_DIR = $${TMPDIR}/moc/bibtex
OBJECTS_DIR = $${TMPDIR}/obj/bibtex

CONFIG += console

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
    TARGET = bibtexd
    LIBS += -lXWCored
} else {
    TARGET = bibtex
    LIBS += -lXWCore
}
