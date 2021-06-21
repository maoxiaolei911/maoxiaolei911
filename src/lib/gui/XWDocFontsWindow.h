/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDOCFONTSWINDOW_H
#define XWDOCFONTSWINDOW_H

#include <QDialog>
#include <QString>
#include "XWGlobal.h"

class XWDict;
class XWDoc;
class XWCoreFont;
class QTableWidget;

class XW_GUI_EXPORT XWDocFontsWindow : public QDialog
{
	Q_OBJECT

public:
	XWDocFontsWindow(XWDoc * docA, QWidget * parent = 0);	
			
private:
	void scanFonts();
	void scanFonts(XWDict *resDict);
	void scanFont(XWCoreFont * font);
	
private:
	XWDoc * doc;
	QTableWidget * table;
	int row;
	int pg;
};

#endif // XWDOCFONTSWINDOW_H
