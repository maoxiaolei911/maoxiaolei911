/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWLATEXWIZARD_H
#define XWLATEXWIZARD_H

#include <QWizard>
#include <QTextStream>

class QLabel;
class QLineEdit;
class QRadioButton;
class QPushButton;
class QCheckBox;
class QComboBox;
class QScrollArea;
class QDoubleSpinBox;
class XWCharacterWidget;

class XWMyDocFont
{
public:
   XWMyDocFont();

   void setTeXFont();

   void writeStream(QTextStream & stm);

public:
  QString fontFile;
  QString family;
  QString series;
  QString shape;
  QString sizes;
  int index,pid, eid,wmode,stemv;
  double size,efacter,slant;
};

class XWMyDocument : public QObject
{
  Q_OBJECT

public:
  XWMyDocument(QObject * parent = 0);

  void createClass();
  void createDoc();
  void createFormat(); 

  void setEditorFont();

public:
  QString path;

  int docClass;
  QString fileName, title,author,subtitle,institute,keywords,
          signature,address,opening,closing,ps,encl,location,
          telephone,name,cc,letterSec;
  
  QString className;
  bool useOldClass, useCJK;
  int orientation;
  QString lineSkip,fontSize, numberFile, sectionFile, dateFile,mathFile;

  bool useOldFormat;
  QString formatName,family, boldFamily, italicFamily, hyphen;
  XWMyDocFont mediumNormal;
  XWMyDocFont mediumSlanted;
  XWMyDocFont mediumItalic;
  XWMyDocFont mediumCapsSmallCaps;
  XWMyDocFont boldNormal;
  XWMyDocFont boldExtendedNormal;
  XWMyDocFont boldExtendedItalic;
  XWMyDocFont boldExtendedSlanted;
  XWMyDocFont boldExtendedCapsSmallCaps;
  XWMyDocFont mediumUltraItalic;
};

class XWLaTeXWizard : public QWizard
{
  Q_OBJECT

public:

  enum {Page_FileName = 0, 
        Page_DocClass, 
        Page_Book, 
        Page_Letter, 
        Page_Beamer,  
        Page_ClassList, 
        Page_ClassParam, 
        Page_Format,
        Page_FormatList, 
        Page_MediumNormal,
        Page_MediumSlanted, 
        Page_MediumItalic,
        Page_MediumCapsSmallCaps,
        Page_BoldNormal,
        Page_BoldExtendedNormal,
        Page_BoldExtendedItalic,
        Page_BoldExtendedSlanted,
        Page_BoldExtendedCapsSmallCaps,
        Page_MediumUltraItalic};

  XWLaTeXWizard(QWidget *parent = 0);

  void accept();

signals:
  void fileNameChanged(const QString & fn);

private:
  XWMyDocument doc;
};

class XWLaTeXFileNamePage : public QWizardPage
{
  Q_OBJECT

public:
  XWLaTeXFileNamePage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
  void open();

private:
  XWMyDocument * doc;
  QLabel * topLabel;
  QLineEdit * fileName;
  QPushButton * browse;
};

class XWLaTeXDocClassPage : public QWizardPage
{
  Q_OBJECT

public:
  XWLaTeXDocClassPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private:
  XWMyDocument * doc;

  QRadioButton * artical;
  QRadioButton * book;
  QRadioButton * report;
  QRadioButton * letter;
  QRadioButton * beamer;
  QRadioButton * listClasses;
  QRadioButton * newClass;
};

class XWLaTeXBookPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXBookPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private:
  XWMyDocument * doc;

  QLineEdit * title;
  QLineEdit * author;

  QCheckBox * useCJK;
};

class XWLaTeXLetterPage : public QWizardPage
{
  Q_OBJECT

public:
  XWLaTeXLetterPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private:
  XWMyDocument * doc;
  QLineEdit * signature;
  QLineEdit * address;
  QLineEdit * opening;
  QLineEdit * closing;
  QLineEdit * ps;
  QLineEdit * encl;
  QLineEdit * cc;

  QCheckBox * useCJK;
};

class XWLaTeXBeamerPage : public QWizardPage
{
  Q_OBJECT

public:
  XWLaTeXBeamerPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private:
  XWMyDocument * doc;

  QLineEdit * title;
  QLineEdit * subtitle;

  QLineEdit * author;  
  QLineEdit * institute;

  QCheckBox * useCJK;
};

class XWLaTeXClassListPage : public QWizardPage
{
  Q_OBJECT

public:
  XWLaTeXClassListPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private:
  XWMyDocument * doc;

  QComboBox * list;
};

class XWLaTeXClassParamPage : public QWizardPage
{
  Q_OBJECT

public:
  XWLaTeXClassParamPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private:
  XWMyDocument * doc;

  QLineEdit * className;
  QComboBox * lineSkip;
  QComboBox * orientation;
  QComboBox * fontSize;

  QComboBox * numberFile;
  QComboBox * dateFile;
  QComboBox * sectionFile;
  QComboBox * mathFile;

  QCheckBox * useCJK;
};

class XWLaTeXFormatPage : public QWizardPage
{
  Q_OBJECT

public:
  XWLaTeXFormatPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private:
  XWMyDocument * doc;

  QRadioButton * list;
  QRadioButton * newFormat;
};

class XWLaTeXFormatListPage : public QWizardPage
{
  Q_OBJECT

public:
  XWLaTeXFormatListPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private:
  XWMyDocument * doc;

  QComboBox * list;
};

class XWLaTeXMediumNormalPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXMediumNormalPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
  void changeFile();
	void displayFont();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
};

class XWLaTeXMediumSlantPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXMediumSlantPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
	void displayFont();
  void changeFile();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
  QLabel * slantLabel;
	QDoubleSpinBox * slantBox; 
};

class XWLaTeXMediumItalicPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXMediumItalicPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
	void displayFont();
  void changeFile();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
};

class XWLaTeXMediumCapsSmallCapsPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXMediumCapsSmallCapsPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
	void displayFont();
  void changeFile();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
};

class XWLaTeXBoldNormalPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXBoldNormalPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
	void displayFont();
  void changeFile();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
};

class XWLaTeXBoldExtendedNormalPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXBoldExtendedNormalPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
	void displayFont();
  void changeFile();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
  QLabel * efacterLabel;
	QDoubleSpinBox * efacterBox; 
};

class XWLaTeXBoldExtendedItalicPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXBoldExtendedItalicPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
	void displayFont();
  void changeFile();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
  QLabel * efacterLabel;
	QDoubleSpinBox * efacterBox; 
  QLabel * slantLabel;
	QDoubleSpinBox * slantBox; 
};

class XWLaTeXBoldExtendedSlantedPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXBoldExtendedSlantedPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
  void changeFile();
	void displayFont();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
  QLabel * efacterLabel;
	QDoubleSpinBox * efacterBox; 
  QLabel * slantLabel;
	QDoubleSpinBox * slantBox; 
};

class XWLaTeXBoldExtendedCapsSmallCapsPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXBoldExtendedCapsSmallCapsPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
  void changeFile();
	void displayFont();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
  QLabel * efacterLabel;
	QDoubleSpinBox * efacterBox; 
};

class XWLaTeXMediumUltraItalicPage : public QWizardPage
{
   Q_OBJECT

public:
  XWLaTeXMediumUltraItalicPage(XWMyDocument * docA, QWidget *parent = 0);

  int nextId() const;

private slots:
	void displayFont();
  void changeFile();

private:
  XWMyDocument * doc;

  XWCharacterWidget *characterWidget;
	QScrollArea *scrollArea;
	
	QLabel * fontFileLabel;
	QComboBox * fontFileCombo;
	QLabel * indexLabel;
	QComboBox * indexCombo;
  QLabel * efacterLabel;
	QDoubleSpinBox * efacterBox; 
  QLabel * slantLabel;
	QDoubleSpinBox * slantBox; 
};

#endif //XWLATEXWIZARD_H
