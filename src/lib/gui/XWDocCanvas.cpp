/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtDebug>
#include <QtGui>
#include <QApplication>
#include "XWDoc.h"
#include "XWLink.h"
#include "XWGuiCore.h"
#include "XWDocCanvas.h"

XWDocCanvas::XWDocCanvas(XWGuiCore * coreA, QWidget * parent)
	:QWidget(parent),
	 core(coreA),
	 dragging(false),
	 playing(false),
	 panning(false),
	 linkAction(0)
{
	setMouseTracking(true);
}

XWDocCanvas::~XWDocCanvas()
{
}

QSize XWDocCanvas::sizeHint() const
{
	int w = core->getTotalWidth();
	int h = core->getTotalHeight();
	return QSize(w, h);
}

void XWDocCanvas::contextMenuEvent(QContextMenuEvent * e)
{
	if (!core || core->getNumPages() == 0)
	{
		e->accept();
		return ;
	}
	
	core->showContextMenu(e->pos());
}

bool XWDocCanvas::doLink(int wx, int wy, bool onlyIfNoSelection)
{
  int pg;
  double xu, yu;
	if (core->cvtWindowToUser(wx, wy, &pg, &xu, &yu) && !(onlyIfNoSelection && core->hasSelection()))
	{
		XWLinkAction *action = core->findLink(pg, xu, yu);
		if (action)
		{
			core->doAction(action);
			return true;
		}
	}
	
	return false;
}

void XWDocCanvas::endSelection(int wx, int wy)
{
	int pg, x, y;
	bool ok = core->cvtWindowToDev(wx, wy, &pg, &x, &y);
	bool ca = false;
	if (dragging)
	{
		dragging = false;
		if (ok) 
			core->moveSelection(pg, wx, wy);
	  
		if (core->hasSelection() && core->okToCopy()) 
	  	ca = true;
	}
	
	if (ok)
		core->setCurrentPage(pg);
	
	emit copyAvailable(ca);
}

void XWDocCanvas::keyPressEvent(QKeyEvent *e)
{
	if (!core || core->getNumPages() == 0)
	{
		e->accept();
		return ;
	}
	
	if (e == QKeySequence::Copy && core->copyEnable())
		core->copy();
	else if (e == QKeySequence::Back)
			core->goBackward();
	else if (e == QKeySequence::Forward)
		core->goForward();
	else if (e == QKeySequence::MoveToEndOfDocument)
		core->moveToDocEnd();
	else if (e == QKeySequence::MoveToEndOfLine && !playing)
		core->moveToLineEnd();
	else if (e == QKeySequence::MoveToNextLine && !playing)
		core->moveToNextLine();
	else if (e == QKeySequence::MoveToNextPage && core->canNextPage() && !playing)
		core->moveToNextPage();
	else if (e == QKeySequence::MoveToPreviousLine && !playing)
		core->moveToPreLine();
	else if (e == QKeySequence::MoveToPreviousPage && core->canPrevPage())
		core->moveToPrePage();
	else if (e == QKeySequence::MoveToStartOfDocument)
		core->moveToDocBegin();
	else if (e == QKeySequence::MoveToStartOfLine)
		core->moveToLineBegin();
	else if (e == QKeySequence::MoveToNextChar && !playing)
		core->moveToNextChar();
	else if (e == QKeySequence::MoveToPreviousChar)
		core->moveToPreChar();
	else if (e == QKeySequence::ZoomIn)
		core->zoomIn();
	else if (e == QKeySequence::ZoomOut)
		core->zoomOut();
	else if (e == QKeySequence::Find)
		core->find();
	else if (e == QKeySequence::FindNext)
		core->findNext();
	else if (e == QKeySequence::FindPrevious)
		core->findPrev();
	else if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Backspace)
		emit fullScreenChanged(false);
		
	e->accept();
}

void XWDocCanvas::mouseMoveEvent(QMouseEvent *e)
{
	if (!core || core->getNumPages() == 0)
	{
		e->ignore();
		return ;
	}
	
	if (core->isMarking() && (e->buttons() & Qt::LeftButton))
	{
		core->markTo(e->pos());
		e->accept();
		return ;
	}
		
	int pg, x, y;
	if (core->cvtWindowToDev(e->x(), e->y(), &pg, &x, &y))
	{
		if (dragging && (e->buttons() & Qt::LeftButton))
			core->moveSelection(pg, e->x(), e->y());		
		else
		{
			double xu, yu;
			core->cvtWindowToUser(e->x(), e->y(), &pg, &xu, &yu);
			XWLinkAction *action = core->findLink(pg, xu, yu);
			if (action)
			{
				if (action != linkAction)
				{
					if (linkAction)
						QApplication::restoreOverrideCursor();
							
					linkAction = action;
					QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
				}
			}
			else if (linkAction)
			{
				linkAction = 0;
				QApplication::restoreOverrideCursor();
			}
			
			if (linkAction)
			{
				QString tip;
				XWLinkDest *dest = 0;
  				XWString *namedDest = 0;
  				XWString * fileName = 0;
  				XWString *actionName = 0;
  				XWObject movieAnnot, obj1, obj2;			
				switch (linkAction->getKind())
				{
					case LINK_ACTION_GOTO:
						if ((dest = ((XWLinkGoTo *)linkAction)->getDest()))
						{
  							int n = dest->getPageNum();
    						tip = QString(tr("Goto page %1")).arg(n);
  						}
  						else if ((namedDest = ((XWLinkGoTo *)linkAction)->getNamedDest()))
  						{
  							QString d = namedDest->toQString();
      						tip = QString(tr("Goto destination %1")).arg(d);
  						}
						break;
						
					case LINK_ACTION_GOTOR:
						fileName = ((XWLinkGoToR *)linkAction)->getFileName();
						if ((dest = ((XWLinkGoToR *)linkAction)->getDest()))
						{
  							int n = dest->getPageNum();		
  							QString fn = fileName->toQString(); 
  							tip = QString(tr("Open file %1 and goto page %2")).arg(fn).arg(n);
  						}
  						else if ((namedDest = ((XWLinkGoToR *)linkAction)->getNamedDest()))	
  						{
  							QString d = namedDest->toQString();
  							QString fn = fileName->toQString();
  							tip = QString(tr("Open file %1 and goto destination %2")).arg(fn).arg(d);
  						}
						break;
						
					case LINK_ACTION_LAUNCH:
						fileName = ((XWLinkLaunch *)linkAction)->getFileName();
						{
							QString fn = fileName->toQString();
							tip = QString(tr("Start program %1")).arg(fn);
						}
						break;
						
					case LINK_ACTION_URI:
						fileName = ((XWLinkURI *)linkAction)->getURI();
						{
							QString fn = fileName->toQString();
							tip = QString(tr("Open URI %1")).arg(fn);
						}
						break;
						
					case LINK_ACTION_NAMED:
						actionName = ((XWLinkNamed *)linkAction)->getName();
  						if (!actionName->cmp("NextPage"))
  							tip = QString(tr("Goto next page"));
  						else if (!actionName->cmp("PrevPage"))
  							tip = QString(tr("Goto previous page"));
  						else if (!actionName->cmp("FirstPage"))
  							tip = QString(tr("Goto first page"));
  						else if (!actionName->cmp("LastPage"))
  							tip = QString(tr("Goto last page"));
  						else if (!actionName->cmp("GoBack"))
  							tip = QString(tr("Go back"));
  						else if (!actionName->cmp("GoForward"))
  							tip = QString(tr("Go forward"));
						break;
						
					case LINK_ACTION_MOVIE:
						tip = QString(tr("Play movie"));
						break;
						
					default:
						break;
				}
				
				if (!tip.isEmpty())
				{
					QPoint pp = mapToParent(e->pos());
					QToolTip::showText(pp, tip);
				}
			}
		}
	}
		
	e->accept();
}

void XWDocCanvas::mousePressEvent(QMouseEvent *e)
{
	if (!core || core->getNumPages() == 0)
	{
		e->ignore();
		return ;
	}
		
	if (e->button() == Qt::LeftButton)
	{
		core->setDragPosition(e->pos());
		if (doLink(e->x(), e->y(), true))
		{
			e->accept();
			return ;
		}
			
		if (playing || core->getFullScreen())
		{
			if (core->canNextPage())
				core->gotoNextPage();
				
			e->accept();
			return ;
		}
		
		if (core->hasSelection())
			core->setSelection(0, 0, 0, 0, 0);
		
			startSelection(e->x(), e->y());
		
		panning = true;
	}
	else if (e->button() == Qt::RightButton)
		core->showContextMenu(e->globalPos());
		
	e->accept();
}

void XWDocCanvas::mouseReleaseEvent(QMouseEvent *e)
{
	if (!core || core->getNumPages() == 0)
	{
		e->ignore();
		return ;
	}
		
	if (e->button() == Qt::LeftButton)
	{
		if (!core->isMarking())
			endSelection(e->x(), e->y());
		else
			core->endMark();
	}
	
	panning = false;
	e->accept();
}

void XWDocCanvas::paintEvent(QPaintEvent *e)
{
	if (core)
	{
		QPainter painter(this);
		core->redraw(&painter, e->rect());
	}
}

void XWDocCanvas::startSelection(int wx, int wy)
{
	int pg, x, y;
	if (core->cvtWindowToDev(wx, wy, &pg, &x, &y)) 
	{
		core->setSelection(pg, wx, wy, wx, wy);
		dragging = true;
  }
}
