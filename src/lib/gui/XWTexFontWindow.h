/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXFONTWINDOW_H
#define XWTEXFONTWINDOW_H

#include <QDialog>
#include <QString>
#include "XWGlobal.h"

class XWCharacterWidget;
class QScrollArea;
class QLabel;
class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;
class QListWidget;
class QPushButton;
class QListWidgetItem;

class XW_GUI_EXPORT XWTexFontWindow : public QDialog
{
	Q_OBJECT

public:
	XWTexFontWindow(QWidget * parent = 0);

private slots:
	void add();
	
	void displayFont();
	
	void modifyList(QListWidgetItem * item);
	
	void setDisplayed() {displayed = false;}
	
	void updateIndexAndMaps();
		
private:
	XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * texnameLabel;
	QLineEdit * texnameEdit;
	
	QLabel * descriptionLabel;
	QLineEdit * descriptionEdit;
	
	QLabel * subfontLabel;
	QComboBox * subfontCombo;
	QLabel * subfontIDLabel;
	QComboBox * subfontIDCombo;
	
	QLabel * inencLabel;
	QLineEdit * inencEdit;
	QLabel * outencLabel;
	QLineEdit * outencEdit;
	
	QLabel * tounicodeLabel;
	QLineEdit * tounicodeEdit;
	
	QLabel * collectionLabel;
	QLineEdit * collectionEdit;
	QLabel * vfLabel;
	QLineEdit * vfEdit;	
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;	
	QLabel * mapsLabel;
	QComboBox * mapsCombo;		
	QLabel * wmodeLabel;
	QComboBox * wmodeCombo;
	
	QLabel * fontSizeLabel;
	QDoubleSpinBox * fontSizeBox;	
	QLabel * efacterLabel;
	QDoubleSpinBox * efacterBox; 
	QLabel * slantLabel;
	QDoubleSpinBox * slantBox; 
	QLabel * boldfLabel;
	QDoubleSpinBox * boldfBox; 
	
	QLabel * capheightLabel;
	QDoubleSpinBox * capheightBox; 
	QLabel * italicangleLabel;
	QDoubleSpinBox * italicangleBox; 
	QLabel * stemvLabel;
	QSpinBox * stemvBox; 
	
	QLabel * yoffsetLabel;
	QDoubleSpinBox * yoffsetBox; 
	
	QCheckBox * smallcapsBox;
	QCheckBox * boldBox;	
	QCheckBox * rotateBox;	
	QCheckBox * italicBox;
	QCheckBox * noembedBox;
	QCheckBox * useVFBox;
	
	QPushButton * displayButton;
	QPushButton * addButton;
		
	QListWidget * texFonts;
	
	int maxFreeType;
	int maxPK;
	int maxVF;
	int maxOVF;
	
	bool displayed;
};

#endif // XWTEXFONTWINDOW_H
