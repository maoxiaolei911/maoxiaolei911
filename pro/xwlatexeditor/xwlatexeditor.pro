TARGET = xwlatexeditor
TEMPLATE = app

QT += network
QT += phonon

include(../dir.prj)

XWLATEXEDITORSRCDIR = ../../src/latexeditor

INCLUDEPATH += $${XWLATEXEDITORSRCDIR}

HEADERS += $${XWLATEXEDITORSRCDIR}/XWLaTeXEditorCore.h \
           $${XWLATEXEDITORSRCDIR}/XWLaTeXEditorDev.h \
           $${XWLATEXEDITORSRCDIR}/XWLaTeXEditorMainWindow.h \
           $${XWLATEXEDITORSRCDIR}/XWLaTeXEditor.h

SOURCES += $${XWLATEXEDITORSRCDIR}/XWLaTeXEditorCore.cpp \
           $${XWLATEXEDITORSRCDIR}/XWLaTeXEditorDev.cpp \
           $${XWLATEXEDITORSRCDIR}/XWLaTeXEditorMainWindow.cpp \
           $${XWLATEXEDITORSRCDIR}/XWLaTeXEditor.cpp \
           $${XWLATEXEDITORSRCDIR}/latexeditor.cpp

RESOURCES    = $${XWRESDIR}/xw_latexeditor.qrc

MOC_DIR = $${TMPDIR}/moc/xwlatexeditor
OBJECTS_DIR = $${TMPDIR}/obj/xwlatexeditor

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = xwlatexeditord
    LIBS += -lXWCored -lXWGuiCored -lXWTeXd  -lXWTeXGuid -lXWTeXDocEditorCored -lXWTikzd
} else {
    TARGET = xwlatexeditor
    LIBS += -lXWCore -lXWGuiCore -lXWTeX  -lXWTeXGui -lXWTeXDocEditorCore -lXWTikz
}
