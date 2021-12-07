TARGET = XWTeXGui
TEMPLATE = lib
DEFINES += XW_BUILD_TEXGUI_LIB

QT += network
QT += sql

include(../dir.prj)

HEADERS += $${XWTEXGUISRCDIR}/XWTeXGuiType.h \
					 $${XWTEXGUISRCDIR}/PGFKeyWord.h \
					 $${XWTEXGUISRCDIR}/PSTricksKeyWord.h \
					 $${XWTEXGUISRCDIR}/LaTeXKeyWord.h \
					 $${XWTEXGUISRCDIR}/BeamerKeyWord.h \
					 $${XWTEXGUISRCDIR}/XWTeXPool.h \
					 $${XWTEXGUISRCDIR}/XWPGFPool.h \
					 $${XWTEXGUISRCDIR}/XWPSTricksPool.h \
					 $${XWTEXGUISRCDIR}/XWLaTeXPool.h \
					 $${XWTEXGUISRCDIR}/XWBeamerPool.h \
					 $${XWTEXGUISRCDIR}/XWTeXDoc.h \
           $${XWTEXGUISRCDIR}/XWTeXHighlighter.h \
           $${XWTEXGUISRCDIR}/XWTeXDocTemplateWindow.h \
           $${XWTEXGUISRCDIR}/XWTeXTextEdit.h \
           $${XWTEXGUISRCDIR}/XWTeXConsole.h \
           $${XWTEXGUISRCDIR}/XWTexFormatWindow.h \
           $${XWTEXGUISRCDIR}/XWTeXGuiDev.h \
					 $${XWTEXGUISRCDIR}/XWSystemFontWidget.h \
					 $${XWTEXGUISRCDIR}/XWTexEditSetting.h \
					 $${XWTEXGUISRCDIR}/XWFindTextDialog.h \
					 $${XWTEXGUISRCDIR}/XWReplaceDialog.h \
					 $${XWTEXGUISRCDIR}/XWLaTeXSymbolWidget.h \
					 $${XWTEXGUISRCDIR}/XWLaTeXFormularWidget.h \
					 $${XWTEXGUISRCDIR}/XWTeXBox.h \
					 $${XWTEXGUISRCDIR}/XWTeXText.h \
					 $${XWTEXGUISRCDIR}/XWTeXTextBox.h \
					 $${XWTEXGUISRCDIR}/XWVisualLaTeXFormularEdit.h \
					 $${XWTEXGUISRCDIR}/XWLaTeXFormularMainWindow.h \
					 $${XWTEXGUISRCDIR}/XWPDFDriver.h \
					 $${XWTEXGUISRCDIR}/XWPDFOTLConf.h

SOURCES += $${XWTEXGUISRCDIR}/XWTeXGuiType.cpp \
					 $${XWTEXGUISRCDIR}/XWTeXPool.cpp \
					 $${XWTEXGUISRCDIR}/XWPGFPool.cpp \
					 $${XWTEXGUISRCDIR}/XWPSTricksPool.cpp \
					 $${XWTEXGUISRCDIR}/XWLaTeXPool.cpp \
					 $${XWTEXGUISRCDIR}/XWBeamerPool.cpp \
					 $${XWTEXGUISRCDIR}/XWTeXDoc.cpp \
           $${XWTEXGUISRCDIR}/XWTeXHighlighter.cpp \
           $${XWTEXGUISRCDIR}/XWTeXDocTemplateWindow.cpp \
           $${XWTEXGUISRCDIR}/XWTeXTextEdit.cpp \
           $${XWTEXGUISRCDIR}/XWTeXConsole.cpp \
           $${XWTEXGUISRCDIR}/XWTexFormatWindow.cpp \
           $${XWTEXGUISRCDIR}/XWTeXGuiDev.cpp \
					 $${XWTEXGUISRCDIR}/XWSystemFontWidget.cpp \
					 $${XWTEXGUISRCDIR}/XWTexEditSetting.cpp \
					 $${XWTEXGUISRCDIR}/XWFindTextDialog.cpp \
					 $${XWTEXGUISRCDIR}/XWReplaceDialog.cpp \
					 $${XWTEXGUISRCDIR}/XWLaTeXSymbolWidget.cpp \
					 $${XWTEXGUISRCDIR}/XWLaTeXFormularWidget.cpp \
					 $${XWTEXGUISRCDIR}/XWTeXBox.cpp \
					 $${XWTEXGUISRCDIR}/XWTeXText.cpp \
					 $${XWTEXGUISRCDIR}/XWTeXTextBox.cpp \
					 $${XWTEXGUISRCDIR}/XWVisualLaTeXFormularEdit.cpp \
					 $${XWTEXGUISRCDIR}/XWLaTeXFormularMainWindow.cpp \
					 $${XWTEXGUISRCDIR}/XWPDFDriver.cpp \
					 $${XWTEXGUISRCDIR}/XWPDFOTLConf.cpp


RESOURCES    = $${XWRESDIR}/xw_texgui.qrc

MOC_DIR = $${TMPDIR}/moc/XWTeXGui
OBJECTS_DIR = $${TMPDIR}/obj/XWTeXGui

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
    TARGET = XWTeXGuid
    LIBS += -lXWCored  -lXWTeXd
} else {
    TARGET = XWTeXGui
    LIBS += -lXWCore  -lXWTeX
}
