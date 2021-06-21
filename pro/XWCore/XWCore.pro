TARGET = XWCore
TEMPLATE = lib
DEFINES += XW_BUILD_UTIL_LIB
DEFINES += XW_BUILD_FONT_LIB
DEFINES += XW_BUILD_RASTER_LIB
DEFINES += XW_BUILD_CORE_LIB
DEFINES += XW_BUILD_DVI_LIB
DEFINES += XW_BUILD_PDF_LIB
DEFINES += XW_BUILD_PS_LIB

QT += network
QT += sql

include(../dir.prj)
include(util.prj)
include(font.prj)
include(raster.prj)
include(core.prj)
include(dvi.prj)
include(pdf.prj)
include(ps.prj)

MOC_DIR = $${TMPDIR}/moc/XWCore
OBJECTS_DIR = $${TMPDIR}/obj/XWCore

CONFIG(debug, debug|release) {
	DEFINES += XW_DEBUG
	CONFIG += console
    TARGET = XWCored
    LIBS += -lfreetype246MT_D -lzlibd -llibpngd -llibjpeg
} else {
    TARGET = XWCore
    LIBS += -lfreetype246MT -lzlib -llibpng -llibjpeg
}
