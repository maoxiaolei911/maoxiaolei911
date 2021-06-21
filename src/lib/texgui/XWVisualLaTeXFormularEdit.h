/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWVISUALLATEXFORMULAREDIT_H
#define XWVISUALLATEXFORMULAREDIT_H

#include <QScrollArea>
#include <QTimer>
#include <QMenu>
#include <QString>
#include <QList>
#include <QRectF>
#include <QRect>
#include <QUndoCommand>
#include <QTransform>

#include "XWTeXTextBox.h"

#define XW_LTX_MATH_ZOOM_PAGE  -1
#define XW_LTX_MATH_ZOOM_WIDTH -2

class QUndoStack;
class QPainter;
class XWLaTeXFormularBox;
class XWLaTeXFormularCanvas;

class XW_TEXGUI_EXPORT XWVisualLaTeXFormularEdit : public QScrollArea
{
  Q_OBJECT

public:
  XWVisualLaTeXFormularEdit(QWidget * parent = 0);
  ~XWVisualLaTeXFormularEdit();

  QString getFormular();

  int getTotalHeight() {return totalH;}
  int getTotalWidth() {return totalW;}

  double getZoom() {return zoomFactor;}
  int getZoomIdx() {return zoomIdx;}
  static QStringList getZooms();

  void redraw(QPainter * painter, const QRect & r);

  void setText(const QString & str);

  void updateCursor(double minx, double miny, double maxx, double maxy);

public slots:  
  void back();

  void clear();
  void copy();

  void del();

  void hitTest(double xA, double yA);

  void insertFormular(const QString & str);

  void keyInput(const QString & str);

  void paste();
  void push(QUndoCommand * cmdA);

  void redo();

  void zoom(int idx);
	void zoomIn();
	void zoomOut();

  void undo();

protected:
  virtual void changeEvent(QEvent *e);
  virtual void contextMenuEvent(QContextMenuEvent *e);
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
  virtual void keyPressEvent(QKeyEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
  virtual void resizeEvent(QResizeEvent *e);
	virtual void showEvent(QShowEvent *);
  virtual void timerEvent(QTimerEvent *e);

private slots:
  void repaintCursor();

private:
  void ensureCursorVisible();

  QTransform getTransform();

  void moveToEndOfLine();
  void moveToNextChar();
  void moveToNextLine();
  void moveToPrevChar();
  void moveToPreLine();
  void moveToStartOfLine();

  void setBlinkingCursorEnabled(bool enable);

  void typeset();

  void updatePage();

private:
  double pageWidth;
  double pageHeight;
  int    totalW;
  int    totalH;
  int zoomIdx;
  double zoomFactor;
  double xScale,yScale;
  bool isEnableChange;
  bool cursorOn;
  int cursorMinX;
	int cursorMinY;
	int cursorMaxX;
	int cursorMaxY;

  XWLaTeXFormularCanvas * canvas;
  XWLaTeXFormularBox * box;

  QUndoStack  * undoStack;

	QTimer cursorBlinkTimer;
};

class XWLaTeXFormularCanvas : public QWidget
{
  Q_OBJECT

public:
  XWLaTeXFormularCanvas(XWVisualLaTeXFormularEdit * editA, QWidget * parent = 0);
  virtual ~XWLaTeXFormularCanvas();

public:
	QSize sizeHint() const;

protected:
  virtual void paintEvent(QPaintEvent *e);

protected:
  XWVisualLaTeXFormularEdit * edit;
};

class XWLaTeXFormularBox : public XWTeXTextBox
{
  Q_OBJECT

public:
  XWLaTeXFormularBox(XWVisualLaTeXFormularEdit * editA, QObject * parent = 0);

  void contextMenu(QMenu & menu);

  void moveToNextChar();
  void moveToPrevChar();

  void push(QUndoCommand * cmdA);

  void scan(const QString & str, int & len, int & pos);

  void updateCursor(double minx, double miny, double maxx,
                    double maxy, int textposA);

public slots:
  void addSubscript();
  void addSupscript();
  
private:
  XWVisualLaTeXFormularEdit  * edit;
};


#endif //XWVISUALLATEXFORMULAREDIT_H
