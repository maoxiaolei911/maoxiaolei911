/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWGOTOPAGEWINDOW_H
#define XWURLWINDOW_H

#include <QDialog>
#include "XWGlobal.h"

class QLabel;
class QComboBox;
class QDialogButtonBox;

class XW_GUI_EXPORT XWGotoPageWindow : public QDialog
{
	Q_OBJECT

public:
	XWGotoPageWindow(int num, QWidget * parent = 0);
	
	int getPage();
	
private:
	QLabel * pageslabel;
	QComboBox * pagesCombo;
	QDialogButtonBox * buttonBox;
};

#endif // XWGOTOPAGEWINDOW_H
