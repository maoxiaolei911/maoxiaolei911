/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWREADERCORE_H
#define XWREADERCORE_H

#include "XWGuiCore.h"

class XWReaderMainWindow;
class QAction;

class XWReaderCore : public XWGuiCore
{
	Q_OBJECT
	
public:       
    XWReaderCore(XWReaderMainWindow * mainwinA, QWidget * parent = 0);
    
protected:
	void showContextMenu(const QPoint &p);
    
private:
	XWReaderMainWindow * mainWin;
};

#endif // XWREADERCORE_H

