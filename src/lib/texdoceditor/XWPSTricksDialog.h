/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWPSTRICKSDIALOG_H
#define XWPSTRICKSDIALOG_H

#include <QDialog>
#include "XWTeXDocumentType.h"

class QListWidget;
class QListWidgetItem;


class XW_TEXDOCUMENT_EXPORT XWPSTricksObjectDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksObjectDialog(QWidget * parent = 0);

  QString getObject();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksTextBoxDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksTextBoxDialog(QWidget * parent = 0);

  QString getBox();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksNodeDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksNodeDialog(QWidget * parent = 0);

  QString getNode();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksConnectionDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksConnectionDialog(QWidget * parent = 0);

  QString getConnection();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksLabelDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksLabelDialog(QWidget * parent = 0);

  QString getLabel();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksTreeNodeDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksTreeNodeDialog(QWidget * parent = 0);

  QString getNode();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksPutDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksPutDialog(QWidget * parent = 0);

  QString getPut();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksRepetionDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksRepetionDialog(QWidget * parent = 0);

  QString getRepetion();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpCoordSystemDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpCoordSystemDialog(QWidget * parent = 0);

  QString getCoordSystem();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpLineStyleDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpLineStyleDialog(QWidget * parent = 0);

  QString getStyle();

private:
	QListWidget * list;
};


class XW_TEXDOCUMENT_EXPORT XWPSTricksOpFillStyleDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpFillStyleDialog(QWidget * parent = 0);

  QString getStyle();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpArrowleDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpArrowleDialog(QWidget * parent = 0);

  QString getArrow();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpArcDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpArcDialog(QWidget * parent = 0);

  QString getArc();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpDotDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpDotDialog(QWidget * parent = 0);

  QString getDot();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpGridDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpGridDialog(QWidget * parent = 0);

  QString getGrid();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpMiscDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpMiscDialog(QWidget * parent = 0);

  QString getMisc();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpPlotDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpPlotDialog(QWidget * parent = 0);

  QString getPlot();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpNodeDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpNodeDialog(QWidget * parent = 0);

  QString getNodeOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpTreeDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpTreeDialog(QWidget * parent = 0);

  QString getTreeOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksOpMatrixDialog : public QDialog
{
  Q_OBJECT

public:
	XWPSTricksOpMatrixDialog(QWidget * parent = 0);

  QString getMatrixOption();

private:
	QListWidget * list;
};

#endif //XWPSTRICKSDIALOG_H
