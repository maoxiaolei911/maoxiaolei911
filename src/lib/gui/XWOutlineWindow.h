/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWOUTLINEWINDOW_H
#define XWOUTLINEWINDOW_H

#include <QTreeWidget>
#include "XWGlobal.h"

class QTreeWidgetItem;
class XWList;
class XWOutlineItem;
class XWLinkAction;
class XWGuiCore;
class XWUnicodeMap;

struct OutlineItemAction
{
	XWLinkAction * action;
};

Q_DECLARE_METATYPE(OutlineItemAction)

class XW_GUI_EXPORT XWOutlineWindow : public QTreeWidget
{
	Q_OBJECT
	
public:       
    XWOutlineWindow(XWGuiCore * coreA, QWidget *parent = 0);
    ~XWOutlineWindow();
    
    void setup();
    
private slots:
    void gotoSelection(QTreeWidgetItem * item, int column);
    
private:
    QTreeWidgetItem * createItem(XWOutlineItem * item, QTreeWidgetItem * parent = 0);
    
    void setupOutlineItem(QTreeWidgetItem * parentitem, XWList * items);
        
private:
    XWGuiCore * core;
};

#endif // XWOUTLINEWINDOW_H

