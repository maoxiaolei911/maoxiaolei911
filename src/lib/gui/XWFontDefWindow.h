/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTDEFWINDOW_H
#define XWFONTDEFWINDOW_H

#include <QDialog>
#include <QString>
#include "XWDVIType.h"

class QTableWidget;

class XW_GUI_EXPORT XWFontDefWindow : public QDialog
{
	Q_OBJECT

public:
	XWFontDefWindow(DVIFontDef * defs, int num, QWidget * parent = 0);	
	
private:
	QTableWidget * table;
};

#endif // XWFONTDEFWINDOW_H
