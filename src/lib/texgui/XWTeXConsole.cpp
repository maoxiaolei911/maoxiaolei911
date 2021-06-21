/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWTeXConsole.h"

XWTeXConsole::XWTeXConsole(QWidget * parent)
	:QPlainTextEdit(parent),
	 threadRequsting(false)
{
	setTextInteractionFlags(Qt::TextEditorInteraction);
}

void XWTeXConsole::requstInput()
{
	threadRequsting = true;
	inputStr.clear();
}

void XWTeXConsole::keyPressEvent(QKeyEvent *e)
{
	if (!threadRequsting)
		return ;
		
	QString t;
	switch (e->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			threadRequsting = false;
			QPlainTextEdit::keyPressEvent(e);
			emit inputFinished(inputStr);
			break;
			
		default:
			t = e->text();
			if (!t.isEmpty())
				inputStr.append(t);
			QPlainTextEdit::keyPressEvent(e);
			break;
	}
}
