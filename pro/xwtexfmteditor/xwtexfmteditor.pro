TARGET = xwtexfmteditor
TEMPLATE = app

QT += network
QT += phonon

include(../dir.prj)

XWTEXFMTEDITORSRCDIR = ../../src/texfmteditor

INCLUDEPATH += $${XWTEXFMTEDITORSRCDIR}

HEADERS += $${XWTEXFMTEDITORSRCDIR}/XWTeXFmtEditorDev.h \
           $${XWTEXFMTEDITORSRCDIR}/XWTeXFmtEditorMainWindow.h \
           $${XWTEXFMTEDITORSRCDIR}/XWTeXFmtEditor.h

SOURCES += $${XWTEXFMTEDITORSRCDIR}/XWTeXFmtEditorDev.cpp \
           $${XWTEXFMTEDITORSRCDIR}/XWTeXFmtEditorMainWindow.cpp \
           $${XWTEXFMTEDITORSRCDIR}/XWTeXFmtEditor.cpp \
           $${XWTEXFMTEDITORSRCDIR}/fmteditor.cpp

RESOURCES    = $${XWRESDIR}/xw_texfmteditor.qrc

MOC_DIR = $${TMPDIR}/moc/xwtexfmteditor
OBJECTS_DIR = $${TMPDIR}/obj/xwtexfmteditor

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = xwtexfmteditord
    LIBS += -lXWCored -lXWGuiCored -lXWTeXd -lXWTeXGuid -lXWTikzd
} else {
    TARGET = xwtexfmteditor
    LIBS += -lXWCore -lXWGuiCore -lXWTeX -lXWTeXGui -lXWTikz
}
