/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QTextStream>
#include "XWList.h"
#include "XWUnicodeMap.h"
#include "XWFontSetting.h"
#include "XWDoc.h"
#include "XWLink.h"
#include "XWOutline.h"
#include "XWGuiCore.h"
#include "XWOutlineWindow.h"

XWOutlineWindow::XWOutlineWindow(XWGuiCore * coreA, QWidget *parent)
    :QTreeWidget(parent), core(coreA)
{
	setHeaderHidden(true);
	setAnimated(true);
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), 
                  this, SLOT(gotoSelection(QTreeWidgetItem *, int)));
    setColumnCount(1);
}

XWOutlineWindow::~XWOutlineWindow()
{
}

void XWOutlineWindow::setup()
{
	clear();
	if (!(core->getDoc()) || !(core->getDoc()->getOutline()))
		return ;
		
	XWList *items = core->getDoc()->getOutline()->getItems();
	if (!items)
		return ;
		
    for (int i = 0; i < items->getLength(); ++i)
    {
        XWOutlineItem * item = (XWOutlineItem *)items->get(i);
        if (item)
        {  
            QTreeWidgetItem * treeitem = createItem(item, 0);
            item->open();
            XWList * kids = item->getKids();
        		if (kids)
            		setupOutlineItem(treeitem, kids);
            if (item->isOpen())
        			expandItem(treeitem);
        }
    }
}

void XWOutlineWindow::gotoSelection(QTreeWidgetItem * item, int column)
{
    if (item && column == 0)
    {
        QVariant var = item->data(0, Qt::UserRole);
        OutlineItemAction a = var.value<OutlineItemAction>();
        if (a.action)
            core->doAction(a.action);
    }
}

QTreeWidgetItem * XWOutlineWindow::createItem(XWOutlineItem * item, QTreeWidgetItem * parent)
{
    QTreeWidgetItem * treeitem = 0;
    if (!parent)
        treeitem = new QTreeWidgetItem(this);
    else
        treeitem = new QTreeWidgetItem(parent);
    
    QString txt;
    for (int i = 0; i < item->getTitleLength(); ++i)
    {
       txt.append(QChar(item->getTitle()[i]));
    }
    
    treeitem->setText(0, txt);
    treeitem->setFlags(treeitem->flags() & ~Qt::ItemIsEditable);
    treeitem->setFlags(treeitem->flags() & ~Qt::ItemIsDragEnabled);
    treeitem->setFlags(treeitem->flags() & ~Qt::ItemIsDropEnabled);
    treeitem->setFlags(treeitem->flags() & ~Qt::ItemIsUserCheckable);
            
    OutlineItemAction a;
    a.action = item->getAction();
    QVariant var;
    var.setValue(a);
        
    treeitem->setData(0, Qt::UserRole, var);
    return treeitem;
}

void XWOutlineWindow::setupOutlineItem(QTreeWidgetItem * parentitem, 
                                       XWList * items)
{
    for (int i = 0; i < items->getLength(); ++i)
    {
        XWOutlineItem * item = (XWOutlineItem *)items->get(i);            
        QTreeWidgetItem * treeitem = createItem(item, parentitem);
        item->open();
        XWList * kids = item->getKids();
        if (kids)
         	setupOutlineItem(treeitem, kids);
        if (item->isOpen())
        	expandItem(treeitem);
    }
}

