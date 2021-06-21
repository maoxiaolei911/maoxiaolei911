TARGET = xwtexeditor
TEMPLATE = app

QT += network
QT += phonon

include(../dir.prj)

XWTEXEDITORSRCDIR = ../../src/texeditor

INCLUDEPATH += $${XWTEXEDITORSRCDIR}

HEADERS += $${XWTEXEDITORSRCDIR}/XWTeXEditorCore.h \
           $${XWTEXEDITORSRCDIR}/XWTeXEditorDev.h \
           $${XWTEXEDITORSRCDIR}/XWTeXEditorMainWindow.h \
           $${XWTEXEDITORSRCDIR}/XWTeXEditor.h

SOURCES += $${XWTEXEDITORSRCDIR}/XWTeXEditorCore.cpp \
           $${XWTEXEDITORSRCDIR}/XWTeXEditorDev.cpp \
           $${XWTEXEDITORSRCDIR}/XWTeXEditorMainWindow.cpp \
           $${XWTEXEDITORSRCDIR}/XWTeXEditor.cpp \
           $${XWTEXEDITORSRCDIR}/texeditor.cpp

RESOURCES    = $${XWRESDIR}/xw_texeditor.qrc

MOC_DIR = $${TMPDIR}/moc/xwtexeditor
OBJECTS_DIR = $${TMPDIR}/obj/xwtexeditor

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = xwtexeditord
    LIBS += -lXWCored -lXWGuiCored -lXWTeXd  -lXWTeXGuid -lXWTikzd
} else {
    TARGET = xwtexeditor
    LIBS += -lXWCore -lXWGuiCore -lXWTeX  -lXWTeXGui -lXWTikz
}
