/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWFontSea.h"
#include "XWFontFileID.h"
#include "XWFTFontFile.h"
#include "XWFTFontEngine.h"
#include "XWFTFont.h"
#include "XWLaTeXSymbolWidget.h"

#define XW_LTX_MATH_ICON_SIZE 30
#define XW_LTX_MATH_FONT_SIZE 20

#define XW_LTX_MATH_LAYOUT_SIZE 1

static XWFTFontEngine * fontEngine = 0;

XWLaTeXSymbolWidget::XWLaTeXSymbolWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));
}

void XWLaTeXSymbolWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  emit symbolClicked(text);
}

QWidget * XWLaTeXSymbolWidget::createButton(const QPainterPath & path, 
                           const QString & cmd)
{
  QSize iconSize(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(cmd);
  button->setToolTip(cmd);
  QPixmap pixmap(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  pixmap.fill(Qt::white);
  QPainter painter(&pixmap);
  painter.setClipRect(0,0,XW_LTX_MATH_ICON_SIZE,XW_LTX_MATH_ICON_SIZE);
  painter.drawPath(path);

  QIcon icon(pixmap);
  button->setIcon(icon);
  button->setCheckable(true);

  buttonGroup->addButton(button);
  return button;
}

QWidget * XWLaTeXSymbolWidget::createButton(XWFTFont * font, int c, 
                           const QString & cmd)
{
  QSize iconSize(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(cmd);
  button->setToolTip(cmd);
  if (font)
  {
    QPixmap pixmap(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setClipRect(0,0,XW_LTX_MATH_ICON_SIZE,XW_LTX_MATH_ICON_SIZE);
    int x = 0;
    int y = 0;
    font->drawChar(&painter, c, &x, &y, XW_LTX_MATH_ICON_SIZE);

    QIcon icon(pixmap);
    button->setIcon(icon);
    button->setCheckable(true);
  }

  buttonGroup->addButton(button);
  return button;
}

QWidget * XWLaTeXSymbolWidget::createButton(XWFTFont * font, int c, 
                           const QTransform & m, 
                           const QString & cmd)
{
  QSize iconSize(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(cmd);
  button->setToolTip(cmd);
  if (font)
  {
    QPixmap pixmap(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setWorldTransform(m);
    painter.setClipRect(0,0,XW_LTX_MATH_ICON_SIZE,XW_LTX_MATH_ICON_SIZE);
    int x = 0;
    int y = 0;
    font->drawChar(&painter, c, &x, &y, XW_LTX_MATH_ICON_SIZE);

    QIcon icon(pixmap);
    button->setIcon(icon);
    button->setCheckable(true);
  }

  buttonGroup->addButton(button);
  return button;
}

QWidget * XWLaTeXSymbolWidget::createButton(XWFTFont * font1, int ch1, 
                          XWFTFont * font2, int ch2, 
                          const QString & cmd)
{
  QSize iconSize(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(cmd);
  button->setToolTip(cmd);
  QPixmap pixmap(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  pixmap.fill(Qt::white);
  QPainter painter(&pixmap);
  painter.setClipRect(0,0,XW_LTX_MATH_ICON_SIZE,XW_LTX_MATH_ICON_SIZE);

  if (font1)
  {
    int x = 0;
    int y = 0;
    font1->drawChar(&painter, ch1, &x, &y, XW_LTX_MATH_ICON_SIZE);
  }

  if (font2)
  {
    int x = 0;
    int y = 0;
    font2->drawChar(&painter, ch2, &x, &y, XW_LTX_MATH_ICON_SIZE);
  }

  QIcon icon(pixmap);
  button->setIcon(icon);
  button->setCheckable(true);

  buttonGroup->addButton(button);
  return button;
}

QWidget * XWLaTeXSymbolWidget::createButton(XWFTFont * font1, int ch1, 
                          XWFTFont * font2, int ch2,                            
                          const QTransform & m1,
                          const QTransform & m2,
                          const QString & cmd)
{
  QSize iconSize(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(cmd);
  button->setToolTip(cmd);
  QPixmap pixmap(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  pixmap.fill(Qt::white);
  QPainter painter(&pixmap);
  painter.setClipRect(0,0,XW_LTX_MATH_ICON_SIZE,XW_LTX_MATH_ICON_SIZE);

  if (font1)
  {
    painter.setWorldTransform(m1);
    int x = 0;
    int y = 0;
    font1->drawChar(&painter, ch1, &x, &y, XW_LTX_MATH_ICON_SIZE);
  }

  if (font2)
  {
    int x = 0;
    int y = 0;
    painter.setWorldTransform(m2);
    font2->drawChar(&painter, ch2, &x, &y, XW_LTX_MATH_ICON_SIZE);
  }

  QIcon icon(pixmap);
  button->setIcon(icon);
  button->setCheckable(true);

  buttonGroup->addButton(button);
  return button;
}

QWidget * XWLaTeXSymbolWidget::createButton(XWFTFont * font1, int ch1, 
                          XWFTFont * font2, int ch2,  
                          XWFTFont * font3, int ch3,                            
                          const QTransform & m1,
                          const QTransform & m2,
                          const QTransform & m3,
                          const QString & cmd)
{
  QSize iconSize(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(cmd);
  button->setToolTip(cmd);
  QPixmap pixmap(XW_LTX_MATH_ICON_SIZE, XW_LTX_MATH_ICON_SIZE);
  pixmap.fill(Qt::white);
  QPainter painter(&pixmap);
  painter.setClipRect(0,0,XW_LTX_MATH_ICON_SIZE,XW_LTX_MATH_ICON_SIZE);

  if (font1)
  {
    painter.setWorldTransform(m1);
    int x = 0;
    int y = 0;
    font1->drawChar(&painter, ch1, &x, &y, XW_LTX_MATH_ICON_SIZE);
  }

  if (font2)
  {
    int x = 0;
    int y = 0;
    painter.setWorldTransform(m2);
    font2->drawChar(&painter, ch2, &x, &y, XW_LTX_MATH_ICON_SIZE);
  }

  if (font3)
  {
    int x = 0;
    int y = 0;
    painter.setWorldTransform(m3);
    font3->drawChar(&painter, ch3, &x, &y, XW_LTX_MATH_ICON_SIZE);
  }

  QIcon icon(pixmap);
  button->setIcon(icon);
  button->setCheckable(true);

  buttonGroup->addButton(button);
  return button;
}

XWFTFont * XWLaTeXSymbolWidget::loadFont(const QString & filename, int index)
{
  QString strname = QString("%1%2").arg(filename).arg(index);
  QByteArray ba = strname.toAscii();
  XWFontFileID * id = new XWFontFileID(ba.constData());
  if (!fontEngine)
    fontEngine = new XWFTFontEngine(true);
  XWFTFontFile * ftfontFile = fontEngine->getFontFile(id);
  if (ftfontFile)
  {
  	delete id;
  	id = 0;
  }
  else
  {
    XWFontSea sea;
    QString fn = sea.findFile(filename);
    if (fn.isEmpty())
    {
  	  delete id;
  	  return 0;
    }

    ba = QFile::encodeName(fn);
  	ftfontFile = fontEngine->load(id, ba.data(), index, 0);
  	if (!ftfontFile)
  	{
  		delete id;
  		return 0;
  	}
  }
  
  return fontEngine->getFont(ftfontFile, XW_LTX_MATH_FONT_SIZE, 1.0, 0.0, 0.0, false,false);
}

XWGreeceLetterWidget::XWGreeceLetterWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont * font = loadFont("cmmi10.pfb", 0);
  if (font)
    font->setCharMap(1);

  layout->addWidget(createButton(font, 0x0B, "\\alpha"),0,0);
  layout->addWidget(createButton(font, 0x0C, "\\beta"),0,1);
  layout->addWidget(createButton(font, 0x0D, "\\gamma"),0,2);
  layout->addWidget(createButton(font, 0x0E, "\\delta"),0,3);
  layout->addWidget(createButton(font, 0x0F, "\\epsilon"),0,4);
  layout->addWidget(createButton(font, 0x10, "\\zeta"),0,5);
  layout->addWidget(createButton(font, 0x11, "\\eta"),0,6);
  layout->addWidget(createButton(font, 0x12, "\\theta"),0,7);

  layout->addWidget(createButton(font, 0x13, "\\iota"),1,0);
  layout->addWidget(createButton(font, 0x14, "\\kappa"),1,1);
  layout->addWidget(createButton(font, 0x15, "\\lambda"),1,2);
  layout->addWidget(createButton(font, 0x16, "\\mu"),1,3);
  layout->addWidget(createButton(font, 0x17, "\\nu"),1,4);
  layout->addWidget(createButton(font, 0x18, "\\xi"),1,5);
  layout->addWidget(createButton(font, 0x19, "\\pi"),1,6);
  layout->addWidget(createButton(font, 0x1A, "\\rho"),1,7);

  layout->addWidget(createButton(font, 0x1B, "\\sigma"),2,0);
  layout->addWidget(createButton(font, 0x1C, "\\tau"),2,1);
  layout->addWidget(createButton(font, 0x1D, "\\upsilon"),2,2);
  layout->addWidget(createButton(font, 0x1E, "\\phi"),2,3);
  layout->addWidget(createButton(font, 0x1F, "\\chi"),2,4);
  layout->addWidget(createButton(font, 0x20, "\\psi"),2,5);
  layout->addWidget(createButton(font, 0x21, "\\omega"),2,6);
  layout->addWidget(createButton(font, 0x22, "\\varepsilon"),2,7);

  layout->addWidget(createButton(font, 0x23, "\\vartheta"),3,0);
  layout->addWidget(createButton(font, 0x24, "\\varpi"),3,1);
  layout->addWidget(createButton(font, 0x25, "\\varrho"),3,2);
  layout->addWidget(createButton(font, 0x26, "\\varsigma"),3,3);
  layout->addWidget(createButton(font, 0x27, "\\varphi"),3,4);
  layout->addWidget(createButton(font, 0x00, "\\Gamma"),3,5);
  layout->addWidget(createButton(font, 0x01, "\\Delta"),3,6);
  layout->addWidget(createButton(font, 0x02, "\\Theta"),3,7);

  layout->addWidget(createButton(font, 0x03, "\\Lambda"),4,0);
  layout->addWidget(createButton(font, 0x04, "\\Xi"),4,1);
  layout->addWidget(createButton(font, 0x05, "\\Pi"),4,2);
  layout->addWidget(createButton(font, 0x06, "\\Sigma"),4,3);
  layout->addWidget(createButton(font, 0x07, "\\Upsilon"),4,4);
  layout->addWidget(createButton(font, 0x08, "\\Phi"),4,5);
  layout->addWidget(createButton(font, 0x09, "\\Psi"),4,6);
  layout->addWidget(createButton(font, 0x0A, "\\Omega"),4,7);

  layout->addWidget(createButton(font, 0x40, "\\aleph"),5,0);

  setLayout(layout);
}

XWBinRelOperatorWidget::XWBinRelOperatorWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("cmsy10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x14, "\\leq"),0,0);
  layout->addWidget(createButton(font1, 0x15, "\\geq"),0,1);
  layout->addWidget(createButton(font1, 0x11, "\\equiv"),0,2);
  layout->addWidget(createButton(font1, 0x1C, "\\ll"),0,3);
  layout->addWidget(createButton(font1, 0x1D, "\\gg"),0,4);
  XWFTFont *font2 = loadFont("cmr10.pfb", 0);
  QTransform dot;
  dot.translate(0,-2);
  QTransform eq;
  eq.translate(-4,9);
  layout->addWidget(createButton(font2, 0x2E, font2, 0x3D,dot,eq,"\\doteq"),0,5);
  layout->addWidget(createButton(font1, 0x1E, "\\prec"),0,6);
  layout->addWidget(createButton(font1, 0x1F, "\\succ"),0,7);

  layout->addWidget(createButton(font1, 0x18, "\\sim"),1,0);  
  layout->addWidget(createButton(font1, 0x16, "\\preceq"),1,1);
  layout->addWidget(createButton(font1, 0x17, "\\succeq"),1,2);
  layout->addWidget(createButton(font1, 0x27, "\\simeq"),1,3);
  layout->addWidget(createButton(font1, 0x1A, "\\subset"),1,4);
  layout->addWidget(createButton(font1, 0x1B, "\\supset"),1,5);
  layout->addWidget(createButton(font1, 0x19, "\\approx"),1,6);
  layout->addWidget(createButton(font1, 0x12, "\\subseteq"),1,7);

  layout->addWidget(createButton(font1, 0x13, "\\supseteq"),2,0);
  QTransform simeq;
  QTransform minus;
  minus.translate(-3,10);
  layout->addWidget(createButton(font1, 0x27,font1, 0x00, simeq, minus, "\\cong"),2,1);
  XWFTFont *font3 = loadFont("lasy10.pfb", 0);
  if (font3)
    font3->setCharMap(1);
  layout->addWidget(createButton(font3, 0x3C, "\\sqsubset"),2,2);
  layout->addWidget(createButton(font3, 0x3D, "\\sqsupset"),2,3);
  layout->addWidget(createButton(font3, 0x31, "\\Join"),2,4);
  layout->addWidget(createButton(font1, 0x76, "\\sqsubseteq"),2,5);
  layout->addWidget(createButton(font1, 0x77, "\\sqsupseteq"),2,6);
  XWFTFont *font4 = loadFont("cmmi10.pfb", 0);
  if (font4)
    font4->setCharMap(1);
  QTransform lt;
  lt.translate(-4,0);
  QTransform rt;
  rt.translate(4,0);
  layout->addWidget(createButton(font4, 0x2E, font4, 0x2F,lt, rt,"\\bowtie"),2,7);

  layout->addWidget(createButton(font1, 0x32, "\\in"),3,0);
  layout->addWidget(createButton(font1, 0x33, "\\ni"),3,1);
  layout->addWidget(createButton(font1, 0x2F, "\\propto"),3,2);
  layout->addWidget(createButton(font1, 0x60, "\\vdash"),3,3);
  layout->addWidget(createButton(font1, 0x61, "\\dashv"),3,4);
  QTransform vert;
  vert.translate(1,-5);
  eq.translate(-4,0);
  layout->addWidget(createButton(font1,0x6A,font2, 0x3D, vert, eq, "\\models"),3,5);
  layout->addWidget(createButton(font1, 0x6A, "\\mid"),3,6);
  layout->addWidget(createButton(font1, 0x6B, "\\parallel"),3,7);

  layout->addWidget(createButton(font1, 0x3F, "\\perp"),4,0);
  layout->addWidget(createButton(font4, 0x5E,"\\smile"),4,1);
  layout->addWidget(createButton(font4, 0x5F,"\\frown"),4,2);
  layout->addWidget(createButton(font1, 0x10, "\\asymp"),4,3);
  QTransform not,in;
  not.translate(4,-5);
  layout->addWidget(createButton(font1, 0x32, font1, 0x36,in,not,"\\notin"),4,4);
  eq.translate(1,-4);
  layout->addWidget(createButton(font1, 0x36, font2, 0x3D,not,eq,"\\neq"),4,5);

  setLayout(layout);
}

XWBinOperatorWidget::XWBinOperatorWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("cmsy10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x06, "\\pm"),0,0);
  layout->addWidget(createButton(font1, 0x07, "\\mp"),0,1);
  XWFTFont *font2 = loadFont("cmmi10.pfb", 0);
  if (font2)
    font2->setCharMap(1);
  layout->addWidget(createButton(font2, 0x2E, "\\triangleleft"),0,2);
  layout->addWidget(createButton(font1, 0x01, "\\cdot"),0,3);
  layout->addWidget(createButton(font1, 0x04, "\\div"),0,4);
  layout->addWidget(createButton(font2, 0x2F, "\\triangleright"),0,5);
  layout->addWidget(createButton(font1, 0x02, "\\times"),0,6);
  layout->addWidget(createButton(font1, 0x6E, "\\setminus"),0,7);

  layout->addWidget(createButton(font2, 0x3F, "\\star"),1,0);
  layout->addWidget(createButton(font1, 0x5C, "\\cup"),1,1);
  layout->addWidget(createButton(font1, 0x5B, "\\cap"),1,2);
  layout->addWidget(createButton(font1, 0x03, "\\ast"),1,3);
  layout->addWidget(createButton(font1, 0x74, "\\sqcup"),1,4);
  layout->addWidget(createButton(font1, 0x75, "\\sqcap"),1,5);
  layout->addWidget(createButton(font1, 0x0E, "\\circ"),1,6);
  layout->addWidget(createButton(font1, 0x5F, "\\vee"),1,7);

  layout->addWidget(createButton(font1, 0x5E, "\\wedge"),2,0);
  layout->addWidget(createButton(font1, 0x0F, "\\bullet"),2,1);
  layout->addWidget(createButton(font1, 0x08, "\\oplus"),2,2);
  layout->addWidget(createButton(font1, 0x09, "\\ominus"),2,3);
  layout->addWidget(createButton(font1, 0x05, "\\diamond"),2,4);
  layout->addWidget(createButton(font1, 0x0C, "\\odot"),2,5);
  layout->addWidget(createButton(font1, 0x0B, "\\oslash"),2,6);
  layout->addWidget(createButton(font1, 0x5D, "\\uplus"),2,7);

  layout->addWidget(createButton(font1, 0x0A, "\\otimes"),3,0);
  layout->addWidget(createButton(font1, 0x0D, "\\bigcirc"),3,1);
  layout->addWidget(createButton(font1, 0x71, "\\amalg"),3,2);
  layout->addWidget(createButton(font1, 0x34, "\\bigtriangleup"),3,3);
  layout->addWidget(createButton(font1, 0x35, "\\bigtriangledown"),3,4);
  layout->addWidget(createButton(font1, 0x79, "\\dagger"),3,5);
  XWFTFont *font3 = loadFont("lasy10.pfb", 0);
  if (font3)
    font3->setCharMap(1);
  layout->addWidget(createButton(font3, 0x01, "\\lhd"),3,6);
  layout->addWidget(createButton(font3, 0x03, "\\rhd"),3,7);

  layout->addWidget(createButton(font1, 0x7A, "\\ddagger"),4,0);
  layout->addWidget(createButton(font3, 0x02, "\\unlhd"),4,1);
  layout->addWidget(createButton(font3, 0x04, "\\unrhd"),4,2);
  layout->addWidget(createButton(font1, 0x6F, "\\wr"),4,3);

  setLayout(layout);
}

XWBigOperatorWidget::XWBigOperatorWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("cmex10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x50, "\\sum"),0,0);
  layout->addWidget(createButton(font1, 0x53, "\\bigcup"),0,1);
  layout->addWidget(createButton(font1, 0x57, "\\bigvee"),0,2);
  layout->addWidget(createButton(font1, 0x51, "\\prod"),0,3);
  layout->addWidget(createButton(font1, 0x54, "\\bigcap"),0,4);
  layout->addWidget(createButton(font1, 0x56, "\\bigwedge"),0,5);
  layout->addWidget(createButton(font1, 0x60, "\\coprod"),0,6);
  layout->addWidget(createButton(font1, 0x46, "\\bigsqcup"),0,7);

  layout->addWidget(createButton(font1, 0x55, "\\biguplus"),1,0);
  layout->addWidget(createButton(font1, 0x52, "\\int"),1,1);
  layout->addWidget(createButton(font1, 0x48, "\\oint"),1,2);
  layout->addWidget(createButton(font1, 0x4A, "\\bigodot"),1,3);
  layout->addWidget(createButton(font1, 0x4C, "\\bigoplus"),1,4);
  layout->addWidget(createButton(font1, 0x4E, "\\bigotimes"),1,5);

  setLayout(layout);
}

XWArrowWidget::XWArrowWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("cmsy10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x20, "\\gets"),0,0);
  QTransform leftarrow;
  leftarrow.translate(-7,0);
  QTransform minus;
  minus.translate(2,2);
  layout->addWidget(createButton(font1, 0x20, font1, 0x00,leftarrow,minus,"\\longleftarrow"),0,1);
  layout->addWidget(createButton(font1, 0x21, "\\to"),0,2);
  QTransform rightarrow;
  rightarrow.translate(1,0);
  minus.translate(-8,0);
  layout->addWidget(createButton(font1, 0x21, font1, 0x00,rightarrow,minus,"\\longrightarrow"),0,3);
  layout->addWidget(createButton(font1, 0x24, "\\leftrightarrow"),0,4);
  leftarrow.translate(-4,0);
  layout->addWidget(createButton(font1, 0x20, font1, 0x21,leftarrow,rightarrow,"\\longleftrightarrow"),0,5);
  layout->addWidget(createButton(font1, 0x28, "\\Leftarrow"),0,6);
  XWFTFont *font2 = loadFont("cmr10.pfb", 0);
  QTransform Leftarrow;
  Leftarrow.translate(-6,0);
  QTransform eq;
  eq.translate(1,2);
  layout->addWidget(createButton(font1, 0x28, font2, 0x3D,Leftarrow,eq,"\\Longleftarrow"),0,7);

  layout->addWidget(createButton(font1, 0x29, "\\Rightarrow"),1,0);
  QTransform Rightarrow;
  Rightarrow.translate(1,0);
  eq.translate(-7,0);
  layout->addWidget(createButton(font1, 0x29, font2, 0x3D,Rightarrow,eq,"\\Longrightarrow"),1,1);
  layout->addWidget(createButton(font1, 0x2C, "\\Leftrightarrow"),1,2);
  layout->addWidget(createButton(font1, 0x28, font1, 0x29,Leftarrow,Rightarrow,"\\Longleftrightarrow"),1,3);
  QTransform mapsto;
  mapsto.translate(-3,-6);
  rightarrow.translate(-2,1);
  layout->addWidget(createButton(font1, 0x37, font1, 0x21,mapsto,rightarrow,"\\mapsto"),1,4);
  mapsto.translate(-6,-1);
  minus.translate(-6,1);
  layout->addWidget(createButton(font1, 0x37, font1, 0x00,font1,0x21,mapsto,minus,rightarrow,"\\longmapsto"),1,5);
  XWFTFont *font3 = loadFont("cmmi10.pfb", 0);
  if (font3)
    font3->setCharMap(1);
  QTransform rhook;
  rhook.translate(3,-1);
  leftarrow.translate(0,0);
  layout->addWidget(createButton(font1, 0x20, font3, 0x2D,leftarrow,rhook,"\\hookleftarrow"),1,6);
  rhook.translate(-10,0);
  rightarrow.translate(0,0);
  layout->addWidget(createButton(font1, 0x21, font3, 0x2C,rightarrow,rhook,"\\hookrightarrow"),1,7);

  layout->addWidget(createButton(font3, 0x28, "\\leftharpoonup"),2,0);
  layout->addWidget(createButton(font3, 0x2A, "\\rightharpoonup"),2,1);
  layout->addWidget(createButton(font3, 0x29, "\\leftharpoondown"),2,2);
  layout->addWidget(createButton(font3, 0x2B, "\\rightharpoondown"),2,3);
  QTransform leftharpoondown;
  leftharpoondown.translate(0,7);
  QTransform rightharpoonup;
  rightharpoonup.translate(0,-1);
  layout->addWidget(createButton(font3, 0x29, font3, 0x2A,leftharpoondown,rightharpoonup,"\\rightleftharpoons"),2,4);
  Leftarrow.translate(-2,0);
  Rightarrow.translate(2,0);
  layout->addWidget(createButton(font1, 0x28, font1, 0x29,Leftarrow,Rightarrow,"\\iff"),2,5);
  layout->addWidget(createButton(font1, 0x22, "\\uparrow"),2,6);
  layout->addWidget(createButton(font1, 0x23, "\\downarrow"),2,7);

  layout->addWidget(createButton(font1, 0x6C, "\\updownarrow"),3,0);
  layout->addWidget(createButton(font1, 0x2A, "\\Uparrow"),3,1);
  layout->addWidget(createButton(font1, 0x2B, "\\Downarrow"),3,2);
  layout->addWidget(createButton(font1, 0x6D, "\\Updownarrow"),3,3);
  layout->addWidget(createButton(font1, 0x25, "\\nearrow"),3,4);
  layout->addWidget(createButton(font1, 0x26, "\\searrow"),3,5);
  layout->addWidget(createButton(font1, 0x2E, "\\swarrow"),3,6);
  layout->addWidget(createButton(font1, 0x2D, "\\nwarrow"),3,7);

  XWFTFont *font4 = loadFont("lasy10.pfb", 0);
  if (font4)
    font4->setCharMap(1);
  layout->addWidget(createButton(font4, 0x3B, "\\leadsto"),4,0);

  setLayout(layout);
}

XWDelimiterWidget::XWDelimiterWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("cmsy10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x22, "\\uparrow"),0,0);
  layout->addWidget(createButton(font1, 0x23, "\\downarrow"),0,1);  
  layout->addWidget(createButton(font1, 0x6C, "\\updownarrow"),0,2);
  layout->addWidget(createButton(font1, 0x0A, "\\langle"),0,3);
  layout->addWidget(createButton(font1, 0x0B, "\\rangle"),0,4);
  layout->addWidget(createButton(font1, 0x6A, "\\vert"),0,5);
  layout->addWidget(createButton(font1, 0x62, "\\lfloor"),0,6);
  layout->addWidget(createButton(font1, 0x63, "\\rfloor"),0,7);

  layout->addWidget(createButton(font1, 0x64, "\\lceil"),1,0);
  layout->addWidget(createButton(font1, 0x6E, "\\backslash"),1,1);
  layout->addWidget(createButton(font1, 0x6D, "\\Updownarrow"),1,2);
  layout->addWidget(createButton(font1, 0x2A, "\\Uparrow"),1,3);
  layout->addWidget(createButton(font1, 0x2B, "\\Downarrow"),1,4);
  layout->addWidget(createButton(font1, 0x6B, "\\Vert"),1,5);
  layout->addWidget(createButton(font1, 0x65, "\\rceil"),1,6);

  XWFTFont *font2 = loadFont("cmex10.pfb", 0);
  if (font2)
    font2->setCharMap(1);
  layout->addWidget(createButton(font2, 0x3A, "\\lgroup"),2,0);
  layout->addWidget(createButton(font2, 0x3B, "\\rgroup"),2,1);
  layout->addWidget(createButton(font2, 0x40, "\\lmoustache"),2,2);
  layout->addWidget(createButton(font2, 0x3C, "\\arrowvert"),2,3);
  layout->addWidget(createButton(font2, 0x3D, "\\Arrowvert"),2,4);
  layout->addWidget(createButton(font2, 0x3E, "\\bracevert"),2,5);
  layout->addWidget(createButton(font2, 0x41, "\\rmoustache"),2,6);

  setLayout(layout);
}

XWOtherWidget::XWOtherWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("cmr10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  QTransform dot1,dot2,dot3;
  dot1.translate(-3,5);
  dot2.translate(0,5);
  dot3.translate(3,5);
  layout->addWidget(createButton(font1, 0x2E, font1, 0x2E,font1, 0x2E,dot1,dot2,dot3,"\\dots"),0,0);
  XWFTFont *font2 = loadFont("cmsy10.pfb", 0);
  if (font2)
    font2->setCharMap(1);
  QTransform cdot1,cdot2,cdot3;
  cdot1.translate(-3,0);
  cdot3.translate(3,0);
  layout->addWidget(createButton(font2, 0x01, font2, 0x01,font2, 0x01,cdot1,cdot2,cdot3,"\\cdots"),0,1);
  QTransform vdot1,vdot2,vdot3;
  vdot1.translate(0,-3);
  vdot3.translate(0,3);
  layout->addWidget(createButton(font1, 0x2E, font1, 0x2E,font1, 0x2E,vdot1,vdot2,vdot3,"\\vdots"),0,2);
  QTransform ddot1,ddot2,ddot3;
  ddot1.translate(-3,-3);
  ddot3.translate(3,3);
  layout->addWidget(createButton(font1, 0x2E, font1, 0x2E,font1, 0x2E,ddot1,ddot2,ddot3,"\\ddots"),0,3);
  QTransform tie,h;
  tie.translate(-4,-1);
  layout->addWidget(createButton(font1, 0x20, font1, 0x68,tie,h,"\\hbar"),0,4);
  XWFTFont *font3 = loadFont("cmmi10.pfb", 0);
  if (font3)
    font3->setCharMap(1);
  layout->addWidget(createButton(font3, 0x7B, "\\imath"),0,5);
  layout->addWidget(createButton(font3, 0x7C, "\\jmath"),0,6);
  layout->addWidget(createButton(font3, 0x60, "\\ell"),0,7);

  layout->addWidget(createButton(font2, 0x3C, "\\Re"),1,0);
  layout->addWidget(createButton(font2, 0x3D, "\\Im"),1,1);
  layout->addWidget(createButton(font2, 0x40, "\\aleph"),1,2);
  layout->addWidget(createButton(font3, 0x7D, "\\wp"),1,3);
  layout->addWidget(createButton(font2, 0x38, "\\forall"),1,4);
  layout->addWidget(createButton(font2, 0x39, "\\exists"),1,5);
  XWFTFont *font4 = loadFont("lasy10.pfb", 0);
  if (font4)
    font4->setCharMap(1);
  layout->addWidget(createButton(font4, 0x30, "\\mho"),1,6);
  layout->addWidget(createButton(font3, 0x40, "\\partial"),1,7);

  layout->addWidget(createButton(font2, 0x30, "\\prime"),2,0);
  layout->addWidget(createButton(font2, 0x3B, "\\emptyset"),2,1);
  layout->addWidget(createButton(font2, 0x31, "\\infty"),2,2);
  layout->addWidget(createButton(font2, 0x72, "\\nabla"),2,3);
  layout->addWidget(createButton(font2, 0x34, "\\triangle"),2,4);
  layout->addWidget(createButton(font4, 0x32, "\\Box"),2,5);
  layout->addWidget(createButton(font4, 0x33, "\\Diamond"),2,6);
  layout->addWidget(createButton(font2, 0x3F, "\\bot"),2,7);

  layout->addWidget(createButton(font2, 0x3E, "\\top"),3,0);
  QPainterPath angle;
  angle.moveTo(15,11);
  angle.lineTo(5,17);
  angle.lineTo(15,17);
  layout->addWidget(createButton(angle, "\\angle"),3,1);
  layout->addWidget(createButton(font2, 0x70, "\\surd"),3,2);
  layout->addWidget(createButton(font2, 0x7D, "\\diamondsuit"),3,3);
  layout->addWidget(createButton(font2, 0x7E, "\\heartsuit"),3,4);
  layout->addWidget(createButton(font2, 0x7C, "\\clubsuit"),3,5);
  layout->addWidget(createButton(font2, 0x7F, "\\spadesuit"),3,6);
  layout->addWidget(createButton(font2, 0x3A, "\\neg"),3,7);

  layout->addWidget(createButton(font3, 0x5B, "\\flat"),4,0);
  layout->addWidget(createButton(font3, 0x5C, "\\natural"),4,1);
  layout->addWidget(createButton(font3, 0x5D, "\\sharp"),4,2);
  XWFTFont *font5 = loadFont("lmr10.pfb", 0);
  if (font5)
    font5->setCharMap(1);
  layout->addWidget(createButton(font5, 0x86, "\\dag"),4,3);
  layout->addWidget(createButton(font5, 0xA7, "\\S"),4,4);
  layout->addWidget(createButton(font5, 0xA9, "\\copyright"),4,5);
  layout->addWidget(createButton(font5, 0xAE, "\\textregistered"),4,6);
  layout->addWidget(createButton(font5, 0x87, "\\ddag"),4,7);

  layout->addWidget(createButton(font5, 0xB6, "\\P"),5,0);
  layout->addWidget(createButton(font5, 0xA3, "\\pounds"),5,1);

  setLayout(layout);
}

XWAMSDelimiterLetterWidget::XWAMSDelimiterLetterWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("msam10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x70, "\\ulcorner"),0,0);
  layout->addWidget(createButton(font1, 0x71, "\\urcorner"),0,1);
  layout->addWidget(createButton(font1, 0x78, "\\llcorner"),0,2);
  layout->addWidget(createButton(font1, 0x79, "\\lrcorner"),0,3);
  XWFTFont *font2 = loadFont("cmbsy10.pfb", 0);
  if (font2)
    font2->setCharMap(1);
  layout->addWidget(createButton(font2, 0x6A, "\\lvert"),0,4);
  QTransform rv;
  rv.translate(19,0);
  layout->addWidget(createButton(font2, 0x6A, rv, "\\rvert"),0,5);
  layout->addWidget(createButton(font2, 0x6B, "\\lVert"),0,6);
  rv.translate(0,0);
  layout->addWidget(createButton(font2, 0x6B, rv,"\\rVert"),0,7);

  XWFTFont *font3 = loadFont("msbm10.pfb", 0);
  if (font3)
    font3->setCharMap(1);
  layout->addWidget(createButton(font3, 0x7A, "\\digamma"),1,0);
  layout->addWidget(createButton(font3, 0x7B, "\\varkappa"),1,1);
  layout->addWidget(createButton(font3, 0x69, "\\beth"),1,2);
  layout->addWidget(createButton(font3, 0x6A, "\\gimel"),1,3);
  layout->addWidget(createButton(font3, 0x6B, "\\daleth"),1,4);

  setLayout(layout);
}

XWAMSBinRelOperatorWidget::XWAMSBinRelOperatorWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("msbm10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x6C, "\\lessdot"),0,0);
  layout->addWidget(createButton(font1, 0x6D, "\\gtrdot"),0,1);
  XWFTFont *font2= loadFont("msam10.pfb", 0);
  if (font2)
    font2->setCharMap(1);
  layout->addWidget(createButton(font2, 0x2B, "\\doteqdot"),0,2);
  layout->addWidget(createButton(font2, 0x36, "\\leqslant"),0,3);
  layout->addWidget(createButton(font2, 0x3E, "\\geqslant"),0,4);
  layout->addWidget(createButton(font2, 0x3A, "\\risingdotseq"),0,5);
  layout->addWidget(createButton(font2, 0x30, "\\eqslantless"),0,6);
  layout->addWidget(createButton(font2, 0x31, "\\eqslantgtr"),0,7);

  layout->addWidget(createButton(font2, 0x3B, "\\fallingdotseq"),1,0);
  layout->addWidget(createButton(font2, 0x35, "\\leqq"),1,1);
  layout->addWidget(createButton(font2, 0x3D, "\\geqq"),1,2);
  layout->addWidget(createButton(font2, 0x50, "\\eqcirc"),1,3);
  layout->addWidget(createButton(font2, 0x6E, "\\lll"),1,4);
  layout->addWidget(createButton(font2, 0x6F, "\\ggg"),1,5);
  layout->addWidget(createButton(font2, 0x24, "\\circeq"),1,6);
  layout->addWidget(createButton(font2, 0x2E, "\\lesssim"),1,7);

  layout->addWidget(createButton(font2, 0x26, "\\gtrsim"),2,0);
  layout->addWidget(createButton(font2, 0x2C, "\\triangleq"),2,1);
  layout->addWidget(createButton(font2, 0x2F, "\\lessapprox"),2,2);
  layout->addWidget(createButton(font2, 0x27, "\\gtrapprox"),2,3);
  layout->addWidget(createButton(font2, 0x6C, "\\bumpeq"),2,4);
  layout->addWidget(createButton(font2, 0x37, "\\lessgtr"),2,5);
  layout->addWidget(createButton(font2, 0x3F, "\\gtrless"),2,6);
  layout->addWidget(createButton(font2, 0x6D, "\\Bumpeq"),2,7);

  layout->addWidget(createButton(font2, 0x51, "\\lesseqgtr"),3,0);
  layout->addWidget(createButton(font2, 0x52, "\\gtreqless"),3,1);
  layout->addWidget(createButton(font1, 0x73, "\\thicksim"),3,2);
  layout->addWidget(createButton(font2, 0x53, "\\lesseqqgtr"),3,3);
  layout->addWidget(createButton(font2, 0x54, "\\gtreqqless"),3,4);
  layout->addWidget(createButton(font1, 0x74, "\\thickapprox"),3,5);
  layout->addWidget(createButton(font2, 0x34, "\\preccurlyeq"),3,6);
  layout->addWidget(createButton(font2, 0x3C, "\\succcurlyeq"),3,7);

  layout->addWidget(createButton(font1, 0x75, "\\approxeq"),4,0);
  layout->addWidget(createButton(font2, 0x32, "\\curlyeqprec"),4,1);
  layout->addWidget(createButton(font2, 0x33, "\\curlyeqsucc"),4,2);
  layout->addWidget(createButton(font2, 0x76, "\\backsim"),4,3);
  layout->addWidget(createButton(font2, 0x2D, "\\precsim"),4,4);
  layout->addWidget(createButton(font2, 0x25, "\\succsim"),4,5);
  layout->addWidget(createButton(font2, 0x77, "\\backsimeq"),4,6);
  layout->addWidget(createButton(font1, 0x77, "\\precapprox"),4,7);

  layout->addWidget(createButton(font1, 0x76, "\\succapprox"),5,0);
  layout->addWidget(createButton(font2, 0x0F, "\\vDash"),5,1);
  layout->addWidget(createButton(font2, 0x6A, "\\subseteqq"),5,2);
  layout->addWidget(createButton(font2, 0x6B, "\\supseteqq"),5,3);
  layout->addWidget(createButton(font2, 0x0D, "\\Vdash"),5,4);
  layout->addWidget(createButton(font1, 0x71, "\\shortparallel"),5,5);
  layout->addWidget(createButton(font2, 0x63, "\\Supset"),5,6);
  layout->addWidget(createButton(font2, 0x0E, "\\Vvdash"),5,7);

  layout->addWidget(createButton(font2, 0x4A, "\\blacktriangleleft"),6,0);
  layout->addWidget(createButton(font2, 0x41, "\\sqsupset"),6,1);
  layout->addWidget(createButton(font1, 0x7F, "\\backepsilon"),6,2);
  layout->addWidget(createButton(font2, 0x42, "\\vartriangleright"),6,3);
  layout->addWidget(createButton(font2, 0x2A, "\\because"),6,4);
  layout->addWidget(createButton(font2, 0x5F, "\\varpropto"),6,5);
  layout->addWidget(createButton(font2, 0x49, "\\blacktriangleright"),6,6);
  layout->addWidget(createButton(font2, 0x62, "\\Subset"),6,7);

  layout->addWidget(createButton(font2, 0x47, "\\between"),7,0);
  layout->addWidget(createButton(font2, 0x44, "\\trianglerighteq"),7,1);
  layout->addWidget(createButton(font2, 0x61, "\\smallfrown"),7,2);
  layout->addWidget(createButton(font2, 0x74, "\\pitchfork"),7,3);
  layout->addWidget(createButton(font2, 0x43, "\\vartriangleleft"),7,4);
  layout->addWidget(createButton(font1, 0x70, "\\shortmid"),7,5);
  layout->addWidget(createButton(font2, 0x60, "\\smallsmile"),7,6);
  layout->addWidget(createButton(font2, 0x45, "\\trianglelefteq"),7,7);

  layout->addWidget(createButton(font2, 0x29, "\\therefore"),8,0);
  layout->addWidget(createButton(font2, 0x40, "\\sqsubset"),8,1);

  setLayout(layout);
}

XWAMSArrowWidget::XWAMSArrowWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("msam10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  QTransform larrow,dabar1,dabar2;
  larrow.translate(-8,0);
  dabar1.translate(-1,4);  
  dabar2.translate(9,4);
  layout->addWidget(createButton(font1, 0x4C, font1, 0x39,font1,0x39,larrow,dabar1,dabar2,"\\dashleftarrow"),0,0);
  QTransform rarrow;
  rarrow.translate(11,0);
  dabar1.translate(-13,0);
  dabar2.translate(-12,0);
  layout->addWidget(createButton(font1, 0x4B, font1, 0x39,font1,0x39,rarrow,dabar1,dabar2,"\\dashrightarrow"),0,1);
  layout->addWidget(createButton(font1, 0x12, "\\leftleftarrows"),0,2);
  layout->addWidget(createButton(font1, 0x13, "\\rightrightarrows"),0,3);
  layout->addWidget(createButton(font1, 0x1C, "\\leftrightarrows"),0,4);
  layout->addWidget(createButton(font1, 0x1D, "\\rightleftarrows"),0,5);
  layout->addWidget(createButton(font1, 0x57, "\\Lleftarrow"),0,6);
  layout->addWidget(createButton(font1, 0x56, "\\Rrightarrow"),0,7);

  layout->addWidget(createButton(font1, 0x11, "\\twoheadleftarrow"),1,0);
  layout->addWidget(createButton(font1, 0x10, "\\twoheadrightarrow"),1,1);
  layout->addWidget(createButton(font1, 0x1B, "\\leftarrowtail"),1,2);
  layout->addWidget(createButton(font1, 0x1A, "\\rightarrowtail"),1,3);
  layout->addWidget(createButton(font1, 0x0B, "\\leftrightharpoons"),1,4);
  layout->addWidget(createButton(font1, 0x0A, "\\rightleftharpoons"),1,5);
  layout->addWidget(createButton(font1, 0x1E, "\\Lsh"),1,6);
  layout->addWidget(createButton(font1, 0x1F, "\\Rsh"),1,7);

  layout->addWidget(createButton(font1, 0x22, "\\looparrowleft"),2,0);
  layout->addWidget(createButton(font1, 0x23, "\\looparrowright"),2,1);
  XWFTFont *font3 = loadFont("msbm10.pfb", 0);
  if (font3)
    font3->setCharMap(1);
  layout->addWidget(createButton(font3, 0x78, "\\curvearrowleft"),2,2);
  layout->addWidget(createButton(font3, 0x79, "\\curvearrowright"),2,3);
  layout->addWidget(createButton(font1, 0x09, "\\circlearrowleft"),2,4);
  layout->addWidget(createButton(font1, 0x08, "\\circlearrowright"),2,5);
  layout->addWidget(createButton(font1, 0x28, "\\multimap"),2,6);
  layout->addWidget(createButton(font1, 0x14, "\\upuparrows"),2,7);

  layout->addWidget(createButton(font1, 0x15, "\\downdownarrows"),3,0);
  layout->addWidget(createButton(font1, 0x18, "\\upharpoonleft"),3,1);
  layout->addWidget(createButton(font1, 0x16, "\\upharpoonright"),3,2);
  layout->addWidget(createButton(font1, 0x17, "\\downharpoonright"),3,3);
  layout->addWidget(createButton(font1, 0x20, "\\rightsquigarrow"),3,4);
  layout->addWidget(createButton(font1, 0x21, "\\leftrightsquigarrow"),3,5);

  setLayout(layout);
}

XWAMSNotRelWidget::XWAMSNotRelWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("msbm10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x04, "\\nless"),0,0);
  layout->addWidget(createButton(font1, 0x05, "\\ngtr"),0,1);
  layout->addWidget(createButton(font1, 0x26, "\\varsubsetneqq"),0,2);
  layout->addWidget(createButton(font1, 0x0C, "\\lneq"),0,3);
  layout->addWidget(createButton(font1, 0x0D, "\\gneq"),0,4);
  layout->addWidget(createButton(font1, 0x27, "\\varsupsetneqq"),0,5);
  layout->addWidget(createButton(font1, 0x02, "\\nleq"),0,6);
  layout->addWidget(createButton(font1, 0x03, "\\ngeq"),0,7);

  layout->addWidget(createButton(font1, 0x22, "\\nsubseteqq"),1,0);
  layout->addWidget(createButton(font1, 0x0A, "\\nleqslant"),1,1);
  layout->addWidget(createButton(font1, 0x0B, "\\ngeqslant"),1,2);
  layout->addWidget(createButton(font1, 0x23, "\\nsupseteqq"),1,3);
  layout->addWidget(createButton(font1, 0x08, "\\lneqq"),1,4);
  layout->addWidget(createButton(font1, 0x09, "\\gneqq"),1,5);
  layout->addWidget(createButton(font1, 0x2D, "\\nmid"),1,6);
  layout->addWidget(createButton(font1, 0x00, "\\lvertneqq"),1,7);

  layout->addWidget(createButton(font1, 0x01, "\\gvertneqq"),2,0);
  layout->addWidget(createButton(font1, 0x2C, "\\nparallel"),2,1);
  layout->addWidget(createButton(font1, 0x14, "\\nleqq"),2,2);
  layout->addWidget(createButton(font1, 0x15, "\\ngeqq"),2,3);
  layout->addWidget(createButton(font1, 0x2E, "\\nshortmid"),2,4);
  layout->addWidget(createButton(font1, 0x12, "\\lnsim"),2,5);
  layout->addWidget(createButton(font1, 0x13, "\\gnsim"),2,6);
  layout->addWidget(createButton(font1, 0x2F, "\\nshortparallel"),2,7);

  layout->addWidget(createButton(font1, 0x1A, "\\lnapprox"),3,0);
  layout->addWidget(createButton(font1, 0x1B, "\\gnapprox"),3,1);
  layout->addWidget(createButton(font1, 0x1C, "\\nsim"),3,2);
  layout->addWidget(createButton(font1, 0x06, "\\nprec"),3,3);
  layout->addWidget(createButton(font1, 0x07, "\\nsucc"),3,4);
  layout->addWidget(createButton(font1, 0x1D, "\\ncong"),3,5);
  layout->addWidget(createButton(font1, 0x0E, "\\npreceq"),3,6);
  layout->addWidget(createButton(font1, 0x0F, "\\nsucceq"),3,7);

  layout->addWidget(createButton(font1, 0x30, "\\nvdash"),4,0);
  layout->addWidget(createButton(font1, 0x16, "\\precneqq"),4,1);
  layout->addWidget(createButton(font1, 0x17, "\\succneqq"),4,2);
  layout->addWidget(createButton(font1, 0x32, "\\nvDash"),4,3);
  layout->addWidget(createButton(font1, 0x10, "\\precnsim"),4,4);
  layout->addWidget(createButton(font1, 0x11, "\\succnsim"),4,5);
  layout->addWidget(createButton(font1, 0x31, "\\nVdash"),4,6);
  layout->addWidget(createButton(font1, 0x18, "\\precnapprox"),4,7);

  layout->addWidget(createButton(font1, 0x19, "\\succnapprox"),5,0);
  layout->addWidget(createButton(font1, 0x33, "\\nVDash"),5,1);
  layout->addWidget(createButton(font1, 0x28, "\\subsetneq"),5,2);
  layout->addWidget(createButton(font1, 0x29, "\\supsetneq"),5,3);
  layout->addWidget(createButton(font1, 0x36, "\\ntriangleleft"),5,4);
  layout->addWidget(createButton(font1, 0x20, "\\varsubsetneq"),5,5);
  layout->addWidget(createButton(font1, 0x21, "\\varsupsetneq"),5,6);
  layout->addWidget(createButton(font1, 0x37, "\\ntriangleright"),5,7);

  layout->addWidget(createButton(font1, 0x2A, "\\nsubseteq"),6,0);
  layout->addWidget(createButton(font1, 0x2B, "\\nsupseteq"),6,1);
  layout->addWidget(createButton(font1, 0x35, "\\ntrianglelefteq"),6,2);
  layout->addWidget(createButton(font1, 0x24, "\\subsetneqq"),6,3);
  layout->addWidget(createButton(font1, 0x25, "\\supsetneqq"),6,4);
  layout->addWidget(createButton(font1, 0x34, "\\ntrianglerighteq"),6,5);
  layout->addWidget(createButton(font1, 0x38, "\\nleftarrow"),6,6);
  layout->addWidget(createButton(font1, 0x39, "\\nrightarrow"),6,7);

  layout->addWidget(createButton(font1, 0x3D, "\\nleftrightarrow"),7,0);
  layout->addWidget(createButton(font1, 0x3A, "\\nLeftarrow"),7,1);
  layout->addWidget(createButton(font1, 0x3B, "\\nRightarrow"),7,2);
  layout->addWidget(createButton(font1, 0x3C, "\\nLeftrightarrow"),7,3);

  setLayout(layout);
}

XWAMSBinOperaterWidget::XWAMSBinOperaterWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("msam10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x75, "\\dotplus"),0,0);
  layout->addWidget(createButton(font1, 0x05, "\\centerdot"),0,1);
  XWFTFont *font2 = loadFont("msbm10.pfb", 0);
  if (font2)
    font2->setCharMap(1);
  layout->addWidget(createButton(font2, 0x6E, "\\ltimes"),0,2);
  layout->addWidget(createButton(font2, 0x6F, "\\rtimes"),0,3);
  layout->addWidget(createButton(font2, 0x3E, "\\divideontimes"),0,4);
  layout->addWidget(createButton(font1, 0x64, "\\doublecup"),0,5);
  layout->addWidget(createButton(font1, 0x65, "\\doublecap"),0,6);
  layout->addWidget(createButton(font2, 0x72, "\\smallsetminus"),0,7);

  layout->addWidget(createButton(font1, 0x59, "\\veebar"),1,0);
  layout->addWidget(createButton(font1, 0x5A, "\\barwedge"),1,1);
  layout->addWidget(createButton(font1, 0x5B, "\\doublebarwedge"),1,2);
  layout->addWidget(createButton(font1, 0x01, "\\boxplus"),1,3);
  layout->addWidget(createButton(font1, 0x0C, "\\boxminus"),1,4);
  layout->addWidget(createButton(font1, 0x7F, "\\circleddash"),1,5);
  layout->addWidget(createButton(font1, 0x02, "\\boxtimes"),1,6);
  layout->addWidget(createButton(font1, 0x00, "\\boxdot"),1,7);

  layout->addWidget(createButton(font1, 0x7D, "\\circledcirc"),2,0);
  layout->addWidget(createButton(font1, 0x7C, "\\intercal"),2,1);
  layout->addWidget(createButton(font1, 0x7E, "\\circledast"),2,2);
  layout->addWidget(createButton(font1, 0x69, "\\rightthreetimes"),2,3);
  layout->addWidget(createButton(font1, 0x67, "\\curlyvee"),2,4);
  layout->addWidget(createButton(font1, 0x66, "\\curlywedge"),2,5);
  layout->addWidget(createButton(font1, 0x68, "\\leftthreetimes"),2,6);

  setLayout(layout);
}

XWAMSOtherWidget::XWAMSOtherWidget(QWidget * parent)
:XWLaTeXSymbolWidget(parent)
{
  QGridLayout *layout = new QGridLayout;
  layout->setHorizontalSpacing(XW_LTX_MATH_LAYOUT_SIZE);
  layout->setVerticalSpacing(XW_LTX_MATH_LAYOUT_SIZE);

  XWFTFont *font1 = loadFont("msbm10.pfb", 0);
  if (font1)
    font1->setCharMap(1);
  layout->addWidget(createButton(font1, 0x7E, "\\hbar"),0,0);
  layout->addWidget(createButton(font1, 0x7D, "\\hslash"),0,1);
  layout->addWidget(createButton(font1, 0x7C, "\\Bbbk"),0,2);
  XWFTFont *font2 = loadFont("msam10.pfb", 0);
  if (font2)
    font2->setCharMap(1);
  layout->addWidget(createButton(font2, 0x03, "\\square"),0,3);
  layout->addWidget(createButton(font2, 0x04, "\\blacksquare"),0,4);
  layout->addWidget(createButton(font2, 0x73, "\\circledS"),0,5);
  layout->addWidget(createButton(font2, 0x4D, "\\vartriangle"),0,6);
  layout->addWidget(createButton(font2, 0x4E, "\\blacktriangle"),0,7);

  layout->addWidget(createButton(font2, 0x7B, "\\complement"),1,0);
  layout->addWidget(createButton(font2, 0x4F, "\\triangledown"),1,1);
  layout->addWidget(createButton(font2, 0x48, "\\blacktriangledown"),1,2);
  layout->addWidget(createButton(font1, 0x61, "\\Game"),1,3);
  layout->addWidget(createButton(font2, 0x06, "\\lozenge"),1,4);
  layout->addWidget(createButton(font2, 0x07, "\\blacklozenge"),1,5);
  layout->addWidget(createButton(font2, 0x46, "\\bigstar"),1,6);
  layout->addWidget(createButton(font2, 0x5C, "\\angle"),1,7);

  layout->addWidget(createButton(font2, 0x5D, "\\measuredangle"),2,0);
  layout->addWidget(createButton(font1, 0x1E, "\\diagup"),2,1);
  layout->addWidget(createButton(font1, 0x1F, "\\diagdown"),2,2);
  layout->addWidget(createButton(font2, 0x38, "\\backprime"),2,3);
  layout->addWidget(createButton(font1, 0x40, "\\nexists"),2,4);
  layout->addWidget(createButton(font1, 0x60, "\\Finv"),2,5);
  layout->addWidget(createButton(font1, 0x3F, "\\varnothing"),2,6);
  layout->addWidget(createButton(font1, 0x67, "\\eth"),2,7);

  layout->addWidget(createButton(font2, 0x5E, "\\sphericalangle"),3,0);
  layout->addWidget(createButton(font1, 0x66, "\\mho"),3,1);

  setLayout(layout);
}

XWLaTeXSymbolToolBox::XWLaTeXSymbolToolBox(QWidget * parent)
:QToolBox(parent)
{
  XWLaTeXSymbolWidget * widget = new XWGreeceLetterWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("Greece Letters"));
  widget = new XWBinRelOperatorWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("Binary Relations"));
  widget = new XWBinOperatorWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("Binary Operators"));
  widget = new XWBigOperatorWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("BIG Operators"));
  widget = new XWArrowWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("Arrows"));
  widget = new XWDelimiterWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("Delimiters"));
  widget = new XWOtherWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("Miscellaneous"));
  widget = new XWAMSDelimiterLetterWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("AMS Delimiters"));
  widget = new XWAMSBinRelOperatorWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("AMS Binary Relations"));
  widget = new XWAMSArrowWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("AMS Arrows"));
  widget = new XWAMSNotRelWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("AMS Negated Binary Relations and Arrows"));
  widget = new XWAMSBinOperaterWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("AMS Binary Operators"));
  widget = new XWAMSOtherWidget(this);
  connect(widget, SIGNAL(symbolClicked(const QString &)), this, SIGNAL(symbolClicked(const QString &)));
  addItem(widget,tr("AMS Miscellaneous"));

  if (fontEngine)
  {
    delete fontEngine;
    fontEngine = 0;
  }
}