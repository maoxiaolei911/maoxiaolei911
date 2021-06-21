/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWTikzDialog.h"

XWTikzPathActionDialog::XWTikzPathActionDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz path action"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("scope"), list);
  item->setData(Qt::ToolTipRole, QString("\\begin{scope}[options]\nenvironment contents\n\\end{scope}"));

  item = new QListWidgetItem(tr("path"), list);
  item->setData(Qt::ToolTipRole, QString("\\path[options]"));

  item = new QListWidgetItem(tr("draw"), list);
  item->setData(Qt::ToolTipRole, QString("\\draw[options]"));

  item = new QListWidgetItem(tr("fill"), list);
  item->setData(Qt::ToolTipRole, QString("\\fill[options]"));

  item = new QListWidgetItem(tr("filldraw"), list);
  item->setData(Qt::ToolTipRole, QString("\\filldraw[options]"));

  item = new QListWidgetItem(tr("pattern"), list);
  item->setData(Qt::ToolTipRole, QString("\\pattern[options]"));

  item = new QListWidgetItem(tr("shade"), list);
  item->setData(Qt::ToolTipRole, QString("\\shade[options]"));

  item = new QListWidgetItem(tr("shadedraw"), list);
  item->setData(Qt::ToolTipRole, QString("\\shadedraw[options]"));

  item = new QListWidgetItem(tr("clip"), list);
  item->setData(Qt::ToolTipRole, QString("\\clip[options]"));

  item = new QListWidgetItem(tr("useasboundingbox"), list);
  item->setData(Qt::ToolTipRole, QString("\\useasboundingbox[options]"));

  item = new QListWidgetItem(tr("node"), list);
  item->setData(Qt::ToolTipRole, QString("\\node[options] (name) at (coordinate) {node contents}"));

  item = new QListWidgetItem(tr("coordinate"), list);
  item->setData(Qt::ToolTipRole, QString("\\coordinate[options] (name) at (coordinate)"));

  item = new QListWidgetItem(tr("matrix"), list);
  item->setData(Qt::ToolTipRole, QString("\\matrix[options] {\nnodes\n}"));

  item = new QListWidgetItem(tr("calendar"), list);
  item->setData(Qt::ToolTipRole, QString("\\calendar[options] (name) at (coordinate) if (conditions) else"));

  item = new QListWidgetItem(tr("chainin"), list);
  item->setData(Qt::ToolTipRole, QString("\\chainin[options] (existing name)"));

  item = new QListWidgetItem(tr("spy"), list);
  item->setData(Qt::ToolTipRole, QString("\\spy[options] on (coordinate) in node[options] at (coordinate)"));

  item = new QListWidgetItem(tr("foreach"), list);
  item->setData(Qt::ToolTipRole, QString("\\foreach   [options] in {list}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzPathActionDialog::getAction()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzPathOperationDialog::XWTikzPathOperationDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz path operation"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("move-to"), list);
  item->setData(Qt::ToolTipRole, QString("(0,0)"));

  item = new QListWidgetItem(tr("line-to"), list);
  item->setData(Qt::ToolTipRole, QString(" -- "));

  item = new QListWidgetItem(tr("first horizontal, then vertical"), list);
  item->setData(Qt::ToolTipRole, QString(" -| "));

  item = new QListWidgetItem(tr("first vertical, then horizontal"), list);
  item->setData(Qt::ToolTipRole, QString(" |- "));

  item = new QListWidgetItem(tr("curve-to"), list);
  item->setData(Qt::ToolTipRole, QString(" ..controls (5,0) and (5,1) .. (4,1)"));

  item = new QListWidgetItem(tr("rectangle"), list);
  item->setData(Qt::ToolTipRole, QString(" rectangle (4,1)"));

  item = new QListWidgetItem(tr("circle"), list);
  item->setData(Qt::ToolTipRole, QString(" circle[radius=1]"));

  item = new QListWidgetItem(tr("ellipse"), list);
  item->setData(Qt::ToolTipRole, QString(" ellipse[x radius=1cm, y radius=.5cm]"));

  item = new QListWidgetItem(tr("arc"), list);
  item->setData(Qt::ToolTipRole, QString(" arc[start angle=0, end angle=60]"));

  item = new QListWidgetItem(tr("grid"), list);
  item->setData(Qt::ToolTipRole, QString(" grid[step=1mm] (2,2)"));

  item = new QListWidgetItem(tr("parabola"), list);
  item->setData(Qt::ToolTipRole, QString(" parabola[bend at end] (1,1.5)"));

  item = new QListWidgetItem(tr("sin"), list);
  item->setData(Qt::ToolTipRole, QString(" sin (1,1)"));

  item = new QListWidgetItem(tr("cos"), list);
  item->setData(Qt::ToolTipRole, QString(" cos (1,1)"));

  item = new QListWidgetItem(tr("svg"), list);
  item->setData(Qt::ToolTipRole, QString(" svg[scale=2] {h 10 v 10 h -10}"));

  item = new QListWidgetItem(tr("to"), list);
  item->setData(Qt::ToolTipRole, QString(" to[out=90,in=180] node[sloped,above] {x} (3,2)"));

  item = new QListWidgetItem(tr("foreach"), list);
  item->setData(Qt::ToolTipRole, QString(" foreach \\x in {1,2,3}"));

  item = new QListWidgetItem(tr("let"), list);
  item->setData(Qt::ToolTipRole, QString(" let \\p{foo} = (1,1), \\p2 = (2,0) in "));

  item = new QListWidgetItem(tr("node"), list);
  item->setData(Qt::ToolTipRole, QString(" node[red] (A) at (1,2) {x\\nodepart{lower}y}"));

  item = new QListWidgetItem(tr("coordinate"), list);
  item->setData(Qt::ToolTipRole, QString(" coordinate[red] (A) at (1,2)"));

  item = new QListWidgetItem(tr("edge"), list);
  item->setData(Qt::ToolTipRole, QString(" edge[->] (a)"));

  item = new QListWidgetItem(tr("child"), list);
  item->setData(Qt::ToolTipRole, QString(" child[left] {node[left] {1}}"));

  item = new QListWidgetItem(tr("edge from parent"), list);
  item->setData(Qt::ToolTipRole, QString(" edge from parent[dashed]"));

  item = new QListWidgetItem(tr("--plot coordinates"), list);
  item->setData(Qt::ToolTipRole, QString(" --plot[domain=-1:1] coordinates {(1,1) (2,0) (3,1) (2,1)}"));

  item = new QListWidgetItem(tr("--plot file"), list);
  item->setData(Qt::ToolTipRole, QString(" --plot[domain=-1:1] file {mycoord.txt}"));

  item = new QListWidgetItem(tr("--plot expression"), list);
  item->setData(Qt::ToolTipRole, QString(" --plot[domain=-1:1] (\\x,{sin(\\x r)})"));

  item = new QListWidgetItem(tr("decorate"), list);
  item->setData(Qt::ToolTipRole, QString(" decorate[decoration={name=zigzag}] { (0,0) .. controls (0,2) and (3,0) .. (3,2) |- (0,0) }"));

  item = new QListWidgetItem(tr("lindenmayer system"), list);
  item->setData(Qt::ToolTipRole, QString(" lindenmayer system [lindenmayer system={Hilbert curve, axiom=4, order=3}]"));

  item = new QListWidgetItem(tr("l-system"), list);
  item->setData(Qt::ToolTipRole, QString(" l-system [l-system={rule set={F -> F[+F]F[-F]}, axiom=F, order=4, angle=25,step=3pt}]"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzPathOperationDialog::getOperation()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzGeneralStyleDialog::XWTikzGeneralStyleDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz general style"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("every picture"), list);
  item->setData(Qt::ToolTipRole, QString("every picture/.style={line width=1ex}"));

  item = new QListWidgetItem(tr("every scope"), list);
  item->setData(Qt::ToolTipRole, QString("every scope"));

  item = new QListWidgetItem(tr("every path"), list);
  item->setData(Qt::ToolTipRole, QString("every path/.style={draw}"));

  item = new QListWidgetItem(tr("every circle"), list);
  item->setData(Qt::ToolTipRole, QString("every circle"));

  item = new QListWidgetItem(tr("every to"), list);
  item->setData(Qt::ToolTipRole, QString("every to/.style={draw,dashed}"));

  item = new QListWidgetItem(tr("every node"), list);
  item->setData(Qt::ToolTipRole, QString("every node/.style = draw"));

  item = new QListWidgetItem(tr("every label"), list);
  item->setData(Qt::ToolTipRole, QString("every label/.style={red}"));

  item = new QListWidgetItem(tr("every pin"), list);
  item->setData(Qt::ToolTipRole, QString("every pin"));

  item = new QListWidgetItem(tr("every pin edge"), list);
  item->setData(Qt::ToolTipRole, QString("every pin edge/.style={<-,shorten <=1pt,decorate,decoration={snake,pre length=4pt}}"));

  item = new QListWidgetItem(tr("every edge"), list);
  item->setData(Qt::ToolTipRole, QString("every edge/.style={draw}"));

  item = new QListWidgetItem(tr("every matrix"), list);
  item->setData(Qt::ToolTipRole, QString("every matrix"));

  item = new QListWidgetItem(tr("every cell"), list);
  item->setData(Qt::ToolTipRole, QString("every cell"));

  item = new QListWidgetItem(tr("every odd column"), list);
  item->setData(Qt::ToolTipRole, QString("every odd column"));

  item = new QListWidgetItem(tr("every even column"), list);
  item->setData(Qt::ToolTipRole, QString("every even column"));

  item = new QListWidgetItem(tr("every odd row"), list);
  item->setData(Qt::ToolTipRole, QString("every odd row"));

  item = new QListWidgetItem(tr("every even row"), list);
  item->setData(Qt::ToolTipRole, QString("every even row"));

  item = new QListWidgetItem(tr("every child"), list);
  item->setData(Qt::ToolTipRole, QString("every child/.style={concept color=blue!50}"));

  item = new QListWidgetItem(tr("every child node"), list);
  item->setData(Qt::ToolTipRole, QString("every child node/.style={anchor=south}"));

  item = new QListWidgetItem(tr("every plot"), list);
  item->setData(Qt::ToolTipRole, QString("every plot/.style={prefix=plots/}"));

  item = new QListWidgetItem(tr("every mark"), list);
  item->setData(Qt::ToolTipRole, QString("every mark/.append style={rotate=90}"));

  item = new QListWidgetItem(tr("every state"), list);
  item->setData(Qt::ToolTipRole, QString("every state/.style={fill=red,draw=none,circular drop shadow,text=white}"));

  item = new QListWidgetItem(tr("every initial by arrow"), list);
  item->setData(Qt::ToolTipRole, QString("every initial by arrow/.style={text=red,-»}"));

  item = new QListWidgetItem(tr("every accepting by arrow"), list);
  item->setData(Qt::ToolTipRole, QString("every accepting by arrow"));

  item = new QListWidgetItem(tr("every calendar"), list);
  item->setData(Qt::ToolTipRole, QString("every calendar/.style={at={(-8ex,4ex)},week list}"));

  item = new QListWidgetItem(tr("every day"), list);
  item->setData(Qt::ToolTipRole, QString("every day/.style=red"));

  item = new QListWidgetItem(tr("every month"), list);
  item->setData(Qt::ToolTipRole, QString("every month/.append style={anchor=base east,xshift=-2em}"));

  item = new QListWidgetItem(tr("every year"), list);
  item->setData(Qt::ToolTipRole, QString("every year"));

  item = new QListWidgetItem(tr("every on chain"), list);
  item->setData(Qt::ToolTipRole, QString("every on chain/.style={join=by ->}"));

  item = new QListWidgetItem(tr("every join"), list);
  item->setData(Qt::ToolTipRole, QString("every join/.style=->"));

  item = new QListWidgetItem(tr("every circuit symbol"), list);
  item->setData(Qt::ToolTipRole, QString("every circuit symbol/.style={ultra thick}"));

  item = new QListWidgetItem(tr("every info"), list);
  item->setData(Qt::ToolTipRole, QString("every info/.style={font=\footnotesize}"));

  item = new QListWidgetItem(tr("every circuit logic"), list);
  item->setData(Qt::ToolTipRole, QString("every circuit logic"));

  item = new QListWidgetItem(tr("every circuit ee"), list);
  item->setData(Qt::ToolTipRole, QString("every circuit ee"));

  item = new QListWidgetItem(tr("every circuit annotation"), list);
  item->setData(Qt::ToolTipRole, QString("every circuit annotation"));

  item = new QListWidgetItem(tr("every light emitting"), list);
  item->setData(Qt::ToolTipRole, QString("every light emitting"));

  item = new QListWidgetItem(tr("every entity"), list);
  item->setData(Qt::ToolTipRole, QString("every entity/.style={draw=blue!50,fill=blue!20,thick}"));

  item = new QListWidgetItem(tr("every relationship"), list);
  item->setData(Qt::ToolTipRole, QString("every relationship/.style={fill=orange!20,draw=orange,thick,aspect=1.5}"));

  item = new QListWidgetItem(tr("every attribute"), list);
  item->setData(Qt::ToolTipRole, QString("every attribute/.style={fill=black!20,draw=black}"));

  item = new QListWidgetItem(tr("every fit"), list);
  item->setData(Qt::ToolTipRole, QString("every fit/.style={ellipse,draw,inner sep=-2pt}"));

  item = new QListWidgetItem(tr("every delimiter"), list);
  item->setData(Qt::ToolTipRole, QString("every delimiter"));

  item = new QListWidgetItem(tr("every left delimiter"), list);
  item->setData(Qt::ToolTipRole, QString("every left delimiter/.style={red,xshift=1ex}"));

  item = new QListWidgetItem(tr("every right delimiter"), list);
  item->setData(Qt::ToolTipRole, QString("every right delimiter/.style={xshift=-1ex}"));

  item = new QListWidgetItem(tr("every above delimiter"), list);
  item->setData(Qt::ToolTipRole, QString("every above delimiter"));

  item = new QListWidgetItem(tr("every below delimiter"), list);
  item->setData(Qt::ToolTipRole, QString("every below delimiter"));

  item = new QListWidgetItem(tr("every mindmap"), list);
  item->setData(Qt::ToolTipRole, QString("every mindmap"));

  item = new QListWidgetItem(tr("every concept"), list);
  item->setData(Qt::ToolTipRole, QString("every concept"));

  item = new QListWidgetItem(tr("every extra concept"), list);
  item->setData(Qt::ToolTipRole, QString("every extra concept"));

  item = new QListWidgetItem(tr("every circle connection bar"), list);
  item->setData(Qt::ToolTipRole, QString("every circle connection bar"));

  item = new QListWidgetItem(tr("every annotation"), list);
  item->setData(Qt::ToolTipRole, QString("every annotation/.style={fill=red!20}"));

  item = new QListWidgetItem(tr("every cut"), list);
  item->setData(Qt::ToolTipRole, QString("every cut/.style=red"));

  item = new QListWidgetItem(tr("every fold"), list);
  item->setData(Qt::ToolTipRole, QString("every fold/.style=dotted"));

  item = new QListWidgetItem(tr("every place"), list);
  item->setData(Qt::ToolTipRole, QString("every place/.style={draw=blue,fill=blue!20,thick,minimum size=9mm}"));

  item = new QListWidgetItem(tr("every transition"), list);
  item->setData(Qt::ToolTipRole, QString("every transition/.style={draw=red,fill=red!20,minimum size=3mm}"));

  item = new QListWidgetItem(tr("every token"), list);
  item->setData(Qt::ToolTipRole, QString("every token"));

  item = new QListWidgetItem(tr("every shadow"), list);
  item->setData(Qt::ToolTipRole, QString("every shadow/.style={opacity=.8,fill=blue!50!black}"));

  item = new QListWidgetItem(tr("every spy in node"), list);
  item->setData(Qt::ToolTipRole, QString("every spy in node/.style={magnifying glass, circular drop shadow,fill=white, draw, ultra thick, cap=round}"));

  item = new QListWidgetItem(tr("every spy on node"), list);
  item->setData(Qt::ToolTipRole, QString("every spy on node/.style={circle,fill, fill opacity=0.2, text opacity=1}"));

  item = new QListWidgetItem(tr("every loop"), list);
  item->setData(Qt::ToolTipRole, QString("every loop/.style={}"));

  item = new QListWidgetItem(tr("every decoration"), list);
  item->setData(Qt::ToolTipRole, QString("every decoration"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzGeneralStyleDialog::getStyle()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzShapeStyleDialog::XWTikzShapeStyleDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz shape style"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("circle"), list);
  item->setData(Qt::ToolTipRole, QString("circle/.style={}"));

  item = new QListWidgetItem(tr("ellipse"), list);
  item->setData(Qt::ToolTipRole, QString("ellipse/.style={}"));

  item = new QListWidgetItem(tr("rectangle"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle/.style={}"));

  item = new QListWidgetItem(tr("diamond"), list);
  item->setData(Qt::ToolTipRole, QString("diamond/.style={}"));

  item = new QListWidgetItem(tr("trapezium"), list);
  item->setData(Qt::ToolTipRole, QString("trapezium/.style={}"));

  item = new QListWidgetItem(tr("semicircle"), list);
  item->setData(Qt::ToolTipRole, QString("semicircle/.style={}"));

  item = new QListWidgetItem(tr("regular polygon"), list);
  item->setData(Qt::ToolTipRole, QString("regular polygon/.style={}"));

  item = new QListWidgetItem(tr("star"), list);
  item->setData(Qt::ToolTipRole, QString("star/.style={}"));

  item = new QListWidgetItem(tr("isosceles triangle"), list);
  item->setData(Qt::ToolTipRole, QString("isosceles triangle/.style={}"));

  item = new QListWidgetItem(tr("kite"), list);
  item->setData(Qt::ToolTipRole, QString("kite/.style={}"));

  item = new QListWidgetItem(tr("dart"), list);
  item->setData(Qt::ToolTipRole, QString("dart/.style={}"));

  item = new QListWidgetItem(tr("circular sector"), list);
  item->setData(Qt::ToolTipRole, QString("circular sector/.style={}"));

  item = new QListWidgetItem(tr("cylinder"), list);
  item->setData(Qt::ToolTipRole, QString("cylinder/.style={}"));

  item = new QListWidgetItem(tr("forbidden sign"), list);
  item->setData(Qt::ToolTipRole, QString("forbidden sign/.style={}"));

  item = new QListWidgetItem(tr("magnifying glass"), list);
  item->setData(Qt::ToolTipRole, QString("magnifying glass/.style={}"));

  item = new QListWidgetItem(tr("cloud"), list);
  item->setData(Qt::ToolTipRole, QString("cloud/.style={}"));

  item = new QListWidgetItem(tr("starburst"), list);
  item->setData(Qt::ToolTipRole, QString("starburst/.style={}"));

  item = new QListWidgetItem(tr("signal"), list);
  item->setData(Qt::ToolTipRole, QString("signal/.style={}"));

  item = new QListWidgetItem(tr("tape"), list);
  item->setData(Qt::ToolTipRole, QString("tape/.style={}"));

  item = new QListWidgetItem(tr("single arrow"), list);
  item->setData(Qt::ToolTipRole, QString("single arrow/.style={}"));

  item = new QListWidgetItem(tr("double arrow"), list);
  item->setData(Qt::ToolTipRole, QString("double arrow/.style={}"));

  item = new QListWidgetItem(tr("arrow box"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box/.style={}"));

  item = new QListWidgetItem(tr("circle split"), list);
  item->setData(Qt::ToolTipRole, QString("circle split/.style={}"));

  item = new QListWidgetItem(tr("circle solidus"), list);
  item->setData(Qt::ToolTipRole, QString("circle solidus/.style={}"));

  item = new QListWidgetItem(tr("ellipse split"), list);
  item->setData(Qt::ToolTipRole, QString("ellipse split/.style={}"));

  item = new QListWidgetItem(tr("rectangle split"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split/.style={}"));

  item = new QListWidgetItem(tr("rectangle callout"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle callout/.style={}"));

  item = new QListWidgetItem(tr("ellipse callout"), list);
  item->setData(Qt::ToolTipRole, QString("ellipse callout/.style={}"));

  item = new QListWidgetItem(tr("cloud callout"), list);
  item->setData(Qt::ToolTipRole, QString("cloud callout/.style={}"));

  item = new QListWidgetItem(tr("cross out"), list);
  item->setData(Qt::ToolTipRole, QString("cross out/.style={}"));

  item = new QListWidgetItem(tr("strike out"), list);
  item->setData(Qt::ToolTipRole, QString("strike out/.style={}"));

  item = new QListWidgetItem(tr("rounded rectangle"), list);
  item->setData(Qt::ToolTipRole, QString("rounded rectangle/.style={}"));

  item = new QListWidgetItem(tr("chamfered rectangle"), list);
  item->setData(Qt::ToolTipRole, QString("chamfered rectangle/.style={}"));

  item = new QListWidgetItem(tr("and gate US"), list);
  item->setData(Qt::ToolTipRole, QString("and gate US/.style={}"));

  item = new QListWidgetItem(tr("and gate CDH"), list);
  item->setData(Qt::ToolTipRole, QString("and gate CDH/.style={}"));

  item = new QListWidgetItem(tr("nand gate US"), list);
  item->setData(Qt::ToolTipRole, QString("nand gate US/.style={}"));

  item = new QListWidgetItem(tr("nand gate CDH"), list);
  item->setData(Qt::ToolTipRole, QString("nand gate CDH/.style={}"));

  item = new QListWidgetItem(tr("or gate US"), list);
  item->setData(Qt::ToolTipRole, QString("or gate US/.style={}"));

  item = new QListWidgetItem(tr("nor gate US"), list);
  item->setData(Qt::ToolTipRole, QString("nor gate US/.style={}"));

  item = new QListWidgetItem(tr("xor gate US"), list);
  item->setData(Qt::ToolTipRole, QString("xor gate US/.style={}"));

  item = new QListWidgetItem(tr("xnor gate US"), list);
  item->setData(Qt::ToolTipRole, QString("xnor gate US/.style={}"));

  item = new QListWidgetItem(tr("not gate US"), list);
  item->setData(Qt::ToolTipRole, QString("not gate US/.style={}"));

  item = new QListWidgetItem(tr("buffer gate US"), list);
  item->setData(Qt::ToolTipRole, QString("buffer gate US/.style={}"));

  item = new QListWidgetItem(tr("and gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("and gate IEC/.style={}"));

  item = new QListWidgetItem(tr("nand gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("nand gate IEC/.style={}"));

  item = new QListWidgetItem(tr("or gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("or gate IEC/.style={}"));

  item = new QListWidgetItem(tr("nor gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("nor gate IEC/.style={}"));

  item = new QListWidgetItem(tr("xor gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("xor gate IEC/.style={}"));

  item = new QListWidgetItem(tr("xnor gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("xnor gate IEC/.style={}"));

  item = new QListWidgetItem(tr("not gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("not gate IEC/.style={}"));

  item = new QListWidgetItem(tr("buffer gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("buffer gate IEC/.style={}"));

  item = new QListWidgetItem(tr("rectangle ee"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle ee/.style={}"));

  item = new QListWidgetItem(tr("circle ee"), list);
  item->setData(Qt::ToolTipRole, QString("circle ee/.style={}"));

  item = new QListWidgetItem(tr("direction ee"), list);
  item->setData(Qt::ToolTipRole, QString("direction ee/.style={}"));

  item = new QListWidgetItem(tr("generic circle IEC"), list);
  item->setData(Qt::ToolTipRole, QString("generic circle IEC/.style={}"));

  item = new QListWidgetItem(tr("generic diode IEC"), list);
  item->setData(Qt::ToolTipRole, QString("generic diode IEC/.style={}"));

  item = new QListWidgetItem(tr("breakdown diode IEC"), list);
  item->setData(Qt::ToolTipRole, QString("breakdown diode IEC/.style={}"));

  item = new QListWidgetItem(tr("var resistor IEC"), list);
  item->setData(Qt::ToolTipRole, QString("var resistor IEC/.style={}"));

  item = new QListWidgetItem(tr("inductor IEC"), list);
  item->setData(Qt::ToolTipRole, QString("inductor IEC/.style={}"));

  item = new QListWidgetItem(tr("capacitor IEC"), list);
  item->setData(Qt::ToolTipRole, QString("capacitor IEC/.style={}"));

  item = new QListWidgetItem(tr("battery IEC"), list);
  item->setData(Qt::ToolTipRole, QString("battery IEC/.style={}"));

  item = new QListWidgetItem(tr("ground IEC"), list);
  item->setData(Qt::ToolTipRole, QString("ground IEC/.style={}"));

  item = new QListWidgetItem(tr("make contact IEC"), list);
  item->setData(Qt::ToolTipRole, QString("make contact IEC/.style={}"));

  item = new QListWidgetItem(tr("var make contact IEC"), list);
  item->setData(Qt::ToolTipRole, QString("var make contact IEC/.style={}"));

  item = new QListWidgetItem(tr("break contact IEC"), list);
  item->setData(Qt::ToolTipRole, QString("break contact IEC/.style={}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzShapeStyleDialog::getStyle()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzGateStyleDialog::XWTikzGateStyleDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz gate style"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("and gate"), list);
  item->setData(Qt::ToolTipRole, QString("and gate/.style={}"));

  item = new QListWidgetItem(tr("nand gate"), list);
  item->setData(Qt::ToolTipRole, QString("nand gate/.style={}"));

  item = new QListWidgetItem(tr("or gate"), list);
  item->setData(Qt::ToolTipRole, QString("or gate/.style={}"));

  item = new QListWidgetItem(tr("nor gate"), list);
  item->setData(Qt::ToolTipRole, QString("nor gate/.style={}"));

  item = new QListWidgetItem(tr("xor gate"), list);
  item->setData(Qt::ToolTipRole, QString("xor gate/.style={}"));

  item = new QListWidgetItem(tr("xnor gate"), list);
  item->setData(Qt::ToolTipRole, QString("xnor gate/.style={}"));

  item = new QListWidgetItem(tr("not gate"), list);
  item->setData(Qt::ToolTipRole, QString("not gate/.style={}"));

  item = new QListWidgetItem(tr("buffer gate"), list);
  item->setData(Qt::ToolTipRole, QString("buffer gate/.style={}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzGateStyleDialog::getStyle()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzExecuteDialog::XWTikzExecuteDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz execute at"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("execute at begin picture"), list);
  item->setData(Qt::ToolTipRole, QString("execute at begin picture={}"));

  item = new QListWidgetItem(tr("execute at end picture"), list);
  item->setData(Qt::ToolTipRole, QString("execute at end picture={}"));

  item = new QListWidgetItem(tr("execute at begin scope"), list);
  item->setData(Qt::ToolTipRole, QString("execute at begin scope={}"));

  item = new QListWidgetItem(tr("execute at end scope"), list);
  item->setData(Qt::ToolTipRole, QString("execute at end scope={}"));

  item = new QListWidgetItem(tr("execute at begin to"), list);
  item->setData(Qt::ToolTipRole, QString("execute at begin to={}"));

  item = new QListWidgetItem(tr("execute at end to"), list);
  item->setData(Qt::ToolTipRole, QString("execute at end to={}"));

  item = new QListWidgetItem(tr("execute at begin cell"), list);
  item->setData(Qt::ToolTipRole, QString("execute at begin cell={}"));

  item = new QListWidgetItem(tr("execute at end cell"), list);
  item->setData(Qt::ToolTipRole, QString("execute at end cell={}"));

  item = new QListWidgetItem(tr("execute at empty cell"), list);
  item->setData(Qt::ToolTipRole, QString("execute at empty cell={}"));

  item = new QListWidgetItem(tr("execute before day scope"), list);
  item->setData(Qt::ToolTipRole, QString("execute before day scope={}"));

  item = new QListWidgetItem(tr("execute at begin day scope"), list);
  item->setData(Qt::ToolTipRole, QString("execute at begin day scope={}"));

  item = new QListWidgetItem(tr("execute at end day scope"), list);
  item->setData(Qt::ToolTipRole, QString("execute at end day scope={}"));

  item = new QListWidgetItem(tr("execute after day scope"), list);
  item->setData(Qt::ToolTipRole, QString("execute after day scope={}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzExecuteDialog::getExecute()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzTransformDialog::XWTikzTransformDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz transform"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("x"), list);
  item->setData(Qt::ToolTipRole, QString("x=2cm"));

  item = new QListWidgetItem(tr("y"), list);
  item->setData(Qt::ToolTipRole, QString("y=2cm"));

  item = new QListWidgetItem(tr("z"), list);
  item->setData(Qt::ToolTipRole, QString("z=2cm"));

  item = new QListWidgetItem(tr("shift"), list);
  item->setData(Qt::ToolTipRole, QString("shift={(1,1)}"));

  item = new QListWidgetItem(tr("shift only"), list);
  item->setData(Qt::ToolTipRole, QString("shift only"));

  item = new QListWidgetItem(tr("xshift"), list);
  item->setData(Qt::ToolTipRole, QString("xshift=2cm"));

  item = new QListWidgetItem(tr("yshift"), list);
  item->setData(Qt::ToolTipRole, QString("yshift=2cm"));

  item = new QListWidgetItem(tr("scale"), list);
  item->setData(Qt::ToolTipRole, QString("scale=2"));

  item = new QListWidgetItem(tr("scale around"), list);
  item->setData(Qt::ToolTipRole, QString("scale around={2:(1,1)}"));

  item = new QListWidgetItem(tr("xscale"), list);
  item->setData(Qt::ToolTipRole, QString("xscale=2"));

  item = new QListWidgetItem(tr("yscale"), list);
  item->setData(Qt::ToolTipRole, QString("yscale=2"));

  item = new QListWidgetItem(tr("xslant"), list);
  item->setData(Qt::ToolTipRole, QString("xslant=2"));

  item = new QListWidgetItem(tr("yslant"), list);
  item->setData(Qt::ToolTipRole, QString("yslant=2"));

  item = new QListWidgetItem(tr("rotate"), list);
  item->setData(Qt::ToolTipRole, QString("rotate=30"));

  item = new QListWidgetItem(tr("rotate around"), list);
  item->setData(Qt::ToolTipRole, QString("rotate around={-20:(1,1)}"));

  item = new QListWidgetItem(tr("cm"), list);
  item->setData(Qt::ToolTipRole, QString("cm={1,1,0,1,(0,0)}"));

  item = new QListWidgetItem(tr("reset cm"), list);
  item->setData(Qt::ToolTipRole, QString("reset cm"));

  item = new QListWidgetItem(tr("transform canvas"), list);
  item->setData(Qt::ToolTipRole, QString("transform canvas={scale=2}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzTransformDialog::getTransform()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzLineDialog::XWTikzLineDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz line"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("line width"), list);
  item->setData(Qt::ToolTipRole, QString("line width=2pt"));

  item = new QListWidgetItem(tr("ultra thin"), list);
  item->setData(Qt::ToolTipRole, QString("ultra thin"));

  item = new QListWidgetItem(tr("very thin"), list);
  item->setData(Qt::ToolTipRole, QString("very thin"));

  item = new QListWidgetItem(tr("thin"), list);
  item->setData(Qt::ToolTipRole, QString("thin"));

  item = new QListWidgetItem(tr("semithick"), list);
  item->setData(Qt::ToolTipRole, QString("semithick"));

  item = new QListWidgetItem(tr("thick"), list);
  item->setData(Qt::ToolTipRole, QString("thick"));

  item = new QListWidgetItem(tr("very thick"), list);
  item->setData(Qt::ToolTipRole, QString("very thick"));

  item = new QListWidgetItem(tr("ultra thick"), list);
  item->setData(Qt::ToolTipRole, QString("ultra thick"));

  item = new QListWidgetItem(tr("line cap"), list);
  item->setData(Qt::ToolTipRole, QString("line cap"));

  item = new QListWidgetItem(tr("line join"), list);
  item->setData(Qt::ToolTipRole, QString("line join"));

  item = new QListWidgetItem(tr("miter limit"), list);
  item->setData(Qt::ToolTipRole, QString("miter limit"));

  item = new QListWidgetItem(tr("dash pattern"), list);
  item->setData(Qt::ToolTipRole, QString("dash pattern"));

  item = new QListWidgetItem(tr("dash phase"), list);
  item->setData(Qt::ToolTipRole, QString("dash phase"));

  item = new QListWidgetItem(tr("solid"), list);
  item->setData(Qt::ToolTipRole, QString("solid"));

  item = new QListWidgetItem(tr("dotted"), list);
  item->setData(Qt::ToolTipRole, QString("dotted"));

  item = new QListWidgetItem(tr("densely dotted"), list);
  item->setData(Qt::ToolTipRole, QString("densely dotted"));

  item = new QListWidgetItem(tr("loosely dotted"), list);
  item->setData(Qt::ToolTipRole, QString("loosely dotted"));

  item = new QListWidgetItem(tr("dashed"), list);
  item->setData(Qt::ToolTipRole, QString("dashed"));

  item = new QListWidgetItem(tr("densely dashed"), list);
  item->setData(Qt::ToolTipRole, QString("densely dashed"));

  item = new QListWidgetItem(tr("loosely dashed"), list);
  item->setData(Qt::ToolTipRole, QString("loosely dashed"));

  item = new QListWidgetItem(tr("dashdotted"), list);
  item->setData(Qt::ToolTipRole, QString("dashdotted"));

  item = new QListWidgetItem(tr("densely dashdotted"), list);
  item->setData(Qt::ToolTipRole, QString("densely dashdotted"));

  item = new QListWidgetItem(tr("loosely dashdotted"), list);
  item->setData(Qt::ToolTipRole, QString("loosely dashdotted"));

  item = new QListWidgetItem(tr("dashdotdotted"), list);
  item->setData(Qt::ToolTipRole, QString("dashdotdotted"));

  item = new QListWidgetItem(tr("densely dashdotdotted"), list);
  item->setData(Qt::ToolTipRole, QString("densely dashdotdotted"));

  item = new QListWidgetItem(tr("loosely dashdotdotted"), list);
  item->setData(Qt::ToolTipRole, QString("loosely dashdotdotted"));

  item = new QListWidgetItem(tr("double"), list);
  item->setData(Qt::ToolTipRole, QString("double"));

  item = new QListWidgetItem(tr("double distance"), list);
  item->setData(Qt::ToolTipRole, QString("double distance"));

  item = new QListWidgetItem(tr("double distance between line centers"), list);
  item->setData(Qt::ToolTipRole, QString("double distance between line centers"));

  item = new QListWidgetItem(tr("double equal sign distance"), list);
  item->setData(Qt::ToolTipRole, QString("double equal sign distance"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzLineDialog::getLine()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzFillDialog::XWTikzFillDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz fill"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("fill"), list);
  item->setData(Qt::ToolTipRole, QString("fill=yellow!80!black"));

  item = new QListWidgetItem(tr("nonzero rule"), list);
  item->setData(Qt::ToolTipRole, QString("nonzero rule"));

  item = new QListWidgetItem(tr("even odd rule"), list);
  item->setData(Qt::ToolTipRole, QString("even odd rule"));

  item = new QListWidgetItem(tr("pattern"), list);
  item->setData(Qt::ToolTipRole, QString("pattern=dots"));

  item = new QListWidgetItem(tr("pattern color"), list);
  item->setData(Qt::ToolTipRole, QString("pattern color=red"));

  item = new QListWidgetItem(tr("draw opacity"), list);
  item->setData(Qt::ToolTipRole, QString("draw opacity=0.5"));

  item = new QListWidgetItem(tr("opacity"), list);
  item->setData(Qt::ToolTipRole, QString("opacity=0.5"));

  item = new QListWidgetItem(tr("transparent"), list);
  item->setData(Qt::ToolTipRole, QString("transparent"));

  item = new QListWidgetItem(tr("ultra nearly transparent"), list);
  item->setData(Qt::ToolTipRole, QString("ultra nearly transparent"));

  item = new QListWidgetItem(tr("very nearly transparent"), list);
  item->setData(Qt::ToolTipRole, QString("very nearly transparent"));

  item = new QListWidgetItem(tr("nearly transparent"), list);
  item->setData(Qt::ToolTipRole, QString("nearly transparent"));

  item = new QListWidgetItem(tr("semitransparent"), list);
  item->setData(Qt::ToolTipRole, QString("semitransparent"));

  item = new QListWidgetItem(tr("nearly opaque"), list);
  item->setData(Qt::ToolTipRole, QString("nearly opaque"));

  item = new QListWidgetItem(tr("very nearly opaque"), list);
  item->setData(Qt::ToolTipRole, QString("very nearly opaque"));

  item = new QListWidgetItem(tr("ultra nearly opaque"), list);
  item->setData(Qt::ToolTipRole, QString("ultra nearly opaque"));

  item = new QListWidgetItem(tr("opaque"), list);
  item->setData(Qt::ToolTipRole, QString("opaque"));

  item = new QListWidgetItem(tr("fill opacity"), list);
  item->setData(Qt::ToolTipRole, QString("fill opacity=0.5"));

  item = new QListWidgetItem(tr("text opacity"), list);
  item->setData(Qt::ToolTipRole, QString("text opacity=0.5"));

  item = new QListWidgetItem(tr("path picture"), list);
  item->setData(Qt::ToolTipRole, QString("path picture={\\node at (path picture bounding box.center) {This is a long text.}}"));

  item = new QListWidgetItem(tr("shading"), list);
  item->setData(Qt::ToolTipRole, QString("shading=ball"));

  item = new QListWidgetItem(tr("shading angle"), list);
  item->setData(Qt::ToolTipRole, QString("shading angle=90"));

  item = new QListWidgetItem(tr("top color"), list);
  item->setData(Qt::ToolTipRole, QString("top color=white"));

  item = new QListWidgetItem(tr("bottom color"), list);
  item->setData(Qt::ToolTipRole, QString("bottom color=white"));

  item = new QListWidgetItem(tr("middle color"), list);
  item->setData(Qt::ToolTipRole, QString("middle color=white"));

  item = new QListWidgetItem(tr("left color"), list);
  item->setData(Qt::ToolTipRole, QString("left color=white"));

  item = new QListWidgetItem(tr("right color"), list);
  item->setData(Qt::ToolTipRole, QString("right color=white"));

  item = new QListWidgetItem(tr("ball color"), list);
  item->setData(Qt::ToolTipRole, QString("ball color=white"));

  item = new QListWidgetItem(tr("lower left"), list);
  item->setData(Qt::ToolTipRole, QString("lower left=blue"));

  item = new QListWidgetItem(tr("upper left"), list);
  item->setData(Qt::ToolTipRole, QString("upper left=blue"));

  item = new QListWidgetItem(tr("upper right"), list);
  item->setData(Qt::ToolTipRole, QString("upper right=blue"));

  item = new QListWidgetItem(tr("lower right"), list);
  item->setData(Qt::ToolTipRole, QString("lower right=blue"));

  item = new QListWidgetItem(tr("inner color"), list);
  item->setData(Qt::ToolTipRole, QString("inner color=blue"));

  item = new QListWidgetItem(tr("outer color"), list);
  item->setData(Qt::ToolTipRole, QString("outer color=blue"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzFillDialog::getFill()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzPathDialog::XWTikzPathDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz path"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("baseline"), list);
  item->setData(Qt::ToolTipRole, QString("baseline=(current bounding box.north)"));

  item = new QListWidgetItem(tr("color"), list);
  item->setData(Qt::ToolTipRole, QString("color=black"));

  item = new QListWidgetItem(tr("draw"), list);
  item->setData(Qt::ToolTipRole, QString("draw=black"));

  item = new QListWidgetItem(tr("fill"), list);
  item->setData(Qt::ToolTipRole, QString("fill=green"));

  item = new QListWidgetItem(tr("clip"), list);
  item->setData(Qt::ToolTipRole, QString("clip"));

  item = new QListWidgetItem(tr("arrows"), list);
  item->setData(Qt::ToolTipRole, QString("arrows=<->"));

  item = new QListWidgetItem(tr("arrows"), list);
  item->setData(Qt::ToolTipRole, QString("arrows=<->"));

  item = new QListWidgetItem(tr(">"), list);
  item->setData(Qt::ToolTipRole, QString(">=Latex"));

  item = new QListWidgetItem(tr("shorten >"), list);
  item->setData(Qt::ToolTipRole, QString("shorten >=1pt"));

  item = new QListWidgetItem(tr("shorten <"), list);
  item->setData(Qt::ToolTipRole, QString("shorten <=1pt"));

  item = new QListWidgetItem(tr("radius"), list);
  item->setData(Qt::ToolTipRole, QString("radius=1.5"));

  item = new QListWidgetItem(tr("x radius"), list);
  item->setData(Qt::ToolTipRole, QString("x radius=1.5"));

  item = new QListWidgetItem(tr("y radius"), list);
  item->setData(Qt::ToolTipRole, QString("y radius=1.5"));

  item = new QListWidgetItem(tr("start angle"), list);
  item->setData(Qt::ToolTipRole, QString("start angle=30"));

  item = new QListWidgetItem(tr("end angle"), list);
  item->setData(Qt::ToolTipRole, QString("end angle=60"));

  item = new QListWidgetItem(tr("delta angle"), list);
  item->setData(Qt::ToolTipRole, QString("delta angle=60"));

  item = new QListWidgetItem(tr("step"), list);
  item->setData(Qt::ToolTipRole, QString("step=1mm"));

  item = new QListWidgetItem(tr("xstep"), list);
  item->setData(Qt::ToolTipRole, QString("xstep=1mm"));

  item = new QListWidgetItem(tr("ystep"), list);
  item->setData(Qt::ToolTipRole, QString("ystep=1mm"));

  item = new QListWidgetItem(tr("bend"), list);
  item->setData(Qt::ToolTipRole, QString("bend=(1,1.5)"));

  item = new QListWidgetItem(tr("bend pos"), list);
  item->setData(Qt::ToolTipRole, QString("bend pos=0.5"));

  item = new QListWidgetItem(tr("parabola height"), list);
  item->setData(Qt::ToolTipRole, QString("parabola height=2cm"));

  item = new QListWidgetItem(tr("bend at start"), list);
  item->setData(Qt::ToolTipRole, QString("bend at start"));

  item = new QListWidgetItem(tr("bend at end"), list);
  item->setData(Qt::ToolTipRole, QString("bend at end"));

  item = new QListWidgetItem(tr("use as bounding box"), list);
  item->setData(Qt::ToolTipRole, QString("use as bounding box"));

  item = new QListWidgetItem(tr("path fading"), list);
  item->setData(Qt::ToolTipRole, QString("path fading=circle with fuzzy edge 15 percent"));

  item = new QListWidgetItem(tr("fit fading"), list);
  item->setData(Qt::ToolTipRole, QString("fit fading=false"));

  item = new QListWidgetItem(tr("fading transform"), list);
  item->setData(Qt::ToolTipRole, QString("fading transform={rotate=90}"));

  item = new QListWidgetItem(tr("fading angle"), list);
  item->setData(Qt::ToolTipRole, QString("fading angle=30"));

  item = new QListWidgetItem(tr("scope fading"), list);
  item->setData(Qt::ToolTipRole, QString("scope fading=south"));

  item = new QListWidgetItem(tr("transparency group"), list);
  item->setData(Qt::ToolTipRole, QString("transparency group=knockout"));

  item = new QListWidgetItem(tr("preaction"), list);
  item->setData(Qt::ToolTipRole, QString("preaction={draw,line width=4mm,blue}"));

  item = new QListWidgetItem(tr("postaction"), list);
  item->setData(Qt::ToolTipRole, QString("postaction={draw,line width=4mm,blue}"));

  item = new QListWidgetItem(tr("trim left"), list);
  item->setData(Qt::ToolTipRole, QString("trim left=1pt"));

  item = new QListWidgetItem(tr("trim right"), list);
  item->setData(Qt::ToolTipRole, QString("trim right=2cm"));

  item = new QListWidgetItem(tr("trim lowlevel"), list);
  item->setData(Qt::ToolTipRole, QString("trim lowlevel=true"));

  item = new QListWidgetItem(tr("help lines"), list);
  item->setData(Qt::ToolTipRole, QString("help lines"));

  item = new QListWidgetItem(tr("name path"), list);
  item->setData(Qt::ToolTipRole, QString("name path=upward line"));

  item = new QListWidgetItem(tr("name path global"), list);
  item->setData(Qt::ToolTipRole, QString("name path global=upward line"));

  item = new QListWidgetItem(tr("name intersections"), list);
  item->setData(Qt::ToolTipRole, QString("name intersections={of=upward line and sloped line, by=x}"));

  item = new QListWidgetItem(tr("insert path"), list);
  item->setData(Qt::ToolTipRole, QString("insert path={circle[radius=2pt]}"));

  item = new QListWidgetItem(tr("append after command"), list);
  item->setData(Qt::ToolTipRole, QString("append after command={(foo)--(1,1)}"));

  item = new QListWidgetItem(tr("prefix after command"), list);
  item->setData(Qt::ToolTipRole, QString("prefix after command={(foo)--(1,1)}"));

  item = new QListWidgetItem(tr("rounded corners"), list);
  item->setData(Qt::ToolTipRole, QString("rounded corners=10pt"));

  item = new QListWidgetItem(tr("sharp corners"), list);
  item->setData(Qt::ToolTipRole, QString("sharp corners"));

  item = new QListWidgetItem(tr("of"), list);
  item->setData(Qt::ToolTipRole, QString("of=curve 1 and curve 2"));

  item = new QListWidgetItem(tr("total"), list);
  item->setData(Qt::ToolTipRole, QString("total=\\t"));

  item = new QListWidgetItem(tr("by"), list);
  item->setData(Qt::ToolTipRole, QString("by={[label=center:a],[label=center:...],[label=center:i]}}"));

  item = new QListWidgetItem(tr("sort by"), list);
  item->setData(Qt::ToolTipRole, QString("sort by=\\pathname"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzPathDialog::getPathOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzFitDialog::XWTikzFitDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz fit"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("fit"), list);
  item->setData(Qt::ToolTipRole, QString("fit=(a) (b) (c) (d) (e)"));

  item = new QListWidgetItem(tr("rotate fit"), list);
  item->setData(Qt::ToolTipRole, QString("rotate fit=30"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzFitDialog::getFit()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzToDialog::XWTikzToDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz to"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("to path"), list);
  item->setData(Qt::ToolTipRole, QString("to path={.. controls +(1,0) and +(1,0) .. (\\tikztotarget) \\tikztonodes}"));

  item = new QListWidgetItem(tr("line to"), list);
  item->setData(Qt::ToolTipRole, QString("line to"));

  item = new QListWidgetItem(tr("move to"), list);
  item->setData(Qt::ToolTipRole, QString("move to"));

  item = new QListWidgetItem(tr("curve to"), list);
  item->setData(Qt::ToolTipRole, QString("curve to"));

  item = new QListWidgetItem(tr("out"), list);
  item->setData(Qt::ToolTipRole, QString("out=45"));

  item = new QListWidgetItem(tr("in"), list);
  item->setData(Qt::ToolTipRole, QString("in=15"));

  item = new QListWidgetItem(tr("relative"), list);
  item->setData(Qt::ToolTipRole, QString("relative=true"));

  item = new QListWidgetItem(tr("bend left"), list);
  item->setData(Qt::ToolTipRole, QString("bend left=15"));

  item = new QListWidgetItem(tr("bend right"), list);
  item->setData(Qt::ToolTipRole, QString("bend right=75"));

  item = new QListWidgetItem(tr("bend angle"), list);
  item->setData(Qt::ToolTipRole, QString("bend angle=15"));

  item = new QListWidgetItem(tr("looseness"), list);
  item->setData(Qt::ToolTipRole, QString("looseness=0.5"));

  item = new QListWidgetItem(tr("out looseness"), list);
  item->setData(Qt::ToolTipRole, QString("out looseness=0.5"));

  item = new QListWidgetItem(tr("in looseness"), list);
  item->setData(Qt::ToolTipRole, QString("in looseness=0.5"));

  item = new QListWidgetItem(tr("min distance"), list);
  item->setData(Qt::ToolTipRole, QString("min distance=10pt"));

  item = new QListWidgetItem(tr("max distance"), list);
  item->setData(Qt::ToolTipRole, QString("max distance=1cm"));

  item = new QListWidgetItem(tr("out min distance"), list);
  item->setData(Qt::ToolTipRole, QString("out min distance=10pt"));

  item = new QListWidgetItem(tr("out max distance"), list);
  item->setData(Qt::ToolTipRole, QString("out max distance=1cm"));

  item = new QListWidgetItem(tr("in min distance"), list);
  item->setData(Qt::ToolTipRole, QString("in min distance=10pt"));

  item = new QListWidgetItem(tr("in max distance"), list);
  item->setData(Qt::ToolTipRole, QString("in max distance=1cm"));

  item = new QListWidgetItem(tr("distance"), list);
  item->setData(Qt::ToolTipRole, QString("distance=1cm"));

  item = new QListWidgetItem(tr("out distance"), list);
  item->setData(Qt::ToolTipRole, QString("out distance=1cm"));

  item = new QListWidgetItem(tr("in distance"), list);
  item->setData(Qt::ToolTipRole, QString("in distance=1cm"));

  item = new QListWidgetItem(tr("out control"), list);
  item->setData(Qt::ToolTipRole, QString("out control=(1,1)"));

  item = new QListWidgetItem(tr("in control"), list);
  item->setData(Qt::ToolTipRole, QString("in control=(0,0)"));

  item = new QListWidgetItem(tr("controls"), list);
  item->setData(Qt::ToolTipRole, QString("controls=+(90:1) and +(90:1)"));

  item = new QListWidgetItem(tr("loop"), list);
  item->setData(Qt::ToolTipRole, QString("loop"));

  item = new QListWidgetItem(tr("loop above"), list);
  item->setData(Qt::ToolTipRole, QString("loop above"));

  item = new QListWidgetItem(tr("loop below"), list);
  item->setData(Qt::ToolTipRole, QString("loop below"));

  item = new QListWidgetItem(tr("loop left"), list);
  item->setData(Qt::ToolTipRole, QString("loop left"));

  item = new QListWidgetItem(tr("loop right"), list);
  item->setData(Qt::ToolTipRole, QString("loop right"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzToDialog::getTo()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzThroughDialog::XWTikzThroughDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz through"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("circle through"), list);
  item->setData(Qt::ToolTipRole, QString("circle through={(a)}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzThroughDialog::getThrough()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzDecorateDialog::XWTikzDecorateDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz decorate"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("decoration"), list);
  item->setData(Qt::ToolTipRole, QString("decoration={coil,aspect=0}"));

  item = new QListWidgetItem(tr("decorate"), list);
  item->setData(Qt::ToolTipRole, QString("decorate"));

  item = new QListWidgetItem(tr("name"), list);
  item->setData(Qt::ToolTipRole, QString("name=zigzag"));

  item = new QListWidgetItem(tr("raise"), list);
  item->setData(Qt::ToolTipRole, QString("raise=2pt"));

  item = new QListWidgetItem(tr("mirror"), list);
  item->setData(Qt::ToolTipRole, QString("mirror=true"));

  item = new QListWidgetItem(tr("transform"), list);
  item->setData(Qt::ToolTipRole, QString("transform={shift only}"));

  item = new QListWidgetItem(tr("pre"), list);
  item->setData(Qt::ToolTipRole, QString("pre=lineto"));

  item = new QListWidgetItem(tr("pre length"), list);
  item->setData(Qt::ToolTipRole, QString("pre length=.5cm"));

  item = new QListWidgetItem(tr("post"), list);
  item->setData(Qt::ToolTipRole, QString("post=zigzag"));

  item = new QListWidgetItem(tr("post length"), list);
  item->setData(Qt::ToolTipRole, QString("post length=5mm"));

  item = new QListWidgetItem(tr("amplitude"), list);
  item->setData(Qt::ToolTipRole, QString("amplitude=2.5pt"));

  item = new QListWidgetItem(tr("meta-amplitude"), list);
  item->setData(Qt::ToolTipRole, QString("meta-amplitude=2.5pt"));

  item = new QListWidgetItem(tr("segment length"), list);
  item->setData(Qt::ToolTipRole, QString("segment length=10pt"));

  item = new QListWidgetItem(tr("meta-segment length"), list);
  item->setData(Qt::ToolTipRole, QString("meta-segment length=10pt"));

  item = new QListWidgetItem(tr("angle"), list);
  item->setData(Qt::ToolTipRole, QString("angle=45"));

  item = new QListWidgetItem(tr("aspect"), list);
  item->setData(Qt::ToolTipRole, QString("aspect=0.5"));

  item = new QListWidgetItem(tr("start radius"), list);
  item->setData(Qt::ToolTipRole, QString("start radius=2.5pt"));

  item = new QListWidgetItem(tr("end radius"), list);
  item->setData(Qt::ToolTipRole, QString("end radius=2.5pt"));

  item = new QListWidgetItem(tr("radius"), list);
  item->setData(Qt::ToolTipRole, QString("radius=2.5pt"));

  item = new QListWidgetItem(tr("path has corners"), list);
  item->setData(Qt::ToolTipRole, QString("path has corners=false"));

  item = new QListWidgetItem(tr("moveto code"), list);
  item->setData(Qt::ToolTipRole, QString("moveto code={\\fill [red] (\\tikzinputsegmentfirst) circle (2pt) node [fill=none, below] {moveto};}"));

  item = new QListWidgetItem(tr("lineto code"), list);
  item->setData(Qt::ToolTipRole, QString("lineto code={\\draw [blue,->] (\\tikzinputsegmentfirst) -- (\\tikzinputsegmentlast)node [above] {lineto};}"));

  item = new QListWidgetItem(tr("curveto code"), list);
  item->setData(Qt::ToolTipRole, QString("curveto code={}"));

  item = new QListWidgetItem(tr("closepath code"), list);
  item->setData(Qt::ToolTipRole, QString("closepath code={}"));

  item = new QListWidgetItem(tr("mark"), list);
  item->setData(Qt::ToolTipRole, QString("mark=at position 1cm with \\node[red]{1cm};"));

  item = new QListWidgetItem(tr("reset marks"), list);
  item->setData(Qt::ToolTipRole, QString("reset marks"));

  item = new QListWidgetItem(tr("mark connection node"), list);
  item->setData(Qt::ToolTipRole, QString("mark connection node=my node"));

  item = new QListWidgetItem(tr("foot length"), list);
  item->setData(Qt::ToolTipRole, QString("foot length=20pt"));

  item = new QListWidgetItem(tr("stride length"), list);
  item->setData(Qt::ToolTipRole, QString("stride length=50pt"));

  item = new QListWidgetItem(tr("foot sep"), list);
  item->setData(Qt::ToolTipRole, QString("foot sep=10pt"));

  item = new QListWidgetItem(tr("foot angle"), list);
  item->setData(Qt::ToolTipRole, QString("foot angle=60"));

  item = new QListWidgetItem(tr("foot of"), list);
  item->setData(Qt::ToolTipRole, QString("foot of=felis silvestris"));

  item = new QListWidgetItem(tr("shape width"), list);
  item->setData(Qt::ToolTipRole, QString("shape width=2.5pt"));

  item = new QListWidgetItem(tr("shape height"), list);
  item->setData(Qt::ToolTipRole, QString("shape height=2.5pt"));

  item = new QListWidgetItem(tr("shape size"), list);
  item->setData(Qt::ToolTipRole, QString("shape size=2.5pt"));

  item = new QListWidgetItem(tr("shape"), list);
  item->setData(Qt::ToolTipRole, QString("shape=signal"));

  item = new QListWidgetItem(tr("shape sep"), list);
  item->setData(Qt::ToolTipRole, QString("shape sep=0.5cm"));

  item = new QListWidgetItem(tr("shape evenly spread"), list);
  item->setData(Qt::ToolTipRole, QString("shape evenly spread=5"));

  item = new QListWidgetItem(tr("shape sloped"), list);
  item->setData(Qt::ToolTipRole, QString("shape sloped=false"));

  item = new QListWidgetItem(tr("shape scaled"), list);
  item->setData(Qt::ToolTipRole, QString("shape scaled=false"));

  item = new QListWidgetItem(tr("shape start width"), list);
  item->setData(Qt::ToolTipRole, QString("shape start width=2.5pt"));

  item = new QListWidgetItem(tr("shape start height"), list);
  item->setData(Qt::ToolTipRole, QString("shape start height=2.5pt"));

  item = new QListWidgetItem(tr("shape start size"), list);
  item->setData(Qt::ToolTipRole, QString("shape start size=2.5pt"));

  item = new QListWidgetItem(tr("shape end width"), list);
  item->setData(Qt::ToolTipRole, QString("shape end width=2.5pt"));

  item = new QListWidgetItem(tr("shape end height"), list);
  item->setData(Qt::ToolTipRole, QString("shape end height=2.5pt"));

  item = new QListWidgetItem(tr("shape end size"), list);
  item->setData(Qt::ToolTipRole, QString("shape end size=2.5pt"));

  item = new QListWidgetItem(tr("text"), list);
  item->setData(Qt::ToolTipRole, QString("text={a big |\\color{green}|green|| juicy apple.}"));

  item = new QListWidgetItem(tr("text format delimiters"), list);
  item->setData(Qt::ToolTipRole, QString("text format delimiters={[}{]}"));

  item = new QListWidgetItem(tr("text color"), list);
  item->setData(Qt::ToolTipRole, QString("text color=green"));

  item = new QListWidgetItem(tr("reverse path"), list);
  item->setData(Qt::ToolTipRole, QString("reverse path=true"));

  item = new QListWidgetItem(tr("text align"), list);
  item->setData(Qt::ToolTipRole, QString("text align={align=right}"));

  item = new QListWidgetItem(tr("align"), list);
  item->setData(Qt::ToolTipRole, QString("align=right"));

  item = new QListWidgetItem(tr("left"), list);
  item->setData(Qt::ToolTipRole, QString("left"));

  item = new QListWidgetItem(tr("right"), list);
  item->setData(Qt::ToolTipRole, QString("right"));

  item = new QListWidgetItem(tr("center"), list);
  item->setData(Qt::ToolTipRole, QString("center"));

  item = new QListWidgetItem(tr("left indent"), list);
  item->setData(Qt::ToolTipRole, QString("left indent=2pt"));

  item = new QListWidgetItem(tr("right indent"), list);
  item->setData(Qt::ToolTipRole, QString("right indent=2pt"));

  item = new QListWidgetItem(tr("fit to path"), list);
  item->setData(Qt::ToolTipRole, QString("fit to path=false"));

  item = new QListWidgetItem(tr("fit to path stretching spaces"), list);
  item->setData(Qt::ToolTipRole, QString("fit to path stretching spaces=false"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzDecorateDialog::getDecorate()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzShadowDialog::XWTikzShadowDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz shadow"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("general shadow"), list);
  item->setData(Qt::ToolTipRole, QString("general shadow={fill=red}"));

  item = new QListWidgetItem(tr("shadow scale"), list);
  item->setData(Qt::ToolTipRole, QString("shadow scale=1.25"));

  item = new QListWidgetItem(tr("shadow xshift"), list);
  item->setData(Qt::ToolTipRole, QString("shadow xshift=2pt"));

  item = new QListWidgetItem(tr("shadow yshift"), list);
  item->setData(Qt::ToolTipRole, QString("shadow yshift=2pt"));

  item = new QListWidgetItem(tr("drop shadow"), list);
  item->setData(Qt::ToolTipRole, QString("drop shadow={opacity=0.25}"));

  item = new QListWidgetItem(tr("copy shadow"), list);
  item->setData(Qt::ToolTipRole, QString("copy shadow={shadow xshift=1ex,shadow yshift=1ex}"));

  item = new QListWidgetItem(tr("double copy shadow"), list);
  item->setData(Qt::ToolTipRole, QString("double copy shadow={shadow xshift=1ex,shadow yshift=1ex}"));

  item = new QListWidgetItem(tr("circular drop shadow"), list);
  item->setData(Qt::ToolTipRole, QString("circular drop shadow={shadow xshift=1ex,shadow yshift=1ex}"));

  item = new QListWidgetItem(tr("circular glow"), list);
  item->setData(Qt::ToolTipRole, QString("circular glow={fill=white}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzShadowDialog::getShadow()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzBackgroundDialog::XWTikzBackgroundDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz background"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("on background layer"), list);
  item->setData(Qt::ToolTipRole, QString("on background layer"));

  item = new QListWidgetItem(tr("show background rectangle"), list);
  item->setData(Qt::ToolTipRole, QString("show background rectangle"));

  item = new QListWidgetItem(tr("inner frame xsep"), list);
  item->setData(Qt::ToolTipRole, QString("inner frame xsep=1ex"));

  item = new QListWidgetItem(tr("inner frame ysep"), list);
  item->setData(Qt::ToolTipRole, QString("inner frame ysep=1ex"));

  item = new QListWidgetItem(tr("inner frame sep"), list);
  item->setData(Qt::ToolTipRole, QString("inner frame sep=1ex"));

  item = new QListWidgetItem(tr("tight background"), list);
  item->setData(Qt::ToolTipRole, QString("tight background"));

  item = new QListWidgetItem(tr("loose background"), list);
  item->setData(Qt::ToolTipRole, QString("loose background"));

  item = new QListWidgetItem(tr("background rectangle"), list);
  item->setData(Qt::ToolTipRole, QString("background rectangle/.style={double,ultra thick,draw=red,top color=blue,rounded corners}"));

  item = new QListWidgetItem(tr("framed"), list);
  item->setData(Qt::ToolTipRole, QString("framed"));

  item = new QListWidgetItem(tr("show background grid"), list);
  item->setData(Qt::ToolTipRole, QString("show background grid"));

  item = new QListWidgetItem(tr("background grid"), list);
  item->setData(Qt::ToolTipRole, QString("background grid/.style={thick,draw=red,step=.5cm}"));

  item = new QListWidgetItem(tr("gridded"), list);
  item->setData(Qt::ToolTipRole, QString("gridded"));

  item = new QListWidgetItem(tr("show background top"), list);
  item->setData(Qt::ToolTipRole, QString("show background top"));

  item = new QListWidgetItem(tr("outer frame xsep"), list);
  item->setData(Qt::ToolTipRole, QString("outer frame xsep=1ex"));

  item = new QListWidgetItem(tr("outer frame ysep"), list);
  item->setData(Qt::ToolTipRole, QString("outer frame ysep=1ex"));

  item = new QListWidgetItem(tr("outer frame sep"), list);
  item->setData(Qt::ToolTipRole, QString("outer frame sep=1ex"));

  item = new QListWidgetItem(tr("background top"), list);
  item->setData(Qt::ToolTipRole, QString("background top/.style={draw=blue!50,line width=1ex}"));

  item = new QListWidgetItem(tr("show background bottom"), list);
  item->setData(Qt::ToolTipRole, QString("show background bottom"));

  item = new QListWidgetItem(tr("show background left"), list);
  item->setData(Qt::ToolTipRole, QString("show background left"));

  item = new QListWidgetItem(tr("show background right"), list);
  item->setData(Qt::ToolTipRole, QString("show background right"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzBackgroundDialog::getBackground()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzNodeDialog::XWTikzNodeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz node"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("anchor"), list);
  item->setData(Qt::ToolTipRole, QString("anchor=north east"));

  item = new QListWidgetItem(tr("name"), list);
  item->setData(Qt::ToolTipRole, QString("name=my node"));

  item = new QListWidgetItem(tr("alias"), list);
  item->setData(Qt::ToolTipRole, QString("alias=other name"));

  item = new QListWidgetItem(tr("at"), list);
  item->setData(Qt::ToolTipRole, QString("at={(1,1)}"));

  item = new QListWidgetItem(tr("shape"), list);
  item->setData(Qt::ToolTipRole, QString("shape=circle"));

  item = new QListWidgetItem(tr("inner sep"), list);
  item->setData(Qt::ToolTipRole, QString("inner sep=1cm"));

  item = new QListWidgetItem(tr("inner xsep"), list);
  item->setData(Qt::ToolTipRole, QString("inner xsep=1cm"));

  item = new QListWidgetItem(tr("inner ysep"), list);
  item->setData(Qt::ToolTipRole, QString("inner ysep=1cm"));

  item = new QListWidgetItem(tr("outer sep"), list);
  item->setData(Qt::ToolTipRole, QString("outer sep=1cm"));

  item = new QListWidgetItem(tr("outer xsep"), list);
  item->setData(Qt::ToolTipRole, QString("outer xsep=1cm"));

  item = new QListWidgetItem(tr("outer ysep"), list);
  item->setData(Qt::ToolTipRole, QString("outer ysep=1cm"));

  item = new QListWidgetItem(tr("minimum height"), list);
  item->setData(Qt::ToolTipRole, QString("minimum height=1cm"));

  item = new QListWidgetItem(tr("minimum width"), list);
  item->setData(Qt::ToolTipRole, QString("minimum width=1cm"));

  item = new QListWidgetItem(tr("minimum size"), list);
  item->setData(Qt::ToolTipRole, QString("minimum size=1cm"));

  item = new QListWidgetItem(tr("shape aspect"), list);
  item->setData(Qt::ToolTipRole, QString("shape aspect=2"));

  item = new QListWidgetItem(tr("shape border uses incircle"), list);
  item->setData(Qt::ToolTipRole, QString("shape border uses incircle=true"));

  item = new QListWidgetItem(tr("shape border rotate"), list);
  item->setData(Qt::ToolTipRole, QString("shape border rotate=30"));

  item = new QListWidgetItem(tr("text"), list);
  item->setData(Qt::ToolTipRole, QString("text=red"));

  item = new QListWidgetItem(tr("text width"), list);
  item->setData(Qt::ToolTipRole, QString("text width=2cm"));

  item = new QListWidgetItem(tr("align"), list);
  item->setData(Qt::ToolTipRole, QString("align=left"));

  item = new QListWidgetItem(tr("node halign header"), list);
  item->setData(Qt::ToolTipRole, QString("node halign header=\\myheader"));

  item = new QListWidgetItem(tr("text height"), list);
  item->setData(Qt::ToolTipRole, QString("text height=10pt"));

  item = new QListWidgetItem(tr("text depth"), list);
  item->setData(Qt::ToolTipRole, QString("text depth=1pt"));

  item = new QListWidgetItem(tr("font"), list);
  item->setData(Qt::ToolTipRole, QString("font=\\small"));

  item = new QListWidgetItem(tr("above"), list);
  item->setData(Qt::ToolTipRole, QString("above=2pt"));

  item = new QListWidgetItem(tr("below"), list);
  item->setData(Qt::ToolTipRole, QString("below=2pt"));

  item = new QListWidgetItem(tr("left"), list);
  item->setData(Qt::ToolTipRole, QString("left=2pt"));

  item = new QListWidgetItem(tr("right"), list);
  item->setData(Qt::ToolTipRole, QString("right=2pt"));

  item = new QListWidgetItem(tr("above left"), list);
  item->setData(Qt::ToolTipRole, QString("above left"));

  item = new QListWidgetItem(tr("above right"), list);
  item->setData(Qt::ToolTipRole, QString("above right"));

  item = new QListWidgetItem(tr("below left"), list);
  item->setData(Qt::ToolTipRole, QString("below left"));

  item = new QListWidgetItem(tr("below right"), list);
  item->setData(Qt::ToolTipRole, QString("below right"));

  item = new QListWidgetItem(tr("on grid"), list);
  item->setData(Qt::ToolTipRole, QString("on grid=true"));

  item = new QListWidgetItem(tr("node distance"), list);
  item->setData(Qt::ToolTipRole, QString("node distance=5mm"));

  item = new QListWidgetItem(tr("base left"), list);
  item->setData(Qt::ToolTipRole, QString("base left=of X"));

  item = new QListWidgetItem(tr("base right"), list);
  item->setData(Qt::ToolTipRole, QString("base right=of a"));

  item = new QListWidgetItem(tr("mid left"), list);
  item->setData(Qt::ToolTipRole, QString("mid left=of a"));

  item = new QListWidgetItem(tr("mid right"), list);
  item->setData(Qt::ToolTipRole, QString("mid right=of a"));

  item = new QListWidgetItem(tr("transform shape"), list);
  item->setData(Qt::ToolTipRole, QString("transform shape"));

  item = new QListWidgetItem(tr("pos"), list);
  item->setData(Qt::ToolTipRole, QString("pos=0.5"));

  item = new QListWidgetItem(tr("auto"), list);
  item->setData(Qt::ToolTipRole, QString("auto=left"));

  item = new QListWidgetItem(tr("swap"), list);
  item->setData(Qt::ToolTipRole, QString("swap"));

  item = new QListWidgetItem(tr("'"), list);
  item->setData(Qt::ToolTipRole, QString("'"));

  item = new QListWidgetItem(tr("sloped"), list);
  item->setData(Qt::ToolTipRole, QString("sloped"));

  item = new QListWidgetItem(tr("allow upside down"), list);
  item->setData(Qt::ToolTipRole, QString("allow upside down=false"));

  item = new QListWidgetItem(tr("midway"), list);
  item->setData(Qt::ToolTipRole, QString("midway"));

  item = new QListWidgetItem(tr("near start"), list);
  item->setData(Qt::ToolTipRole, QString("near start"));

  item = new QListWidgetItem(tr("near end"), list);
  item->setData(Qt::ToolTipRole, QString("near end"));

  item = new QListWidgetItem(tr("very near start"), list);
  item->setData(Qt::ToolTipRole, QString("very near start"));

  item = new QListWidgetItem(tr("very near end"), list);
  item->setData(Qt::ToolTipRole, QString("very near end"));

  item = new QListWidgetItem(tr("at start"), list);
  item->setData(Qt::ToolTipRole, QString("at start"));

  item = new QListWidgetItem(tr("at end"), list);
  item->setData(Qt::ToolTipRole, QString("at end"));

  item = new QListWidgetItem(tr("label"), list);
  item->setData(Qt::ToolTipRole, QString("label=[red]above:label"));

  item = new QListWidgetItem(tr("label position"), list);
  item->setData(Qt::ToolTipRole, QString("label position=above"));

  item = new QListWidgetItem(tr("absolute"), list);
  item->setData(Qt::ToolTipRole, QString("absolute=true"));

  item = new QListWidgetItem(tr("label distance"), list);
  item->setData(Qt::ToolTipRole, QString("label distance=5mm"));

  item = new QListWidgetItem(tr("pin"), list);
  item->setData(Qt::ToolTipRole, QString("pin=[green]60:$q_0$"));

  item = new QListWidgetItem(tr("pin distance"), list);
  item->setData(Qt::ToolTipRole, QString("pin distance=1cm"));

  item = new QListWidgetItem(tr("pin position"), list);
  item->setData(Qt::ToolTipRole, QString("pin position=above"));

  item = new QListWidgetItem(tr("pin edge"), list);
  item->setData(Qt::ToolTipRole, QString("pin edge={blue,thick}"));

  item = new QListWidgetItem(tr("remember picture"), list);
  item->setData(Qt::ToolTipRole, QString("remember picture=true"));

  item = new QListWidgetItem(tr("overlay"), list);
  item->setData(Qt::ToolTipRole, QString("overlay=false"));

  item = new QListWidgetItem(tr("late options"), list);
  item->setData(Qt::ToolTipRole, QString("late options={name=a, label=above:world}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzNodeDialog::getNodeOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzNodeShapeDialog::XWTikzNodeShapeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz node shape"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("circle"), list);
  item->setData(Qt::ToolTipRole, QString("shape=circle"));

  item = new QListWidgetItem(tr("ellipse"), list);
  item->setData(Qt::ToolTipRole, QString("ellipse"));

  item = new QListWidgetItem(tr("rectangle"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle"));

  item = new QListWidgetItem(tr("coordinate"), list);
  item->setData(Qt::ToolTipRole, QString("coordinate"));

  item = new QListWidgetItem(tr("diamond"), list);
  item->setData(Qt::ToolTipRole, QString("diamond"));

  item = new QListWidgetItem(tr("trapezium"), list);
  item->setData(Qt::ToolTipRole, QString("trapezium"));

  item = new QListWidgetItem(tr("semicircle"), list);
  item->setData(Qt::ToolTipRole, QString("semicircle"));

  item = new QListWidgetItem(tr("regular polygon"), list);
  item->setData(Qt::ToolTipRole, QString("regular polygon"));

  item = new QListWidgetItem(tr("star"), list);
  item->setData(Qt::ToolTipRole, QString("star"));

  item = new QListWidgetItem(tr("isosceles triangle"), list);
  item->setData(Qt::ToolTipRole, QString("isosceles triangle"));

  item = new QListWidgetItem(tr("kite"), list);
  item->setData(Qt::ToolTipRole, QString("kite"));

  item = new QListWidgetItem(tr("dart"), list);
  item->setData(Qt::ToolTipRole, QString("dart"));

  item = new QListWidgetItem(tr("circular sector"), list);
  item->setData(Qt::ToolTipRole, QString("circular sector"));

  item = new QListWidgetItem(tr("cylinder"), list);
  item->setData(Qt::ToolTipRole, QString("cylinder"));

  item = new QListWidgetItem(tr("forbidden sign"), list);
  item->setData(Qt::ToolTipRole, QString("forbidden sign"));

  item = new QListWidgetItem(tr("magnifying glass"), list);
  item->setData(Qt::ToolTipRole, QString("magnifying glass"));

  item = new QListWidgetItem(tr("cloud"), list);
  item->setData(Qt::ToolTipRole, QString("cloud"));

  item = new QListWidgetItem(tr("starburst"), list);
  item->setData(Qt::ToolTipRole, QString("starburst"));

  item = new QListWidgetItem(tr("signal"), list);
  item->setData(Qt::ToolTipRole, QString("signal"));

  item = new QListWidgetItem(tr("tape"), list);
  item->setData(Qt::ToolTipRole, QString("tape"));

  item = new QListWidgetItem(tr("single arrow"), list);
  item->setData(Qt::ToolTipRole, QString("single arrow"));

  item = new QListWidgetItem(tr("double arrow"), list);
  item->setData(Qt::ToolTipRole, QString("double arrow"));

  item = new QListWidgetItem(tr("arrow box"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box"));

  item = new QListWidgetItem(tr("circle split"), list);
  item->setData(Qt::ToolTipRole, QString("circle split"));

  item = new QListWidgetItem(tr("circle solidus"), list);
  item->setData(Qt::ToolTipRole, QString("circle solidus"));

  item = new QListWidgetItem(tr("ellipse split"), list);
  item->setData(Qt::ToolTipRole, QString("ellipse split"));

  item = new QListWidgetItem(tr("rectangle split"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split"));

  item = new QListWidgetItem(tr("rectangle callout"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle callout"));

  item = new QListWidgetItem(tr("ellipse callout"), list);
  item->setData(Qt::ToolTipRole, QString("ellipse callout"));

  item = new QListWidgetItem(tr("cloud callout"), list);
  item->setData(Qt::ToolTipRole, QString("cloud callout"));

  item = new QListWidgetItem(tr("cross out"), list);
  item->setData(Qt::ToolTipRole, QString("cross out"));

  item = new QListWidgetItem(tr("strike out"), list);
  item->setData(Qt::ToolTipRole, QString("strike out"));

  item = new QListWidgetItem(tr("rounded rectangle"), list);
  item->setData(Qt::ToolTipRole, QString("rounded rectangle"));

  item = new QListWidgetItem(tr("chamfered rectangle"), list);
  item->setData(Qt::ToolTipRole, QString("chamfered rectangle"));

  item = new QListWidgetItem(tr("and gate US"), list);
  item->setData(Qt::ToolTipRole, QString("and gate US"));

  item = new QListWidgetItem(tr("and gate CDH"), list);
  item->setData(Qt::ToolTipRole, QString("and gate CDH"));

  item = new QListWidgetItem(tr("nand gate US"), list);
  item->setData(Qt::ToolTipRole, QString("nand gate US"));

  item = new QListWidgetItem(tr("nand gate CDH"), list);
  item->setData(Qt::ToolTipRole, QString("nand gate CDH"));

  item = new QListWidgetItem(tr("or gate US"), list);
  item->setData(Qt::ToolTipRole, QString("or gate US"));

  item = new QListWidgetItem(tr("nor gate US"), list);
  item->setData(Qt::ToolTipRole, QString("nor gate US"));

  item = new QListWidgetItem(tr("xor gate US"), list);
  item->setData(Qt::ToolTipRole, QString("xor gate US"));

  item = new QListWidgetItem(tr("xnor gate US"), list);
  item->setData(Qt::ToolTipRole, QString("xnor gate US"));

  item = new QListWidgetItem(tr("not gate US"), list);
  item->setData(Qt::ToolTipRole, QString("not gate US"));

  item = new QListWidgetItem(tr("buffer gate US"), list);
  item->setData(Qt::ToolTipRole, QString("buffer gate US"));

  item = new QListWidgetItem(tr("and gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("and gate IEC"));

  item = new QListWidgetItem(tr("nand gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("nand gate IEC"));

  item = new QListWidgetItem(tr("or gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("or gate IEC"));

  item = new QListWidgetItem(tr("nor gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("nor gate IEC"));

  item = new QListWidgetItem(tr("xor gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("xor gate IEC"));

  item = new QListWidgetItem(tr("xnor gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("xnor gate IEC"));

  item = new QListWidgetItem(tr("not gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("not gate IEC"));

  item = new QListWidgetItem(tr("buffer gate IEC"), list);
  item->setData(Qt::ToolTipRole, QString("buffer gate IEC"));

  item = new QListWidgetItem(tr("rectangle ee"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle ee"));

  item = new QListWidgetItem(tr("circle ee"), list);
  item->setData(Qt::ToolTipRole, QString("circle ee"));

  item = new QListWidgetItem(tr("direction ee"), list);
  item->setData(Qt::ToolTipRole, QString("direction ee"));

  item = new QListWidgetItem(tr("generic circle IEC"), list);
  item->setData(Qt::ToolTipRole, QString("generic circle IEC"));

  item = new QListWidgetItem(tr("generic diode IEC"), list);
  item->setData(Qt::ToolTipRole, QString("generic diode IEC"));

  item = new QListWidgetItem(tr("breakdown diode IEC"), list);
  item->setData(Qt::ToolTipRole, QString("breakdown diode IEC"));

  item = new QListWidgetItem(tr("var resistor IEC"), list);
  item->setData(Qt::ToolTipRole, QString("var resistor IEC"));

  item = new QListWidgetItem(tr("inductor IEC"), list);
  item->setData(Qt::ToolTipRole, QString("inductor IEC"));

  item = new QListWidgetItem(tr("capacitor IEC"), list);
  item->setData(Qt::ToolTipRole, QString("capacitor IEC"));

  item = new QListWidgetItem(tr("battery IEC"), list);
  item->setData(Qt::ToolTipRole, QString("battery IEC"));

  item = new QListWidgetItem(tr("ground IEC"), list);
  item->setData(Qt::ToolTipRole, QString("ground IEC"));

  item = new QListWidgetItem(tr("make contact IEC"), list);
  item->setData(Qt::ToolTipRole, QString("make contact IEC"));

  item = new QListWidgetItem(tr("var make contact IEC"), list);
  item->setData(Qt::ToolTipRole, QString("var make contact IEC"));

  item = new QListWidgetItem(tr("break contact IEC"), list);
  item->setData(Qt::ToolTipRole, QString("break contact IEC"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzNodeShapeDialog::getShape()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzShapeDialog::XWTikzShapeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz shape option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("aspect"), list);
  item->setData(Qt::ToolTipRole, QString("aspect=1"));

  item = new QListWidgetItem(tr("trapezium left angle"), list);
  item->setData(Qt::ToolTipRole, QString("trapezium left angle=30"));

  item = new QListWidgetItem(tr("trapezium right angle"), list);
  item->setData(Qt::ToolTipRole, QString("trapezium right angle=30"));

  item = new QListWidgetItem(tr("trapezium angle"), list);
  item->setData(Qt::ToolTipRole, QString("trapezium angle=30"));

  item = new QListWidgetItem(tr("trapezium stretches"), list);
  item->setData(Qt::ToolTipRole, QString("trapezium stretches=false"));

  item = new QListWidgetItem(tr("trapezium stretches body"), list);
  item->setData(Qt::ToolTipRole, QString("trapezium stretches body=false"));

  item = new QListWidgetItem(tr("trapezium stretches body"), list);
  item->setData(Qt::ToolTipRole, QString("trapezium stretches body=false"));

  item = new QListWidgetItem(tr("regular polygon sides"), list);
  item->setData(Qt::ToolTipRole, QString("regular polygon sides=6"));

  item = new QListWidgetItem(tr("star points"), list);
  item->setData(Qt::ToolTipRole, QString("star points=6"));

  item = new QListWidgetItem(tr("star point height"), list);
  item->setData(Qt::ToolTipRole, QString("star point height=0.5cm"));

  item = new QListWidgetItem(tr("star point ratio"), list);
  item->setData(Qt::ToolTipRole, QString("star point ratio=1.5"));

  item = new QListWidgetItem(tr("isosceles triangle apex angle"), list);
  item->setData(Qt::ToolTipRole, QString("isosceles triangle apex angle=30"));

  item = new QListWidgetItem(tr("isosceles triangle stretches"), list);
  item->setData(Qt::ToolTipRole, QString("isosceles triangle stretches=false"));

  item = new QListWidgetItem(tr("kite upper vertex angle"), list);
  item->setData(Qt::ToolTipRole, QString("kite upper vertex angle=120"));

  item = new QListWidgetItem(tr("kite lower vertex angle"), list);
  item->setData(Qt::ToolTipRole, QString("kite lower vertex angle=60"));

  item = new QListWidgetItem(tr("kite vertex angles"), list);
  item->setData(Qt::ToolTipRole, QString("kite vertex angles=90 and 45"));

  item = new QListWidgetItem(tr("dart tip angle"), list);
  item->setData(Qt::ToolTipRole, QString("dart tip angle=45"));

  item = new QListWidgetItem(tr("dart tail angle"), list);
  item->setData(Qt::ToolTipRole, QString("dart tail angle=135"));

  item = new QListWidgetItem(tr("circular sector angle"), list);
  item->setData(Qt::ToolTipRole, QString("circular sector angle=60"));

  item = new QListWidgetItem(tr("cylinder uses custom fill"), list);
  item->setData(Qt::ToolTipRole, QString("cylinder uses custom fill=true"));

  item = new QListWidgetItem(tr("cylinder end fill"), list);
  item->setData(Qt::ToolTipRole, QString("cylinder end fill=white"));

  item = new QListWidgetItem(tr("cylinder body fill"), list);
  item->setData(Qt::ToolTipRole, QString("cylinder body fill=white"));

  item = new QListWidgetItem(tr("magnifying glass handle angle fill"), list);
  item->setData(Qt::ToolTipRole, QString("magnifying glass handle angle fill=-45"));

  item = new QListWidgetItem(tr("magnifying glass handle angle aspect"), list);
  item->setData(Qt::ToolTipRole, QString("magnifying glass handle angle aspect=1.5"));

  item = new QListWidgetItem(tr("cloud puffs"), list);
  item->setData(Qt::ToolTipRole, QString("cloud puffs=10"));

  item = new QListWidgetItem(tr("cloud puff arc"), list);
  item->setData(Qt::ToolTipRole, QString("cloud puff arc=135"));

  item = new QListWidgetItem(tr("cloud ignores aspect"), list);
  item->setData(Qt::ToolTipRole, QString("cloud ignores aspect=false"));

  item = new QListWidgetItem(tr("starburst points"), list);
  item->setData(Qt::ToolTipRole, QString("starburst points=17"));

  item = new QListWidgetItem(tr("starburst point height"), list);
  item->setData(Qt::ToolTipRole, QString("starburst point height=0.5cm"));

  item = new QListWidgetItem(tr("random starburst"), list);
  item->setData(Qt::ToolTipRole, QString("random starburst=100"));

  item = new QListWidgetItem(tr("signal pointer angle"), list);
  item->setData(Qt::ToolTipRole, QString("signal pointer angle=90"));

  item = new QListWidgetItem(tr("signal from"), list);
  item->setData(Qt::ToolTipRole, QString("signal from=west"));

  item = new QListWidgetItem(tr("signal to"), list);
  item->setData(Qt::ToolTipRole, QString("signal to=east"));

  item = new QListWidgetItem(tr("tape bend top"), list);
  item->setData(Qt::ToolTipRole, QString("tape bend top=out and in"));

  item = new QListWidgetItem(tr("tape bend bottom"), list);
  item->setData(Qt::ToolTipRole, QString("tape bend bottom=out and in"));

  item = new QListWidgetItem(tr("tape bend height"), list);
  item->setData(Qt::ToolTipRole, QString("tape bend height=5pt"));

  item = new QListWidgetItem(tr("single arrow tip angle"), list);
  item->setData(Qt::ToolTipRole, QString("single arrow tip angle=90"));

  item = new QListWidgetItem(tr("single arrow head extend"), list);
  item->setData(Qt::ToolTipRole, QString("single arrow head extend=0.5cm"));

  item = new QListWidgetItem(tr("single arrow head indent"), list);
  item->setData(Qt::ToolTipRole, QString("single arrow head indent=1ex"));

  item = new QListWidgetItem(tr("double arrow tip angle"), list);
  item->setData(Qt::ToolTipRole, QString("double arrow tip angle=90"));

  item = new QListWidgetItem(tr("double arrow head extend"), list);
  item->setData(Qt::ToolTipRole, QString("double arrow head extend=0.5cm"));

  item = new QListWidgetItem(tr("double arrow head indent"), list);
  item->setData(Qt::ToolTipRole, QString("double arrow head indent=1ex"));

  item = new QListWidgetItem(tr("arrow box tip angle"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box tip angle=90"));

  item = new QListWidgetItem(tr("arrow box head extend"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box head extend=0.125cm"));

  item = new QListWidgetItem(tr("arrow box head indent"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box head indent=0.1cm"));

  item = new QListWidgetItem(tr("arrow box shaft width"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box shaft width=0.125cm"));

  item = new QListWidgetItem(tr("arrow box north arrow"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box north arrow=0.5cm"));

  item = new QListWidgetItem(tr("arrow box south arrow"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box south arrow=0.5cm"));

  item = new QListWidgetItem(tr("arrow box east arrow"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box east arrow=0.5cm"));

  item = new QListWidgetItem(tr("arrow box west arrow"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box west arrow=0.5cm"));

  item = new QListWidgetItem(tr("arrow box arrows"), list);
  item->setData(Qt::ToolTipRole, QString("arrow box arrows={north,south, east, west}"));

  item = new QListWidgetItem(tr("rectangle split allocate boxes"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split allocate boxes=4"));

  item = new QListWidgetItem(tr("rectangle split parts"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split parts=4"));

  item = new QListWidgetItem(tr("rectangle split horizontal"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split horizontal=false"));

  item = new QListWidgetItem(tr("rectangle split ignore empty parts"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split ignore empty parts=false"));

  item = new QListWidgetItem(tr("rectangle split empty part width"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split empty part width=1ex"));

  item = new QListWidgetItem(tr("rectangle split empty part height"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split empty part height=1ex"));

  item = new QListWidgetItem(tr("rectangle split empty part depth"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split empty part depth=1pt"));

  item = new QListWidgetItem(tr("rectangle split part align"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split part align={center,left}"));

  item = new QListWidgetItem(tr("rectangle split draw splits"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split draw splits=false"));

  item = new QListWidgetItem(tr("rectangle split use custom fill"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split use custom fill=false"));

  item = new QListWidgetItem(tr("rectangle split part fill"), list);
  item->setData(Qt::ToolTipRole, QString("rectangle split part fill={red!50, green!50, blue!50}"));

  item = new QListWidgetItem(tr("callout relative pointer"), list);
  item->setData(Qt::ToolTipRole, QString("callout relative pointer={(3,2)}"));

  item = new QListWidgetItem(tr("callout absolute pointer"), list);
  item->setData(Qt::ToolTipRole, QString("callout absolute pointer={(3,2)}"));

  item = new QListWidgetItem(tr("callout pointer shorten"), list);
  item->setData(Qt::ToolTipRole, QString("callout pointer shorten=1cm"));

  item = new QListWidgetItem(tr("callout pointer width"), list);
  item->setData(Qt::ToolTipRole, QString("callout pointer width=0.25cm"));

  item = new QListWidgetItem(tr("callout pointer arc"), list);
  item->setData(Qt::ToolTipRole, QString("callout pointer arc=15"));

  item = new QListWidgetItem(tr("callout pointer start size"), list);
  item->setData(Qt::ToolTipRole, QString("callout pointer start size=5pt"));

  item = new QListWidgetItem(tr("callout pointer end size"), list);
  item->setData(Qt::ToolTipRole, QString("callout pointer end size=5pt"));

  item = new QListWidgetItem(tr("callout pointer segments"), list);
  item->setData(Qt::ToolTipRole, QString("callout pointer segments=2"));

  item = new QListWidgetItem(tr("rounded rectangle arc length"), list);
  item->setData(Qt::ToolTipRole, QString("rounded rectangle arc length=180"));

  item = new QListWidgetItem(tr("rounded rectangle west arc"), list);
  item->setData(Qt::ToolTipRole, QString("rounded rectangle west arc=concave"));

  item = new QListWidgetItem(tr("rounded rectangle left arc"), list);
  item->setData(Qt::ToolTipRole, QString("rounded rectangle left arc=convex"));

  item = new QListWidgetItem(tr("rounded rectangle east arc"), list);
  item->setData(Qt::ToolTipRole, QString("rounded rectangle east arc=none"));

  item = new QListWidgetItem(tr("rounded rectangle right arc"), list);
  item->setData(Qt::ToolTipRole, QString("rounded rectangle right arc=none"));

  item = new QListWidgetItem(tr("chamfered rectangle angle"), list);
  item->setData(Qt::ToolTipRole, QString("chamfered rectangle angle=45"));

  item = new QListWidgetItem(tr("chamfered rectangle xsep"), list);
  item->setData(Qt::ToolTipRole, QString("chamfered rectangle xsep=0.666ex"));

  item = new QListWidgetItem(tr("chamfered rectangle ysep"), list);
  item->setData(Qt::ToolTipRole, QString("chamfered rectangle ysep=0.666ex"));

  item = new QListWidgetItem(tr("chamfered rectangle sep"), list);
  item->setData(Qt::ToolTipRole, QString("chamfered rectangle sep=0.666ex"));

  item = new QListWidgetItem(tr("chamfered rectangle corners"), list);
  item->setData(Qt::ToolTipRole, QString("chamfered rectangle corners={north east, south east}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzShapeDialog::getShapeOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzTreeDialog::XWTikzTreeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz tree option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("edge from parent"), list);
  item->setData(Qt::ToolTipRole, QString("edge from parent/.style={draw,red,thick}"));

  item = new QListWidgetItem(tr("level"), list);
  item->setData(Qt::ToolTipRole, QString("level 1/.style={sibling distance=15mm}"));

  item = new QListWidgetItem(tr("level distance"), list);
  item->setData(Qt::ToolTipRole, QString("level distance=5mm"));

  item = new QListWidgetItem(tr("sibling distance"), list);
  item->setData(Qt::ToolTipRole, QString("sibling distance=8mm"));

  item = new QListWidgetItem(tr("grow"), list);
  item->setData(Qt::ToolTipRole, QString("grow=right"));

  item = new QListWidgetItem(tr("grow'"), list);
  item->setData(Qt::ToolTipRole, QString("grow'=right"));

  item = new QListWidgetItem(tr("missing"), list);
  item->setData(Qt::ToolTipRole, QString("missing=true"));

  item = new QListWidgetItem(tr("growth parent anchor"), list);
  item->setData(Qt::ToolTipRole, QString("growth parent anchor=south"));

  item = new QListWidgetItem(tr("growth function"), list);
  item->setData(Qt::ToolTipRole, QString("growth function=\\func"));

  item = new QListWidgetItem(tr("edge from parent path"), list);
  item->setData(Qt::ToolTipRole, QString("edge from parent path={(\\tikzparentnode.south) .. controls +(0,-1) and +(0,1).. (\\tikzchildnode.north)}"));

  item = new QListWidgetItem(tr("child anchor"), list);
  item->setData(Qt::ToolTipRole, QString("child anchor=north"));

  item = new QListWidgetItem(tr("parent anchor"), list);
  item->setData(Qt::ToolTipRole, QString("parent anchor=north"));

  item = new QListWidgetItem(tr("grow via three points"), list);
  item->setData(Qt::ToolTipRole, QString("grow via three points={one child at (0,1) and two children at (-.5,1) and (.5,1)}"));

  item = new QListWidgetItem(tr("grow cyclic"), list);
  item->setData(Qt::ToolTipRole, QString("grow cyclic"));

  item = new QListWidgetItem(tr("sibling angle"), list);
  item->setData(Qt::ToolTipRole, QString("sibling angle=60"));

  item = new QListWidgetItem(tr("clockwise from"), list);
  item->setData(Qt::ToolTipRole, QString("clockwise from=30"));

  item = new QListWidgetItem(tr("counterclockwise from"), list);
  item->setData(Qt::ToolTipRole, QString("counterclockwise from=30"));

  item = new QListWidgetItem(tr("edge from parent fork down"), list);
  item->setData(Qt::ToolTipRole, QString("edge from parent fork down"));

  item = new QListWidgetItem(tr("edge from parent fork right"), list);
  item->setData(Qt::ToolTipRole, QString("edge from parent fork right"));

  item = new QListWidgetItem(tr("edge from parent fork left"), list);
  item->setData(Qt::ToolTipRole, QString("edge from parent fork left"));

  item = new QListWidgetItem(tr("edge from parent fork up"), list);
  item->setData(Qt::ToolTipRole, QString("edge from parent fork up"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzTreeDialog::getTreeOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzMatrixDialog::XWTikzMatrixDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz matrix option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("matrix"), list);
  item->setData(Qt::ToolTipRole, QString("matrix"));

  item = new QListWidgetItem(tr("anchor"), list);
  item->setData(Qt::ToolTipRole, QString("anchor=base"));

  item = new QListWidgetItem(tr("column sep"), list);
  item->setData(Qt::ToolTipRole, QString("column sep=3mm"));

  item = new QListWidgetItem(tr("row sep"), list);
  item->setData(Qt::ToolTipRole, QString("row sep=3mm"));

  item = new QListWidgetItem(tr("cells"), list);
  item->setData(Qt::ToolTipRole, QString("cells={fill=red}"));

  item = new QListWidgetItem(tr("nodes"), list);
  item->setData(Qt::ToolTipRole, QString("nodes={fill=blue!20,minimum size=5mm}"));

  item = new QListWidgetItem(tr("column"), list);
  item->setData(Qt::ToolTipRole, QString("column 2/.style={green!50!black}"));

  item = new QListWidgetItem(tr("row"), list);
  item->setData(Qt::ToolTipRole, QString("row 1/.style={red}"));

  item = new QListWidgetItem(tr("row column"), list);
  item->setData(Qt::ToolTipRole, QString("row 3 column 3/.style={blue}"));

  item = new QListWidgetItem(tr("matrix anchor"), list);
  item->setData(Qt::ToolTipRole, QString("matrix anchor=west"));

  item = new QListWidgetItem(tr("ampersand replacement"), list);
  item->setData(Qt::ToolTipRole, QString("ampersand replacement=\\mymacro"));

  item = new QListWidgetItem(tr("matrix of nodes"), list);
  item->setData(Qt::ToolTipRole, QString("matrix of nodes"));

  item = new QListWidgetItem(tr("matrix of math nodes"), list);
  item->setData(Qt::ToolTipRole, QString("matrix of math nodes"));

  item = new QListWidgetItem(tr("nodes in empty cells"), list);
  item->setData(Qt::ToolTipRole, QString("nodes in empty cells=false"));

  item = new QListWidgetItem(tr("left delimiter"), list);
  item->setData(Qt::ToolTipRole, QString("left delimiter"));

  item = new QListWidgetItem(tr("right delimiter"), list);
  item->setData(Qt::ToolTipRole, QString("right delimiter"));

  item = new QListWidgetItem(tr("above delimiter"), list);
  item->setData(Qt::ToolTipRole, QString("above delimiter"));

  item = new QListWidgetItem(tr("below delimiter"), list);
  item->setData(Qt::ToolTipRole, QString("below delimiter"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzMatrixDialog::getMatrixOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzCalendarDialog::XWTikzCalendarDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz calendar option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("dates"), list);
  item->setData(Qt::ToolTipRole, QString("dates=2000-01-01 to 2000-01-31"));

  item = new QListWidgetItem(tr("day xshift"), list);
  item->setData(Qt::ToolTipRole, QString("day xshift=3ex"));

  item = new QListWidgetItem(tr("day yshift"), list);
  item->setData(Qt::ToolTipRole, QString("day yshift=3ex"));

  item = new QListWidgetItem(tr("month xshift"), list);
  item->setData(Qt::ToolTipRole, QString("month xshift=3ex"));

  item = new QListWidgetItem(tr("month yshift"), list);
  item->setData(Qt::ToolTipRole, QString("month yshift=3ex"));

  item = new QListWidgetItem(tr("day code"), list);
  item->setData(Qt::ToolTipRole, QString("day code={\\fill[blue] (0,0) circle (2pt);}"));

  item = new QListWidgetItem(tr("day text"), list);
  item->setData(Qt::ToolTipRole, QString("day text=x"));

  item = new QListWidgetItem(tr("month code"), list);
  item->setData(Qt::ToolTipRole, QString("month code={\\node[every month]{\\tikzmonthtext};}"));

  item = new QListWidgetItem(tr("month text"), list);
  item->setData(Qt::ToolTipRole, QString("month text=y"));

  item = new QListWidgetItem(tr("year code"), list);
  item->setData(Qt::ToolTipRole, QString("year code={\\fill[black] (0,0) circle (2pt);}"));

  item = new QListWidgetItem(tr("year text"), list);
  item->setData(Qt::ToolTipRole, QString("year text=y"));

  item = new QListWidgetItem(tr("day list downward"), list);
  item->setData(Qt::ToolTipRole, QString("day list downward"));

  item = new QListWidgetItem(tr("day list upward"), list);
  item->setData(Qt::ToolTipRole, QString("day list upward"));

  item = new QListWidgetItem(tr("day list right"), list);
  item->setData(Qt::ToolTipRole, QString("day list right"));

  item = new QListWidgetItem(tr("day list left"), list);
  item->setData(Qt::ToolTipRole, QString("day list left"));

  item = new QListWidgetItem(tr("week list"), list);
  item->setData(Qt::ToolTipRole, QString("week list"));

  item = new QListWidgetItem(tr("month list"), list);
  item->setData(Qt::ToolTipRole, QString("month list"));

  item = new QListWidgetItem(tr("month label left"), list);
  item->setData(Qt::ToolTipRole, QString("month label left"));

  item = new QListWidgetItem(tr("month label left vertical"), list);
  item->setData(Qt::ToolTipRole, QString("month label left vertical"));

  item = new QListWidgetItem(tr("month label right"), list);
  item->setData(Qt::ToolTipRole, QString("month label right"));

  item = new QListWidgetItem(tr("month label right vertical"), list);
  item->setData(Qt::ToolTipRole, QString("month label right vertical"));

  item = new QListWidgetItem(tr("month label above left"), list);
  item->setData(Qt::ToolTipRole, QString("month label above left"));

  item = new QListWidgetItem(tr("month label above centered"), list);
  item->setData(Qt::ToolTipRole, QString("month label above centered"));

  item = new QListWidgetItem(tr("month label above right"), list);
  item->setData(Qt::ToolTipRole, QString("month label above right"));

  item = new QListWidgetItem(tr("month label below left"), list);
  item->setData(Qt::ToolTipRole, QString("month label below left"));

  item = new QListWidgetItem(tr("month label below centered"), list);
  item->setData(Qt::ToolTipRole, QString("month label below centered"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzCalendarDialog::getCalendarOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzMindmapDialog::XWTikzMindmapDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz mindmap option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("mindmap"), list);
  item->setData(Qt::ToolTipRole, QString("mindmap"));

  item = new QListWidgetItem(tr("small mindmap"), list);
  item->setData(Qt::ToolTipRole, QString("small mindmap"));

  item = new QListWidgetItem(tr("large mindmap"), list);
  item->setData(Qt::ToolTipRole, QString("large mindmap"));

  item = new QListWidgetItem(tr("huge mindmap"), list);
  item->setData(Qt::ToolTipRole, QString("huge mindmap"));

  item = new QListWidgetItem(tr("concept"), list);
  item->setData(Qt::ToolTipRole, QString("concept"));

  item = new QListWidgetItem(tr("concept color"), list);
  item->setData(Qt::ToolTipRole, QString("concept color=blue!80"));

  item = new QListWidgetItem(tr("extra concept"), list);
  item->setData(Qt::ToolTipRole, QString("extra concept"));

  item = new QListWidgetItem(tr("root concept"), list);
  item->setData(Qt::ToolTipRole, QString("root concept/.append style={concept color=blue!80,minimum size=3.5cm}"));

  item = new QListWidgetItem(tr("level 1 concept"), list);
  item->setData(Qt::ToolTipRole, QString("level 1 concept/.append style={concept color=red!50}"));

  item = new QListWidgetItem(tr("level 2 concept"), list);
  item->setData(Qt::ToolTipRole, QString("level 2 concept/.append style={concept color=red!50}"));

  item = new QListWidgetItem(tr("level 3 concept"), list);
  item->setData(Qt::ToolTipRole, QString("level 3 concept/.append style={concept color=red!50}"));

  item = new QListWidgetItem(tr("level 4 concept"), list);
  item->setData(Qt::ToolTipRole, QString("level 4 concept/.append style={concept color=red!50}"));

  item = new QListWidgetItem(tr("concept connection"), list);
  item->setData(Qt::ToolTipRole, QString("concept connection"));

  item = new QListWidgetItem(tr("circle connection bar"), list);
  item->setData(Qt::ToolTipRole, QString("circle connection bar"));

  item = new QListWidgetItem(tr("circle connection bar switch color"), list);
  item->setData(Qt::ToolTipRole, QString("circle connection bar switch color=from (red) to (blue)"));

  item = new QListWidgetItem(tr("annotation"), list);
  item->setData(Qt::ToolTipRole, QString("annotation"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzMindmapDialog::getMindmapOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzChainDialog::XWTikzChainDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz chain option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("start chain"), list);
  item->setData(Qt::ToolTipRole, QString("start chain=going right"));

  item = new QListWidgetItem(tr("chain default direction"), list);
  item->setData(Qt::ToolTipRole, QString("chain default direction=rigth"));

  item = new QListWidgetItem(tr("continue chain"), list);
  item->setData(Qt::ToolTipRole, QString("continue chain=going below"));

  item = new QListWidgetItem(tr("on chain"), list);
  item->setData(Qt::ToolTipRole, QString("on chain=going below"));

  item = new QListWidgetItem(tr("join"), list);
  item->setData(Qt::ToolTipRole, QString("join=with chain-1 by {blue,<-}"));

  item = new QListWidgetItem(tr("start branch"), list);
  item->setData(Qt::ToolTipRole, QString("start branch=numbers going below"));

  item = new QListWidgetItem(tr("continue branch"), list);
  item->setData(Qt::ToolTipRole, QString("continue branch=greek"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzChainDialog::getChainOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzSpyDialog::XWTikzSpyDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz spy option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("spy scope"), list);
  item->setData(Qt::ToolTipRole, QString("spy scope={every spy on node/.style={circle,fill, fill opacity=0.2, text opacity=1},}"));

  item = new QListWidgetItem(tr("size"), list);
  item->setData(Qt::ToolTipRole, QString("size=2cm"));

  item = new QListWidgetItem(tr("height"), list);
  item->setData(Qt::ToolTipRole, QString("height=2cm"));

  item = new QListWidgetItem(tr("width"), list);
  item->setData(Qt::ToolTipRole, QString("width=2cm"));

  item = new QListWidgetItem(tr("lens"), list);
  item->setData(Qt::ToolTipRole, QString("lens={scale=3,rotate=20}"));

  item = new QListWidgetItem(tr("magnification"), list);
  item->setData(Qt::ToolTipRole, QString("magnification=2"));

  item = new QListWidgetItem(tr("spy connection path"), list);
  item->setData(Qt::ToolTipRole, QString("spy connection path={\\draw[thin] (tikzspyonnode) -- (tikzspyinnode);}"));

  item = new QListWidgetItem(tr("spy using outlines"), list);
  item->setData(Qt::ToolTipRole, QString("spy using outlines={lens={scale=3,rotate=20}"));

  item = new QListWidgetItem(tr("spy using overlays"), list);
  item->setData(Qt::ToolTipRole, QString("spy using overlays={circle, magnification=3, size=1cm, connect spies}"));

  item = new QListWidgetItem(tr("connect spies"), list);
  item->setData(Qt::ToolTipRole, QString("connect spies"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzSpyDialog::getSpyOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzTurtleDialog::XWTikzTurtleDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz turtle option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("turtle"), list);
  item->setData(Qt::ToolTipRole, QString("turtle={home,fd,rt,fd,lt,fd,lt,fd}"));

  item = new QListWidgetItem(tr("home"), list);
  item->setData(Qt::ToolTipRole, QString("home"));

  item = new QListWidgetItem(tr("forward"), list);
  item->setData(Qt::ToolTipRole, QString("forward=2cm"));

  item = new QListWidgetItem(tr("how"), list);
  item->setData(Qt::ToolTipRole, QString("how/.style={bend left},home,forward,right,forward}"));

  item = new QListWidgetItem(tr("fd"), list);
  item->setData(Qt::ToolTipRole, QString("fd"));

  item = new QListWidgetItem(tr("back"), list);
  item->setData(Qt::ToolTipRole, QString("back=1cm"));

  item = new QListWidgetItem(tr("bk"), list);
  item->setData(Qt::ToolTipRole, QString("bk"));

  item = new QListWidgetItem(tr("lt"), list);
  item->setData(Qt::ToolTipRole, QString("lt"));

  item = new QListWidgetItem(tr("left=90"), list);
  item->setData(Qt::ToolTipRole, QString("lt"));

  item = new QListWidgetItem(tr("rt"), list);
  item->setData(Qt::ToolTipRole, QString("rt"));

  item = new QListWidgetItem(tr("right"), list);
  item->setData(Qt::ToolTipRole, QString("right=90"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzTurtleDialog::getTurtleOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzPlotDialog::XWTikzPlotDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz plot option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("variable"), list);
  item->setData(Qt::ToolTipRole, QString("variable=\\x"));

  item = new QListWidgetItem(tr("samples"), list);
  item->setData(Qt::ToolTipRole, QString("samples=25"));

  item = new QListWidgetItem(tr("domain"), list);
  item->setData(Qt::ToolTipRole, QString("domain=-3.14:3.14"));

  item = new QListWidgetItem(tr("samples at"), list);
  item->setData(Qt::ToolTipRole, QString("samples at={1,2,8,9,10}"));

  item = new QListWidgetItem(tr("parametric"), list);
  item->setData(Qt::ToolTipRole, QString("parametric=false"));

  item = new QListWidgetItem(tr("id"), list);
  item->setData(Qt::ToolTipRole, QString("id=myname"));

  item = new QListWidgetItem(tr("prefix"), list);
  item->setData(Qt::ToolTipRole, QString("prefix=\\jobname"));

  item = new QListWidgetItem(tr("raw gnuplot"), list);
  item->setData(Qt::ToolTipRole, QString("raw gnuplot"));

  item = new QListWidgetItem(tr("mark"), list);
  item->setData(Qt::ToolTipRole, QString("mark=x"));

  item = new QListWidgetItem(tr("mark repeat"), list);
  item->setData(Qt::ToolTipRole, QString("mark repeat=3"));

  item = new QListWidgetItem(tr("mark phase"), list);
  item->setData(Qt::ToolTipRole, QString("mark phase=6"));

  item = new QListWidgetItem(tr("mark indices"), list);
  item->setData(Qt::ToolTipRole, QString("mark indices={1,4,...,10,11,12,...,16,20}"));

  item = new QListWidgetItem(tr("mark size"), list);
  item->setData(Qt::ToolTipRole, QString("mark size=10pt"));

  item = new QListWidgetItem(tr("mark options"), list);
  item->setData(Qt::ToolTipRole, QString("mark options={color=blue,rotate=180}"));

  item = new QListWidgetItem(tr("no marks"), list);
  item->setData(Qt::ToolTipRole, QString("no marks"));

  item = new QListWidgetItem(tr("no markers"), list);
  item->setData(Qt::ToolTipRole, QString("no markers"));

  item = new QListWidgetItem(tr("sharp plot"), list);
  item->setData(Qt::ToolTipRole, QString("sharp plot"));

  item = new QListWidgetItem(tr("smooth"), list);
  item->setData(Qt::ToolTipRole, QString("smooth"));

  item = new QListWidgetItem(tr("tension"), list);
  item->setData(Qt::ToolTipRole, QString("tension=0.2"));

  item = new QListWidgetItem(tr("smooth cycle"), list);
  item->setData(Qt::ToolTipRole, QString("smooth cycle"));

  item = new QListWidgetItem(tr("const plot"), list);
  item->setData(Qt::ToolTipRole, QString("const plot"));

  item = new QListWidgetItem(tr("const plot mark left"), list);
  item->setData(Qt::ToolTipRole, QString("const plot mark left"));

  item = new QListWidgetItem(tr("const plot mark right"), list);
  item->setData(Qt::ToolTipRole, QString("const plot mark right"));

  item = new QListWidgetItem(tr("jump mark left"), list);
  item->setData(Qt::ToolTipRole, QString("jump mark left"));

  item = new QListWidgetItem(tr("jump mark right"), list);
  item->setData(Qt::ToolTipRole, QString("jump mark right"));

  item = new QListWidgetItem(tr("ycomb"), list);
  item->setData(Qt::ToolTipRole, QString("ycomb"));

  item = new QListWidgetItem(tr("xcomb"), list);
  item->setData(Qt::ToolTipRole, QString("xcomb"));

  item = new QListWidgetItem(tr("polar comb"), list);
  item->setData(Qt::ToolTipRole, QString("polar comb"));

  item = new QListWidgetItem(tr("ybar"), list);
  item->setData(Qt::ToolTipRole, QString("ybar"));

  item = new QListWidgetItem(tr("xbar"), list);
  item->setData(Qt::ToolTipRole, QString("xbar"));

  item = new QListWidgetItem(tr("ybar interval"), list);
  item->setData(Qt::ToolTipRole, QString("ybar interval"));

  item = new QListWidgetItem(tr("xbar interval"), list);
  item->setData(Qt::ToolTipRole, QString("xbar interval"));

  item = new QListWidgetItem(tr("only marks"), list);
  item->setData(Qt::ToolTipRole, QString("only marks"));

  item = new QListWidgetItem(tr("bar width"), list);
  item->setData(Qt::ToolTipRole, QString("bar width=10pt"));

  item = new QListWidgetItem(tr("bar shift"), list);
  item->setData(Qt::ToolTipRole, QString("bar shift=2pt"));

  item = new QListWidgetItem(tr("bar interval shift"), list);
  item->setData(Qt::ToolTipRole, QString("bar interval shift={0.5}"));

  item = new QListWidgetItem(tr("bar interval width"), list);
  item->setData(Qt::ToolTipRole, QString("bar interval width={1}"));

  item = new QListWidgetItem(tr("mark color"), list);
  item->setData(Qt::ToolTipRole, QString("mark color=red"));

  item = new QListWidgetItem(tr("text mark"), list);
  item->setData(Qt::ToolTipRole, QString("text mark={A}"));

  item = new QListWidgetItem(tr("text mark style"), list);
  item->setData(Qt::ToolTipRole, QString("text mark style={}"));

  item = new QListWidgetItem(tr("text mark as node"), list);
  item->setData(Qt::ToolTipRole, QString("text mark as node={false}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzPlotDialog::getPlotOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzLSystemDialog::XWTikzLSystemDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz Lindenmayer system option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("step"), list);
  item->setData(Qt::ToolTipRole, QString("step=5pt"));

  item = new QListWidgetItem(tr("randomize step percent"), list);
  item->setData(Qt::ToolTipRole, QString("randomize step percent=0"));

  item = new QListWidgetItem(tr("left angle"), list);
  item->setData(Qt::ToolTipRole, QString("left angle=90"));

  item = new QListWidgetItem(tr("right angle"), list);
  item->setData(Qt::ToolTipRole, QString("right angle=90"));

  item = new QListWidgetItem(tr("randomize angle percent"), list);
  item->setData(Qt::ToolTipRole, QString("randomize angle percent=0"));

  item = new QListWidgetItem(tr("name"), list);
  item->setData(Qt::ToolTipRole, QString("name=myname"));

  item = new QListWidgetItem(tr("axiom"), list);
  item->setData(Qt::ToolTipRole, QString("axiom=F"));

  item = new QListWidgetItem(tr("order"), list);
  item->setData(Qt::ToolTipRole, QString("order=4"));

  item = new QListWidgetItem(tr("rule set"), list);
  item->setData(Qt::ToolTipRole, QString("rule set={F -> F[+F]F[-F]}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzLSystemDialog::getLSystemOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzAutomataDialog::XWTikzAutomataDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz automata option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("state without output"), list);
  item->setData(Qt::ToolTipRole, QString("state without output"));

  item = new QListWidgetItem(tr("state with output"), list);
  item->setData(Qt::ToolTipRole, QString("state with output"));

  item = new QListWidgetItem(tr("state"), list);
  item->setData(Qt::ToolTipRole, QString("state/.style=state with output"));

  item = new QListWidgetItem(tr("initial"), list);
  item->setData(Qt::ToolTipRole, QString("initial"));

  item = new QListWidgetItem(tr("initial by arrow"), list);
  item->setData(Qt::ToolTipRole, QString("initial by arrow"));

  item = new QListWidgetItem(tr("initial text"), list);
  item->setData(Qt::ToolTipRole, QString("initial text={start}"));

  item = new QListWidgetItem(tr("initial where"), list);
  item->setData(Qt::ToolTipRole, QString("initial where=left"));

  item = new QListWidgetItem(tr("initial distance"), list);
  item->setData(Qt::ToolTipRole, QString("initial distance=3ex"));

  item = new QListWidgetItem(tr("initial above"), list);
  item->setData(Qt::ToolTipRole, QString("initial above"));

  item = new QListWidgetItem(tr("initial below"), list);
  item->setData(Qt::ToolTipRole, QString("initial below"));

  item = new QListWidgetItem(tr("initial left"), list);
  item->setData(Qt::ToolTipRole, QString("initial left"));

  item = new QListWidgetItem(tr("initial right"), list);
  item->setData(Qt::ToolTipRole, QString("initial right"));

  item = new QListWidgetItem(tr("initial by diamond"), list);
  item->setData(Qt::ToolTipRole, QString("initial by diamond"));

  item = new QListWidgetItem(tr("accepting"), list);
  item->setData(Qt::ToolTipRole, QString("accepting"));

  item = new QListWidgetItem(tr("accepting by double"), list);
  item->setData(Qt::ToolTipRole, QString("accepting by double"));

  item = new QListWidgetItem(tr("accepting by arrow"), list);
  item->setData(Qt::ToolTipRole, QString("accepting by arrow"));

  item = new QListWidgetItem(tr("accepting text"), list);
  item->setData(Qt::ToolTipRole, QString("accepting text="));

  item = new QListWidgetItem(tr("accepting where"), list);
  item->setData(Qt::ToolTipRole, QString("accepting where=right"));

  item = new QListWidgetItem(tr("accepting above"), list);
  item->setData(Qt::ToolTipRole, QString("accepting above"));

  item = new QListWidgetItem(tr("accepting below"), list);
  item->setData(Qt::ToolTipRole, QString("accepting below"));

  item = new QListWidgetItem(tr("accepting left"), list);
  item->setData(Qt::ToolTipRole, QString("accepting left"));

  item = new QListWidgetItem(tr("accepting right"), list);
  item->setData(Qt::ToolTipRole, QString("accepting right"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzAutomataDialog::getAutomataOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzPetrinetDialog::XWTikzPetrinetDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz Patri-Net option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("place"), list);
  item->setData(Qt::ToolTipRole, QString("place"));

  item = new QListWidgetItem(tr("transition"), list);
  item->setData(Qt::ToolTipRole, QString("transition"));

  item = new QListWidgetItem(tr("pre and post"), list);
  item->setData(Qt::ToolTipRole, QString("pre and post"));

  item = new QListWidgetItem(tr("token"), list);
  item->setData(Qt::ToolTipRole, QString("token"));

  item = new QListWidgetItem(tr("children are tokens"), list);
  item->setData(Qt::ToolTipRole, QString("children are tokens"));

  item = new QListWidgetItem(tr("token distance"), list);
  item->setData(Qt::ToolTipRole, QString("token distance=1.1ex"));

  item = new QListWidgetItem(tr("tokens"), list);
  item->setData(Qt::ToolTipRole, QString("tokens=3"));

  item = new QListWidgetItem(tr("colored tokens"), list);
  item->setData(Qt::ToolTipRole, QString("colored tokens={black,black,red,blue}"));

  item = new QListWidgetItem(tr("structured tokens"), list);
  item->setData(Qt::ToolTipRole, QString("structured tokens={$x$,$y$,$z$}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzPetrinetDialog::getPetrinetOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzCircuitDialog::XWTikzCircuitDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz circuit option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("circuits"), list);
  item->setData(Qt::ToolTipRole, QString("circuits"));

  item = new QListWidgetItem(tr("circuit symbol unit"), list);
  item->setData(Qt::ToolTipRole, QString("circuit symbol unit=7pt"));

  item = new QListWidgetItem(tr("huge circuit symbols"), list);
  item->setData(Qt::ToolTipRole, QString("huge circuit symbols"));

  item = new QListWidgetItem(tr("large circuit symbols"), list);
  item->setData(Qt::ToolTipRole, QString("large circuit symbols"));

  item = new QListWidgetItem(tr("medium circuit symbols"), list);
  item->setData(Qt::ToolTipRole, QString("medium circuit symbols"));

  item = new QListWidgetItem(tr("small circuit symbols"), list);
  item->setData(Qt::ToolTipRole, QString("small circuit symbols"));

  item = new QListWidgetItem(tr("tiny circuit symbols"), list);
  item->setData(Qt::ToolTipRole, QString("tiny circuit symbols"));

  item = new QListWidgetItem(tr("circuit symbol size"), list);
  item->setData(Qt::ToolTipRole, QString("circuit symbol size=width 3 height 1"));

  item = new QListWidgetItem(tr("circuit declare symbol"), list);
  item->setData(Qt::ToolTipRole, QString("circuit declare symbol=foo"));

  item = new QListWidgetItem(tr("circuit handle symbol"), list);
  item->setData(Qt::ToolTipRole, QString("circuit handle symbol={draw,shape=rectangle,near start}"));

  item = new QListWidgetItem(tr("point up"), list);
  item->setData(Qt::ToolTipRole, QString("point up"));

  item = new QListWidgetItem(tr("point down"), list);
  item->setData(Qt::ToolTipRole, QString("point down"));

  item = new QListWidgetItem(tr("point left"), list);
  item->setData(Qt::ToolTipRole, QString("point left"));

  item = new QListWidgetItem(tr("point right"), list);
  item->setData(Qt::ToolTipRole, QString("point right"));

  item = new QListWidgetItem(tr("info"), list);
  item->setData(Qt::ToolTipRole, QString("info=[red]center:$3\\Omega$"));

  item = new QListWidgetItem(tr("info'"), list);
  item->setData(Qt::ToolTipRole, QString("info'=[red]center:$3\\Omega$"));

  item = new QListWidgetItem(tr("info sloped"), list);
  item->setData(Qt::ToolTipRole, QString("info sloped=[red]center:$3\\Omega$"));

  item = new QListWidgetItem(tr("info' sloped"), list);
  item->setData(Qt::ToolTipRole, QString("info' sloped=[red]center:$3\\Omega$"));

  item = new QListWidgetItem(tr("circuit declare unit"), list);
  item->setData(Qt::ToolTipRole, QString("circuit declare unit={my ohm}{O}"));

  item = new QListWidgetItem(tr("circuit declare annotation"), list);
  item->setData(Qt::ToolTipRole, QString("circuit declare annotation={circular annotation}{9pt}{(0pt,8pt) arc (-270:80:3.5pt)}"));

  item = new QListWidgetItem(tr("annotation arrow"), list);
  item->setData(Qt::ToolTipRole, QString("annotation arrow"));

  item = new QListWidgetItem(tr("circuit symbol open"), list);
  item->setData(Qt::ToolTipRole, QString("circuit symbol open/.style={thick,draw,fill=yellow}"));

  item = new QListWidgetItem(tr("circuit symbol filled"), list);
  item->setData(Qt::ToolTipRole, QString("circuit symbol filled/.style={thick,draw,fill=yellow}"));

  item = new QListWidgetItem(tr("circuit symbol lines"), list);
  item->setData(Qt::ToolTipRole, QString("circuit symbol lines/.style={thick,draw=red}"));

  item = new QListWidgetItem(tr("circuit symbol wires"), list);
  item->setData(Qt::ToolTipRole, QString("circuit symbol wires/.style={draw,very thick}"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzCircuitDialog::getCircuitOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzCircuitLogicDialog::XWTikzCircuitLogicDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz circuit logic option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("circuit logic"), list);
  item->setData(Qt::ToolTipRole, QString("circuit logic"));

  item = new QListWidgetItem(tr("circuit logic IEC"), list);
  item->setData(Qt::ToolTipRole, QString("circuit logic IEC"));

  item = new QListWidgetItem(tr("circuit logic US"), list);
  item->setData(Qt::ToolTipRole, QString("circuit logic US"));

  item = new QListWidgetItem(tr("circuit logic CDH"), list);
  item->setData(Qt::ToolTipRole, QString("circuit logic CDH"));

  item = new QListWidgetItem(tr("inputs"), list);
  item->setData(Qt::ToolTipRole, QString("inputs={inini}"));

  item = new QListWidgetItem(tr("logic gate inputs"), list);
  item->setData(Qt::ToolTipRole, QString("logic gate inputs={inverted, normal, inverted}"));

  item = new QListWidgetItem(tr("and gate"), list);
  item->setData(Qt::ToolTipRole, QString("and gate"));

  item = new QListWidgetItem(tr("nand gate"), list);
  item->setData(Qt::ToolTipRole, QString("nand gate"));

  item = new QListWidgetItem(tr("or gate"), list);
  item->setData(Qt::ToolTipRole, QString("or gate"));

  item = new QListWidgetItem(tr("nor gate"), list);
  item->setData(Qt::ToolTipRole, QString("nor gate"));

  item = new QListWidgetItem(tr("xor gate"), list);
  item->setData(Qt::ToolTipRole, QString("xor gate"));

  item = new QListWidgetItem(tr("xnor gate"), list);
  item->setData(Qt::ToolTipRole, QString("xnor gate"));

  item = new QListWidgetItem(tr("not gate"), list);
  item->setData(Qt::ToolTipRole, QString("not gate"));

  item = new QListWidgetItem(tr("buffer gate"), list);
  item->setData(Qt::ToolTipRole, QString("buffer gate"));

  item = new QListWidgetItem(tr("logic gate inverted radius"), list);
  item->setData(Qt::ToolTipRole, QString("logic gate inverted radius=4pt"));

  item = new QListWidgetItem(tr("logic gate input sep"), list);
  item->setData(Qt::ToolTipRole, QString("logic gate input sep=.125cm"));

  item = new QListWidgetItem(tr("logic gate anchors use bounding box"), list);
  item->setData(Qt::ToolTipRole, QString("logic gate anchors use bounding box=true"));

  item = new QListWidgetItem(tr("and gate IEC symbol"), list);
  item->setData(Qt::ToolTipRole, QString("and gate IEC symbol=\\&"));

  item = new QListWidgetItem(tr("nand gate IEC symbol"), list);
  item->setData(Qt::ToolTipRole, QString("nand gate IEC symbol=\\&"));

  item = new QListWidgetItem(tr("or gate IEC symbol"), list);
  item->setData(Qt::ToolTipRole, QString("or gate IEC symbol=$\\geq1$"));

  item = new QListWidgetItem(tr("nor gate IEC symbol"), list);
  item->setData(Qt::ToolTipRole, QString("nor gate IEC symbol=$\\geq1$"));

  item = new QListWidgetItem(tr("xor gate IEC symbol"), list);
  item->setData(Qt::ToolTipRole, QString("xor gate IEC symbol=$=1$"));

  item = new QListWidgetItem(tr("xnor gate IEC symbol"), list);
  item->setData(Qt::ToolTipRole, QString("xnor gate IEC symbol=$=1$"));

  item = new QListWidgetItem(tr("not gate IEC symbol"), list);
  item->setData(Qt::ToolTipRole, QString("not gate IEC symbol=$1$"));

  item = new QListWidgetItem(tr("buffer gate IEC symbol"), list);
  item->setData(Qt::ToolTipRole, QString("buffer gate IEC symbol=$1$"));

  item = new QListWidgetItem(tr("logic gate IEC symbol align"), list);
  item->setData(Qt::ToolTipRole, QString("logic gate IEC symbol align={bottom, right}"));

  item = new QListWidgetItem(tr("logic gate IEC symbol align"), list);
  item->setData(Qt::ToolTipRole, QString("logic gate IEC symbol color=red"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzCircuitLogicDialog::getCircuitLogicOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}

XWTikzCircuitElectricalDialog::XWTikzCircuitElectricalDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setWindowTitle(tr("Tikz circuit electrical option"));

  list = new QListWidget;

  QListWidgetItem *item = new QListWidgetItem(tr("circuit ee"), list);
  item->setData(Qt::ToolTipRole, QString("circuit ee"));

  item = new QListWidgetItem(tr("circuit ee IEC"), list);
  item->setData(Qt::ToolTipRole, QString("circuit ee IEC"));

  item = new QListWidgetItem(tr("current direction"), list);
  item->setData(Qt::ToolTipRole, QString("current direction"));

  item = new QListWidgetItem(tr("current direction'"), list);
  item->setData(Qt::ToolTipRole, QString("current direction'"));

  item = new QListWidgetItem(tr("resistor"), list);
  item->setData(Qt::ToolTipRole, QString("resistor"));

  item = new QListWidgetItem(tr("inductor"), list);
  item->setData(Qt::ToolTipRole, QString("inductor"));

  item = new QListWidgetItem(tr("capacitor"), list);
  item->setData(Qt::ToolTipRole, QString("capacitor"));

  item = new QListWidgetItem(tr("battery"), list);
  item->setData(Qt::ToolTipRole, QString("battery"));

  item = new QListWidgetItem(tr("bulb"), list);
  item->setData(Qt::ToolTipRole, QString("bulb"));

  item = new QListWidgetItem(tr("current source"), list);
  item->setData(Qt::ToolTipRole, QString("current source"));

  item = new QListWidgetItem(tr("voltage source"), list);
  item->setData(Qt::ToolTipRole, QString("voltage source"));

  item = new QListWidgetItem(tr("ground"), list);
  item->setData(Qt::ToolTipRole, QString("ground"));

  item = new QListWidgetItem(tr("diode"), list);
  item->setData(Qt::ToolTipRole, QString("diode"));

  item = new QListWidgetItem(tr("Zener diode"), list);
  item->setData(Qt::ToolTipRole, QString("Zener diode"));

  item = new QListWidgetItem(tr("Schottky diode"), list);
  item->setData(Qt::ToolTipRole, QString("Schottky diode"));

  item = new QListWidgetItem(tr("tunnel diode"), list);
  item->setData(Qt::ToolTipRole, QString("tunnel diode"));

  item = new QListWidgetItem(tr("backward diode"), list);
  item->setData(Qt::ToolTipRole, QString("backward diode"));

  item = new QListWidgetItem(tr("breakdown diode"), list);
  item->setData(Qt::ToolTipRole, QString("breakdown diode"));

  item = new QListWidgetItem(tr("contact"), list);
  item->setData(Qt::ToolTipRole, QString("contact"));

  item = new QListWidgetItem(tr("make contact"), list);
  item->setData(Qt::ToolTipRole, QString("make contact"));

  item = new QListWidgetItem(tr("break contact"), list);
  item->setData(Qt::ToolTipRole, QString("break contact"));

  item = new QListWidgetItem(tr("ampere"), list);
  item->setData(Qt::ToolTipRole, QString("ampere"));

  item = new QListWidgetItem(tr("volt"), list);
  item->setData(Qt::ToolTipRole, QString("volt"));

  item = new QListWidgetItem(tr("ohm"), list);
  item->setData(Qt::ToolTipRole, QString("ohm"));

  item = new QListWidgetItem(tr("ohm'"), list);
  item->setData(Qt::ToolTipRole, QString("ohm'"));

  item = new QListWidgetItem(tr("ohm sloped"), list);
  item->setData(Qt::ToolTipRole, QString("ohm sloped"));

  item = new QListWidgetItem(tr("ohm' sloped"), list);
  item->setData(Qt::ToolTipRole, QString("ohm' sloped"));

  item = new QListWidgetItem(tr("siemens"), list);
  item->setData(Qt::ToolTipRole, QString("siemens"));

  item = new QListWidgetItem(tr("henry"), list);
  item->setData(Qt::ToolTipRole, QString("henry"));

  item = new QListWidgetItem(tr("farad"), list);
  item->setData(Qt::ToolTipRole, QString("farad"));

  item = new QListWidgetItem(tr("coulomb"), list);
  item->setData(Qt::ToolTipRole, QString("coulomb"));

  item = new QListWidgetItem(tr("voltampere"), list);
  item->setData(Qt::ToolTipRole, QString("voltampere"));

  item = new QListWidgetItem(tr("watt"), list);
  item->setData(Qt::ToolTipRole, QString("watt"));

  item = new QListWidgetItem(tr("hertz"), list);
  item->setData(Qt::ToolTipRole, QString("hertz"));

  item = new QListWidgetItem(tr("light emitting"), list);
  item->setData(Qt::ToolTipRole, QString("light emitting"));

  item = new QListWidgetItem(tr("light emitting'"), list);
  item->setData(Qt::ToolTipRole, QString("light emitting'"));

  item = new QListWidgetItem(tr("light dependent"), list);
  item->setData(Qt::ToolTipRole, QString("light dependent"));

  item = new QListWidgetItem(tr("direction info"), list);
  item->setData(Qt::ToolTipRole, QString("direction info"));

  item = new QListWidgetItem(tr("adjustable"), list);
  item->setData(Qt::ToolTipRole, QString("adjustable"));

  item = new QListWidgetItem(tr("direction ee arrow"), list);
  item->setData(Qt::ToolTipRole, QString("direction ee arrow"));

  item = new QListWidgetItem(tr("/pgf/generic circle IEC/before background"), list);
  item->setData(Qt::ToolTipRole, QString("/pgf/generic circle IEC/before background"));

  item = new QListWidgetItem(tr("/pgf/generic diode IEC/before background"), list);
  item->setData(Qt::ToolTipRole, QString("/pgf/generic diode IEC/before background"));

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(list);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  resize(400, 300);
}

QString XWTikzCircuitElectricalDialog::getCircuitElectricalOption()
{
  QListWidgetItem *item = list->currentItem();
  if (!item)
    return QString();

  return item->data(Qt::ToolTipRole).toString();
}
