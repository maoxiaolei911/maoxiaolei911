del /q ../../../xwrelease/xiuwen/bin/XWTeXGui.*
del /q ../../../xwrelease/xiuwen/bin/XWTikz.*
del /q ../../../xwrelease/xiuwen/bin/XWTeXDocEditorCore.*
del /q ../../../xwrelease/xiuwen/bin/xwlatexeditor.*
del /q ../../../xwrelease/xiuwen/bin/xwtexeditor.*
del /q ../../../xwrelease/xiuwen/bin/xwtexfmteditor.*

copy ../../../bin/XWTeXGui.dll ../../../xwrelease/xiuwen/bin/XWTeXGui.dll
copy ../../../bin/XWTikz.dll ../../../xwrelease/xiuwen/bin/XWTikz.dll
copy ../../../bin/XWTeXDocEditorCore.dll ../../../xwrelease/xiuwen/bin/XWTeXDocEditorCore.dll
copy ../../../bin/xwlatexeditor.exe ../../../xwrelease/xiuwen/bin/xwlatexeditor.exe
copy ../../../bin/xwtexeditor.exe ../../../xwrelease/xiuwen/bin/xwtexeditor.exe
copy ../../../bin/xwtexfmteditor.exe ../../../xwrelease/xiuwen/bin/xwtexfmteditor.exe

del /q ../../../xwrelease/xiuwen/xwtex/pool/*.pool
copy ../../pool/*.pool ../../../xwrelease/xiuwen/xwtex/pool/*.pool
