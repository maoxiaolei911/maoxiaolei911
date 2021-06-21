/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWPDFDriver.h"
#include "XWTikzGraphic.h"
#include "XWTikzCanvas.h"

XWTikzCanvas::XWTikzCanvas(XWTikzGraphic * graphicA,int areaA,QWidget * parent)
:QWidget(parent),
graphic(graphicA),
drawArea(areaA),
dragging(false)
{
  QPalette p = palette();
  p.setColor(QPalette::Window,Qt::white);
  setPalette(p);
  setAttribute(Qt::WA_InputMethodEnabled);
  setMouseTracking(true);

  cursorOn = false;
  cursorMinX = 0;
  cursorMinY = 0;
  cursorMaxX = 0;
  cursorMaxY = 0;

  zoomFactor = 1.0;

  showGrid = false;
  showBg = false;

  connect(graphic, SIGNAL(changed()), this, SLOT(updateGraphicImage()));
  connect(graphic, SIGNAL(cursorChanged(double,double,double,double)), 
       this, SLOT(updateCursor(double,double,double,double)));
  connect(graphic, SIGNAL(viewChanged()), this, SLOT(updateView()));
  connect(&cursorBlinkTimer, SIGNAL(timeout()), this, SLOT(repaintCursor()));
}

void XWTikzCanvas::loadBgImage(const QString & filename)
{
  bgImage.load(filename);
  update();
}

void XWTikzCanvas::saveAsImage()
{
  QString path = graphic->getLocalPath();

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                            path,
                            tr("Images (*.png)"));

  graphicImage.save(fileName,"png");
}

void XWTikzCanvas::setShowBg(bool e)
{
  if (showBg != e)
  {
    showBg = e;
    update();
  }
}

void XWTikzCanvas::setShowGrid(bool e)
{
  if (showGrid != e)
  {
    showGrid = e;
    update();
  }
}

void XWTikzCanvas::setZoomFactor(double f)
{
  if (zoomFactor != f)
  {
    zoomFactor = f;
    update();
  }
}

QSize XWTikzCanvas::sizeHint() const
{
  int w = (int)(zoomFactor * graphic->getWidth() + 0.5);
	int h = (int)(zoomFactor * graphic->getHeight() + 0.5);
	return QSize(w, h);
}

void XWTikzCanvas::contextMenuEvent(QContextMenuEvent *e)
{
  QMenu menu;
  if (graphic->addMenuAction(menu))
    menu.exec(e->pos());
}

void XWTikzCanvas::inputMethodEvent(QInputMethodEvent *e)
{
  if (!e->commitString().isEmpty())
    graphic->insertText(e->commitString());
}

QVariant XWTikzCanvas::inputMethodQuery(Qt::InputMethodQuery property) const
{
  switch(property)
  {
    case Qt::ImMicroFocus:
      {
        QRect r(cursorMinX, cursorMinY, cursorMaxX - cursorMinX, cursorMaxY - cursorMinY);
        return r;
      }
      break;

    case Qt::ImFont:
      return QVariant(QApplication::font());

    case Qt::ImCursorPosition:
      return QVariant(graphic->getCursorPosition());

    case Qt::ImSurroundingText:
      return QVariant(graphic->getCurrentText());

    case Qt::ImCurrentSelection:
      return QVariant(graphic->getSelectedText());

    case Qt::ImMaximumTextLength:
        return QVariant(); // No limit.

    case Qt::ImAnchorPosition:
        return QVariant(graphic->getAnchorPosition());

    default:
        return QVariant();
  }
}

void XWTikzCanvas::keyPressEvent(QKeyEvent *e)
{
  if (e == QKeySequence::AddTab)
    e->ignore();
  else if (e == QKeySequence::Back)
  {
    graphic->back();
    e->accept();
  }
  else if (e == QKeySequence::Bold)
    e->ignore();
  else if (e == QKeySequence::Close)
    e->ignore();
  else if (e == QKeySequence::Copy)
  {
    QString str = graphic->getSelectedText();
    QApplication::clipboard()->setText(str);
    e->accept();
  }
  else if (e == QKeySequence::Cut)
  {
    graphic->cut();
    e->accept();
  }
    
  else if (e == QKeySequence::Delete)
  {
    graphic->del();
    e->accept();
  }
  else if (e == QKeySequence::DeleteEndOfLine)
    e->ignore();
  else if (e == QKeySequence::DeleteEndOfWord)
    e->ignore();
  else if (e == QKeySequence::DeleteStartOfWord)
    e->ignore();
  else if (e == QKeySequence::Find)
    e->ignore();
  else if (e == QKeySequence::FindNext)
    e->ignore();
  else if (e == QKeySequence::FindPrevious)
    e->ignore();
  else if (e == QKeySequence::Forward)
    e->ignore();
  else if (e == QKeySequence::HelpContents)
    e->ignore();
  else if (e == QKeySequence::InsertLineSeparator)
    e->ignore();
  else if (e == QKeySequence::InsertParagraphSeparator)
  {
    graphic->newPar();
    e->accept();
  }
  else if (e == QKeySequence::Italic)
    e->ignore();
  else if (e == QKeySequence::MoveToEndOfBlock)
  {
    graphic->goToOperationEnd();
    e->accept();
  }
  else if (e == QKeySequence::MoveToEndOfDocument)
  {
    graphic->goToEnd();
    e->accept();
  }
  else if (e == QKeySequence::MoveToEndOfLine)
  {
    graphic->goToPathEnd();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextChar)
  {
    graphic->goToNext();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextLine)
  {
    graphic->goToNextPath();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextPage)
  {
    graphic->goToNextScope();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextWord)
  {
    graphic->goToNextOperation();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousChar)
  {
    graphic->goToPrevious();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousLine)
  {
    graphic->goToPreviousPath();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousPage)
  {
    graphic->goToPreviousScope();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousWord)
  {
    graphic->goToPreviousOperation();
    e->accept();
  }
  else if (e == QKeySequence::MoveToStartOfDocument)
  {
    graphic->goToStart();
    e->accept();
  }
  else if (e == QKeySequence::MoveToStartOfLine)
  {
    graphic->goToPathStart();
    e->accept();
  }
  else if (e == QKeySequence::MoveToStartOfBlock)
  {
    graphic->goToOperationStart();
    e->accept();
  }
  else if (e == QKeySequence::New)
    e->ignore();
  else if (e == QKeySequence::NextChild)
    e->ignore();
  else if (e == QKeySequence::Open)
    e->ignore();
  else if (e == QKeySequence::Paste)
  {
    QString str = QApplication::clipboard()->text();
    graphic->paste(str);
    e->accept();
  }
  else if (e == QKeySequence::Preferences)
    e->ignore();
  else if (e == QKeySequence::PreviousChild)
    e->ignore();
  else if (e == QKeySequence::Print)
    e->ignore();
  else if (e == QKeySequence::Quit)
    e->ignore();
  else if (e == QKeySequence::Redo)
  {
    graphic->redo();
    e->accept();
  }
  else if (e == QKeySequence::Refresh)
    e->ignore();
  else if (e == QKeySequence::Replace)
    e->ignore();
  else if (e == QKeySequence::SaveAs)
    e->ignore();
  else if (e == QKeySequence::Save)
    e->ignore();
  else if (e == QKeySequence::SelectAll)
    e->ignore();  
  else if (e == QKeySequence::SelectEndOfBlock)
  {
    graphic->moveRightBig();
    e->accept();
  }
  else if (e == QKeySequence::SelectEndOfDocument)
  {
    graphic->moveDownBig();
    e->accept();
  }
  else if (e == QKeySequence::SelectEndOfLine)
  {
    graphic->moveRightHuge();
    e->accept();
  }
  else if (e == QKeySequence::SelectNextChar)
  {
    graphic->moveRightSmall();
    e->accept();
  }
  else if (e == QKeySequence::SelectNextLine)
  {
    graphic->moveDownSmall();
    e->accept();
  }
  else if (e == QKeySequence::SelectNextPage)
  {
    graphic->moveDown();
    e->accept();
  }
  else if (e == QKeySequence::SelectNextWord)
  {
    graphic->moveRight();
    e->accept();
  }
  else if (e == QKeySequence::SelectPreviousChar)
  {
    graphic->moveLeftSmall();
    e->accept();
  }
  else if (e == QKeySequence::SelectPreviousLine)
  {
    graphic->moveUpSmall();
    e->accept();
  }
  else if (e == QKeySequence::SelectPreviousPage)
  {
    graphic->moveUp();
    e->accept();
  }
  else if (e == QKeySequence::SelectPreviousWord)
  {
    graphic->moveLeft();
    e->accept();
  }
  else if (e == QKeySequence::SelectStartOfBlock)
  {
    graphic->moveLeftBig();
    e->accept();
  }
  else if (e == QKeySequence::SelectStartOfDocument)
  {
    graphic->moveUpBig();
    e->accept();
  }
  else if (e == QKeySequence::SelectStartOfLine)
  {
    graphic->moveLeftHuge();
    e->accept();
  }
  else if (e == QKeySequence::Underline)
    e->ignore();
  else if (e == QKeySequence::Undo)
  {
    graphic->undo();
    e->accept();
  }
  else if (e == QKeySequence::UnknownKey)
    e->ignore();
  else if (e == QKeySequence::WhatsThis)
    e->ignore();
  else if (e == QKeySequence::ZoomIn)
    e->ignore();
  else if (e == QKeySequence::ZoomOut)
    e->ignore();
  else
  {
    QString text = e->text();
    if (!text.isEmpty())
    {
      graphic->keyInput(text);
      e->accept();
    }
    else
      e->ignore();
  }
}

void XWTikzCanvas::mouseMoveEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    dragging = true;
    if (drawArea == XW_TIKZ_S_OPERATION || drawArea == XW_TIKZ_S_PATH)
    {
      QPointF p = e->posF();
      QTransform transform = getTransform();
      p = transform.map(p);
      int h = (int)(zoomFactor * graphic->getHeight() + 0.5);
      int w = (int)(zoomFactor * graphic->getWidth() + 0.5);	
      XWPDFDriver driver(w,h);
      graphic->dragTo(&driver,p);
      QImage img(w,h,QImage::Format_RGB32);
      img.fill(Qt::transparent);
      QPainter painter(&img);
      driver.display(&painter, zoomFactor);
      graphicImage = img;
      update();
    }
    return ;
  }

  if (drawArea == XW_TIKZ_S_OPERATION || drawArea == XW_TIKZ_S_PATH)
  {
    QTransform transform = getTransform();
    QPointF p = e->posF();
    p = transform.map(p);
    QString tip = graphic->getTips(p);
    if (!tip.isEmpty())
		{
			QPoint pp = mapToParent(e->pos());
			QToolTip::showText(pp, tip);
		}
  }  
}

void XWTikzCanvas::mousePressEvent(QMouseEvent *e)
{
  graphic->setScope(drawArea);

  if (e->button() != Qt::LeftButton)
  {
    e->ignore();
    return ;
  }

  if (!dragging)
  {
    QTransform transform = getTransform();
    QPointF p = e->posF();
    p = transform.map(p);
    graphic->hitTest(p);
  }
}

void XWTikzCanvas::mouseReleaseEvent(QMouseEvent * e)
{
  if (dragging)
  {
    QTransform transform = getTransform();
    QPointF p = e->posF();
    p = transform.map(p);
    graphic->dropTo(p);
    dragging = false;
  }
}

void XWTikzCanvas::paintEvent(QPaintEvent * e)
{
  QPainter painter(this);
  QRect rect = e->rect();
  if (showBg || showGrid)
  {
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    if (showBg && !bgImage.isNull())
      painter.drawImage(0,0,bgImage);

    if (showGrid)
    {
      int h = (int)(zoomFactor * graphic->getHeight() + 0.5);
      int minx =  (int)(zoomFactor * graphic->getMinX() + 0.5);
      int miny =  (int)(zoomFactor * graphic->getMinY() + 0.5);
      int step = (int)(zoomFactor * graphic->getGridStep() + 0.5);

      painter.save();
      QPen pen = painter.pen();
      pen.setWidth(0.2);
      pen.setColor(Qt::gray);
      painter.setPen(pen);

      int i = rect.left() + (minx + rect.left()) % step;
      for (; i < rect.right(); i += step)
        painter.drawLine(i, rect.top(), i, rect.bottom());

      i = rect.bottom() - (miny + h - rect.bottom()) % step;
      for (; i > rect.top(); i -= step)
        painter.drawLine(rect.left(), i, rect.right(), i);

      painter.restore();
    }
  } 

  painter.drawImage(rect,graphicImage,rect);

  if (cursorMinY != cursorMaxY)
  {
    QRect cr(cursorMinX,cursorMinY,cursorMaxX-cursorMinX,cursorMaxY-cursorMinY);
    if (cr.intersects(rect))
    {
      if (cursorOn)
        painter.fillRect(cr,Qt::black);
      else
        painter.fillRect(cr,Qt::white);
    }      
  }
}

QTransform XWTikzCanvas::getTransform()
{
  double dx = graphic->getMinX();
  double dy = graphic->getMaxY();
  double ctm[6];
  ctm[0] = 1/zoomFactor;
  ctm[1] = 0;
  ctm[2] = 0;
  ctm[3] = -1/zoomFactor;
  ctm[4] = dx;
  ctm[5] = -dy;
  QTransform transform(ctm[0],ctm[1],ctm[2],ctm[3],ctm[4], ctm[5]);

  return transform;
}

void XWTikzCanvas::repaintCursor()
{
  cursorOn = !cursorOn;
  update(cursorMinX,cursorMinY,cursorMaxX - cursorMinX, cursorMaxY - cursorMinY);  
}

void XWTikzCanvas::updateCursor(double minxA,double minyA,double maxxA,double maxyA)
{
  cursorOn = false;
  repaintCursor();

  double dx = graphic->getMinX();
  double dy = graphic->getMaxY();
  cursorMinX = (int)(zoomFactor * (minxA - dx) + 0.5);
  cursorMinY = (int)(zoomFactor * (dy - minyA) + 0.5);
  cursorMaxX = (int)(zoomFactor * (maxxA - dx) + 0.5);
  cursorMaxY = (int)(zoomFactor * (dy - maxyA) + 0.5);
  bool istxt = (cursorMinX != cursorMaxX && cursorMinY != cursorMaxY);
  setBlinkingCursorEnabled(istxt);
}

void XWTikzCanvas::updateGraphicImage()
{
  int h = (int)(zoomFactor * graphic->getHeight() + 0.5);
  int w = (int)(zoomFactor * graphic->getWidth() + 0.5);	
  XWPDFDriver driver(w,h);
  switch (drawArea)
  {
    default:
      graphic->doGraphic(&driver);
      break;

    case XW_TIKZ_S_OPERATION:
      graphic->doOperation(&driver);
      break;

    case XW_TIKZ_S_PATH:
      graphic->doPath(&driver);
      break;

    case XW_TIKZ_S_SCOPE:
      graphic->doScope(&driver);
      break;
  }

  QImage img(w,h,QImage::Format_RGB32);
  img.fill(Qt::transparent);
  QPainter painter(&img);
  driver.display(&painter, zoomFactor);
  graphicImage = img;
  update();
}

void XWTikzCanvas::updateView()
{
  switch (graphic->getCurrentScope())
  {
    case XW_TIKZ_S_GRAPHIC:
      if (drawArea != XW_TIKZ_S_GRAPHIC)
        updateGraphicImage();
      break;

    case XW_TIKZ_S_SCOPE:
      if (drawArea == XW_TIKZ_S_PATH || 
          drawArea == XW_TIKZ_S_OPERATION)
        updateGraphicImage();
      break;

    case XW_TIKZ_S_PATH:
      if (drawArea == XW_TIKZ_S_OPERATION)
        updateGraphicImage();
      break;

    default:
      if (drawArea == XW_TIKZ_S_OPERATION)
        updateGraphicImage();
      break;
  }
}

void XWTikzCanvas::setBlinkingCursorEnabled(bool enable)
{
  if (enable && QApplication::cursorFlashTime() > 0)
    cursorBlinkTimer.start(QApplication::cursorFlashTime() / 2);
  else
    cursorBlinkTimer.stop();

  cursorOn = enable;
}
