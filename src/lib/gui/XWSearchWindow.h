/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSEARCHWINDOW_H
#define XWSEARCHWINDOW_H

#include <QDialog>
#include <QString>

class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;
class XWGuiCore;

class XWSearchWindow : public QDialog
{
	Q_OBJECT
	
public:       
    XWSearchWindow(XWGuiCore * coreA, QWidget *parent = 0);
    
private slots:
	void find();
	void findNext();
            
private:
	XWGuiCore * core;
    QLabel    * searchTextLabel;
    QLineEdit * searchText;
    
    QCheckBox * caseSensitive;
    QCheckBox * backward;
    QCheckBox * wholeWord;
    QCheckBox * allPage;
    
    QPushButton * findButton;
    QPushButton * nextButton;
    QPushButton * closeButton;
};

#endif // XWSEARCHWINDOW_H

