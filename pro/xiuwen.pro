TEMPLATE = subdirs
SUBDIRS = XWCore \
          XWGuiCore \
          XWTeX \
          XWTeXGui \
          XWTikz \
          XWTeXDocEditorCore \
          bibtex \
          extractbb \
          makeindex \
          pdfstring \
          xwdvitype \
          xwlatexeditor \
          xwlog \
          xwreader \
          xwsettings \
          xwtexeditor \
          xwtexfmteditor \
          xwtexreader \
          xwtray

XWGuiCore.depends = XWCore
XWTeX.depends = XWCore
XWTeXGui.depends = XWTeX
XWTikz.depends = XWTeXGui
XWTeXDocEditorCore.depends = XWTeXGui
bibtex.depends = XWCore
extractbb.depends = XWCore
makeindex.depends = XWCore
xwdvitype.depends = XWGuiCore
xwlatexeditor.depends = XWGuiCore XWTeXGui XWTeXDocEditorCore
xwreader.depends = XWGuiCore
xwtexeditor.depends = XWGuiCore XWTeX
xwtexfmteditor.depends = XWTeXGui 
xwtexreader.depends = XWGuiCore XWTeXGui
