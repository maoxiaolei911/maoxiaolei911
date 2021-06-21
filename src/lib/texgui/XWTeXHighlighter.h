/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXHIGHLIGHTER_H
#define XWTEXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QCompleter>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QVector>
#include <QTextCharFormat>

#include "XWTeXGuiType.h"

class QTextDocument;

class XW_TEXGUI_EXPORT XWTeXHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	XWTeXHighlighter(QObject * parent = 0);
protected:
	void highlightBlock(const QString &text);

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};

	QVector<HighlightingRule> highlightingRules;

	QTextCharFormat textFormat;
	QTextCharFormat csFormat;
	QTextCharFormat commentFormat;
	QTextCharFormat optionFormat;
  QTextCharFormat paramFormat;
	QTextCharFormat spcharFormat;
};

class XW_TEXGUI_EXPORT XWTeXCompleter : public QCompleter
{
	Q_OBJECT

public:
	XWTeXCompleter(QObject * parent = 0);

	void loadFiles(const QStringList & files, const QList<int> & maxcses);
	void setWords(const QStringList & words);

private:
	void loadFile(const QString & filename,
	              int maxcs,
	              QStringList & cslist,
	              QStringList & keylist);
};

#endif //XWTEXHIGHLIGHTER_H
