/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTINFOWINDOW_H
#define XWFONTINFOWINDOW_H

#include <QDialog>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include "XWGlobal.h"

class XWCharacterWidget;
class QScrollArea;
class QLabel;
class QComboBox;
class QDoubleSpinBox;
class QCheckBox;

class XW_GUI_EXPORT XWFontInfoWindow : public QDialog
{
	Q_OBJECT

public:
	XWFontInfoWindow(QWidget * parent = 0);

private slots:
	void displayFont();
		
private:
	XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
	QLabel * mapsLabel;
	QComboBox * mapsCombo;	
	QLabel * fontSizeLabel;
	QDoubleSpinBox * fontSizeBox;	
	QLabel * efacterLabel;
	QDoubleSpinBox * efacterBox; 
	QLabel * slantLabel;
	QDoubleSpinBox * slantBox; 
	QLabel * boldfLabel;
	QDoubleSpinBox * boldfBox; 
	QCheckBox * boldBox;
	QCheckBox * rotateBox;
	
	QLabel * typeLabel;
	QLabel * familyLabel;
	QLabel * styleLabel;
	QLabel * numGlyphsLabel;
	QLabel * codingSchemeLabel;
	QLabel * peLabel;
	QCheckBox * scalableBox;
	QCheckBox * fixedWidthBox;
	QCheckBox * sfntBox;
	QCheckBox * cidBox;
	QCheckBox * glyphNameBox;
	QCheckBox * psnameBox;
	QCheckBox * horizontalBox;
	QCheckBox * verticalBox;
	QCheckBox * kerningBox;
	
	int maxFreeType;
	int maxPK;
	int maxVF;
	int maxOVF;
};

#endif // XWRECENTFILEWINDOW_H
