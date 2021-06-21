/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXREADERCORE_H
#define XWTEXREADERCORE_H

#include "XWGuiCore.h"

class XWTeXReaderMainWindow;
class QAction;

class XWTeXReaderCore : public XWGuiCore
{
	Q_OBJECT
	
public:       
    XWTeXReaderCore(XWTeXReaderMainWindow * mainwinA, QWidget * parent = 0);
    
protected:
	void showContextMenu(const QPoint &p);
    
private:
	XWTeXReaderMainWindow * mainWin;
};

#endif // XWTEXREADERCORE_H

