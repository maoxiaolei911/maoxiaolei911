/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include <QTextStream>
#include <QStringListModel>
#include "XWApplication.h"
#include "XWTexSea.h"
#include "XWTeXHighlighter.h"


static void loadPoolFile(const QString & filename,
	                       int maxcs,
	                       QStringList & cslist,
	                       QStringList & keylist)
{
	XWXWTexSea sea;
	QFile * file = sea.openFile(filename, XWXWTexSea::Pool);
	if (file)
	{
		QTextStream stream(file);
		int i = 0;
		while (!stream.atEnd())
		{
			QString line = stream.readLine();
			line = line.simplified();
			if (line.isEmpty())
				continue;

			if (i < maxcs)
			{
				if (!cslist.contains(line))
					cslist << line;
			}
			else
			{
				if (!keylist.contains(line))
					keylist << line;
			}

			i++;
		}

		file->close();
		delete file;
	}
}

XWTeXHighlighter::XWTeXHighlighter(QObject * parent)
	:QSyntaxHighlighter(parent)
{
	HighlightingRule rule;

  QString fam = xwApp->getSetting("ControlSequenceFontFamily").toString();
	csFormat.setFontFamily(fam);
	int weight = xwApp->getSetting("ControlSequenceFontWeight").toInt();
	csFormat.setFontWeight(weight);
	bool italic = xwApp->getSetting("ControlSequenceFontItalic").toBool();
	csFormat.setFontItalic(italic);
	int size = xwApp->getSetting("ControlSequenceFontSize").toInt();
	if (size < 0 || size > 72)
	  size = 10;
	csFormat.setFontPointSize(size);
	QVariant variant = xwApp->getSetting("ControlSequenceColor");
	QColor color = variant.value<QColor>();
	csFormat.setForeground(color);
	rule.pattern = QRegExp("\\\\[a-zA-Z]+");
	rule.format = csFormat;
  highlightingRules.append(rule);
	rule.pattern = QRegExp("\\\\[^a-zA-Z]{1}");
	rule.format = csFormat;
  highlightingRules.append(rule);

	fam = xwApp->getSetting("CommentFontFamily").toString();
	commentFormat.setFontFamily(fam);
	weight = xwApp->getSetting("CommentFontWeight").toInt();
	commentFormat.setFontWeight(weight);
	italic = xwApp->getSetting("CommentFontItalic").toBool();
	commentFormat.setFontItalic(italic);
	size = xwApp->getSetting("CommentFontSize").toInt();
	if (size < 0 || size > 72)
	  size = 10;
	commentFormat.setFontPointSize(size);
	variant = xwApp->getSetting("CommentColor");
	color = variant.value<QColor>();
	commentFormat.setForeground(color);
	rule.pattern = QRegExp("^\\%[^\n]*");
	rule.format = commentFormat;
  highlightingRules.append(rule);

	fam = xwApp->getSetting("OptionFontFamily").toString();
	optionFormat.setFontFamily(fam);
  weight = xwApp->getSetting("OptionFontWeight").toInt();
  optionFormat.setFontWeight(weight);
	italic = xwApp->getSetting("OptionFontItalic").toBool();
	optionFormat.setFontItalic(italic);
	size = xwApp->getSetting("OptionFontSize").toInt();
	if (size < 0 || size > 72)
	  size = 10;
	optionFormat.setFontPointSize(size);
	variant = xwApp->getSetting("OptionColor");
	color = variant.value<QColor>();
	optionFormat.setForeground(color);
	rule.pattern = QRegExp("\\[[a-zA-Z0-9;:,<>/=#&_~\\!\\?\\+\\-\\*\\.\\s\r\n\\$\\^\\|\\\\\\{\\}\\(\\)]*\\]");
	rule.format = optionFormat;
  highlightingRules.append(rule);
	rule.pattern = QRegExp("<[0-9,\\-]*>");
	rule.format = optionFormat;
  highlightingRules.append(rule);

	fam = xwApp->getSetting("ParamFontFamily").toString();
	paramFormat.setFontFamily(fam);
	weight = xwApp->getSetting("ParamFontWeight").toInt();
	paramFormat.setFontWeight(weight);
	italic = xwApp->getSetting("ParamFontItalic").toBool();
	paramFormat.setFontItalic(italic);
	size = xwApp->getSetting("ParamFontSize").toInt();
	if (size < 0 || size > 72)
	  size = 10;
	paramFormat.setFontPointSize(size);
	variant = xwApp->getSetting("ParamColor");
	color = variant.value<QColor>();
	paramFormat.setForeground(color);
	rule.pattern = QRegExp("\\{[a-zA-Z0-9;:,<>/=#&_~\\!\\?\\+\\-\\*\\.\\s\r\n\\$\\^\\|\\\\\\(\\)]*\\}");
	rule.format = paramFormat;
  highlightingRules.append(rule);
	rule.pattern = QRegExp("\\([a-zA-Z0-9;:,<>/=#&_~\\!\\?\\+\\-\\*\\.\\s\r\n\\$\\^\\|\\\\\\{\\}]*\\)");
	rule.format = paramFormat;
  highlightingRules.append(rule);

	fam = xwApp->getSetting("SpecialCharFontFamily").toString();
	spcharFormat.setFontFamily(fam);
	weight = xwApp->getSetting("SpecialCharFontWeight").toInt();
	spcharFormat.setFontWeight(weight);
	italic = xwApp->getSetting("SpecialCharFontItalic").toBool();
	spcharFormat.setFontItalic(italic);
	size = xwApp->getSetting("SpecialCharFontSize").toInt();
	if (size < 0 || size > 72)
	  size = 10;
	spcharFormat.setFontPointSize(size);
	variant = xwApp->getSetting("SpecialCharColor");
	color = variant.value<QColor>();
	spcharFormat.setForeground(color);
	rule.pattern = QRegExp("[\\\\]{0}[\\$\\^#&_~]");
  rule.format = spcharFormat;
  highlightingRules.append(rule);
}

void XWTeXHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules)
	{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0)
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
}

XWTeXCompleter::XWTeXCompleter(QObject * parent)
	:QCompleter(parent)
{
	setCompletionMode(QCompleter::PopupCompletion);
	QStringList files;
	QList<int>  cses;
	files << "tex.pool" << "pgf.pool" << "pstricks.pool" << "latex.pool" << "beamer.pool";
	cses << 466 << 568 << 234 << 991 << 205;
	loadFiles(files, cses);
}

void XWTeXCompleter::loadFiles(const QStringList & files, const QList<int> & maxcses)
{
	QStringList cslist;
	QStringList keylist;

	for (int i = 0; i < files.size(); i++)
	{
		QString file = files[i];
		int k = maxcses[i];
		loadPoolFile(file, k, cslist, keylist);
	}

	cslist.sort();
	keylist.sort();

	QStringList list;
	for (int i = 0; i < cslist.size(); i++)
	{
		QString pattern = QString("\\%1").arg(cslist.at(i));
		list << pattern;
	}

	list << keylist;

	QStringListModel * m = new QStringListModel(list, this);
	setModel(m);
}

void XWTeXCompleter::setWords(const QStringList & words)
{
	QStringListModel * m = new QStringListModel(words, this);
	setModel(m);
}
