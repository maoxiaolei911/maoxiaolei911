/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZCANVAS_H
#define XWTIKZCANVAS_H

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QString>

class XWTikzGraphic;

class XWTikzCanvas : public QWidget
{
  Q_OBJECT

public:
  XWTikzCanvas(XWTikzGraphic * graphicA,int areaA,QWidget * parent = 0);

public:
  void loadBgImage(const QString & filename);

  void saveAsImage();

  void  setShowBg(bool e);
  void  setShowGrid(bool e);
  void  setZoomFactor(double f);
  QSize sizeHint() const;

protected:
  void contextMenuEvent(QContextMenuEvent *e);
  void inputMethodEvent(QInputMethodEvent *e);
	QVariant inputMethodQuery(Qt::InputMethodQuery property) const;
  void keyPressEvent(QKeyEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

  void paintEvent(QPaintEvent *e);

  void setBlinkingCursorEnabled(bool enable);

private slots:
  QTransform getTransform();

  void repaintCursor();
  
  void updateCursor(double minxA,double minyA,double maxA,double maxyA);
  void updateGraphicImage();
  void updateView();

private:
  XWTikzGraphic * graphic;
  int drawArea;
  bool dragging;

  bool cursorOn;
	int cursorMinX;
	int cursorMinY;
	int cursorMaxX;
	int cursorMaxY;

  double zoomFactor;
  bool showGrid;
  bool  showBg;
  QImage bgImage;

  QImage graphicImage;

  QTimer cursorBlinkTimer;
};

#endif //XWTIKZCANVAS_H
