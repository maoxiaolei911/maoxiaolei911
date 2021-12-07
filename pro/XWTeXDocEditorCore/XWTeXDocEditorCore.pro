TARGET = XWTeXDocEditorCore
TEMPLATE = lib
DEFINES += XW_BUILD_TEXDOCEDITORCORE_LIB

QT += network
QT += sql

include(../dir.prj)

HEADERS += $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentType.h \
           $${XWTEXDOCEDITORSRCDIR}/XWTexDocFontSetting.h \
           $${XWTEXDOCEDITORSRCDIR}/XWTeXDocument.h \
           $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentUndoCommand.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentObject.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXObject.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocSearcher.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWTikzObject.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWPSTricksObject.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentPage.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentCanvas.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentCore.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWTikzDialog.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWPSTricksDialog.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWLaTeXDocumentObject.h \
           $${XWTEXDOCEDITORSRCDIR}/XWBeamerObject.h \
					 $${XWTEXDOCEDITORSRCDIR}/XWLaTeXDocument.h

SOURCES += $${XWTEXDOCEDITORSRCDIR}/XWTexDocFontSetting.cpp \
           $${XWTEXDOCEDITORSRCDIR}/XWTeXDocument.cpp \
           $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentUndoCommand.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentObject.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXObject.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocSearcher.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWTikzObject.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWPSTricksObject.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentPage.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentCanvas.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWTeXDocumentCore.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWTikzDialog.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWPSTricksDialog.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWLaTeXDocumentObject.cpp \
           $${XWTEXDOCEDITORSRCDIR}/XWBeamerObject.cpp \
					 $${XWTEXDOCEDITORSRCDIR}/XWLaTeXDocument.cpp

RESOURCES    = $${XWRESDIR}/xw_texdoceditorcore.qrc

MOC_DIR = $${TMPDIR}/moc/XWTeXDocEditorCore
OBJECTS_DIR = $${TMPDIR}/obj/XWTeXDocEditorCore

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
    TARGET = XWTeXDocEditorCored
    LIBS += -lXWCored -lXWGuiCored -lXWTeXGuid
} else {
    TARGET = XWTeXDocEditorCore
    LIBS += -lXWCore -lXWGuiCore -lXWTeXGui
}
