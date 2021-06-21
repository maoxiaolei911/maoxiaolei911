/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXCONSOLE_H
#define XWTEXCONSOLE_H

#include <QString>
#include <QPlainTextEdit>

#include "XWTeXGuiType.h"

class XW_TEXGUI_EXPORT XWTeXConsole : public QPlainTextEdit
{
	Q_OBJECT
	
public:
	XWTeXConsole(QWidget * parent);
	
public slots:
	void requstInput();
	
signals:
	void inputFinished(const QString & txt);
	
protected:
	void keyPressEvent(QKeyEvent *e);	
	
private:
	bool threadRequsting;
	QString inputStr;
};

#endif //XWTEXCONSOLE_H
