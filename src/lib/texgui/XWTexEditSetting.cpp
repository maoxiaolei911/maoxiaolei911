/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWSystemFontWidget.h"
#include "XWTexEditSetting.h"

XWTexEditSPWidget::XWTexEditSPWidget(QWidget * parent)
 :QWidget(parent)
{
  wdiget = new XWSystemFontWidget;
  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addWidget(wdiget);
  setLayout(mainlayout);

  QString fam = wdiget->getFamily();
  if (xwApp->hasSetting("SpecialCharFontFamily"))
    fam = xwApp->getSetting("SpecialCharFontFamily").toString();

  int weight = wdiget->getWeight();
  if (xwApp->hasSetting("SpecialCharFontWeight"))
    weight = xwApp->getSetting("SpecialCharFontWeight").toInt();

  bool italic = wdiget->getItalic();
  if (xwApp->hasSetting("SpecialCharFontItalic"))
    italic = xwApp->getSetting("SpecialCharFontItalic").toBool();

  int size = wdiget->getSize();
  if (xwApp->hasSetting("SpecialCharFontSize"))
    size = xwApp->getSetting("SpecialCharFontSize").toInt();

  if (size < 0 || size > 72)
    size = 10;

  QColor color = wdiget->getColor();
  if (xwApp->hasSetting("SpecialCharColor"))
  {
    QVariant variant = xwApp->getSetting("SpecialCharColor");
    color = variant.value<QColor>();
  }

  wdiget->setFont(fam,weight,italic,size,color);
}

void XWTexEditSPWidget::save()
{
  QString fam = wdiget->getFamily();
  xwApp->saveSetting("SpecialCharFontFamily",fam);
  int weight = wdiget->getWeight();
  xwApp->saveSetting("SpecialCharFontWeight",weight);
  bool italic = wdiget->getItalic();
  xwApp->saveSetting("SpecialCharFontItalic",italic);
  int size = wdiget->getSize();
  if (size < 0 || size > 72)
    size = 10;
  xwApp->saveSetting("SpecialCharFontSize",size);
  QColor color = wdiget->getColor();
  QVariant variant = color;
  xwApp->saveSetting("SpecialCharColor",variant);
}

void XWTexEditSPWidget::setBgColor(const QColor & c)
{
  wdiget->setBgColor(c);
}

XWTexEditCommentWidget::XWTexEditCommentWidget(QWidget * parent)
 :QWidget(parent)
{
  wdiget = new XWSystemFontWidget;
  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addWidget(wdiget);
  setLayout(mainlayout);

  QString fam = wdiget->getFamily();
  if (xwApp->hasSetting("CommentFontFamily"))
    fam = xwApp->getSetting("CommentFontFamily").toString();

  int weight = wdiget->getWeight();
  if (xwApp->hasSetting("CommentFontWeight"))
    weight = xwApp->getSetting("CommentFontWeight").toInt();

  bool italic = wdiget->getItalic();
  if (xwApp->hasSetting("CommentFontItalic"))
    italic = xwApp->getSetting("CommentFontItalic").toBool();

  int size = wdiget->getSize();
  if (xwApp->hasSetting("CommentFontSize"))
    size = xwApp->getSetting("CommentFontSize").toInt();

  if (size < 0 || size > 72)
    size = 10;

  QColor color = wdiget->getColor();
  if (xwApp->hasSetting("CommentColor"))
  {
    QVariant variant = xwApp->getSetting("CommentColor");
    color = variant.value<QColor>();
  }

  wdiget->setFont(fam,weight,italic,size,color);
}

void XWTexEditCommentWidget::save()
{
  QString fam = wdiget->getFamily();
  xwApp->saveSetting("CommentFontFamily",fam);
  int weight = wdiget->getWeight();
  xwApp->saveSetting("CommentFontWeight",weight);
  bool italic = wdiget->getItalic();
  xwApp->saveSetting("CommentFontItalic",italic);
  int size = wdiget->getSize();
  if (size < 0 || size > 72)
    size = 10;
  xwApp->saveSetting("CommentFontSize",size);
  QColor color = wdiget->getColor();
  QVariant variant = color;
  xwApp->saveSetting("CommentColor",variant);
}

void XWTexEditCommentWidget::setBgColor(const QColor & c)
{
  wdiget->setBgColor(c);
}

XWTexEditCSWidget::XWTexEditCSWidget(QWidget * parent)
 :QWidget(parent)
{
  wdiget = new XWSystemFontWidget;
  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addWidget(wdiget);
  setLayout(mainlayout);

  QString fam = wdiget->getFamily();
  if (xwApp->hasSetting("ControlSequenceFontFamily"))
    fam = xwApp->getSetting("ControlSequenceFontFamily").toString();

  int weight = wdiget->getWeight();
  if (xwApp->hasSetting("ControlSequenceFontWeight"))
    weight = xwApp->getSetting("ControlSequenceFontWeight").toInt();

  bool italic = wdiget->getItalic();
  if (xwApp->hasSetting("ControlSequenceFontItalic"))
    italic = xwApp->getSetting("ControlSequenceFontItalic").toBool();

  int size = wdiget->getSize();
  if (xwApp->hasSetting("ControlSequenceFontSize"))
    size = xwApp->getSetting("ControlSequenceFontSize").toInt();

  if (size < 0 || size > 72)
    size = 10;

  QColor color = wdiget->getColor();
  if (xwApp->hasSetting("ControlSequenceColor"))
  {
    QVariant variant = xwApp->getSetting("ControlSequenceColor");
    color = variant.value<QColor>();
  }

  wdiget->setFont(fam,weight,italic,size,color);
}

void XWTexEditCSWidget::save()
{
  QString fam = wdiget->getFamily();
  xwApp->saveSetting("ControlSequenceFontFamily",fam);
  int weight = wdiget->getWeight();
  xwApp->saveSetting("ControlSequenceFontWeight",weight);
  bool italic = wdiget->getItalic();
  xwApp->saveSetting("ControlSequenceFontItalic",italic);
  int size = wdiget->getSize();
  if (size < 0 || size > 72)
    size = 10;
  xwApp->saveSetting("ControlSequenceFontSize",size);
  QColor color = wdiget->getColor();
  QVariant variant = color;
  xwApp->saveSetting("ControlSequenceColor",variant);
}

void XWTexEditCSWidget::setBgColor(const QColor & c)
{
  wdiget->setBgColor(c);
}

XWTexEditOptionWidget::XWTexEditOptionWidget(QWidget * parent)
 :QWidget(parent)
{
  wdiget = new XWSystemFontWidget;
  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addWidget(wdiget);
  setLayout(mainlayout);

  QString fam = wdiget->getFamily();
  if (xwApp->hasSetting("OptionFontFamily"))
    fam = xwApp->getSetting("OptionFontFamily").toString();

  int weight = wdiget->getWeight();
  if (xwApp->hasSetting("OptionFontWeight"))
    weight = xwApp->getSetting("OptionFontWeight").toInt();

  bool italic = wdiget->getItalic();
  if (xwApp->hasSetting("OptionFontItalic"))
    italic = xwApp->getSetting("OptionFontItalic").toBool();

  int size = wdiget->getSize();
  if (xwApp->hasSetting("OptionFontSize"))
    size = xwApp->getSetting("OptionFontSize").toInt();

  if (size < 0 || size > 72)
    size = 10;

  QColor color = wdiget->getColor();
  if (xwApp->hasSetting("OptionColor"))
  {
    QVariant variant = xwApp->getSetting("OptionColor");
    color = variant.value<QColor>();
  }

  wdiget->setFont(fam,weight,italic,size,color);
}

void XWTexEditOptionWidget::save()
{
  QString fam = wdiget->getFamily();
  xwApp->saveSetting("OptionFontFamily",fam);
  int weight = wdiget->getWeight();
  xwApp->saveSetting("OptionFontWeight",weight);
  bool italic = wdiget->getItalic();
  xwApp->saveSetting("OptionFontItalic",italic);
  int size = wdiget->getSize();
  if (size < 0 || size > 72)
    size = 10;
  xwApp->saveSetting("OptionFontSize",size);
  QColor color = wdiget->getColor();
  QVariant variant = color;
  xwApp->saveSetting("OptionColor",variant);
}

void XWTexEditOptionWidget::setBgColor(const QColor & c)
{
  wdiget->setBgColor(c);
}

XWTexEditParamWidget::XWTexEditParamWidget(QWidget * parent)
 :QWidget(parent)
{
  wdiget = new XWSystemFontWidget;
  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addWidget(wdiget);
  setLayout(mainlayout);

  QString fam = wdiget->getFamily();
  if (xwApp->hasSetting("ParamFontFamily"))
    fam = xwApp->getSetting("ParamFontFamily").toString();

  int weight = wdiget->getWeight();
  if (xwApp->hasSetting("ParamFontWeight"))
    weight = xwApp->getSetting("ParamFontWeight").toInt();

  bool italic = wdiget->getItalic();
  if (xwApp->hasSetting("ParamFontItalic"))
    italic = xwApp->getSetting("ParamFontItalic").toBool();

  int size = wdiget->getSize();
  if (xwApp->hasSetting("ParamFontSize"))
    size = xwApp->getSetting("ParamFontSize").toInt();

  if (size < 0 || size > 72)
    size = 10;

  QColor color = wdiget->getColor();
  if (xwApp->hasSetting("ParamColor"))
  {
    QVariant variant = xwApp->getSetting("ParamColor");
    color = variant.value<QColor>();
  }

  wdiget->setFont(fam,weight,italic,size,color);
}

void XWTexEditParamWidget::save()
{
  QString fam = wdiget->getFamily();
  xwApp->saveSetting("ParamFontFamily",fam);
  QString weight = wdiget->getWeight();
  xwApp->saveSetting("ParamFontWeight",weight);
  bool italic = wdiget->getItalic();
  xwApp->saveSetting("ParamFontItalic",italic);
  int size = wdiget->getSize();
  if (size < 0 || size > 72)
    size = 10;
  xwApp->saveSetting("ParamFontSize",size);
  QColor color = wdiget->getColor();
  QVariant variant = color;
  xwApp->saveSetting("ParamColor",variant);
}

void XWTexEditParamWidget::setBgColor(const QColor & c)
{
  wdiget->setBgColor(c);
}

XWTeXEditTexTWidget::XWTeXEditTexTWidget(QWidget * parent)
 :QWidget(parent)
{
  wdiget = new XWSystemFontWidget;
  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addWidget(wdiget);
  setLayout(mainlayout);

  QString fam = wdiget->getFamily();
  if (xwApp->hasSetting("TextFontFamily"))
    fam = xwApp->getSetting("TextFontFamily").toString();

  int weight = wdiget->getWeight();
  if (xwApp->hasSetting("TextFontWeight"))
    weight = xwApp->getSetting("TextFontWeight").toInt();

  bool italic = wdiget->getItalic();
  if (xwApp->hasSetting("TextFontItalic"))
    italic = xwApp->getSetting("TextFontItalic").toBool();

  int size = wdiget->getSize();
  if (xwApp->hasSetting("TextFontSize"))
    size = xwApp->getSetting("TextFontSize").toInt();

  if (size < 0 || size > 72)
    size = 10;

  QColor color = wdiget->getColor();
  if (xwApp->hasSetting("TextColor"))
  {
    QVariant variant = xwApp->getSetting("TextColor");
    color = variant.value<QColor>();
  }

  wdiget->setFont(fam,weight,italic,size,color);
}

void XWTeXEditTexTWidget::save()
{
  QString fam = wdiget->getFamily();
  xwApp->saveSetting("TextFontFamily",fam);
  int weight = wdiget->getWeight();
  xwApp->saveSetting("TextFontWeight",weight);
  bool italic = wdiget->getItalic();
  xwApp->saveSetting("TextFontItalic",italic);
  int size = wdiget->getSize();
  if (size < 0 || size > 72)
    size = 10;
  xwApp->saveSetting("TextFontSize",size);
  QColor color = wdiget->getColor();
  QVariant variant = color;
  xwApp->saveSetting("TextColor",variant);
}

void XWTeXEditTexTWidget::setBgColor(const QColor & c)
{
  wdiget->setBgColor(c);
}

XWTexEditBackgroundWidget::XWTexEditBackgroundWidget(QWidget * parent)
 :QWidget(parent)
 {
   colorButton = new QPushButton;
   colorButton->setFlat(true);
   colorButton->setAutoFillBackground(true);
   color = QColor(Qt::white);
   if (xwApp->hasSetting("TextBackgroundColor"))
   {
     QVariant variant = xwApp->getSetting("TextBackgroundColor");
     color = variant.value<QColor>();
   }

   QPalette pa(color);
   QBrush bru(color);
   pa.setBrush(QPalette::Button,bru);
   colorButton->setPalette(pa);

   QVBoxLayout * mainlayout = new QVBoxLayout;
   mainlayout->addWidget(colorButton);
   mainlayout->addStretch();
   setLayout(mainlayout);

   connect(colorButton, SIGNAL(clicked()), this, SLOT(setColor()));
}

void XWTexEditBackgroundWidget::save()
{
  QVariant variant = color;
  xwApp->saveSetting("TextBackgroundColor",variant);
}

void XWTexEditBackgroundWidget::setColor()
{
  color = QColorDialog::getColor(color, this);
  QPalette pa(color);
  QBrush bru(color);
  pa.setBrush(QPalette::Button,bru);
  colorButton->setPalette(pa);
  emit colorChanged(color);
}

XWTexEditSetting::XWTexEditSetting(QWidget * parent)
 :QDialog(parent)
{
  setWindowTitle(tr("Document style setting"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  contentsWidget = new QListWidget;

  QListWidgetItem *bgButton = new QListWidgetItem(contentsWidget);
  bgButton->setText(tr("Text background"));

  QListWidgetItem *textButton = new QListWidgetItem(contentsWidget);
  textButton->setText(tr("Text"));

  QListWidgetItem *csButton = new QListWidgetItem(contentsWidget);
  csButton->setText(tr("Control sequence"));

  QListWidgetItem *commentButton = new QListWidgetItem(contentsWidget);
  commentButton->setText(tr("Comment"));

  QListWidgetItem *optionButton = new QListWidgetItem(contentsWidget);
  optionButton->setText(tr("Option"));

  QListWidgetItem *paramButton = new QListWidgetItem(contentsWidget);
  paramButton->setText(tr("Parameter"));

  QListWidgetItem *spButton = new QListWidgetItem(contentsWidget);
  spButton->setText(tr("Special char"));

  textSetting = new XWTeXEditTexTWidget;
  csSetting = new XWTexEditCSWidget;
  commentSetting = new XWTexEditCommentWidget;
  optionSetting = new XWTexEditOptionWidget;
  paramSetting = new XWTexEditParamWidget;
  spSetting = new XWTexEditSPWidget;
  bgSetting = new XWTexEditBackgroundWidget;

  pagesWidget = new QStackedWidget;
  pagesWidget->addWidget(bgSetting);
  pagesWidget->addWidget(textSetting);
  pagesWidget->addWidget(csSetting);
  pagesWidget->addWidget(commentSetting);
  pagesWidget->addWidget(optionSetting);
  pagesWidget->addWidget(paramSetting);
  pagesWidget->addWidget(spSetting);
  contentsWidget->setCurrentRow(0);

  QColor color = bgSetting->getColor();
  setBgColor(color);

  QHBoxLayout *horizontalLayout = new QHBoxLayout;
  horizontalLayout->addWidget(contentsWidget);
  horizontalLayout->addWidget(pagesWidget, 1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(horizontalLayout);
  mainLayout->addStretch(1);
  mainLayout->addSpacing(12);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
             this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
  connect(bgSetting, SIGNAL(colorChanged(const QColor & )), this, SLOT(setBgColor(const QColor & )));
}

void XWTexEditSetting::accept()
{
  textSetting->save();
  csSetting->save();
  commentSetting->save();
  optionSetting->save();
  paramSetting->save();
  spSetting->save();
  bgSetting->save();
}

void XWTexEditSetting::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
    current = previous;

  pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void XWTexEditSetting::setBgColor(const QColor & c)
{
  textSetting->setBgColor(c);
  csSetting->setBgColor(c);
  commentSetting->setBgColor(c);
  optionSetting->setBgColor(c);
  paramSetting->setBgColor(c);
  spSetting->setBgColor(c);
}
