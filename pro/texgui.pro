TEMPLATE = subdirs
SUBDIRS = XWTeXGui \
          XWTikz \
          XWTeXDocEditorCore \
          xwlatexeditor \
          xwtexeditor \
          xwtexfmteditor

xwlatexeditor.depends = XWTeXGui XWTikz XWTeXDocEditorCore
xwtexeditor.depends = XWTeXGui XWTikz
xwtexfmteditor.depends = XWTeXGui XWTikz
