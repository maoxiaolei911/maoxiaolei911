/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include <QtGui>
#include "LaTeXKeyWord.h"
#include "XWLaTeXPool.h"
#include "XWTeXText.h"
#include "XWVisualLaTeXFormularEdit.h"

struct LaTeXFormularZoomMenuInfo
{
    char *label;
    double zoom;
};

static LaTeXFormularZoomMenuInfo formularZoomMenuInfo[] = {
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "400%"),       400 },
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "200%"),       200 },
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "150%"),       150 },
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "100%"),       100 },
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "50%"),        50 },
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "25%"),        25 },
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "12.5%"),      12.5 },
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "8.33%"),      8.33 },
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "fit page"),  XW_LTX_MATH_ZOOM_PAGE },
    { QT_TRANSLATE_NOOP("XWVisualLaTeXFormularEdit", "fit width"), XW_LTX_MATH_ZOOM_WIDTH }
};

#define XW_LTX_MATH_MIX_ZOOM_IDX   0
#define XW_LTX_MATH_MAX_ZOOM_IDX   7
#define XW_LTX_MATH_ZOOM_PAGE_IDX  8
#define XW_LTX_MATH_ZOOM_WIDTH_IDX 9

XWVisualLaTeXFormularEdit::XWVisualLaTeXFormularEdit(QWidget * parent)
 :QScrollArea(parent),
  pageWidth(596.95),
  pageHeight(419.53),
  totalW(0),
  totalH(0),
  zoomIdx(XW_LTX_MATH_ZOOM_WIDTH),
  zoomFactor(1.0),
  xScale(1.0),
  yScale(1.0),
  isEnableChange(true),
  cursorOn(false),
  cursorMinX(0),
  cursorMinY(0),
  cursorMaxX(0),
  cursorMaxY(0)
{
  setBackgroundRole(QPalette::NoRole);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setWidgetResizable(false);
	horizontalScrollBar()->setSingleStep(16);
	verticalScrollBar()->setSingleStep(16);
  canvas = new XWLaTeXFormularCanvas(this);
  setWidget(canvas);
  undoStack = new QUndoStack;
  box = new XWLaTeXFormularBox(this);
  updatePage();

  connect(&cursorBlinkTimer, SIGNAL(timeout()), this, SLOT(repaintCursor()));
}

XWVisualLaTeXFormularEdit::~XWVisualLaTeXFormularEdit()
{
   if (box)
   {
     delete box;
     box = 0;
   }

   if (undoStack)
   {
     delete undoStack;
     undoStack = 0;
   }
}

QString XWVisualLaTeXFormularEdit::getFormular()
{
  QString ret = box->getText();  
  return ret;
}

QStringList XWVisualLaTeXFormularEdit::getZooms()
{
  QStringList ret;
	for (int i = XW_LTX_MATH_MIX_ZOOM_IDX; i <= XW_LTX_MATH_ZOOM_WIDTH_IDX; i++)
		ret << tr(formularZoomMenuInfo[i].label);

	return ret;
}

void XWVisualLaTeXFormularEdit::redraw(QPainter * painter, const QRect & r)
{
  QScrollBar * hbar = horizontalScrollBar();
  QScrollBar * vbar = verticalScrollBar();
  int x = hbar->value();
  int y = vbar->value();
  QRect vr(x, y, viewport()->width(), viewport()->height());
  QRect ir = vr.intersected(r);
  painter->save();
  painter->fillRect(ir,Qt::white);
  QTransform transform(xScale,0,0,-yScale,0, 0);
  painter->setTransform(transform);
  double fx = ir.x() / xScale;
  double fy = ir.y() / yScale;
  double w = ir.width() / xScale;
  double h = ir.height() / yScale;
  QRectF fr(fx,-fy,w,h);
  box->draw(painter, fr);
  if (cursorOn && isEnableChange)
  {
    QRect cr(cursorMinX,cursorMinY,cursorMaxX-cursorMinX,cursorMaxY-cursorMinY);
    if (cr.intersects(r))
    {
      cr = cr.intersected(r);
      painter->fillRect(cr,Qt::black);
    }
  }

  painter->restore();
}

void XWVisualLaTeXFormularEdit::setText(const QString & str)
{
  int len = str.length();
  int pos = 0;
  box->scan(str,len,pos);
  typeset();
}

void XWVisualLaTeXFormularEdit::updateCursor(double minx, double miny, double maxx,
                    double maxy)
{
  cursorOn = true;
  repaintCursor();

  cursorMinX = (int)(minx * xScale + 0.5);
  cursorMinY = (int)(-miny * yScale + 0.5);
  cursorMaxX = (int)(maxx * xScale + 0.5);
  cursorMaxY = (int)(maxy * yScale + 0.5);
  cursorOn = true;
}

void XWVisualLaTeXFormularEdit::back()
{
  box->back();
  typeset();
}

void XWVisualLaTeXFormularEdit::clear()
{
  if (box)
  {
    delete box;
    box =  0;
  }
  box = new XWLaTeXFormularBox(this);
  cursorMinX = 0;
  cursorMinY = 0;
  cursorMaxX = 0;
  cursorMaxY = 0;
  undoStack->clear();
  cursorBlinkTimer.stop();
  canvas->update();
}

void XWVisualLaTeXFormularEdit::copy()
{
  QString str = box->getText();
  QApplication::clipboard()->setText(str);
}

void XWVisualLaTeXFormularEdit::del()
{
  box->del();
  typeset();
}

void XWVisualLaTeXFormularEdit::hitTest(double xA, double yA)
{
  box->hitTest(xA,yA);
}

void XWVisualLaTeXFormularEdit::insertFormular(const QString & str)
{
  box->insert(str);
  typeset();
}

void XWVisualLaTeXFormularEdit::keyInput(const QString & str)
{
  box->keyInput(str);
  typeset();
}

void XWVisualLaTeXFormularEdit::paste()
{
  QString str = QApplication::clipboard()->text();
  if (str.isEmpty())
    return ;

  insertFormular(str);
}

void XWVisualLaTeXFormularEdit::push(QUndoCommand * cmdA)
{
  if (!cmdA)
    return ;

  undoStack->push(cmdA);
}

void XWVisualLaTeXFormularEdit::redo()
{
  undoStack->redo();
  typeset();
}

void XWVisualLaTeXFormularEdit::zoom(int idx)
{
  if (idx == zoomIdx)
		return ;

	if (idx >= XW_LTX_MATH_MIX_ZOOM_IDX && idx <= XW_LTX_MATH_MAX_ZOOM_IDX)
		zoomFactor = formularZoomMenuInfo[idx].zoom;
	else if (idx == XW_LTX_MATH_ZOOM_PAGE_IDX)
		zoomFactor = XW_LTX_MATH_ZOOM_PAGE;
	else if (idx == XW_LTX_MATH_ZOOM_WIDTH_IDX)
		zoomFactor = XW_LTX_MATH_ZOOM_WIDTH;

	zoomIdx = idx;
  updatePage();
  canvas->update();
}

void XWVisualLaTeXFormularEdit::zoomIn()
{
  if (zoomIdx > XW_LTX_MATH_MIX_ZOOM_IDX && zoomIdx <= XW_LTX_MATH_MAX_ZOOM_IDX)
		zoom(zoomIdx - 1);
}

void XWVisualLaTeXFormularEdit::zoomOut()
{
  if (zoomIdx < XW_LTX_MATH_MAX_ZOOM_IDX)
		zoom(zoomIdx + 1);
}

void XWVisualLaTeXFormularEdit::undo()
{
  undoStack->undo();
  typeset();
}

void XWVisualLaTeXFormularEdit::changeEvent(QEvent *e)
{
  QScrollArea::changeEvent(e);
  if(e->type() == QEvent::ActivationChange)
  {
    if (!isActiveWindow())
      setBlinkingCursorEnabled(false);
  }
  else if (e->type() == QEvent::EnabledChange)
  {
    e->setAccepted(isEnabled());
    isEnableChange = e->isAccepted();
    if (isEnableChange)
      setBlinkingCursorEnabled(true);
    else
      setBlinkingCursorEnabled(false);
  }
}

void XWVisualLaTeXFormularEdit::contextMenuEvent(QContextMenuEvent *e)
{
  QMenu menu;
  QAction * act = 0;
  if (undoStack->canUndo())
  {
    act = menu.addAction(tr("Undo"));
    connect(act, SIGNAL(triggered()), this, SLOT(undo()));
  }

  if (undoStack->canRedo())
  {
    act = menu.addAction(tr("Redo"));
    connect(act, SIGNAL(triggered()), this, SLOT(redo()));
  }

  if (act)
    menu.addSeparator();

  box->contextMenu(menu);
  
  act = menu.addAction(tr("Copy"));
  connect(act, SIGNAL(triggered()), this, SLOT(copy()));

  menu.addSeparator();
  act = menu.addAction(tr("Clear"));
  connect(act, SIGNAL(triggered()), this, SLOT(clear()));
  menu.exec(e->pos());
}

void XWVisualLaTeXFormularEdit::focusInEvent(QFocusEvent *e)
{
  QScrollArea::focusInEvent(e);
  if (e->gotFocus())
    setBlinkingCursorEnabled(true);
  else
    setBlinkingCursorEnabled(false);
}

void XWVisualLaTeXFormularEdit::focusOutEvent(QFocusEvent *e)
{
  QScrollArea::focusOutEvent(e);
  setBlinkingCursorEnabled(false);
}

void XWVisualLaTeXFormularEdit::keyPressEvent(QKeyEvent *e)
{
  if (e == QKeySequence::AddTab)
  {
    box->insertColumn();
    e->accept();
  }
  else if (e == QKeySequence::Back)
  {
    back();
    e->accept();
  }
  else if (e == QKeySequence::Bold)
    e->ignore();
  else if (e == QKeySequence::Close)
    e->ignore();
  else if (e == QKeySequence::Copy)
  {
    copy();
    e->accept();
  }
  else if (e == QKeySequence::Cut)
    e->ignore();
  else if (e == QKeySequence::Delete)
  {
    del();
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
    box->insertRow();
    e->accept();
  }
  else if (e == QKeySequence::Italic)
    e->ignore();
  else if (e == QKeySequence::MoveToEndOfBlock)
    e->ignore();
  else if (e == QKeySequence::MoveToEndOfDocument)
    e->ignore();
  else if (e == QKeySequence::MoveToEndOfLine)
  {
    moveToEndOfLine();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextChar)
  {
    moveToNextChar();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextLine)
  {
    moveToNextLine();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextPage)
    e->ignore();
  else if (e == QKeySequence::MoveToNextWord)
    e->ignore();
  else if (e == QKeySequence::MoveToPreviousChar)
  {
    moveToPrevChar();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousLine)
  {
    moveToPreLine();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousPage)
    e->ignore();
  else if (e == QKeySequence::MoveToPreviousWord)
    e->ignore();
  else if (e == QKeySequence::MoveToStartOfDocument)
    e->ignore();
  else if (e == QKeySequence::MoveToStartOfLine)
  {
    moveToStartOfLine();
    e->accept();
  }
  else if (e == QKeySequence::New)
  {
    clear();
    e->accept();
  }    
  else if (e == QKeySequence::NextChild)
    e->ignore();
  else if (e == QKeySequence::Open)
    e->ignore();
  else if (e == QKeySequence::Paste)
  {
    paste();
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
    redo();
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
    e->ignore();
  else if (e == QKeySequence::SelectEndOfDocument)
    e->ignore();
  else if (e == QKeySequence::SelectEndOfLine)
    e->ignore();
  else if (e == QKeySequence::SelectNextChar)
    e->ignore();
  else if (e == QKeySequence::SelectNextLine)
    e->ignore();
  else if (e == QKeySequence::SelectNextPage)
    e->ignore();
  else if (e == QKeySequence::SelectNextWord)
    e->ignore();
  else if (e == QKeySequence::SelectPreviousChar)
    e->ignore();
  else if (e == QKeySequence::SelectPreviousLine)
    e->ignore();
  else if (e == QKeySequence::SelectPreviousPage)
    e->ignore();
  else if (e == QKeySequence::SelectPreviousWord)
    e->ignore();
  else if (e == QKeySequence::SelectStartOfBlock)
    e->ignore();
  else if (e == QKeySequence::SelectStartOfDocument)
    e->ignore();
  else if (e == QKeySequence::SelectStartOfLine)
    e->ignore();
  else if (e == QKeySequence::Underline)
    e->ignore();
  else if (e == QKeySequence::Undo)
  {
    undo();
    e->accept();
  }
  else if (e == QKeySequence::UnknownKey)
    e->ignore();
  else if (e == QKeySequence::WhatsThis)
    e->ignore();
  else if (e == QKeySequence::ZoomIn)
  {
    zoomIn();
    e->accept();
  }
  else if (e == QKeySequence::ZoomOut)
  {
    zoomOut();
    e->accept();
  }
  else
  {
    QString text = e->text();
    if (!text.isEmpty())
    {
      keyInput(text);
      e->accept();
    }
    else
      e->ignore();
  }
}

void XWVisualLaTeXFormularEdit::mousePressEvent(QMouseEvent *e)
{
  if (e->button() != Qt::LeftButton)
  {
    e->ignore();
    return ;
  }

  QTransform transform = getTransform();
  QPointF p = e->posF();
  p = transform.map(p);

  hitTest(p.x(), p.y());
}

void XWVisualLaTeXFormularEdit::resizeEvent(QResizeEvent *e)
{
  updatePage();
  QScrollArea::resizeEvent(e);
}

void XWVisualLaTeXFormularEdit::showEvent(QShowEvent *)
{
  ensureCursorVisible();
}

void XWVisualLaTeXFormularEdit::timerEvent(QTimerEvent *e)
{
  if (e->timerId() == cursorBlinkTimer.timerId())
  {
    cursorOn = !cursorOn;
    repaintCursor();
  }

  QScrollArea::timerEvent(e);
}

void XWVisualLaTeXFormularEdit::repaintCursor()
{
  cursorOn = !cursorOn;
  canvas->update(cursorMinX,cursorMinY,cursorMaxX - cursorMinX, cursorMaxY - cursorMinY);
}

void XWVisualLaTeXFormularEdit::ensureCursorVisible()
{
  ensureVisible(cursorMinX,cursorMinY);
}

QTransform XWVisualLaTeXFormularEdit::getTransform()
{
  double ctm[6];
  ctm[0] = 1/xScale;
  ctm[1] = 0;
  ctm[2] = 0;
  ctm[3] = -1/yScale;
  ctm[4] = 0;
  ctm[5] = 0;

  QTransform transform(ctm[0],ctm[1],ctm[2],ctm[3],ctm[4], ctm[5]);

  return transform;
}

void XWVisualLaTeXFormularEdit::moveToEndOfLine()
{
  box->moveToEndOfLine();
}

void XWVisualLaTeXFormularEdit::moveToNextChar()
{
  box->moveToNextChar();
}

void XWVisualLaTeXFormularEdit::moveToNextLine()
{
  box->moveToNextLine();
}

void XWVisualLaTeXFormularEdit::moveToPrevChar()
{
  box->moveToPrevChar();
}

void XWVisualLaTeXFormularEdit::moveToPreLine()
{
  box->moveToPreLine();
}

void XWVisualLaTeXFormularEdit::moveToStartOfLine()
{
  box->moveToStartOfLine();
}

void XWVisualLaTeXFormularEdit::setBlinkingCursorEnabled(bool enable)
{
  cursorOn = true;
  repaintCursor();

  if (enable && QApplication::cursorFlashTime() > 0)
    cursorBlinkTimer.start(QApplication::cursorFlashTime() / 2);
  else
    cursorBlinkTimer.stop();

  cursorOn = enable;
}

void XWVisualLaTeXFormularEdit::typeset()
{
  box->typeset(pageWidth, pageHeight);
  updatePage();
  canvas->update();
}

void XWVisualLaTeXFormularEdit::updatePage()
{
  int drawAreaWidth = viewport()->width();
	int drawAreaHeight = viewport()->height();
  if (zoomFactor == XW_LTX_MATH_ZOOM_PAGE)
  {
    xScale = (double)drawAreaWidth / pageWidth;
    yScale = (double)drawAreaHeight / pageHeight;
  }
  else if (zoomFactor == XW_LTX_MATH_ZOOM_WIDTH)
  {
    xScale = (double)drawAreaWidth / pageWidth;
    yScale = xScale;
  }
  else
  {
    xScale = 0.01 * zoomFactor;
    yScale = 0.01 * zoomFactor;
  }

  totalW = (int)(pageWidth * xScale + 0.5);
  totalH = (int)(pageHeight * yScale + 0.5);
}

XWLaTeXFormularCanvas::XWLaTeXFormularCanvas(XWVisualLaTeXFormularEdit * editA, QWidget * parent)
 : QWidget(parent),
   edit(editA)
{
  setMouseTracking(true);
}

XWLaTeXFormularCanvas::~XWLaTeXFormularCanvas()
{}

QSize XWLaTeXFormularCanvas::sizeHint() const
{
	int w = edit->getTotalWidth();
	int h = edit->getTotalHeight();
	return QSize(w, h);
}

void XWLaTeXFormularCanvas::paintEvent(QPaintEvent *e)
{
  QPainter painter(this);
  edit->redraw(&painter, e->rect());
}

XWLaTeXFormularBox::XWLaTeXFormularBox(XWVisualLaTeXFormularEdit * editA, QObject * parent)
:XWTeXTextBox(parent),
 edit(editA)
{}

void XWLaTeXFormularBox::contextMenu(QMenu & menu)
{
  if (parts.size() <= 0)
    return ;

  QAction * act = 0;
  XWTeXText * curObj = parts[cur]->getCurrent();
  if (curObj)
  {
    act = menu.addAction(tr("Add Supscript"));
    connect(act, SIGNAL(triggered()), this, SLOT(addSupscript()));

    act = menu.addAction(tr("Add Subscript"));
    connect(act, SIGNAL(triggered()), this, SLOT(addSubscript()));

    XWTeXText * robj = curObj;
    XWTeXText * cobj = 0;
    while (robj->parent())
    {
      if (robj->keyWord == XW_TEX_ROW)
        break;

      cobj = robj;
      robj = (XWTeXText*)(robj->parent());
    }

    if (robj->keyWord == XW_TEX_ROW)
    {
      menu.addSeparator();
      act = menu.addAction(tr("Insert Row"));
      connect(act, SIGNAL(triggered()), this, SLOT(insertRow()));
      act = menu.addAction(tr("Delete Row"));
      connect(act, SIGNAL(triggered()), this, SLOT(delRow()));

      XWTeXText * pobj = (XWTeXText*)(robj->parent());
      if (pobj->keyWord == LAarray)
      {
        if (cobj && cobj->keyWord == XW_TEX_COL)
        {
          menu.addSeparator();
          act = menu.addAction(tr("Insert Column"));
          connect(act, SIGNAL(triggered()), this, SLOT(insertColumn()));

          act = menu.addAction(tr("Delete Column"));
          connect(act, SIGNAL(triggered()), this, SLOT(delColumn()));
        }
      }
    }
  }

  if (act)
    menu.addSeparator();
}

void XWLaTeXFormularBox::moveToNextChar()
{
  goToNext();
}

void XWLaTeXFormularBox::moveToPrevChar()
{
  goToPrevious();
}

void XWLaTeXFormularBox::push(QUndoCommand * cmdA)
{
  edit->push(cmdA);
}

void XWLaTeXFormularBox::scan(const QString & str, int & len, int & pos)
{
  cur = 0;
  XWTeXTextBoxPart * part = new XWTeXTextBoxPart(this,this);
  parts << part;
  XWTeXText * obj = 0;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      skipComment(str,len,pos);
    else
    {
      if (str[pos] == QChar('$'))
      {
        pos++;
        int id = XW_TEX_FOMULAR;
        if (str[pos] == QChar('$'))
        {
          pos++;
          id = XW_TEX_DISFOMULAR;
        }
        obj = new XWTeXText(id,this,0);
      }
      else if (str[pos] == QChar('\\'))
      {
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAbegin)
        {
          key = scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
        }
        
        obj = new XWTeXText(id,this,0);
      }
      else if (str[pos] == QChar('+') || 
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||
               str[pos] == QChar('*') ||
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?') )
      {
        obj = new XWTeXText(XW_TEX_ASE,this,0);
      }        
      else if (str[pos] == QChar('.') || str[pos].isDigit())
        obj = new XWTeXText(XW_TEX_DIGIT,this,0);
      else
        obj = new XWTeXText(XW_TEX_LETTER,this,0);

      parts[cur]->append(obj);
      
      obj->scan(str,len,pos);
    }    
  }
}

void XWLaTeXFormularBox::updateCursor(double minx, double miny, double maxx,
                    double maxy, int textposA)
{
  if (cur >= 0)
    parts[cur]->setTextPos(textposA);
  edit->updateCursor(minx,miny,maxx,maxy);
}

void XWLaTeXFormularBox::addSubscript()
{
  if (cur < 0)
    return ;

  parts[cur]->addSubscript();
}

void XWLaTeXFormularBox::addSupscript()
{
  if (cur < 0)
    return ;

  parts[cur]->addSupscript();
}
