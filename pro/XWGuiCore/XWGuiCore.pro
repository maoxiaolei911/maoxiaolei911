TARGET = XWGuiCore
TEMPLATE = lib
DEFINES += XW_BUILD_GUI_LIB

QT += network
QT += phonon

include(../dir.prj)

HEADERS += $${XWGUICORESRCDIR}/XWDocCanvas.h \
           $${XWGUICORESRCDIR}/XWAnimationPlayer.h \
		   $${XWGUICORESRCDIR}/XWCharacterWidget.h \
		   $${XWGUICORESRCDIR}/XWDisplaySettingDialog.h \
		   $${XWGUICORESRCDIR}/XWSlide.h \
		   $${XWGUICORESRCDIR}/XWGuiCore.h \
		   $${XWGUICORESRCDIR}/XWEncryptWindow.h \
		   $${XWGUICORESRCDIR}/XWExternalToolWindow.h \
		   $${XWGUICORESRCDIR}/XWFontInfoWindow.h \
		   $${XWGUICORESRCDIR}/XWFontWindow.h \
		   $${XWGUICORESRCDIR}/XWFTPWindow.h \
		   $${XWGUICORESRCDIR}/XWGotoPageWindow.h \
		   $${XWGUICORESRCDIR}/XWMediaPlayer.h \
		   $${XWGUICORESRCDIR}/XWOutlineWindow.h \
		   $${XWGUICORESRCDIR}/XWPasswardWindow.h \
		   $${XWGUICORESRCDIR}/XWRecentFileWindow.h \
		   $${XWGUICORESRCDIR}/XWReferenceWindow.h \
		   $${XWGUICORESRCDIR}/XWSearchWindow.h \
		   $${XWGUICORESRCDIR}/XWTexFontWindow.h \
		   $${XWGUICORESRCDIR}/XWToImageWindow.h \
		   $${XWGUICORESRCDIR}/XWToPDFWindow.h \
		   $${XWGUICORESRCDIR}/XWToPSWindow.h \
		   $${XWGUICORESRCDIR}/XWToTextWindow.h \
		   $${XWGUICORESRCDIR}/XWBaseNameWindow.h \
		   $${XWGUICORESRCDIR}/XWDocFontsWindow.h \
		   $${XWGUICORESRCDIR}/XWFontDefWindow.h \
		   $${XWGUICORESRCDIR}/XWURLWindow.h \
			 $${XWGUICORESRCDIR}/XWRuler.h

SOURCES += $${XWGUICORESRCDIR}/XWDocCanvas.cpp \
           $${XWGUICORESRCDIR}/XWAnimationPlayer.cpp \
		   $${XWGUICORESRCDIR}/XWCharacterWidget.cpp \
		   $${XWGUICORESRCDIR}/XWDisplaySettingDialog.cpp \
		   $${XWGUICORESRCDIR}/XWSlide.cpp \
		   $${XWGUICORESRCDIR}/XWGuiCore.cpp \
		   $${XWGUICORESRCDIR}/XWEncryptWindow.cpp \
		   $${XWGUICORESRCDIR}/XWExternalToolWindow.cpp \
		   $${XWGUICORESRCDIR}/XWFontInfoWindow.cpp \
		   $${XWGUICORESRCDIR}/XWFontWindow.cpp \
		   $${XWGUICORESRCDIR}/XWFTPWindow.cpp \
		   $${XWGUICORESRCDIR}/XWGotoPageWindow.cpp \
		   $${XWGUICORESRCDIR}/XWMediaPlayer.cpp \
		   $${XWGUICORESRCDIR}/XWOutlineWindow.cpp \
		   $${XWGUICORESRCDIR}/XWPasswardWindow.cpp \
		   $${XWGUICORESRCDIR}/XWRecentFileWindow.cpp \
		   $${XWGUICORESRCDIR}/XWReferenceWindow.cpp \
		   $${XWGUICORESRCDIR}/XWSearchWindow.cpp \
		   $${XWGUICORESRCDIR}/XWTexFontWindow.cpp \
		   $${XWGUICORESRCDIR}/XWToImageWindow.cpp \
		   $${XWGUICORESRCDIR}/XWToPDFWindow.cpp \
		   $${XWGUICORESRCDIR}/XWToPSWindow.cpp \
		   $${XWGUICORESRCDIR}/XWToTextWindow.cpp \
		   $${XWGUICORESRCDIR}/XWBaseNameWindow.cpp \
		   $${XWGUICORESRCDIR}/XWDocFontsWindow.cpp \
		   $${XWGUICORESRCDIR}/XWFontDefWindow.cpp \
		   $${XWGUICORESRCDIR}/XWURLWindow.cpp \
			 $${XWGUICORESRCDIR}/XWRuler.cpp

RESOURCES    = $${XWRESDIR}/xw_guicore.qrc

MOC_DIR = $${TMPDIR}/moc/XWGuiCore
OBJECTS_DIR = $${TMPDIR}/obj/XWGuiCore

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = XWGuiCored
    LIBS += -lXWCored
} else {
    TARGET = XWGuiCore
    LIBS += -lXWCore
}
