/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZDIALOG_H
#define XWTIKZDIALOG_H

#include <QDialog>
#include "XWTeXDocumentType.h"

class QListWidget;
class QListWidgetItem;

class XW_TEXDOCUMENT_EXPORT XWTikzPathActionDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzPathActionDialog(QWidget * parent = 0);

  QString getAction();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzPathOperationDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzPathOperationDialog(QWidget * parent = 0);

  QString getOperation();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzGeneralStyleDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzGeneralStyleDialog(QWidget * parent = 0);

  QString getStyle();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzShapeStyleDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzShapeStyleDialog(QWidget * parent = 0);

  QString getStyle();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzGateStyleDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzGateStyleDialog(QWidget * parent = 0);

  QString getStyle();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzExecuteDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzExecuteDialog(QWidget * parent = 0);

  QString getExecute();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzTransformDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzTransformDialog(QWidget * parent = 0);

  QString getTransform();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzLineDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzLineDialog(QWidget * parent = 0);

  QString getLine();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzFillDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzFillDialog(QWidget * parent = 0);

  QString getFill();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzPathDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzPathDialog(QWidget * parent = 0);

  QString getPathOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzFitDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzFitDialog(QWidget * parent = 0);

  QString getFit();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzToDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzToDialog(QWidget * parent = 0);

  QString getTo();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzThroughDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzThroughDialog(QWidget * parent = 0);

  QString getThrough();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzDecorateDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzDecorateDialog(QWidget * parent = 0);

  QString getDecorate();

private:
	QListWidget * list;
};


class XW_TEXDOCUMENT_EXPORT XWTikzShadowDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzShadowDialog(QWidget * parent = 0);

  QString getShadow();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzBackgroundDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzBackgroundDialog(QWidget * parent = 0);

  QString getBackground();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzNodeDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzNodeDialog(QWidget * parent = 0);

  QString getNodeOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzNodeShapeDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzNodeShapeDialog(QWidget * parent = 0);

  QString getShape();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzShapeDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzShapeDialog(QWidget * parent = 0);

  QString getShapeOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzTreeDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzTreeDialog(QWidget * parent = 0);

  QString getTreeOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzMatrixDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzMatrixDialog(QWidget * parent = 0);

  QString getMatrixOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzCalendarDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzCalendarDialog(QWidget * parent = 0);

  QString getCalendarOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzMindmapDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzMindmapDialog(QWidget * parent = 0);

  QString getMindmapOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzChainDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzChainDialog(QWidget * parent = 0);

  QString getChainOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzSpyDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzSpyDialog(QWidget * parent = 0);

  QString getSpyOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzTurtleDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzTurtleDialog(QWidget * parent = 0);

  QString getTurtleOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzPlotDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzPlotDialog(QWidget * parent = 0);

  QString getPlotOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzLSystemDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzLSystemDialog(QWidget * parent = 0);

  QString getLSystemOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzAutomataDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzAutomataDialog(QWidget * parent = 0);

  QString getAutomataOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzPetrinetDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzPetrinetDialog(QWidget * parent = 0);

  QString getPetrinetOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzCircuitDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzCircuitDialog(QWidget * parent = 0);

  QString getCircuitOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzCircuitLogicDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzCircuitLogicDialog(QWidget * parent = 0);

  QString getCircuitLogicOption();

private:
	QListWidget * list;
};

class XW_TEXDOCUMENT_EXPORT XWTikzCircuitElectricalDialog : public QDialog
{
  Q_OBJECT

public:
	XWTikzCircuitElectricalDialog(QWidget * parent = 0);

  QString getCircuitElectricalOption();

private:
	QListWidget * list;
};

#endif //XWTIKZDIALOG_H
