/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXEDITORCORE_H
#define XWTEXEDITORCORE_H

#include "XWGuiCore.h"

class XWTeXEditorMainWindow;
class QAction;

class XWTeXEditorCore : public XWGuiCore
{
	Q_OBJECT
	
public:       
    XWTeXEditorCore(XWTeXEditorMainWindow * mainwinA, QWidget * parent = 0);
    
protected:
	void showContextMenu(const QPoint &p);
    
private:
	XWTeXEditorMainWindow * mainWin;
};

#endif // XWTEXEDITORCORE_H

