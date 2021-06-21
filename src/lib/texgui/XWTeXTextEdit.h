/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXTEXTEDIT_H
#define XWTEXTEXTEDIT_H

#include <QPlainTextEdit>
#include <QObject>
#include <QByteArray>
#include <QStringList>
#include <QList>

#include "XWTeXGuiType.h"

class QIODevice;
class XWTeXHighlighter;
class XWTeXCompleter;
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class XWLineNumberArea;

class XW_TEXGUI_EXPORT XWTeXTextEdit : public QPlainTextEdit
{
	Q_OBJECT
	
public:
	XWTeXTextEdit(bool w = false, QWidget * parent = 0);
	virtual ~XWTeXTextEdit() {}

  void find(const QString & str, bool casesensitivity, bool wholeword, bool regexpA);
	void findAll(const QString & str, bool casesensitivity, bool wholeword, bool regexpA);
	void findNext();
	
	QString getFileName() {return fullFileName;}
	QString getFormular();
	QString getInputCodecName();
	QString getOutputCodecName();
	QString getSelected();
	QString getTikzPicture();

	bool hasSelected();
	
	void lineNumberAreaPaintEvent(QPaintEvent *event);
  int  lineNumberAreaWidth();
	void loadFile(const QString & filename);

  void replace(const QString & str, const QString & bystr, bool casesensitivity, bool wholeword, bool regexpA);
	void replaceAll(const QString & str, const QString & bystr, bool casesensitivity, bool wholeword, bool regexpA);
	void replaceNext();
	
	void save();	
	void saveTo(QIODevice * file);	
	void setCompleter(XWTeXCompleter *c);
	void setInputCodec(const QString & name);
  void setFileName(const QString & filename);
  void setHighlighter(XWTeXHighlighter * h);
	void setOutputCodec(const QString & name);
	
signals:
	void cursorChanged(int r, int c);

public slots:
  void insertFormular(const QString & str);
	void insertTikz(const QString & str);
	
protected:
	void closeEvent(QCloseEvent *e);
	void focusInEvent(QFocusEvent *e);
	void focusOutEvent(QFocusEvent *e);
	void keyPressEvent(QKeyEvent *e);	
	void mouseDoubleClickEvent(QMouseEvent *e);	
	void resizeEvent(QResizeEvent *event);
	
protected slots:
	void documentWasModified();
	
	void highlightCurrentLine();
	
	void insertCompletion(const QString &completion);
	
	void matchParentheses();
	
	void updateLineNumberArea(const QRect & rect, int dy);
	void updateLineNumberAreaWidth(int newBlockCount);
	
protected:	
	void setCurrentFile();
	
	QString textUnderCursor() const;
	
protected:
	bool writer;
	QWidget *lineNumberArea;
	XWTeXHighlighter * highlighter;
	XWTeXCompleter * completer;
	QByteArray inCodecName;
	QByteArray outCodecName;
	QString fullFileName;
	QString findString;
	QString replaceString;
	QRegExp regexp;
	bool loaded;
	bool isFirstTime;
	bool isRegexpFind;
	QTextDocument::FindFlags findFlags;
	QTextCursor highlightCursor;
};

class XWLineNumberArea : public QWidget
{
	Q_OBJECT
	
public:
	XWLineNumberArea(XWTeXTextEdit *editor);
	
	QSize sizeHint() const;
	
protected:
	void paintEvent(QPaintEvent *event);
	
private:
	XWTeXTextEdit *codeEditor;
};

#endif //XWTEXTEXTEDIT_H

