TEMPLATE = subdirs
SUBDIRS = XWTikz \
          XWTeXDocEditorCore \
          xwlatexeditor \
          xwtexeditor \
          xwtexfmteditor

xwlatexeditor.depends = XWTikz XWTeXDocEditorCore
xwtexeditor.depends = XWTikz
xwtexfmteditor.depends = XWTikz
