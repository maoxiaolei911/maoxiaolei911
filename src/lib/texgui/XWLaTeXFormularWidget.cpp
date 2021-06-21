/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWLaTeXFormularWidget.h"

XWLaTeXFormularToolBox::XWLaTeXFormularToolBox(QWidget * parent)
:QToolBox(parent)
{
  XWLaTeXFormularWidget * widget = new XWSetFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Set"));

  widget = new XWFunctionFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Function"));

  widget = new XWExponentFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Exponent"));

  widget = new XWLogarithmFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Logarithm"));

  widget = new XWGeometryFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Geometry"));

  widget = new XWStatisticsFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Statistics"));

  widget = new XWTrigonometricFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Trigonometric"));

  widget = new XWCalculusFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Calculus"));

  widget = new XWMatricFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Matric and Determinant"));

  widget = new XWTransformationFormularWidget(this);
  connect(widget, SIGNAL(formularClicked(const QString &)), this, SIGNAL(formularClicked(const QString &)));
  addItem(widget,tr("Transformation"));
}

XWLaTeXFormularWidget::XWLaTeXFormularWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));
}

void XWLaTeXFormularWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  emit formularClicked(text);
}

QWidget * XWLaTeXFormularWidget::createButton(int iconwidth,int iconheight,
                          const QString & imgfile, 
                          const QString & formular)
{
  QSize iconSize(iconwidth, iconheight);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(formular);
  button->setToolTip(formular);

  button->setIcon(QIcon(QPixmap(imgfile)));
  button->setCheckable(true);
  buttonGroup->addButton(button);
  return button;
}

XWSetFormularWidget::XWSetFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(48, 25, ":/images/N.png","\\mathbf{N}"),0,0);
  layout->addWidget(createButton(48, 25, ":/images/NP.png","\\mathbf{N}_+"),0,1);
  layout->addWidget(createButton(48, 25, ":/images/NS.png","\\mathbf{N}^{\\ast}"),0,2);
  layout->addWidget(createButton(48, 25, ":/images/Z.png","\\mathbf{Z}"),0,3);

  layout->addWidget(createButton(48, 25, ":/images/Q.png","\\mathbf{Q}"),1,0);
  layout->addWidget(createButton(48, 25, ":/images/R.png","\\mathbf{R}"),1,1);
  layout->addWidget(createButton(48, 25, ":/images/RP.png","\\mathbf{R}^+"),1,2);
  layout->addWidget(createButton(48, 25, ":/images/CUA.png","\\complement_UA"),1,3);

  setLayout(layout);
}

XWFunctionFormularWidget::XWFunctionFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(48, 25, ":/images/Deltax.png","\\Delta x"),0,0);
  layout->addWidget(createButton(67, 25, ":/images/f-1x.png","f^{-1}(x)"),0,1);
  layout->addWidget(createButton(60, 25, ":/images/fx_2.png","f(x_2)"),0,2);

  layout->addWidget(createButton(92, 25, ":/images/fArarrowB.png","f:A\\rightarrow B"),1,0,1,2);

  layout->addWidget(createButton(135, 50, ":/images/b24ac.png","\\frac{-b\\pm\\sqrt{b^2-4ac}}{2a}"),2,0,2,3);

  layout->addWidget(createButton(222, 75, ":/images/fenduan.png",
                 "|x|=\\left\\{\\begin{array}{rl}\n"
                 "-x & x\\in (-\\infty,0)\\\\\n"
                 "0 & x = 0 \\\\\n"
                 "x & x \\in (0,\\infty)\\\\\n"
                 "\\end{array}\\right."),3,0,3,5);

  setLayout(layout);
}

XWExponentFormularWidget::XWExponentFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(40, 25, ":/images/an.png","a^n"),0,0);
  layout->addWidget(createButton(44, 25, ":/images/sqrtan.png","\\sqrt[n]{a}"),0,1);
  layout->addWidget(createButton(54, 25, ":/images/aapb.png","a^{\\alpha + \\beta}"),0,2);
  layout->addWidget(createButton(52, 25, ":/images/aab.png","\\left(a^{\\alpha}\\right)^{\\beta}"),0,3);

  layout->addWidget(createButton(32, 50, ":/images/am-n.png","\\frac{a^m}{a^n}"),1,0,2,1);

  setLayout(layout);
}

XWLogarithmFormularWidget::XWLogarithmFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(61, 25, ":/images/logaN.png","\\log_aN"),0,0);
  layout->addWidget(createButton(42, 25, ":/images/lgN.png","\\lg N"),0,1);
  layout->addWidget(createButton(43, 25, ":/images/lnN.png","\\ln N"),0,2);

  setLayout(layout);
}

XWGeometryFormularWidget::XWGeometryFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(45, 25, ":/images/45circ.png","45^{\\circ}"),0,0);
  layout->addWidget(createButton(48, 25, ":/images/rarrowAB.png","\\overrightarrow{AB}"),0,1);
  layout->addWidget(createButton(45, 25, ":/images/absAB.png","|AB|"),0,2);
  layout->addWidget(createButton(81, 25, ":/images/absx2x1.png","|x_2-x_1|"),0,3);

  layout->addWidget(createButton(58, 25, ":/images/l1l2.png","l_1\\perp l_2"),1,0);
  layout->addWidget(createButton(129, 28, ":/images/a1an.png","(a_1,a_2,\\dots,a_n)"),1,1,1,3);

  layout->addWidget(createButton(159, 25, ":/images/xa2yb2.png","(x-a)^2+(y-b)^2"),2,0,1,4);

  layout->addWidget(createButton(215, 25, ":/images/sqrtx2x1y2y1.png","\\sqrt{(x_2-x_1)^2+(y_2-y_1)^2}"),3,0,1,5);

  layout->addWidget(createButton(312, 25, ":/images/sqrtx2x1y2y1z2z1.png",
             "\\sqrt{(x_2-x_1)^2+(y_2-y_1)^2+(z_2-z_1)^2}"),4,0,1,7);

  layout->addWidget(createButton(73, 50, ":/images/x2px12.png","\\frac{x_2+x_1}{2}"),5,0,2,2);
  layout->addWidget(createButton(80, 50, ":/images/x2py22.png","\\frac{x^2}{a^2}+\\frac{y^2}{b^2}"),5,2,2,2);
  layout->addWidget(createButton(80, 50, ":/images/x2sy22.png","\\frac{x^2}{a^2}-\\frac{y^2}{b^2}"),5,4,2,2);

  layout->addWidget(createButton(224, 50, ":/images/A1xB1yC1A2xB2yC2.png",
       "\\left\\{\\begin{array}{lll}A_1x+B_1y+C_1 & = & 0\\\\\nA_2x+B_2y+C_2 & = & 0\\\\\n\\end{array}\\right."),7,0,2,5);

  setLayout(layout);
}

XWStatisticsFormularWidget::XWStatisticsFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(41, 25, ":/images/Cmn.png","C^m_n"),0,0);
  layout->addWidget(createButton(43, 25, ":/images/haty.png","\\hat{y}"),0,1);
  layout->addWidget(createButton(41, 25, ":/images/olA.png","\\overline{A}"),0,2);
  layout->addWidget(createButton(64, 25, ":/images/olxshatbolx.png","\\overline{x}-\\hat{b}\\overline{x}"),0,3);
  
  layout->addWidget(createButton(98, 39, ":/images/sumxisolx.png","\\sum (x_i-\\overline{x})"),1,0,2,3);
  layout->addWidget(createButton(152, 52, ":/images/suminyisasbxi.png","\\sum^n_{i=1}(y_i-a-bx_i)^2"),1,3,2,3);

  layout->addWidget(createButton(323, 37, ":/images/sumxisolx2.png",
         "(x_1-\\overline{x})^2+(x_2-\\overline{x})^2+\\cdots+(x_n-\\overline{x})^2"),3,0,1,7);

  layout->addWidget(createButton(128, 50, ":/images/Normality.png",
                               "\\frac{1}{\\sigma \\sqrt{2\\pi}}e^{-\\frac{(x-\\mu)^2}{2\\sigma^2}}"),4,0,2,3);

  layout->addWidget(createButton(322, 50, ":/images/avgvariance.png",
        "\\frac{(x_1-\\overline{x})^2+(x_2-\\overline{x})^2+\\cdots+(x_n-\\overline{x})^2}{n}"),6,0,2,7);

  layout->addWidget(createButton(342, 50, ":/images/sqrtavgvariance.png",
        "\\sqrt{\\frac{(x_1-\\overline{x})^2+(x_2-\\overline{x})^2+\\cdots+(x_n-\\overline{x})^2}{n}}"),8,0,2,8);

  layout->addWidget(createButton(236, 51, ":/images/sqrtsumxi2yi2.png",
      "\\frac{\\sum (x_iy_i)-n\\overline{xy}}{\\sqrt{(\\sum x^2_i-nx^2)(\\sum y^2_i-ny^2)}}"),10,0,2,6);

  layout->addWidget(createButton(200, 51, ":/images/sumxi2yi2.png",
      "\\frac{\\sum^n_{i=1}x_iy_i-n\\overline{x}\\overline{y}}{\\sum^n_{i=1}x^2_i-n\\overline{x}}"),12,0,2,5);

  setLayout(layout);
}

XWTrigonometricFormularWidget::XWTrigonometricFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(45, 22, ":/images/sin.png","\\sin\\alpha"),0,0);
  layout->addWidget(createButton(45, 22, ":/images/cos.png","\\cos\\alpha"),0,1);
  layout->addWidget(createButton(47, 22, ":/images/tan.png","\\tan\\alpha"),0,2);
  layout->addWidget(createButton(45, 22, ":/images/cot.png","\\cot\\alpha"),0,3);
  layout->addWidget(createButton(43, 22, ":/images/sec.png","\\sec\\alpha"),0,4);
  layout->addWidget(createButton(44, 22, ":/images/csc.png","\\csc\\alpha"),0,5);

  layout->addWidget(createButton(55, 26, ":/images/sin2.png","\\sin^2\\alpha"),1,0);
  layout->addWidget(createButton(51, 26, ":/images/cos2.png","\\cos^2\\alpha"),1,1);
  layout->addWidget(createButton(116, 26, ":/images/asinwx.png","A\\sin(\\omega x+\\varphi)"),1,2,1,3);

  layout->addWidget(createButton(31, 50, ":/images/2piw.png","\\frac{2\\pi}{\\omega}"),2,0,2,1);
  layout->addWidget(createButton(83, 50, ":/images/arcsin.png","\\arcsin\\frac{\\sqrt{2}}{2}"),2,1,2,2);
  layout->addWidget(createButton(84, 50, ":/images/arccos.png","\\arccos\\frac{\\sqrt{3}}{2}"),2,3,2,2);
  layout->addWidget(createButton(83, 50, ":/images/arctan.png","\\arctan\\frac{\\sqrt{3}}{3}"),2,5,2,2);

  setLayout(layout);
}

XWCalculusFormularWidget::XWCalculusFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(203, 50, ":/images/Derivative.png",
           "\\lim_{\\Delta\\to 0}\\frac{f(x_0+\\Delta x)-f(x_0)}{\\Delta x}"),0,0,2,5);

  layout->addWidget(createButton(109, 50, ":/images/integral.png",
           "\\sum^{n-1}_{i=0}f(\\xi_i)\\Delta X"),1,0,2,3);
  layout->addWidget(createButton(90, 50, ":/images/int.png", "\\int^b_af(x)dx"),1,2,2,2);

  layout->addWidget(createButton(140, 53, ":/images/integral1.png", 
                       "\\lim_{\\lambda\\to 0}\\sum^{n-1}_{i=0}f(\\xi_i)\\Delta X"),3,0,2,4);

  setLayout(layout);
}

XWMatricFormularWidget::XWMatricFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(34, 24, ":/images/AT.png", "\\mathbf{A}^T"),0,0);
  layout->addWidget(createButton(56, 24, ":/images/ATT.png", "(\\mathbf{A}^T)^T"),0,1,1,2);
  layout->addWidget(createButton(101, 24, ":/images/vector.png", "(a_1a_2\\dots a_n)"),0,2,1,3);

  layout->addWidget(createButton(220, 104, ":/images/PMatrix.png", 
         "\\left(\\begin{array}{cccc}\na_{11} & a_{12} & \\dots & a_{1n}\\\\\n"
          "a_{21} & a_{22} & \\dots & a_{2n}\\\\\n\\vdots & vdots & \\ddots & \\vdots\\\\\n"
          "a_{m1} & a_{m2} & \\dots & a_{mn}\\\\\n\\end{array}\\right)"),1,0,5,5);

  layout->addWidget(createButton(219, 102, ":/images/SMatrix.png", 
         "\\left[\\begin{array}{cccc}\na_{11} & a_{12} & \\dots & a_{1n}\\\\\n"
          "a_{21} & a_{22} & dots & a_{2n}\\\\\n\\vdots & \\vdots & \\ddots & \\vdots\\\\\n"
          "a_{m1} & a_{m2} & \\dots & a_{mn}\\\\\n\\end{array}\\right]"),6,0,5,5);

  layout->addWidget(createButton(196, 105, ":/images/Det.png", 
         "\\left|\\begin{array}{cccc}\na_{11} & a_{12} & \\dots & a_{1n}\\\\\n"
          "a_{21} & a_{22} & \\dots & a_{2n}\\\\\n\\vdots & \\vdots & \\ddots & \\vdots\\\\\n"
          "a_{m1} & a_{m2} & \\dots & a_{mn}\\\\\n\\end{array}\\right|"),11,0,5,5);

  layout->addWidget(createButton(74, 107, ":/images/PColVect.png", 
         "\\left(\\begin{array}{c}\nb_1\\\\\nb_2\\\\\n\\vdots\\\\\nb_m\\\\\n\\end{array}\\right)"),16,0,5,2);

  layout->addWidget(createButton(67, 104, ":/images/SColVect.png", 
         "\\left[\\begin{array}{c}\nb_1\\\\\nb_2\\\\\n\\vdots\\\\\nb_m\\\\\n\\end{array}\\right]"),16,2,5,2);

  setLayout(layout);
}

XWTransformationFormularWidget::XWTransformationFormularWidget(QWidget * parent)
:XWLaTeXFormularWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  layout->addWidget(createButton(39, 33, ":/images/stackrel.png", "\\stackrel{\\sigma}{\\longrightarrow}"),0,0);

  setLayout(layout);
}
