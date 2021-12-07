/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QTextStream>
#include "XWApplication.h"
#include "XWTeXHighlighter.h"
#include "XWTeXTextEdit.h"

XWTeXTextEdit::XWTeXTextEdit(bool w, QWidget * parent)
	:QPlainTextEdit(parent)
{
	setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setTextInteractionFlags(Qt::TextEditorInteraction);
	writer = w;
	loaded = false;
	isFirstTime = true;
	isRegexpFind = false;
	findFlags = 0;
	QTextCursor cursor(document());
	highlightCursor = cursor;
	lineNumberArea = new XWLineNumberArea(this);

	if (xwApp->hasSetting("TextBackgroundColor"))
	{
		QVariant variant = xwApp->getSetting("TextBackgroundColor");
		QColor color = variant.value<QColor>();
		QPalette p = viewport()->palette();
		p.setColor(QPalette::Base, color);
		p.setColor(QPalette::HighlightedText, color);

		variant = xwApp->getSetting("TextColor");
    color = variant.value<QColor>();
		p.setColor(QPalette::Text, color);
		p.setColor(QPalette::Highlight, color);

		viewport()->setPalette(p);
		setPalette(p);

		QString fam = xwApp->getSetting("TextFontFamily").toString();
		int weight = xwApp->getSetting("TextFontWeight").toInt();
		bool italic = xwApp->getSetting("TextFontItalic").toBool();
		int size = xwApp->getSetting("TextFontSize").toInt();
		if (size < 0 || size > 72)
		  size = 10;

		QFont font = document()->defaultFont();
		font.setFamily(fam);
		font.setWeight(weight);
		font.setItalic(italic);
		font.setPointSize(size);
		document()->setDefaultFont(font);
	}

	highlighter = new XWTeXHighlighter(this);
	highlighter->setDocument(document());
	completer = new XWTeXCompleter(this);
	completer->setWidget(this);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseSensitive);
	connect(completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchParentheses()));
}

void XWTeXTextEdit::find(const QString & str, bool casesensitivity, bool wholeword, bool regexpA)
{
	findString = str;
	findFlags = 0;
	if (casesensitivity)
	  findFlags |= QTextDocument::FindCaseSensitively;

	if (wholeword)
	  findFlags |= QTextDocument::FindWholeWords;

	isRegexpFind = regexpA;
	if (isRegexpFind)
	  regexp.setPattern(str);
}

void XWTeXTextEdit::findNext()
{
	highlightCursor = textCursor();
	if (highlightCursor.isNull())
	{
		QTextCursor cursor(document());
	  highlightCursor = cursor;
	}
	
  isFirstTime = true;
	if (isRegexpFind)
	  highlightCursor = document()->find(regexp, highlightCursor, findFlags);
	else
	  highlightCursor = document()->find(findString, highlightCursor, findFlags);

	if (!highlightCursor.isNull())
	{
		QTextCharFormat colorFormat = highlightCursor.charFormat();
		QTextCharFormat f = colorFormat;
		colorFormat.setForeground(f.background());
		colorFormat.setBackground(f.foreground());
		highlightCursor.mergeCharFormat(colorFormat);		
		isFirstTime = false;
	}

	setTextCursor(highlightCursor);
}

QString XWTeXTextEdit::getFormular()
{
	QTextCursor cursor = textCursor();
	if (cursor.hasSelection())
	  return cursor.selectedText();

	return QString();
}

QString XWTeXTextEdit::getInputCodecName()
{
	QString ret = inCodecName;
	return ret;
}

QString XWTeXTextEdit::getOutputCodecName()
{
	QString ret = outCodecName;
	return ret;
}

QString XWTeXTextEdit::getSelected()
{
	highlightCursor = textCursor();
	return highlightCursor.selectedText();
}

QString XWTeXTextEdit::getTikzPicture()
{
	QTextCursor cursor = textCursor();
	if (cursor.hasSelection())
	  return cursor.selectedText();

	return QString();
}

bool XWTeXTextEdit::hasSelected()
{
	highlightCursor = textCursor();
	return highlightCursor.hasSelection();
}

void XWTeXTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(lineNumberArea);
  painter.fillRect(event->rect(), Qt::lightGray);

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();

  while (block.isValid() && top <= event->rect().bottom())
  {
     if (block.isVisible() && bottom >= event->rect().top())
     {
         QString number = QString::number(blockNumber + 1);
         painter.setPen(Qt::black);
         painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                              Qt::AlignRight, number);
     }

     block = block.next();
     top = bottom;
     bottom = top + (int) blockBoundingRect(block).height();
     ++blockNumber;
  }
}

int XWTeXTextEdit::lineNumberAreaWidth()
{
	int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10)
  {
     max /= 10;
     ++digits;
  }

  int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

  return space;
}

void XWTeXTextEdit::loadFile(const QString & filename)
{
	save();
	fullFileName = filename;
	if (!fullFileName.isEmpty())
	{
		QFile file(fullFileName);
		if (file.open(QFile::ReadOnly | QFile::Text))
		{
			if (inCodecName.isEmpty())
			{
				char buf[1025];
				qint64 len = file.read(buf, 1024);
				file.seek(0);
				if (len > 2)
				{
					if ((buf[0] == 0x5c || buf[0] == 0x25) && buf[1] == 0)
					    inCodecName = "UTF-16LE";
					else if (buf[0] == 0 && (buf[0] == 0x5c || buf[0] == 0x25))
						inCodecName = "UTF-16BE";
					else
					{
						QByteArray ba = QByteArray::fromRawData(buf, len);
						if (buf[0] == 0x25)
						{
							int l = ba.indexOf("\n");
			  			if (l > 1)
			  			{
			  				ba = ba.left(l);
			  				if (ba.contains("utf-8") || ba.contains("UTF-8"))
									inCodecName = "UTF-8";
								else if (ba.contains("gb18030") || ba.contains("GB18030"))
									inCodecName = "GB18030-0";
								else if (ba.contains("gbk") || ba.contains("GBK"))
									inCodecName = "GBK";
								else if (ba.contains("gb") || ba.contains("GB"))
									inCodecName = "GB";
								else if (ba.contains("big5") || ba.contains("Big5") || ba.contains("BIG5"))
									inCodecName = "Big5";
								else if (ba.contains("EUC-JP") || ba.contains("euc-jp"))
									inCodecName = "EUC-JP";
								else if (ba.contains("EUC-KR") || ba.contains("euc-kr"))
									inCodecName = "EUC-KR";
								else if (ba.contains("sjis") || ba.contains("SJIS"))
									inCodecName = "Shift-JIS";
			  			}
						}

						if (inCodecName.isEmpty())
						{
							if (ba.contains("\\begin{CJK}") || ba.contains("\\begin{CJK*}"))
							{
								if (ba.contains("{GBK}"))
									inCodecName = "GBK";
								else if (ba.contains("{GB}"))
									inCodecName = "GB";
								else if (ba.contains("{Bg5}"))
									inCodecName = "Big5";
								else if (ba.contains("{EUC-JP}"))
									inCodecName = "EUC-JP";
								else if (ba.contains("{KS}"))
									inCodecName = "EUC-KR";
								else if (ba.contains("{SJIS}"))
									inCodecName = "Shift-JIS";
							}
						}
					}
				}
			}

			QTextStream stream(&file);
			if (!inCodecName.isEmpty())
			{
				QTextCodec *codec = QTextCodec::codecForName(inCodecName);
				stream.setCodec(codec);
				outCodecName = inCodecName;
			}
			else
			{
				QTextCodec *codec = QTextCodec::codecForName("UTF-8");
				inCodecName = codec->name();
				stream.setCodec(codec);
				outCodecName = inCodecName;
			}

			QApplication::setOverrideCursor(Qt::WaitCursor);
			setPlainText(stream.readAll());
			QApplication::restoreOverrideCursor();
			setCurrentFile();
			connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
			loaded = true;
			file.close();
		}
		else
			xwApp->openError(fullFileName, false);
	}
}

void XWTeXTextEdit::replace(const QString & str, const QString & bystr, 
                            bool casesensitivity, bool wholeword, bool regexpA)
{
	findString = str;
	replaceString = bystr;
	findFlags = 0;
	if (casesensitivity)
	  findFlags |= QTextDocument::FindCaseSensitively;

	if (wholeword)
	  findFlags |= QTextDocument::FindWholeWords;

	isRegexpFind = regexpA;
	if (isRegexpFind)
	  regexp.setPattern(str);
}

void XWTeXTextEdit::replaceNext()
{
	highlightCursor = textCursor();
	if (highlightCursor.isNull())
	{
		QTextCursor cursor(document());
	  highlightCursor = cursor;
	}
	if (isRegexpFind)
		highlightCursor = document()->find(regexp, highlightCursor, findFlags);
	else
		highlightCursor = document()->find(findString, highlightCursor, findFlags);

	if (!highlightCursor.isNull())
	{
		QTextCharFormat colorFormat = highlightCursor.charFormat();
		QTextCharFormat f = colorFormat;
		colorFormat.setForeground(f.background());
		colorFormat.setBackground(f.foreground());
		highlightCursor.insertText(replaceString, colorFormat);		
	}
	setTextCursor(highlightCursor);
}

void XWTeXTextEdit::save()
{
	if (!fullFileName.isEmpty() && document()->isModified())
	{
		QFile file(fullFileName);
		if (file.open(QFile::WriteOnly | QFile::Truncate))
		{
			QTextStream stream(&file);
			stream.setGenerateByteOrderMark(false);
			if (!outCodecName.isEmpty())
			{
				QTextCodec *codec = QTextCodec::codecForName(outCodecName);
				stream.setCodec(codec);
			}
			QApplication::setOverrideCursor(Qt::WaitCursor);
			QString txt = toPlainText();
			stream << txt;
			QApplication::restoreOverrideCursor();
			setCurrentFile();
		}
		else
			xwApp->openError(fullFileName, true);
	}
}

void XWTeXTextEdit::saveTo(QIODevice * file)
{
	if (!file)
		return ;

	if (file->open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream stream(file);
		stream.setGenerateByteOrderMark(false);
		if (!outCodecName.isEmpty())
		{
			QTextCodec *codec = QTextCodec::codecForName(outCodecName);
			stream.setCodec(codec);
		}
		QString txt = toPlainText();
		stream << txt;
		file->close();
	}
}

void XWTeXTextEdit::setCompleter(XWTeXCompleter *)
{
}

void XWTeXTextEdit::setInputCodec(const QString & name)
{
	QByteArray tmp = name.toAscii();
	if (tmp != inCodecName && loaded)
	{
		inCodecName = tmp;
		outCodecName = tmp;
		disconnect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
		QFile file(fullFileName);
		if (file.open(QFile::ReadOnly | QFile::Text))
		{
			QTextStream stream(&file);
			if (!inCodecName.isEmpty())
			{
				stream.setAutoDetectUnicode(false);
				QTextCodec *codec = QTextCodec::codecForName(inCodecName);
				stream.setCodec(codec);
			}
			QApplication::setOverrideCursor(Qt::WaitCursor);
			setPlainText(stream.readAll());
			QApplication::restoreOverrideCursor();
			setCurrentFile();
			connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
		}
	}
}

void XWTeXTextEdit::setFileName(const QString & filename)
{
	fullFileName = filename;
	setCurrentFile();
  connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
}

void XWTeXTextEdit::setHighlighter(XWTeXHighlighter *)
{
}

void XWTeXTextEdit::setOutputCodec(const QString & name)
{
	QByteArray tmp = name.toAscii();
	if (tmp != outCodecName && loaded)
	{
		outCodecName = tmp;
		document()->setModified(true);
		setWindowModified(document()->isModified());
	}
}

void XWTeXTextEdit::setSelected(int pos, int len)
{
	highlightCursor.setPosition(pos, QTextCursor::MoveAnchor);
	highlightCursor.setPosition(pos + len, QTextCursor::KeepAnchor);
	setTextCursor(highlightCursor);
}

void XWTeXTextEdit::insertFormular(const QString & str)
{
	QTextCursor cursor = textCursor();
	if (cursor.hasSelection() || str.startsWith("\\begin{"))
	  cursor.insertText(str);
	else
	{
		QString df = QString("$%1$").arg(str);
		cursor.insertText(df);
	}
}

void XWTeXTextEdit::insertTikz(const QString & str)
{
	QTextCursor cursor = textCursor();
	cursor.insertText(str);
}

void XWTeXTextEdit::closeEvent(QCloseEvent *e)
{
	if (!writer)
		save();

	QPlainTextEdit::closeEvent(e);
}

void XWTeXTextEdit::focusInEvent(QFocusEvent *e)
{
	QPlainTextEdit::focusInEvent(e);
}

void XWTeXTextEdit::focusOutEvent(QFocusEvent *e)
{
	QPlainTextEdit::focusOutEvent(e);
}

void XWTeXTextEdit::keyPressEvent(QKeyEvent *e)
{
	if (completer->popup()->isVisible())
	{
		switch (e->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				e->ignore();
				return;

			case QKeySequence::FindNext:
			  if (!findString.isEmpty())
				{
					findNext();
				  e->accept();
				}
				else
				  e->ignore();			  
			  return ;

			default:
				break;
		}
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E);
	if (!isShortcut)
	{
		isFirstTime = true;
		QPlainTextEdit::keyPressEvent(e);
	}		

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if ((ctrlOrShift && e->text().isEmpty()))
		return;

	if (!completer || completer->widget() != this)
		return ;

	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();

	if (!isShortcut &&
		 (hasModifier || e->text().isEmpty() ||
		  completionPrefix.length() < 3 ||
		  eow.contains(e->text().right(1))))
	{
		completer->popup()->hide();
		return;
	}

	if (completionPrefix != completer->completionPrefix())
	{
		completer->setCompletionPrefix(completionPrefix);
		completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
	}

	QRect cr = cursorRect();
	cr.setWidth(completer->popup()->sizeHintForColumn(0) + completer->popup()->verticalScrollBar()->sizeHint().width());
	completer->complete(cr);
}

void XWTeXTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
	QPlainTextEdit::mouseDoubleClickEvent(e);
}

void XWTeXTextEdit::resizeEvent(QResizeEvent *event)
{
	QPlainTextEdit::resizeEvent(event);

  QRect cr = contentsRect();
  lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void XWTeXTextEdit::documentWasModified()
{
	setWindowModified(document()->isModified());
}

void XWTeXTextEdit::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly())
  {
      QTextEdit::ExtraSelection selection;

      selection.format.setBackground(Qt::gray);
      selection.format.setProperty(QTextFormat::FullWidthSelection, true);
      selection.cursor = textCursor();
      selection.cursor.clearSelection();
      extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}

void XWTeXTextEdit::insertCompletion(const QString &completion)
{
	if (!completer || completer->widget() != this)
         return;

    QTextCursor tc = textCursor();
    int extra = completion.length() - completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

void XWTeXTextEdit::matchParentheses()
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	QTextCursor cursor = textCursor();
	emit cursorChanged(cursor.blockNumber() + 1, cursor.positionInBlock() + 1);

	QTextBlock sblock = cursor.block();
	if (!sblock.isValid())
		return ;

	QString blocktext = sblock.text();
	int p = cursor.positionInBlock();
	if (blocktext.length() < 1 || p < 0 || p >= blocktext.length())
		return ;

	int k = blocktext.indexOf(QChar('%'));
	if (k == 0)
		return ;

	if (k > 0 && k <= p)
		return ;

	QTextBlock eblock;
	int s = cursor.position();
	QChar c = document()->characterAt(s);
	int e = s;
	int a = 1;
	if (c == QChar('{'))
	{
		eblock = sblock;
		p++;
		while (eblock.isValid())
		{
			if (p >= blocktext.length() ||
				  blocktext[p] == QChar('%'))
			{
				eblock = eblock.next();
				blocktext = eblock.text();
				p = 0;
				while (eblock.isValid() &&
				       (blocktext.isEmpty() ||
				       blocktext[p] == QChar('%')))
				{
					eblock = eblock.next();
				  blocktext = eblock.text();
				}

				if (blocktext[p] == QChar('}'))
				    p--;
			}
			else if (blocktext[p] == QChar('{'))
				a++;
			else if (blocktext[p] == QChar('}'))
				a--;
			else if (blocktext[p] == QChar('\\'))
			{
				p++;
				QString str = blocktext.mid(p, 7);
				if (str.startsWith("left{"))
					p += 4;
				else if (str.startsWith("right}"))
					p += 5;
			}

			if (a == 0)
			{
				e = eblock.position() + p;
				break;
			}

			p++;
		}
	}
	else if (c == QChar('['))
	{
		eblock = sblock;
		p++;
		while (eblock.isValid())
		{
			if (p >= blocktext.length() ||
				  blocktext[p] == QChar('%'))
			{
				eblock = eblock.next();
				blocktext = eblock.text();
				p = 0;
				while (eblock.isValid() &&
				       (blocktext.isEmpty() ||
				       blocktext[p] == QChar('%')))
				{
					eblock = eblock.next();
				  blocktext = eblock.text();
				}
				if (blocktext[p] == QChar(']'))
				    p--;
			}
			else if (blocktext[p] == QChar('['))
				a++;
			else if (blocktext[p] == QChar(']'))
				a--;

			if (a == 0)
			{
				e = eblock.position() + p;
				break;
			}

			p++;
		}
	}
	else if (c == QChar('('))
	{
		eblock = sblock;
		p++;
		while (eblock.isValid())
		{
			if (p >= blocktext.length() ||
				  blocktext[p] == QChar('%'))
			{
				eblock = eblock.next();
				blocktext = eblock.text();
				p = 0;
				while (eblock.isValid() &&
				       (blocktext.isEmpty() ||
				       blocktext[p] == QChar('%')))
				{
					eblock = eblock.next();
				  blocktext = eblock.text();
				}
				if (blocktext[p] == QChar(')'))
				    p--;
			}
			else if (blocktext[p] == QChar('('))
				a++;
			else if (blocktext[p] == QChar(')'))
				a--;

			if (a == 0)
			{
				e = eblock.position() + p;
				break;
			}

			p++;
		}
	}
	else if (c == QChar('<'))
	{
		eblock = sblock;
		p++;
		while (eblock.isValid())
		{
			if (p >= blocktext.length() ||
				  blocktext[p] == QChar('%'))
			{
				eblock = eblock.next();
				blocktext = eblock.text();
				p = 0;
				while (eblock.isValid() &&
				       (blocktext.isEmpty() ||
				       blocktext[p] == QChar('%')))
				{
					eblock = eblock.next();
				  blocktext = eblock.text();
				}
				if (blocktext[p] == QChar('>'))
				    p--;
			}
			else if (blocktext[p] == QChar('<'))
				a++;
			else if (blocktext[p] == QChar('>'))
				a--;

			if (a == 0)
			{
				e = eblock.position() + p;
				break;
			}

			p++;
		}
	}
	else if (c == QChar('}'))
	{
		eblock = sblock;
		p--;
		while (sblock.isValid())
		{
			if (p < 0)
			{
				sblock = sblock.previous();
				blocktext = sblock.text();
				while (sblock.isValid() &&
				       (blocktext.isEmpty() ||
				       blocktext[0] == QChar('%')))
				{
					sblock = sblock.previous();
				  blocktext = sblock.text();
				}
				p = blocktext.lastIndexOf(QChar('%'));
				if (p < 0)
					p = blocktext.length();
			}
			else if (blocktext[p] == QChar('{'))
			{
				bool l = false;
				if (p > 4)
				{
					QString str = blocktext.mid(p - 5, 5);
					if (str == "\\left")
						l = true;
				}

				if (!(p > 0 && blocktext[p - 1] == QChar('\\')) && !l)
				{
				  a--;
				}
			}
			else if (blocktext[p] == QChar('}'))
			{
				bool l = false;
				if (p > 5)
				{
					QString str = blocktext.mid(p - 6, 6);
					if (str == "\\right")
						l = true;
				}

				if (!(p > 0 && blocktext[p - 1] == QChar('\\')) && !l)
				  a++;
			}

			if (a == 0)
			{
				s = sblock.position() + p;
				break;
			}

			p--;
		}
	}
	else if (c == QChar(']'))
	{
		eblock = sblock;
		p--;
		while (sblock.isValid())
		{
			if (p < 0)
			{
				sblock = sblock.previous();
				blocktext = sblock.text();
				while (sblock.isValid() &&
				       (blocktext.isEmpty() ||
				       blocktext[0] == QChar('%')))
				{
					sblock = sblock.previous();
				  blocktext = sblock.text();
				}
				p = blocktext.lastIndexOf(QChar('%'));
				if (p < 0)
					p = blocktext.length();
			}
			else if (blocktext[p] == QChar('['))
				a--;
			else if (blocktext[p] == QChar(']'))
				a++;

			if (a == 0)
			{
				s = sblock.position() + p;
				break;
			}

			p--;
		}
	}
	else if (c == QChar(')'))
	{
		eblock = sblock;
		p--;
		while (sblock.isValid())
		{
			if (p < 0)
			{
				sblock = sblock.previous();
				blocktext = sblock.text();
				while (sblock.isValid() &&
				       (blocktext.isEmpty() ||
				       blocktext[0] == QChar('%')))
				{
					sblock = sblock.previous();
				  blocktext = sblock.text();
				}
				p = blocktext.lastIndexOf(QChar('%'));
				if (p < 0)
					p = blocktext.length();
			}
			else if (blocktext[p] == QChar('('))
				a--;
			else if (blocktext[p] == QChar(')'))
				a++;

			if (a == 0)
			{
				s = sblock.position() + p;
				break;
			}

			p--;
		}
	}
	else if (c == QChar('>'))
	{
		eblock = sblock;
		p--;
		while (sblock.isValid())
		{
			if (p < 0)
			{
				sblock = sblock.previous();
				blocktext = sblock.text();
				while (sblock.isValid() &&
				       (blocktext.isEmpty() ||
				       blocktext[0] == QChar('%')))
				{
					sblock = sblock.previous();
				  blocktext = sblock.text();
				}
				p = blocktext.lastIndexOf(QChar('%'));
				if (p < 0)
					p = blocktext.length();
			}
			else if (blocktext[p] == QChar('<'))
				a--;
			else if (blocktext[p] == QChar('>'))
				a++;

			if (a == 0)
			{
				s = sblock.position() + p;
				break;
			}

			p--;
		}
	}
	else if (c == QChar('\\') && blocktext.length() > 2)
	{
		if (p > 0 && blocktext[p - 1] == QChar('\\'))
			return ;

		p++;
		QString str = blocktext.mid(p, 2);
		if (str == "if")
		{
			p += 2;
			eblock = sblock;
			while (eblock.isValid())
		  {
			  if (p >= blocktext.length() ||
				    blocktext[p] == QChar('%'))
			  {
				    eblock = eblock.next();
				    blocktext = eblock.text();
				    p = 0;
				    while (eblock.isValid() &&
				           (blocktext.isEmpty() || blocktext[p] == QChar('%')))
				    {
				    	eblock = eblock.next();
				      blocktext = eblock.text();
				    }

				    if (blocktext[p] == QChar('\\'))
				    	p--;
			  }
			  else if (blocktext[p] == QChar('\\'))
			  {
			  	p++;
					str = blocktext.mid(p, 2);
					if (str == "if")
						a++;
					else if (str == "fi")
					{
						if (!blocktext[p + 2].isLetter())
							a--;
					}
					p++;
			  }

			  if (a == 0)
			  {
				    e = eblock.position() + p;
				    break;
			  }

			  p++;
		  }

		}
		else if (str == "fi")
		{
			if (!blocktext[p + 2].isLetter())
			{
				e += 2;
				p -= 3;
			  eblock = sblock;
			  while (sblock.isValid())
		    {
			     if (p < 0)
			     {
				     sblock = sblock.previous();
				     blocktext = sblock.text();
				     while (sblock.isValid() &&
				            (blocktext.isEmpty() || blocktext[0] == QChar('%')))
				     {
				     	  sblock = sblock.previous();
				        blocktext = sblock.text();
				     }

				     p = blocktext.lastIndexOf(QChar('%'));
				     if (p < 0)
					      p = blocktext.length();
			     }
			     else if (blocktext[p] == QChar('\\') &&
			     	        (!(p > 0 && blocktext[p - 1] == QChar('\\'))))
			     {
			     	   int i = p + 1;
					     str = blocktext.mid(i, 2);
					     if (str == "if")
						      a--;
					     else if (str == "fi")
					     {
						     if (!blocktext[i + 2].isLetter())
							      a++;
					     }
			     }

			     if (a == 0)
			     {
				     s = sblock.position() + p;
				     break;
			     }

			     p--;
		     }
			}
		}
		else if (str == "be")
		{
			str = blocktext.mid(p, 5);
			if (str == "begin")
			{
				if (!blocktext[p + 5].isLetter())
				{
					p += 5;
			    eblock = sblock;
			    while (eblock.isValid())
			    {
			    	if (p >= blocktext.length() ||
				        blocktext[p] == QChar('%'))
			      {
				        eblock = eblock.next();
				        blocktext = eblock.text();
				        p = 0;
				        while (eblock.isValid() &&
				               (blocktext.isEmpty() || blocktext[p] == QChar('%')))
				        {
				        	eblock = eblock.next();
				        	blocktext = eblock.text();
				        }

				        if (blocktext[p] == QChar('\\'))
				    	      p--;
			      }
			      else if (blocktext[p] == QChar('\\'))
			      {
			  	    p++;
					    str = blocktext.mid(p, 3);
					    if (str == "end")
					    {
						    if (!blocktext[p + 3].isLetter())
							    a--;

						    p += 2;
					    }
					    else if (str == "beg")
					    {
					    	str = blocktext.mid(p, 5);
					    	if (str == "begin")
					    	{
					    		p += 4;
					    		if (!blocktext[p + 2].isLetter())
					    			a++;
					    	}
					    	else
					    		p += 2;
					    }
			      }

			      if (a == 0)
			      {
			      	p++;
			      	while (eblock.isValid())
		          {
			            if (p >= blocktext.length() ||
				              blocktext[p] == QChar('%'))
			            {
				             eblock = eblock.next();
				             blocktext = eblock.text();
				             p = 0;
			            }
			            else if (blocktext[p] == QChar('{'))
				             a++;
			            else if (blocktext[p] == QChar('}'))
				            a--;

			            if (a == 0)
			            {
				            e = eblock.position() + p;
				            break;
			            }

			            p++;
		          }
				      break;
			      }

			      p++;
			    }
				}
			}
		}
		else if (str == "en")
		{
			str = blocktext.mid(p, 3);
			if (str == "end")
			{
				if (!blocktext[p + 3].isLetter())
				{
					p -= 4;
					eblock = sblock;
					while (sblock.isValid())
					{
						if (p < 0)
						{
							sblock = sblock.previous();
				      blocktext = sblock.text();
				      while (sblock.isValid() &&
				             (blocktext.isEmpty() || blocktext[0] == QChar('%')))
				      {
				      	sblock = sblock.previous();
				      	blocktext = sblock.text();
				      }

				      p = blocktext.lastIndexOf(QChar('%'));
				      if (p < 0)
					      p = blocktext.length();
						}
						else if (blocktext[p] == QChar('\\') &&
							      (!(p > 0 && blocktext[p - 1] == QChar('\\'))))
						{
							int i = p + 1;
							str = blocktext.mid(i, 5);
							if (str == "begin")
							{
								if (!blocktext[i + 5].isLetter())
									a--;
							}
							else if (str.startsWith("end"))
							{
								if (str.length() == 3 || !str[3].isLetter())
									a++;
							}
						}

						if (a == 0)
			      {
				      s = sblock.position() + p;

				      e += 4;
				      p = e;
				      blocktext = eblock.text();
				      while (eblock.isValid())
				      {
				      	if (p >= blocktext.length() ||
				              blocktext[p] == QChar('%'))
			            {
				             eblock = eblock.next();
				             blocktext = eblock.text();
				             p = 0;
			            }
			            else if (blocktext[p] == QChar('{'))
				             a++;
			            else if (blocktext[p] == QChar('}'))
				            a--;

			            if (a == 0)
			            {
				            e = eblock.position() + p;
				            break;
			            }

			            p++;
				      }
				      break;
			      }
						p--;
					}
				}
			}
		}
	}

	if (a == 0)
	{
		QTextCursor scursor(document());
		scursor.setPosition(s, QTextCursor::MoveAnchor);
		scursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, e - s + 1);
		QTextEdit::ExtraSelection selection;
		QTextCharFormat format = selection.format;
		format.setBackground(Qt::darkGray);
	  selection.format = format;
		selection.cursor = scursor;
	  extraSelections.append(selection);
	}
	setExtraSelections(extraSelections);
}

void XWTeXTextEdit::updateLineNumberArea(const QRect & rect, int dy)
{
	if (dy)
     lineNumberArea->scroll(0, dy);
  else
     lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

  if (rect.contains(viewport()->rect()))
     updateLineNumberAreaWidth(0);
}

void XWTeXTextEdit::updateLineNumberAreaWidth(int )
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void XWTeXTextEdit::setCurrentFile()
{
	document()->setModified(false);
	setWindowModified(false);
	QString title = QString("%1[*]").arg(fullFileName);
	setWindowTitle(title);
}

QString XWTeXTextEdit::textUnderCursor() const
{
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}

XWLineNumberArea::XWLineNumberArea(XWTeXTextEdit *editor)
	: QWidget(editor)
{
	codeEditor = editor;
}

QSize XWLineNumberArea::sizeHint() const
{
	return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void XWLineNumberArea::paintEvent(QPaintEvent *event)
{
	codeEditor->lineNumberAreaPaintEvent(event);
}
