TARGET = XWTeX
TEMPLATE = lib
DEFINES += XW_BUILD_TEX_LIB
DEFINES += XW_TEX
DEFINES += XW_TEX_DEBUG
DEFINES += XW_TEX_STAT

QT += network
QT += sql

include(../dir.prj)

HEADERS += $${XWTEXSRCDIR}/TeXKeyWord.h \
           $${XWTEXSRCDIR}/XWTeXType.h \
           $${XWTEXSRCDIR}/XWTeXIODev.h \
           $${XWTEXSRCDIR}/XWTeX.h

SOURCES += $${XWTEXSRCDIR}/XWTeXIODev.cpp \
           $${XWTEXSRCDIR}/XWTeX.cpp \
           $${XWTEXSRCDIR}/arithmetic.cpp \
           $${XWTEXSRCDIR}/error.cpp \
           $${XWTEXSRCDIR}/fmt.cpp \
           $${XWTEXSRCDIR}/font.cpp \
           $${XWTEXSRCDIR}/getNext.cpp \
           $${XWTEXSRCDIR}/hash.cpp \
           $${XWTEXSRCDIR}/hyphenate.cpp \
           $${XWTEXSRCDIR}/init.cpp \
           $${XWTEXSRCDIR}/line.cpp \
           $${XWTEXSRCDIR}/maincontrol.cpp \
           $${XWTEXSRCDIR}/node.cpp \
           $${XWTEXSRCDIR}/ocp.cpp \
           $${XWTEXSRCDIR}/page.cpp \
           $${XWTEXSRCDIR}/pool.cpp \
           $${XWTEXSRCDIR}/print.cpp \
           $${XWTEXSRCDIR}/scan.cpp \
           $${XWTEXSRCDIR}/shipout.cpp \
           
MOC_DIR = $${TMPDIR}/moc/XWTeX
OBJECTS_DIR = $${TMPDIR}/obj/XWTeX

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
    TARGET = XWTeXd
    LIBS += -lXWCored
} else {
    TARGET = XWTeX
    LIBS += -lXWCore
}
