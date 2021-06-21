/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWPSTricksDialog.h"

XWPSTricksObjectDialog::XWPSTricksObjectDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks object"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("psline"), list);
  item->setData(Qt::ToolTipRole, QString("\\psline*[par](0,0)(1,1)...(n,n)"));

  item = new QListWidgetItem(tr("qline"), list);
  item->setData(Qt::ToolTipRole, QString("\\qline[par](0,0)(1,1)"));

  item = new QListWidgetItem(tr("pspolygon"), list);
  item->setData(Qt::ToolTipRole, QString("\\pspolygon*[par](0,0)(1,1)...(n,n)"));

  item = new QListWidgetItem(tr("psframe"), list);
  item->setData(Qt::ToolTipRole, QString("\\psframe*[par](0,0)(1,1)"));

  item = new QListWidgetItem(tr("psdiamond"), list);
  item->setData(Qt::ToolTipRole, QString("\\psdiamond*[par](0,0)(1,1)"));

  item = new QListWidgetItem(tr("pstriangle"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstriangle*[par](0,0)(1,1)"));

  item = new QListWidgetItem(tr("pscircle"), list);
  item->setData(Qt::ToolTipRole, QString("\\pscircle*[par](0,0){1}"));

  item = new QListWidgetItem(tr("qdisk"), list);
  item->setData(Qt::ToolTipRole, QString("\\qdisk[par](0,0){1}"));

  item = new QListWidgetItem(tr("pswedge"), list);
  item->setData(Qt::ToolTipRole, QString("\\pswedge*[par](0,0){1}{30}{60}"));

  item = new QListWidgetItem(tr("psellipse"), list);
  item->setData(Qt::ToolTipRole, QString("\\psellipse*[par](0,0)(1,1)"));

  item = new QListWidgetItem(tr("psarc"), list);
  item->setData(Qt::ToolTipRole, QString("\\psarc*[par]{<->}(0,0){1}{45}{60}"));

  item = new QListWidgetItem(tr("psarcn"), list);
  item->setData(Qt::ToolTipRole, QString("\\psarcn*[par]{<->}(0,0){1}{45}{60}"));

  item = new QListWidgetItem(tr("psellipticarc"), list);
  item->setData(Qt::ToolTipRole, QString("\\psellipticarc*[par]{<->}(0,0)(1,1){45}{60}"));

  item = new QListWidgetItem(tr("psellipticarcn"), list);
  item->setData(Qt::ToolTipRole, QString("\\psellipticarcn*[par]{<->}(0,0)(1,1){45}{60}"));

  item = new QListWidgetItem(tr("psbezier"), list);
  item->setData(Qt::ToolTipRole, QString("\\psbezier*[par]{<->}(0,0)(1,1)(2,2)(3,3)"));

  item = new QListWidgetItem(tr("parabola"), list);
  item->setData(Qt::ToolTipRole, QString("\\parabola*[par]{<->}(0,0)(1,1)"));

  item = new QListWidgetItem(tr("pscurve"), list);
  item->setData(Qt::ToolTipRole, QString("\\pscurve*[par]{<->}(0,0)(1,1)...(n,n)"));

  item = new QListWidgetItem(tr("psecurve"), list);
  item->setData(Qt::ToolTipRole, QString("\\psecurve*[par]{<->}(0,0)(1,1)...(n,n)"));

  item = new QListWidgetItem(tr("psccurve"), list);
  item->setData(Qt::ToolTipRole, QString("\\psccurve*[par]{<->}(0,0)(1,1)...(n,n)"));

  item = new QListWidgetItem(tr("psdot"), list);
  item->setData(Qt::ToolTipRole, QString("\\psdot*[par](0,0)"));

  item = new QListWidgetItem(tr("psdots"), list);
  item->setData(Qt::ToolTipRole, QString("\\psdots*[par](0,0)(1,1)...(n,n)"));

  item = new QListWidgetItem(tr("psgrid"), list);
  item->setData(Qt::ToolTipRole, QString("\\psgrid[par](0,0)(1,1)(3,3)"));

  item = new QListWidgetItem(tr("fileplot"), list);
  item->setData(Qt::ToolTipRole, QString("\\fileplot*[par]{file}"));

  item = new QListWidgetItem(tr("dataplot"), list);
  item->setData(Qt::ToolTipRole, QString("\\dataplot*[par]{commands}"));

  item = new QListWidgetItem(tr("savedata"), list);
  item->setData(Qt::ToolTipRole, QString("\\savedata{command}[data]"));

  item = new QListWidgetItem(tr("readdata"), list);
  item->setData(Qt::ToolTipRole, QString("\\readdata{command}{file}"));

  item = new QListWidgetItem(tr("listplot"), list);
  item->setData(Qt::ToolTipRole, QString("\\listplot*[par]{list}"));

  item = new QListWidgetItem(tr("psplot"), list);
  item->setData(Qt::ToolTipRole, QString("\\psplot*[par]{xmin}{xmax}{function}"));

  item = new QListWidgetItem(tr("parametricplot"), list);
  item->setData(Qt::ToolTipRole, QString("\\parametricplot*[par]{tmin}{tmax}{function}"));

  item = new QListWidgetItem(tr("psaxes"), list);
  item->setData(Qt::ToolTipRole, QString("\\psaxes*[par]{<->}(0,0)(1,1)(2,2)"));

  item = new QListWidgetItem(tr("pscoil"), list);
  item->setData(Qt::ToolTipRole, QString("\\pscoil*[par]{<->}(0,0)(1,1)"));

  item = new QListWidgetItem(tr("psCoil"), list);
  item->setData(Qt::ToolTipRole, QString("\\psCoil*[par]{30}{45}"));

  item = new QListWidgetItem(tr("pszigzag"), list);
  item->setData(Qt::ToolTipRole, QString("\\pszigzag*[par]{<->}(0,0)(1,1)"));

  item = new QListWidgetItem(tr("pstThreeDCoor"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDCoor[IIIDticks,IIIDticksize=0.05]"));

  item = new QListWidgetItem(tr("pstThreeDPlaneGrid"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDPlaneGrid[planeGrid=xz](0,0)(4,4)"));

  item = new QListWidgetItem(tr("pstThreeDNode"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDNode[options](0,0,0){name}"));

  item = new QListWidgetItem(tr("pstThreeDDot"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDDot[options](0,0,0)"));

  item = new QListWidgetItem(tr("pstThreeDLine"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDLine[options][->](0,0,0)(1,1,1)"));

  item = new QListWidgetItem(tr("pstThreeDTriangle"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDTriangle[options](3,1,-2)(1,4,-1)(-2,2,0)"));

  item = new QListWidgetItem(tr("pstThreeDSquare"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDSquare[options](-2,2,3)(4,0,0)(0,1,0)"));

  item = new QListWidgetItem(tr("pstThreeDBox"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDBox[options](-1,1,2)(0,0,2)(2,0,0)(0,1,0)"));

  item = new QListWidgetItem(tr("psBox"), list);
  item->setData(Qt::ToolTipRole, QString("\\psBox[options](0,0,0){2}{4}{3}"));

  item = new QListWidgetItem(tr("pstThreeDEllipse"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDEllipse[options](0,0,0)(0,0,3)(3,0,0)"));

  item = new QListWidgetItem(tr("pstThreeDCircle"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDCircle[options](1.6,0.6,1.7)(0.8,0.4,0.8)(0.8,-0.8,-0.4)"));

  item = new QListWidgetItem(tr("pstIIIDCylinder"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstIIIDCylinder[options](0,0,0){2}{5}"));

  item = new QListWidgetItem(tr("psCylinder"), list);
  item->setData(Qt::ToolTipRole, QString("\\psCylinder[options](0,0,0){2}{5}"));

  item = new QListWidgetItem(tr("pstParaboloid"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstParaboloid[options]{5}{1}"));

  item = new QListWidgetItem(tr("pstThreeDSphere"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDSphere[options](1,-1,2){2}"));

  item = new QListWidgetItem(tr("psplotThreeD"), list);
  item->setData(Qt::ToolTipRole, QString("\\psplotThreeD[options](xmin,xmax)(yin,ymax){function}"));

  item = new QListWidgetItem(tr("parametricplotThreeD"), list);
  item->setData(Qt::ToolTipRole, QString("\\parametricplotThreeD[options](t1,t2)(u1,u2){function}"));

  item = new QListWidgetItem(tr("fileplotThreeD"), list);
  item->setData(Qt::ToolTipRole, QString("\\fileplotThreeD[options]{datafile}"));

  item = new QListWidgetItem(tr("dataplotThreeD"), list);
  item->setData(Qt::ToolTipRole, QString("\\dataplotThreeD[options]{data object}"));

  item = new QListWidgetItem(tr("listplotThreeD"), list);
  item->setData(Qt::ToolTipRole, QString("\\listplotThreeD[options]{data object}"));

  item = new QListWidgetItem(tr("pscustom"), list);
  item->setData(Qt::ToolTipRole, QString("\\pscustom[options]{commands}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksObjectDialog::getObject()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksTextBoxDialog::XWPSTricksTextBoxDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks text box"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("psframebox"), list);
  item->setData(Qt::ToolTipRole, QString("\\psframebox*[par]{stuff}"));

  item = new QListWidgetItem(tr("psdblframebox"), list);
  item->setData(Qt::ToolTipRole, QString("\\psdblframebox*[par]{stuff}"));

  item = new QListWidgetItem(tr("psshadowbox"), list);
  item->setData(Qt::ToolTipRole, QString("\\psshadowbox*[par]{stuff}"));

  item = new QListWidgetItem(tr("pscirclebox"), list);
  item->setData(Qt::ToolTipRole, QString("\\pscirclebox*[par]{stuff}"));

  item = new QListWidgetItem(tr("psovalbox"), list);
  item->setData(Qt::ToolTipRole, QString("\\psovalbox*[par]{stuff}"));

  item = new QListWidgetItem(tr("psdiabox"), list);
  item->setData(Qt::ToolTipRole, QString("\\psdiabox*[par]{stuff}"));

  item = new QListWidgetItem(tr("pstribox"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstribox*[par]{stuff}"));

  item = new QListWidgetItem(tr("clipbox"), list);
  item->setData(Qt::ToolTipRole, QString("\\clipbox[par]{stuff}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksTextBoxDialog::getBox()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksNodeDialog::XWPSTricksNodeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks node"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("rnode"), list);
  item->setData(Qt::ToolTipRole, QString("\\rnode[refpoint]{name}{stuff}"));

  item = new QListWidgetItem(tr("Rnode"), list);
  item->setData(Qt::ToolTipRole, QString("\\Rnode*[par]{name}{stuff}"));

  item = new QListWidgetItem(tr("pnode"), list);
  item->setData(Qt::ToolTipRole, QString("\\pnode(x,y){name}"));

  item = new QListWidgetItem(tr("cnode"), list);
  item->setData(Qt::ToolTipRole, QString("\\cnode*[par](x,y){radius}{name}"));

  item = new QListWidgetItem(tr("Cnode"), list);
  item->setData(Qt::ToolTipRole, QString("\\Cnode*[par](x,y){name}"));

  item = new QListWidgetItem(tr("circlenode"), list);
  item->setData(Qt::ToolTipRole, QString("\\circlenode*[par]{name}{stuff}"));

  item = new QListWidgetItem(tr("ovalnode"), list);
  item->setData(Qt::ToolTipRole, QString("\\ovalnode*[par]{name}{stuff}"));

  item = new QListWidgetItem(tr("dianode"), list);
  item->setData(Qt::ToolTipRole, QString("\\dianode*[par]{name}{stuff}"));

  item = new QListWidgetItem(tr("trinode"), list);
  item->setData(Qt::ToolTipRole, QString("\\trinode*[par]{name}{stuff}"));

  item = new QListWidgetItem(tr("dotnode"), list);
  item->setData(Qt::ToolTipRole, QString("\\dotnode*[par](x,y){name}"));

  item = new QListWidgetItem(tr("fnode"), list);
  item->setData(Qt::ToolTipRole, QString("\\fnode*[par](x,y){name}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksNodeDialog::getNode()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksConnectionDialog::XWPSTricksConnectionDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks connection"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("ncline"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncline*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("ncarc"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncarc*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("ncdiag"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncdiag*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("ncdiagg"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncdiagg*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("ncbar"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncbar*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("ncangle"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncangle*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("ncangles"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncangles*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("ncloop"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncloop*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("nccurve"), list);
  item->setData(Qt::ToolTipRole, QString("\\nccurve*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("nccircle"), list);
  item->setData(Qt::ToolTipRole, QString("\\nccircle*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("ncbox"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncbox*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("ncarcbox"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncarcbox*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("nccoil"), list);
  item->setData(Qt::ToolTipRole, QString("\\nccoil*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("nczigzag"), list);
  item->setData(Qt::ToolTipRole, QString("\\nczigzag*[par]{<->}{nodeA}{nodeB}"));

  item = new QListWidgetItem(tr("pcline"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcline*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pccurve"), list);
  item->setData(Qt::ToolTipRole, QString("\\pccurve*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pcarc"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcarc*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pcbar"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcbar*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pcdiag"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcdiag*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pcdiagg"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcdiagg*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pcangle"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcangle*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pcangles"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcangles*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pcloop"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcloop*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pcbox"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcbox*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pcarcbox"), list);
  item->setData(Qt::ToolTipRole, QString("\\pcarcbox*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pccoil"), list);
  item->setData(Qt::ToolTipRole, QString("\\pccoil*[par]{<->}(x1,y1)(x2,y2)"));

  item = new QListWidgetItem(tr("pczigzag"), list);
  item->setData(Qt::ToolTipRole, QString("\\pczigzag*[par]{<->}(x1,y1)(x2,y2)"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksConnectionDialog::getConnection()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksLabelDialog::XWPSTricksLabelDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks label"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("ncput"), list);
  item->setData(Qt::ToolTipRole, QString("\\ncput*[par]{stuff}"));

  item = new QListWidgetItem(tr("naput"), list);
  item->setData(Qt::ToolTipRole, QString("\\naput*[par]{stuff}"));

  item = new QListWidgetItem(tr("nbput"), list);
  item->setData(Qt::ToolTipRole, QString("\\nbput*[par]{stuff}"));

  item = new QListWidgetItem(tr("tvput"), list);
  item->setData(Qt::ToolTipRole, QString("\\tvput*[par]{stuff}"));

  item = new QListWidgetItem(tr("tlput"), list);
  item->setData(Qt::ToolTipRole, QString("\\tlput*[par]{stuff}"));

  item = new QListWidgetItem(tr("trput"), list);
  item->setData(Qt::ToolTipRole, QString("\\trput*[par]{stuff}"));

  item = new QListWidgetItem(tr("thput"), list);
  item->setData(Qt::ToolTipRole, QString("\\thput*[par]{stuff}"));

  item = new QListWidgetItem(tr("taput"), list);
  item->setData(Qt::ToolTipRole, QString("\\taput*[par]{stuff}"));

  item = new QListWidgetItem(tr("tbput"), list);
  item->setData(Qt::ToolTipRole, QString("\\tbput*[par]{stuff}"));

  item = new QListWidgetItem(tr("nput"), list);
  item->setData(Qt::ToolTipRole, QString("\\nput*[par]{stuff}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksLabelDialog::getLabel()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksTreeNodeDialog::XWPSTricksTreeNodeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks tree node"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("Tp"), list);
  item->setData(Qt::ToolTipRole, QString("\\Tp*[par]"));

  item = new QListWidgetItem(tr("Tc"), list);
  item->setData(Qt::ToolTipRole, QString("\\Tc*[par]{dim}"));

  item = new QListWidgetItem(tr("TC"), list);
  item->setData(Qt::ToolTipRole, QString("\\TC*[par]"));

  item = new QListWidgetItem(tr("Tf"), list);
  item->setData(Qt::ToolTipRole, QString("\\Tf*[par]"));

  item = new QListWidgetItem(tr("Tdot"), list);
  item->setData(Qt::ToolTipRole, QString("\\Tdot*[par]"));

  item = new QListWidgetItem(tr("Tr"), list);
  item->setData(Qt::ToolTipRole, QString("\\Tr*[par]{stuff}"));

  item = new QListWidgetItem(tr("TR"), list);
  item->setData(Qt::ToolTipRole, QString("\\TR*[par]{stuff}"));

  item = new QListWidgetItem(tr("Tcircle"), list);
  item->setData(Qt::ToolTipRole, QString("\\Tcircle*[par]{stuff}"));

  item = new QListWidgetItem(tr("TCircle"), list);
  item->setData(Qt::ToolTipRole, QString("\\TCircle*[par]{stuff}"));

  item = new QListWidgetItem(tr("Toval"), list);
  item->setData(Qt::ToolTipRole, QString("\\Toval*[par]{stuff}"));

  item = new QListWidgetItem(tr("Tdia"), list);
  item->setData(Qt::ToolTipRole, QString("\\Tdia*[par]{stuff}"));

  item = new QListWidgetItem(tr("Ttri"), list);
  item->setData(Qt::ToolTipRole, QString("\\Ttri*[par]{stuff}"));

  item = new QListWidgetItem(tr("Tn"), list);
  item->setData(Qt::ToolTipRole, QString("\\Tn"));

  item = new QListWidgetItem(tr("Tfan"), list);
  item->setData(Qt::ToolTipRole, QString("\\Tfan*[par]"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksTreeNodeDialog::getNode()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksPutDialog::XWPSTricksPutDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks put"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("rput"), list);
  item->setData(Qt::ToolTipRole, QString("\\rput*[refpoint]{rotation}(x,y){stuff}"));

  item = new QListWidgetItem(tr("uput"), list);
  item->setData(Qt::ToolTipRole, QString("\\uput*{labelsep}[refangle]{rotation}(x,y){stuff}"));

  item = new QListWidgetItem(tr("cput"), list);
  item->setData(Qt::ToolTipRole, QString("\\cput*[par]{angle}(x,y){stuff}"));

  item = new QListWidgetItem(tr("cnodeput"), list);
  item->setData(Qt::ToolTipRole, QString("\\cnodeput*[par]{angle}(x,y){name}{stuff}"));

  item = new QListWidgetItem(tr("pstThreeDPut"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstThreeDPut[options](x,y,z){stuff}"));

  item = new QListWidgetItem(tr("pstPlanePut"), list);
  item->setData(Qt::ToolTipRole, QString("\\pstPlanePut[options](x,y,z){object}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksPutDialog::getPut()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksRepetionDialog::XWPSTricksRepetionDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks repetion"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("multirput"), list);
  item->setData(Qt::ToolTipRole, QString("\\multirput*[refpoint]{angle}(x0,y0)(x1,y1){int}{stuff}"));

  item = new QListWidgetItem(tr("multips"), list);
  item->setData(Qt::ToolTipRole, QString("\\multips{angle}(x0,y0)(x1,y1){int}{graphics}"));

  item = new QListWidgetItem(tr("multido"), list);
  item->setData(Qt::ToolTipRole, QString("\\multido{variables}{repetions}{stuff}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksRepetionDialog::getRepetion()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpCoordSystemDialog::XWPSTricksOpCoordSystemDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks coordinate system option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("unit"), list);
  item->setData(Qt::ToolTipRole, QString("unit=1cm"));

  item = new QListWidgetItem(tr("xunit"), list);
  item->setData(Qt::ToolTipRole, QString("xunit=1cm"));

  item = new QListWidgetItem(tr("yunit"), list);
  item->setData(Qt::ToolTipRole, QString("yunit=1cm"));

  item = new QListWidgetItem(tr("runit"), list);
  item->setData(Qt::ToolTipRole, QString("runit=1cm"));

  item = new QListWidgetItem(tr("origin"), list);
  item->setData(Qt::ToolTipRole, QString("origin={x,y}"));

  item = new QListWidgetItem(tr("Ox"), list);
  item->setData(Qt::ToolTipRole, QString("Ox=-2"));

  item = new QListWidgetItem(tr("Oy"), list);
  item->setData(Qt::ToolTipRole, QString("Oy=1"));

  item = new QListWidgetItem(tr("Dx"), list);
  item->setData(Qt::ToolTipRole, QString("Dx=.5"));

  item = new QListWidgetItem(tr("Dy"), list);
  item->setData(Qt::ToolTipRole, QString("Dy=.5"));

  item = new QListWidgetItem(tr("dx"), list);
  item->setData(Qt::ToolTipRole, QString("dx=1"));

  item = new QListWidgetItem(tr("dy"), list);
  item->setData(Qt::ToolTipRole, QString("dy=1"));

  item = new QListWidgetItem(tr("labels"), list);
  item->setData(Qt::ToolTipRole, QString("labels=all"));

  item = new QListWidgetItem(tr("ticks"), list);
  item->setData(Qt::ToolTipRole, QString("ticks=all"));

  item = new QListWidgetItem(tr("tickstyle"), list);
  item->setData(Qt::ToolTipRole, QString("tickstyle=full"));

  item = new QListWidgetItem(tr("ticksize"), list);
  item->setData(Qt::ToolTipRole, QString("ticksize=3pt"));

  item = new QListWidgetItem(tr("axesstyle"), list);
  item->setData(Qt::ToolTipRole, QString("axesstyle=axes"));

  item = new QListWidgetItem(tr("xThreeDunit"), list);
  item->setData(Qt::ToolTipRole, QString("xThreeDunit=1"));

  item = new QListWidgetItem(tr("yThreeDunit"), list);
  item->setData(Qt::ToolTipRole, QString("yThreeDunit=1"));

  item = new QListWidgetItem(tr("zThreeDunit"), list);
  item->setData(Qt::ToolTipRole, QString("zThreeDunit=1"));

  item = new QListWidgetItem(tr("Alpha"), list);
  item->setData(Qt::ToolTipRole, QString("Alpha=45"));

  item = new QListWidgetItem(tr("Beta"), list);
  item->setData(Qt::ToolTipRole, QString("Beta=30"));

  item = new QListWidgetItem(tr("xMin"), list);
  item->setData(Qt::ToolTipRole, QString("xMin=-2"));

  item = new QListWidgetItem(tr("xMax"), list);
  item->setData(Qt::ToolTipRole, QString("xMax=2"));

  item = new QListWidgetItem(tr("yMin"), list);
  item->setData(Qt::ToolTipRole, QString("yMin=-2"));

  item = new QListWidgetItem(tr("yMax"), list);
  item->setData(Qt::ToolTipRole, QString("yMax=2"));

  item = new QListWidgetItem(tr("zMin"), list);
  item->setData(Qt::ToolTipRole, QString("zMin=-2"));

  item = new QListWidgetItem(tr("zMax"), list);
  item->setData(Qt::ToolTipRole, QString("zMax=2"));

  item = new QListWidgetItem(tr("nameX"), list);
  item->setData(Qt::ToolTipRole, QString("nameX=u"));

  item = new QListWidgetItem(tr("spotX"), list);
  item->setData(Qt::ToolTipRole, QString("spotX=90"));

  item = new QListWidgetItem(tr("nameY"), list);
  item->setData(Qt::ToolTipRole, QString("nameY=v"));

  item = new QListWidgetItem(tr("spotY"), list);
  item->setData(Qt::ToolTipRole, QString("spotY=0"));

  item = new QListWidgetItem(tr("nameZ"), list);
  item->setData(Qt::ToolTipRole, QString("nameZ=w"));

  item = new QListWidgetItem(tr("spotZ"), list);
  item->setData(Qt::ToolTipRole, QString("spotZ=90"));

  item = new QListWidgetItem(tr("IIIDticks"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDticks=false"));

  item = new QListWidgetItem(tr("IIIDlabels"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDlabels=false"));

  item = new QListWidgetItem(tr("Dz"), list);
  item->setData(Qt::ToolTipRole, QString("Dz=0.25"));

  item = new QListWidgetItem(tr("IIIDxTicksPlane"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDxTicksPlane=xy"));

  item = new QListWidgetItem(tr("IIIDyTicksPlane"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDyTicksPlane=yz"));

  item = new QListWidgetItem(tr("IIIDzTicksPlane"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDzTicksPlane=yz"));

  item = new QListWidgetItem(tr("IIIDticksize"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDticksize=0.1"));

  item = new QListWidgetItem(tr("IIIDxticksep"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDxticksep=-0.2"));

  item = new QListWidgetItem(tr("IIIDyticksep"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDyticksep=-0.2"));

  item = new QListWidgetItem(tr("IIIDzticksep"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDzticksep=0.2"));

  item = new QListWidgetItem(tr("RotX"), list);
  item->setData(Qt::ToolTipRole, QString("RotX=90"));

  item = new QListWidgetItem(tr("RotY"), list);
  item->setData(Qt::ToolTipRole, QString("RotY=90"));

  item = new QListWidgetItem(tr("RotZ"), list);
  item->setData(Qt::ToolTipRole, QString("RotZ=90"));

  item = new QListWidgetItem(tr("RotAngle"), list);
  item->setData(Qt::ToolTipRole, QString("RotAngle=30"));

  item = new QListWidgetItem(tr("xRotVec"), list);
  item->setData(Qt::ToolTipRole, QString("xRotVec=0"));

  item = new QListWidgetItem(tr("yRotVec"), list);
  item->setData(Qt::ToolTipRole, QString("yRotVec=0"));

  item = new QListWidgetItem(tr("zRotVec"), list);
  item->setData(Qt::ToolTipRole, QString("zRotVec=1"));

  item = new QListWidgetItem(tr("RotSequence"), list);
  item->setData(Qt::ToolTipRole, QString("RotSequence=zyx"));

  item = new QListWidgetItem(tr("RotSet"), list);
  item->setData(Qt::ToolTipRole, QString("RotSet=set"));

  item = new QListWidgetItem(tr("eulerRotation"), list);
  item->setData(Qt::ToolTipRole, QString("eulerRotation=false"));

  item = new QListWidgetItem(tr("IIIDOffset"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDOffset={(1,-2,1)}"));

  item = new QListWidgetItem(tr("zlabelFactor"), list);
  item->setData(Qt::ToolTipRole, QString("zlabelFactor=$\\cdot10^3$"));

  item = new QListWidgetItem(tr("SphericalCoor"), list);
  item->setData(Qt::ToolTipRole, QString("SphericalCoor=true"));

  item = new QListWidgetItem(tr("CylindricalCoor"), list);
  item->setData(Qt::ToolTipRole, QString("CylindricalCoor=true"));

  item = new QListWidgetItem(tr("CoorCheck"), list);
  item->setData(Qt::ToolTipRole, QString("CoorCheck=true"));

  item = new QListWidgetItem(tr("leftHanded"), list);
  item->setData(Qt::ToolTipRole, QString("leftHanded=true"));

  item = new QListWidgetItem(tr("comma"), list);
  item->setData(Qt::ToolTipRole, QString("comma=true"));

  item = new QListWidgetItem(tr("coorType"), list);
  item->setData(Qt::ToolTipRole, QString("coorType=1"));

  item = new QListWidgetItem(tr("drawing"), list);
  item->setData(Qt::ToolTipRole, QString("drawing=true"));

  item = new QListWidgetItem(tr("swapaxes"), list);
  item->setData(Qt::ToolTipRole, QString("swapaxes=true"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpCoordSystemDialog::getCoordSystem()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpLineStyleDialog::XWPSTricksOpLineStyleDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks line style"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("linewidth"), list);
  item->setData(Qt::ToolTipRole, QString("linewidth=1pt"));

  item = new QListWidgetItem(tr("linecolor"), list);
  item->setData(Qt::ToolTipRole, QString("linecolor=gray"));

  item = new QListWidgetItem(tr("linestyle"), list);
  item->setData(Qt::ToolTipRole, QString("linestyle=solid"));

  item = new QListWidgetItem(tr("dash"), list);
  item->setData(Qt::ToolTipRole, QString("dash=3pt 2pt"));

  item = new QListWidgetItem(tr("dotsep"), list);
  item->setData(Qt::ToolTipRole, QString("dotsep=3pt"));

  item = new QListWidgetItem(tr("border"), list);
  item->setData(Qt::ToolTipRole, QString("border=1pt"));

  item = new QListWidgetItem(tr("bordercolor"), list);
  item->setData(Qt::ToolTipRole, QString("bordercolor=white"));

  item = new QListWidgetItem(tr("doubleline"), list);
  item->setData(Qt::ToolTipRole, QString("doubleline=true"));

  item = new QListWidgetItem(tr("doublesep"), list);
  item->setData(Qt::ToolTipRole, QString("doublesep=1.25\\pslinewidth"));

  item = new QListWidgetItem(tr("doublecolor"), list);
  item->setData(Qt::ToolTipRole, QString("doublecolor=white"));

  item = new QListWidgetItem(tr("shadow"), list);
  item->setData(Qt::ToolTipRole, QString("shadow=true"));

  item = new QListWidgetItem(tr("shadowsize"), list);
  item->setData(Qt::ToolTipRole, QString("shadowsize=3pt"));

  item = new QListWidgetItem(tr("shadowangle"), list);
  item->setData(Qt::ToolTipRole, QString("shadowangle=45"));

  item = new QListWidgetItem(tr("shadowcolor"), list);
  item->setData(Qt::ToolTipRole, QString("shadowcolor=gray"));

  item = new QListWidgetItem(tr("dimen"), list);
  item->setData(Qt::ToolTipRole, QString("dimen=inner"));

  item = new QListWidgetItem(tr("linejoin"), list);
  item->setData(Qt::ToolTipRole, QString("linejoin=0|1|2"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpLineStyleDialog::getStyle()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpFillStyleDialog::XWPSTricksOpFillStyleDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks fill style"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("fillstyle"), list);
  item->setData(Qt::ToolTipRole, QString("fillstyle=crosshatch"));

  item = new QListWidgetItem(tr("fillcolor"), list);
  item->setData(Qt::ToolTipRole, QString("fillcolor=gray"));

  item = new QListWidgetItem(tr("hatchwidth"), list);
  item->setData(Qt::ToolTipRole, QString("hatchwidth=.8pt"));

  item = new QListWidgetItem(tr("hatchsep"), list);
  item->setData(Qt::ToolTipRole, QString("hatchsep=4pt"));

  item = new QListWidgetItem(tr("hatchcolor"), list);
  item->setData(Qt::ToolTipRole, QString("hatchcolor=black"));

  item = new QListWidgetItem(tr("hatchangle"), list);
  item->setData(Qt::ToolTipRole, QString("hatchangle=45"));

  item = new QListWidgetItem(tr("gradbegin"), list);
  item->setData(Qt::ToolTipRole, QString("gradbegin=red"));

  item = new QListWidgetItem(tr("gradend"), list);
  item->setData(Qt::ToolTipRole, QString("gradend=white"));

  item = new QListWidgetItem(tr("gradlines"), list);
  item->setData(Qt::ToolTipRole, QString("gradend=500"));

  item = new QListWidgetItem(tr("gradmidpoint"), list);
  item->setData(Qt::ToolTipRole, QString("gradmidpoint=.9"));

  item = new QListWidgetItem(tr("gradangle"), list);
  item->setData(Qt::ToolTipRole, QString("gradangle=0"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpFillStyleDialog::getStyle()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpArrowleDialog::XWPSTricksOpArrowleDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks arrow option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("arrowsize"), list);
  item->setData(Qt::ToolTipRole, QString("arrowsize=1.5pt 2"));

  item = new QListWidgetItem(tr("arrowlength"), list);
  item->setData(Qt::ToolTipRole, QString("arrowlength=1.4"));

  item = new QListWidgetItem(tr("arrowinset"), list);
  item->setData(Qt::ToolTipRole, QString("arrowinset=.4"));

  item = new QListWidgetItem(tr("tbarsize"), list);
  item->setData(Qt::ToolTipRole, QString("tbarsize=2pt 5"));

  item = new QListWidgetItem(tr("bracketlength"), list);
  item->setData(Qt::ToolTipRole, QString("bracketlength=.15"));

  item = new QListWidgetItem(tr("rbracketlength"), list);
  item->setData(Qt::ToolTipRole, QString("rbracketlength=.15"));

  item = new QListWidgetItem(tr("arrowscale"), list);
  item->setData(Qt::ToolTipRole, QString("arrowscale=1"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpArrowleDialog::getArrow()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpArcDialog::XWPSTricksOpArcDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks arc option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("linearc"), list);
  item->setData(Qt::ToolTipRole, QString("linearc=5"));

  item = new QListWidgetItem(tr("framearc"), list);
  item->setData(Qt::ToolTipRole, QString("framearc=15"));

  item = new QListWidgetItem(tr("cornersize"), list);
  item->setData(Qt::ToolTipRole, QString("cornersize=absolute"));

  item = new QListWidgetItem(tr("arcsepA"), list);
  item->setData(Qt::ToolTipRole, QString("arcsepA=2pt"));

  item = new QListWidgetItem(tr("arcsepB"), list);
  item->setData(Qt::ToolTipRole, QString("arcsepB=2pt"));

  item = new QListWidgetItem(tr("arcsep"), list);
  item->setData(Qt::ToolTipRole, QString("arcsep=2pt"));

  item = new QListWidgetItem(tr("curvature"), list);
  item->setData(Qt::ToolTipRole, QString("curvature=1 .1 0"));

  item = new QListWidgetItem(tr("beginAngle"), list);
  item->setData(Qt::ToolTipRole, QString("beginAngle=30"));

  item = new QListWidgetItem(tr("endAngle"), list);
  item->setData(Qt::ToolTipRole, QString("endAngle=60"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpArcDialog::getArc()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpDotDialog::XWPSTricksOpDotDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks dot option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("dotstyle"), list);
  item->setData(Qt::ToolTipRole, QString("dotstyle=o"));

  item = new QListWidgetItem(tr("dotsize"), list);
  item->setData(Qt::ToolTipRole, QString("dotsize=2pt 2"));

  item = new QListWidgetItem(tr("dotscale"), list);
  item->setData(Qt::ToolTipRole, QString("dotscale=1"));

  item = new QListWidgetItem(tr("dotangle"), list);
  item->setData(Qt::ToolTipRole, QString("dotangle=0"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpDotDialog::getDot()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpGridDialog::XWPSTricksOpGridDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks grid option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("gridwidth"), list);
  item->setData(Qt::ToolTipRole, QString("gridwidth=.8pt"));

  item = new QListWidgetItem(tr("gridcolor"), list);
  item->setData(Qt::ToolTipRole, QString("gridcolor=gray"));

  item = new QListWidgetItem(tr("griddots"), list);
  item->setData(Qt::ToolTipRole, QString("griddots=1"));

  item = new QListWidgetItem(tr("gridlabels"), list);
  item->setData(Qt::ToolTipRole, QString("gridlabels=10pt"));

  item = new QListWidgetItem(tr("gridlabelcolor"), list);
  item->setData(Qt::ToolTipRole, QString("gridlabelcolor=gray"));

  item = new QListWidgetItem(tr("subgriddiv"), list);
  item->setData(Qt::ToolTipRole, QString("subgriddiv=10"));

  item = new QListWidgetItem(tr("subgridwidth"), list);
  item->setData(Qt::ToolTipRole, QString("subgridwidth=.5pt"));

  item = new QListWidgetItem(tr("subgridcolor"), list);
  item->setData(Qt::ToolTipRole, QString("subgridcolor=gray"));

  item = new QListWidgetItem(tr("subgriddots"), list);
  item->setData(Qt::ToolTipRole, QString("subgriddots=1"));

  item = new QListWidgetItem(tr("planeGrid"), list);
  item->setData(Qt::ToolTipRole, QString("planeGrid=xy"));

  item = new QListWidgetItem(tr("subticks"), list);
  item->setData(Qt::ToolTipRole, QString("subticks=10"));

  item = new QListWidgetItem(tr("planeGridOffset"), list);
  item->setData(Qt::ToolTipRole, QString("planeGridOffset=1"));

  item = new QListWidgetItem(tr("increment"), list);
  item->setData(Qt::ToolTipRole, QString("increment=0.4"));

  item = new QListWidgetItem(tr("Hincrement"), list);
  item->setData(Qt::ToolTipRole, QString("Hincrement=0.1"));

  item = new QListWidgetItem(tr("SegmentColor"), list);
  item->setData(Qt::ToolTipRole, QString("SegmentColor={[cmyk]{0.8,0.1,.11,0}}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpGridDialog::getGrid()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpMiscDialog::XWPSTricksOpMiscDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks miscellany option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("coilwidth"), list);
  item->setData(Qt::ToolTipRole, QString("coilwidth=1cm"));

  item = new QListWidgetItem(tr("coilheight"), list);
  item->setData(Qt::ToolTipRole, QString("coilheight=1"));

  item = new QListWidgetItem(tr("coilarm"), list);
  item->setData(Qt::ToolTipRole, QString("coilarm=.5cm"));

  item = new QListWidgetItem(tr("coilaspect"), list);
  item->setData(Qt::ToolTipRole, QString("coilaspect=45"));

  item = new QListWidgetItem(tr("coilinc"), list);
  item->setData(Qt::ToolTipRole, QString("coilinc=10"));

  item = new QListWidgetItem(tr("labelsep"), list);
  item->setData(Qt::ToolTipRole, QString("labelsep=5pt"));

  item = new QListWidgetItem(tr("framesep"), list);
  item->setData(Qt::ToolTipRole, QString("framesep=3pt"));

  item = new QListWidgetItem(tr("boxsep"), list);
  item->setData(Qt::ToolTipRole, QString("boxsep=false"));

  item = new QListWidgetItem(tr("trimode"), list);
  item->setData(Qt::ToolTipRole, QString("trimode=R"));

  item = new QListWidgetItem(tr("gangle"), list);
  item->setData(Qt::ToolTipRole, QString("gangle=0"));

  item = new QListWidgetItem(tr("showpoints"), list);
  item->setData(Qt::ToolTipRole, QString("showpoints=true"));

  item = new QListWidgetItem(tr("showorigin"), list);
  item->setData(Qt::ToolTipRole, QString("showorigin=true"));

  item = new QListWidgetItem(tr("showInside"), list);
  item->setData(Qt::ToolTipRole, QString("showInside=true"));

  item = new QListWidgetItem(tr("IIIDshowgrid"), list);
  item->setData(Qt::ToolTipRole, QString("IIIDshowgrid=true"));

  item = new QListWidgetItem(tr("pOrigin"), list);
  item->setData(Qt::ToolTipRole, QString("pOrigin=c"));

  item = new QListWidgetItem(tr("plane"), list);
  item->setData(Qt::ToolTipRole, QString("plane=xy"));

  item = new QListWidgetItem(tr("planecorr"), list);
  item->setData(Qt::ToolTipRole, QString("planecorr=normal"));

  item = new QListWidgetItem(tr("drawCoor"), list);
  item->setData(Qt::ToolTipRole, QString("drawCoor=true"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpMiscDialog::getMisc()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpPlotDialog::XWPSTricksOpPlotDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks plot option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("plotstyle"), list);
  item->setData(Qt::ToolTipRole, QString("plotstyle=dots"));

  item = new QListWidgetItem(tr("plotpoints"), list);
  item->setData(Qt::ToolTipRole, QString("plotpoints=100"));

  item = new QListWidgetItem(tr("xPlotpoints"), list);
  item->setData(Qt::ToolTipRole, QString("xPlotpoints=50"));

  item = new QListWidgetItem(tr("yPlotpoints"), list);
  item->setData(Qt::ToolTipRole, QString("yPlotpoints=50"));

  item = new QListWidgetItem(tr("drawStyle"), list);
  item->setData(Qt::ToolTipRole, QString("drawStyle=yLines"));

  item = new QListWidgetItem(tr("hiddenLine"), list);
  item->setData(Qt::ToolTipRole, QString("hiddenLine=true"));

  item = new QListWidgetItem(tr("algebraic"), list);
  item->setData(Qt::ToolTipRole, QString("algebraic=true"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpPlotDialog::getPlot()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpNodeDialog::XWPSTricksOpNodeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks node option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("href"), list);
  item->setData(Qt::ToolTipRole, QString("href=-1"));

  item = new QListWidgetItem(tr("vref"), list);
  item->setData(Qt::ToolTipRole, QString("vref=.7ex"));

  item = new QListWidgetItem(tr("radius"), list);
  item->setData(Qt::ToolTipRole, QString("radius=.25cm"));

  item = new QListWidgetItem(tr("framesize"), list);
  item->setData(Qt::ToolTipRole, QString("framesize=10pt"));

  item = new QListWidgetItem(tr("nodesep"), list);
  item->setData(Qt::ToolTipRole, QString("nodesep=1pt"));

  item = new QListWidgetItem(tr("nodesepA"), list);
  item->setData(Qt::ToolTipRole, QString("nodesepA=1pt"));

  item = new QListWidgetItem(tr("nodesepB"), list);
  item->setData(Qt::ToolTipRole, QString("nodesepB=1pt"));

  item = new QListWidgetItem(tr("arcangle"), list);
  item->setData(Qt::ToolTipRole, QString("arcangle=8"));

  item = new QListWidgetItem(tr("angle"), list);
  item->setData(Qt::ToolTipRole, QString("angle=0"));

  item = new QListWidgetItem(tr("angleA"), list);
  item->setData(Qt::ToolTipRole, QString("angleA=0"));

  item = new QListWidgetItem(tr("angleB"), list);
  item->setData(Qt::ToolTipRole, QString("angleB=0"));

  item = new QListWidgetItem(tr("arm"), list);
  item->setData(Qt::ToolTipRole, QString("arm=10pt"));

  item = new QListWidgetItem(tr("armA"), list);
  item->setData(Qt::ToolTipRole, QString("armA=10pt"));

  item = new QListWidgetItem(tr("armB"), list);
  item->setData(Qt::ToolTipRole, QString("armB=10pt"));

  item = new QListWidgetItem(tr("loopsize"), list);
  item->setData(Qt::ToolTipRole, QString("loopsize=1cm"));

  item = new QListWidgetItem(tr("ncurv"), list);
  item->setData(Qt::ToolTipRole, QString("ncurv=.67"));

  item = new QListWidgetItem(tr("ncurvA"), list);
  item->setData(Qt::ToolTipRole, QString("ncurvA=.67"));

  item = new QListWidgetItem(tr("ncurvB"), list);
  item->setData(Qt::ToolTipRole, QString("ncurvB=.67"));

  item = new QListWidgetItem(tr("boxsize"), list);
  item->setData(Qt::ToolTipRole, QString("boxsize=.4cm"));

  item = new QListWidgetItem(tr("boxheight"), list);
  item->setData(Qt::ToolTipRole, QString("boxheight=.4cm"));

  item = new QListWidgetItem(tr("boxdepth"), list);
  item->setData(Qt::ToolTipRole, QString("boxdepth=.4cm"));

  item = new QListWidgetItem(tr("offset"), list);
  item->setData(Qt::ToolTipRole, QString("offset=0pt"));

  item = new QListWidgetItem(tr("offsetA"), list);
  item->setData(Qt::ToolTipRole, QString("offsetA=0pt"));

  item = new QListWidgetItem(tr("offsetB"), list);
  item->setData(Qt::ToolTipRole, QString("offsetB=0pt"));

  item = new QListWidgetItem(tr("ref"), list);
  item->setData(Qt::ToolTipRole, QString("ref=c"));

  item = new QListWidgetItem(tr("nrot"), list);
  item->setData(Qt::ToolTipRole, QString("nrot=0"));

  item = new QListWidgetItem(tr("npos"), list);
  item->setData(Qt::ToolTipRole, QString("npos=0.5"));

  item = new QListWidgetItem(tr("shortput"), list);
  item->setData(Qt::ToolTipRole, QString("shortput=nab"));

  item = new QListWidgetItem(tr("tpos"), list);
  item->setData(Qt::ToolTipRole, QString("tpos=.5"));

  item = new QListWidgetItem(tr("rot"), list);
  item->setData(Qt::ToolTipRole, QString("rot=0"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpNodeDialog::getNodeOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpTreeDialog::XWPSTricksOpTreeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks tree option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("fansize"), list);
  item->setData(Qt::ToolTipRole, QString("fansize=1cm"));

  item = new QListWidgetItem(tr("treemode"), list);
  item->setData(Qt::ToolTipRole, QString("treemode=R"));

  item = new QListWidgetItem(tr("treeflip"), list);
  item->setData(Qt::ToolTipRole, QString("treeflip=true"));

  item = new QListWidgetItem(tr("treesep"), list);
  item->setData(Qt::ToolTipRole, QString("treesep=.75cm"));

  item = new QListWidgetItem(tr("treefit"), list);
  item->setData(Qt::ToolTipRole, QString("treefit=loose"));

  item = new QListWidgetItem(tr("treenodesize"), list);
  item->setData(Qt::ToolTipRole, QString("treenodesize=-1pt"));

  item = new QListWidgetItem(tr("levelsep"), list);
  item->setData(Qt::ToolTipRole, QString("levelsep=2cm"));

  item = new QListWidgetItem(tr("edge"), list);
  item->setData(Qt::ToolTipRole, QString("edge=\\ncline"));

  item = new QListWidgetItem(tr("tnpos"), list);
  item->setData(Qt::ToolTipRole, QString("tnpos=a"));

  item = new QListWidgetItem(tr("tnsep"), list);
  item->setData(Qt::ToolTipRole, QString("tnsep=2pt"));

  item = new QListWidgetItem(tr("tnheight"), list);
  item->setData(Qt::ToolTipRole, QString("tnheight=2pt"));

  item = new QListWidgetItem(tr("tndepth"), list);
  item->setData(Qt::ToolTipRole, QString("tndepth=2pt"));

  item = new QListWidgetItem(tr("tnyref"), list);
  item->setData(Qt::ToolTipRole, QString("tnyref=2pt"));

  item = new QListWidgetItem(tr("bbl"), list);
  item->setData(Qt::ToolTipRole, QString("bbl=2pt"));

  item = new QListWidgetItem(tr("bbr"), list);
  item->setData(Qt::ToolTipRole, QString("bbr=2pt"));

  item = new QListWidgetItem(tr("bbh"), list);
  item->setData(Qt::ToolTipRole, QString("bbh=2pt"));

  item = new QListWidgetItem(tr("bbd"), list);
  item->setData(Qt::ToolTipRole, QString("bbd=2pt"));

  item = new QListWidgetItem(tr("xbbl"), list);
  item->setData(Qt::ToolTipRole, QString("xbbl=2pt"));

  item = new QListWidgetItem(tr("xbbr"), list);
  item->setData(Qt::ToolTipRole, QString("xbbr=2pt"));

  item = new QListWidgetItem(tr("xbbh"), list);
  item->setData(Qt::ToolTipRole, QString("xbbh=2pt"));

  item = new QListWidgetItem(tr("xbbd"), list);
  item->setData(Qt::ToolTipRole, QString("xbbd=2pt"));

  item = new QListWidgetItem(tr("showbbox"), list);
  item->setData(Qt::ToolTipRole, QString("showbbox=true"));

  item = new QListWidgetItem(tr("thistreesep"), list);
  item->setData(Qt::ToolTipRole, QString("thistreesep=20pt"));

  item = new QListWidgetItem(tr("thistreenodesize"), list);
  item->setData(Qt::ToolTipRole, QString("thistreenodesize=-1pt"));

  item = new QListWidgetItem(tr("thistreefit"), list);
  item->setData(Qt::ToolTipRole, QString("thistreefit=tight"));

  item = new QListWidgetItem(tr("thislevelsep"), list);
  item->setData(Qt::ToolTipRole, QString("thislevelsep=tight"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpTreeDialog::getTreeOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWPSTricksOpMatrixDialog::XWPSTricksOpMatrixDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("PSTricks matrix option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("mnode"), list);
  item->setData(Qt::ToolTipRole, QString("mnode=R"));

  item = new QListWidgetItem(tr("emnode"), list);
  item->setData(Qt::ToolTipRole, QString("emnode=circle"));

  item = new QListWidgetItem(tr("name"), list);
  item->setData(Qt::ToolTipRole, QString("name=myname"));

  item = new QListWidgetItem(tr("nodealign"), list);
  item->setData(Qt::ToolTipRole, QString("nodealign=false"));

  item = new QListWidgetItem(tr("mcol"), list);
  item->setData(Qt::ToolTipRole, QString("mcol=r"));

  item = new QListWidgetItem(tr("rowsep"), list);
  item->setData(Qt::ToolTipRole, QString("rowsep=1.5cm"));

  item = new QListWidgetItem(tr("colsep"), list);
  item->setData(Qt::ToolTipRole, QString("colsep=1.5cm"));

  item = new QListWidgetItem(tr("mnodesize"), list);
  item->setData(Qt::ToolTipRole, QString("mnodesize=-1pt"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWPSTricksOpMatrixDialog::getMatrixOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}
