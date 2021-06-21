TARGET = XWTikz
TEMPLATE = lib
DEFINES += XW_BUILD_TIKZ_LIB

QT += network
QT += sql

include(../dir.prj)

HEADERS += $${XWTIKZSRCDIR}/XWTikzType.h \
           $${XWTIKZSRCDIR}/tikzcolor.h \
					 $${XWTIKZSRCDIR}/XWTikzCommandDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzOperationDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzOptionDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzOpacityDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzArrowDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzLineDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzShapeDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzPatternDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzShadeDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzFadingDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzDecorationDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzPlotDialog.h \
					 $${XWTIKZSRCDIR}/XWTikzExpress.h \
					 $${XWTIKZSRCDIR}/XWTikzState.h \
					 $${XWTIKZSRCDIR}/XWTikzShape.h \
					 $${XWTIKZSRCDIR}/XWTikzArrow.h \
					 $${XWTIKZSRCDIR}/XWTikzFading.h \
					 $${XWTIKZSRCDIR}/XWTikzPattern.h \
					 $${XWTIKZSRCDIR}/XWTikzShade.h \
					 $${XWTIKZSRCDIR}/XWTikzTransparency.h \
					 $${XWTIKZSRCDIR}/XWTikzDecoration.h \
					 $${XWTIKZSRCDIR}/XWTikzPlotHandler.h \
					 $${XWTIKZSRCDIR}/XWTikzPlotMark.h \
					 $${XWTIKZSRCDIR}/XWTikzCommand.h \
					 $${XWTIKZSRCDIR}/XWTikzGraphic.h \
					 $${XWTIKZSRCDIR}/XWTikzOperation.h \
					 $${XWTIKZSRCDIR}/XWTikzCoord.h \
					 $${XWTIKZSRCDIR}/XWTikzOption.h \
					 $${XWTIKZSRCDIR}/XWTikzOptions.h \
					 $${XWTIKZSRCDIR}/XWTikzNode.h \
					 $${XWTIKZSRCDIR}/XWTikzTextBox.h \
					 $${XWTIKZSRCDIR}/XWTikzMatrix.h \
					 $${XWTIKZSRCDIR}/XWTikzUndoCommand.h \
					 $${XWTIKZSRCDIR}/XWTikzCanvas.h \
					 $${XWTIKZSRCDIR}/XWTikzArea.h \
					 $${XWTIKZSRCDIR}/XWTikzToolWidget.h \
					 $${XWTIKZSRCDIR}/XWTikzMainWindow.h

SOURCES += $${XWTIKZSRCDIR}/tikzcolor.cpp \
           $${XWTIKZSRCDIR}/XWTikzCommandDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzOperationDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzOptionDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzOpacityDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzArrowDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzLineDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzShapeDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzPatternDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzShadeDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzFadingDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzDecorationDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzPlotDialog.cpp \
					 $${XWTIKZSRCDIR}/XWTikzExpress.cpp \
					 $${XWTIKZSRCDIR}/XWTikzState.cpp \
					 $${XWTIKZSRCDIR}/XWTikzShape.cpp \
					 $${XWTIKZSRCDIR}/XWTikzArrow.cpp \
					 $${XWTIKZSRCDIR}/XWTikzFading.cpp \
					 $${XWTIKZSRCDIR}/XWTikzPattern.cpp \
					 $${XWTIKZSRCDIR}/XWTikzShade.cpp \
					 $${XWTIKZSRCDIR}/XWTikzTransparency.cpp \
					 $${XWTIKZSRCDIR}/XWTikzDecoration.cpp \
					 $${XWTIKZSRCDIR}/XWTikzPlotHandler.cpp \
					 $${XWTIKZSRCDIR}/XWTikzPlotMark.cpp \
					 $${XWTIKZSRCDIR}/XWTikzCommand.cpp \
					 $${XWTIKZSRCDIR}/XWTikzGraphic.cpp \
					 $${XWTIKZSRCDIR}/XWTikzOperation.cpp \
					 $${XWTIKZSRCDIR}/XWTikzCoord.cpp \
					 $${XWTIKZSRCDIR}/XWTikzOption.cpp \
					 $${XWTIKZSRCDIR}/XWTikzOptions.cpp \
					 $${XWTIKZSRCDIR}/XWTikzNode.cpp \
					 $${XWTIKZSRCDIR}/XWTikzTextBox.cpp \
					 $${XWTIKZSRCDIR}/XWTikzMatrix.cpp \
					 $${XWTIKZSRCDIR}/XWTikzUndoCommand.cpp \
					 $${XWTIKZSRCDIR}/XWTikzCanvas.cpp \
					 $${XWTIKZSRCDIR}/XWTikzArea.cpp \
					 $${XWTIKZSRCDIR}/XWTikzToolWidget.cpp \
					 $${XWTIKZSRCDIR}/XWTikzMainWindow.cpp

RESOURCES    = $${XWRESDIR}/xw_tikz.qrc

MOC_DIR = $${TMPDIR}/moc/XWTikz
OBJECTS_DIR = $${TMPDIR}/obj/XWTikz

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
    TARGET = XWTikzd
    LIBS += -lXWCored  -lXWTeXGuid
} else {
    TARGET = XWTikz
    LIBS += -lXWCore  -lXWTeXGui
}
