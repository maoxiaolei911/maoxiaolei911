TARGET = xwlog
TEMPLATE = app

include(../dir.prj)

HEADERS += $${XWTEXGUISRCDIR}/XWLogMainWindow.h 

SOURCES += $${XWTEXGUISRCDIR}/XWLogMainWindow.cpp \
           $${XWTEXGUISRCDIR}/xwlog.cpp 

RESOURCES    = $${XWRESDIR}/xw_log.qrc


MOC_DIR = $${TMPDIR}/moc/xwlog
OBJECTS_DIR = $${TMPDIR}/obj/xwlog
