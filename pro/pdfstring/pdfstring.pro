TARGET = pdfstring
TEMPLATE = app

include(../dir.prj)

SOURCES = $${XWWRITE18SRCDIR}/pdfstring.cpp 

MOC_DIR = $${TMPDIR}/moc/write18
OBJECTS_DIR = $${TMPDIR}/obj/write18

CONFIG += console
