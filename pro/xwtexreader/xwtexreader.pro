TARGET = xwtexreader
TEMPLATE = app

QT += network
QT += phonon

include(../dir.prj)

XWTEXREADERSRCDIR = ../../src/texreader

INCLUDEPATH += $${XWTEXREADERSRCDIR}

HEADERS += $${XWTEXREADERSRCDIR}/XWTeXReaderCore.h \
           $${XWTEXREADERSRCDIR}/XWTeXReaderDev.h \
           $${XWTEXREADERSRCDIR}/XWTeXReaderMainWindow.h \
           $${XWTEXREADERSRCDIR}/XWTeXReader.h

SOURCES += $${XWTEXREADERSRCDIR}/XWTeXReaderCore.cpp \
           $${XWTEXREADERSRCDIR}/XWTeXReaderDev.cpp \
           $${XWTEXREADERSRCDIR}/XWTeXReaderMainWindow.cpp \
           $${XWTEXREADERSRCDIR}/XWTeXReader.cpp \
           $${XWTEXREADERSRCDIR}/texreader.cpp

RESOURCES    = $${XWRESDIR}/xw_texreader.qrc

MOC_DIR = $${TMPDIR}/moc/xwtexreader
OBJECTS_DIR = $${TMPDIR}/obj/xwtexreader

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = xwtexreaderd
    LIBS += -lXWCored -lXWGuiCored -lXWTeXd  -lXWTeXGuid
} else {
    TARGET = xwtexreader
    LIBS += -lXWCore -lXWGuiCore -lXWTeX  -lXWTeXGui
}
