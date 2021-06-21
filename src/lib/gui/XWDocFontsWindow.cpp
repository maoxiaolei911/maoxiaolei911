/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWObject.h"
#include "XWDict.h"
#include "XWCoreFont.h"
#include "XWAnnot.h"
#include "XWPage.h"
#include "XWDoc.h"
#include "XWDocFontsWindow.h"

XWDocFontsWindow::XWDocFontsWindow(XWDoc * docA, QWidget * parent)
	:QDialog(parent),
	 doc(docA)
{
	row = 0;
	pg = 0;
	setWindowTitle(tr("Document Font information"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
  table = new QTableWidget(this);
  table->setColumnCount(5);
  QStringList labels;
  labels << tr("Page") << tr("Name") << tr("Type") << tr("Embedded") << tr("Unicode");
  table->setHorizontalHeaderLabels(labels);
  
  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(table);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);
 scanFonts();
 int w = table->columnWidth(0);
  w += table->columnWidth(1);
  w += table->columnWidth(2);
  w += table->columnWidth(3);
  w += table->columnWidth(4);
  w += 70;
  
  resize(w, 320);
}

void XWDocFontsWindow::scanFonts()
{
	if (!doc)
		return ;
	
	XWObject obj1, obj2;
	for (pg = 1; pg < doc->getNumPages(); pg++)
	{
		XWPage * page = doc->getCatalog()->getPage(pg);
		XWDict * resDict = page->getResourceDict();
		if (resDict)
			scanFonts(resDict);
			
		XWAnnots * annots = new XWAnnots(doc, page->getAnnots(&obj1));
    obj1.free();
    for (int i = 0; i < annots->getNumAnnots(); ++i)
    {
    	if (annots->getAnnot(i)->getAppearance(&obj1)->isStream())
    	{
    		obj1.streamGetDict()->lookup("Resources", &obj2);
				if (obj2.isDict()) 
	  			scanFonts(obj2.getDict());
				obj2.free();
    	}
    	
    	 obj1.free();
    }
    
    delete annots;
	}
}

void XWDocFontsWindow::scanFonts(XWDict *resDict)
{
	XWObject obj1, obj2, xObjDict, xObj, resObj;
	XWCoreFontDict * fontDict = 0;
	resDict->lookupNF("Font", &obj1);
	if (obj1.isRef())
	{
		obj1.fetch(doc->getXRef(), &obj2);
		if (obj2.isDict())
		{
			ObjRef r = obj1.getRef();
			fontDict = new XWCoreFontDict(doc->getXRef(), &r, obj2.getDict());
		}				
		obj2.free();
	}
	else if (obj1.isDict())
		fontDict = new XWCoreFontDict(doc->getXRef(), NULL, obj1.getDict());
				
	if (fontDict)
	{
		for (int i = 0; i < fontDict->getNumFonts(); ++i)
		{
			XWCoreFont * font = fontDict->getFont(i);
			if (font)
				scanFont(font);
		}
				
		delete fontDict;
	}
			
	obj1.free();
	
	resDict->lookup("XObject", &xObjDict);
  if (xObjDict.isDict()) 
  {  
    for (int i = 0; i < xObjDict.dictGetLength(); ++i) 
   	{
      xObjDict.dictGetVal(i, &xObj);
      if (xObj.isStream()) 
      {
				xObj.streamGetDict()->lookup("Resources", &resObj);
				if (resObj.isDict()) 
				{
	  			scanFonts(resObj.getDict());
				}
				resObj.free();
      }
      xObj.free();
   }
  }
  xObjDict.free();
}

void XWDocFontsWindow::scanFont(XWCoreFont * font)
{
	ObjRef fontRef = *font->getID();
	XWString * name = font->getName();
	bool emb = true;
	ObjRef embRef;
	if (font->getType() != fontType3)
		emb = font->getEmbeddedFontID(&embRef);
		
	XWObject fontObj, toUnicodeObj;
	bool hasToUnicode = false;
  if (doc->getXRef()->fetch(fontRef.num, fontRef.gen, &fontObj, 0)->isDict()) 
  {
    hasToUnicode = fontObj.dictLookup("ToUnicode", &toUnicodeObj)->isStream();
    toUnicodeObj.free();
  }
  fontObj.free();
  
  table->insertRow(row);
	QTableWidgetItem *item = new QTableWidgetItem(tr("%1").arg(pg));
	table->setItem(row, 0, item);
	QString tmp;
	if (name)
		tmp = name->toQString();
	else
		tmp = tr("None");
						
	item = new QTableWidgetItem(tmp);
	table->setItem(row, 1, item);
	
	switch (font->getType())
	{
    case fontType1:
    	tmp = "Type 1";
			break;
								
    case fontType1C:
    	tmp = "Type 1C";
			break;
								
    case fontType1COT:
    	tmp = "Type 1C (OT)";
			break;
								
    case fontType3:
    	tmp = "Type 3";
			break;
								
    case fontTrueType:
    	tmp = "TrueType";
			break;
								
    case fontTrueTypeOT:
    	tmp = "TrueType (OT)";
			break;
	  
    case fontCIDType0:
    	tmp = "CID Type 0";
			break;
								
    case fontCIDType0C:
    	tmp = "CID Type 0C";
			break;
								
    case fontCIDType0COT:
    	tmp = "CID Type 0C (OT)";
			break;
								
    case fontCIDType2:
    	tmp = "CID TrueType";
			break;
								
    case fontCIDType2OT:
    	tmp = "CID TrueType (OT)";
			break;
			
		default:
			tmp = tr("Unknown");
			break;
	}
						
	item = new QTableWidgetItem(tmp);
	table->setItem(row, 2, item);
	
	if (emb)
		tmp = "Yes";
	else
		tmp = "No";
		
	item = new QTableWidgetItem(tmp);
	table->setItem(row, 3, item);
	
	if (hasToUnicode)
		tmp = "Yes";
	else
		tmp = "No";
		
	item = new QTableWidgetItem(tmp);
	table->setItem(row, 4, item);
	table->resizeColumnToContents(1);
	row++;
}