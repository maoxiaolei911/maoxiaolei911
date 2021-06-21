/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWSystemFontWidget.h"
#include "XWTexDocFontSetting.h"

XWTexDocFontWidget::XWTexDocFontWidget(QWidget * parent)
:QWidget(parent)
{
  wdiget = new XWSystemFontWidget;
  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addWidget(wdiget);
  setLayout(mainlayout);
}

void XWTexDocFontWidget::get()
{
  getSetting("Normal");
}

void XWTexDocFontWidget::set()
{
  setSetting("Normal");
}

void XWTexDocFontWidget::setBgColor(const QColor & c)
{
  wdiget->setBgColor(c);
}

void XWTexDocFontWidget::getSetting(const QString & grp)
{
  QSettings * settings = xwApp->getSettings();
  QString curstyle = settings->value("CurrentDocumentStyle").toString();
  if (curstyle.isEmpty())
  {
    settings->endGroup();
    return ;
  }

  settings->beginGroup("DocumentStyles");
  settings->beginGroup(curstyle);
  settings->beginGroup(grp);

  QString fam = settings->value("Family").toString();
  QVariant variant = settings->value("Color");
  QColor color = variant.value<QColor>();

  if (!fam.isEmpty())
    wdiget->setFont(fam,QFont::Normal,false,14,color);

  settings->endGroup();
  settings->endGroup();
  settings->endGroup();
  settings->endGroup();
}

void XWTexDocFontWidget::setSetting(const QString & grp)
{
  QSettings * settings = xwApp->getSettings();
  QString curstyle = settings->value("CurrentDocumentStyle").toString();
  if (curstyle.isEmpty())
  {
    settings->endGroup();
    return ;
  }
  settings->beginGroup("DocumentStyles");
  settings->beginGroup(curstyle);
  settings->beginGroup(grp);

  QString fam = wdiget->getFamily();
  settings->setValue("Family",fam);
  QColor color = wdiget->getColor();
  QVariant variant = color;
  settings->setValue("Color",variant);

  settings->endGroup();
  settings->endGroup();
  settings->endGroup();
  settings->endGroup();
}

XWTexDocNormalFontWidget::XWTexDocNormalFontWidget(QWidget * parent)
:XWTexDocFontWidget(parent)
{}

void XWTexDocNormalFontWidget::get()
{
  getSetting("Normal");
}

void XWTexDocNormalFontWidget::set()
{
  setSetting("Normal");
}

XWTexDocBoldFontWidget::XWTexDocBoldFontWidget(QWidget * parent)
:XWTexDocFontWidget(parent)
{}

void XWTexDocBoldFontWidget::get()
{
  getSetting("Bold");
}

void XWTexDocBoldFontWidget::set()
{
  setSetting("Bold");
}

XWTexDocItalicFontWidget::XWTexDocItalicFontWidget(QWidget * parent)
:XWTexDocFontWidget(parent)
{}

void XWTexDocItalicFontWidget::get()
{
  getSetting("Italic");
}

void XWTexDocItalicFontWidget::set()
{
  setSetting("Italic");
}

XWTexDocMathFontWidget::XWTexDocMathFontWidget(QWidget * parent)
:XWTexDocFontWidget(parent)
{}

void XWTexDocMathFontWidget::get()
{
  getSetting("Math");
}

void XWTexDocMathFontWidget::set()
{
  setSetting("Math");
}

XWTexDocTypewriterFontWidget::XWTexDocTypewriterFontWidget(QWidget * parent)
:XWTexDocFontWidget(parent)
{}

void XWTexDocTypewriterFontWidget::get()
{
  getSetting("Typewriter");
}

void XWTexDocTypewriterFontWidget::set()
{
  setSetting("Typewriter");
}

XWTexDocCommentFontWidget::XWTexDocCommentFontWidget(QWidget * parent)
:XWTexDocFontWidget(parent)
{}

void XWTexDocCommentFontWidget::get()
{
  getSetting("Comment");
}

void XWTexDocCommentFontWidget::set()
{
  setSetting("Comment");
}

XWTexDocControlSequenceFontWidget::XWTexDocControlSequenceFontWidget(QWidget * parent)
:XWTexDocFontWidget(parent)
{}

void XWTexDocControlSequenceFontWidget::get()
{
  getSetting("ControlSequence");
}

void XWTexDocControlSequenceFontWidget::set()
{
  setSetting("ControlSequence");
}

XWTexDocParameterFontWidget::XWTexDocParameterFontWidget(QWidget * parent)
:XWTexDocFontWidget(parent)
{}

void XWTexDocParameterFontWidget::get()
{
  getSetting("Parameter");
}

void XWTexDocParameterFontWidget::set()
{
  setSetting("Parameter");
}

XWTexDocOptionFontWidget::XWTexDocOptionFontWidget(QWidget * parent)
:XWTexDocFontWidget(parent)
{}

void XWTexDocOptionFontWidget::get()
{
  getSetting("Option");
}

void XWTexDocOptionFontWidget::set()
{
  setSetting("Option");
}

XWTexDocPageColorWidget::XWTexDocPageColorWidget(QWidget * parent)
:QWidget(parent)
{
  colorButton = new QPushButton;
  colorButton->setFlat(true);
  colorButton->setAutoFillBackground(true);
  color = QColor(Qt::white);
  QPalette pa(color);
  QBrush bru(color);
  pa.setBrush(QPalette::Button,bru);
  colorButton->setPalette(pa);

  QVBoxLayout * mainlayout = new QVBoxLayout;
  mainlayout->addWidget(colorButton);
  setLayout(mainlayout);

  connect(colorButton, SIGNAL(clicked()), this, SLOT(setColor()));
}

void XWTexDocPageColorWidget::get()
{
  QSettings * settings = xwApp->getSettings();
  QString curstyle = settings->value("CurrentDocumentStyle").toString();
  if (!curstyle.isEmpty())
  {
    settings->beginGroup(curstyle);
    QVariant variant = settings->value("PageColor");
    color = variant.value<QColor>();

    QPalette pa(color);
    QBrush bru(color);
    pa.setBrush(QPalette::Button,bru);
    colorButton->setPalette(pa);
    settings->endGroup();
  }
  settings->endGroup();
}

void XWTexDocPageColorWidget::set()
{
  QSettings * settings = xwApp->getSettings();
  QString curstyle = settings->value("CurrentDocumentStyle").toString();
  settings->beginGroup("DocumentStyles");
  settings->beginGroup(curstyle);
  QVariant variant = color;
  settings->setValue("PageColor",variant);
  settings->endGroup();
  settings->endGroup();
  settings->endGroup();
}

void XWTexDocPageColorWidget::setColor()
{
  color = QColorDialog::getColor(color, this);
  QPalette pa(color);
  QBrush bru(color);
  pa.setBrush(QPalette::Button,bru);
  colorButton->setPalette(pa);
  emit colorChanged(color);
}

XWTexDocFontSetting::XWTexDocFontSetting(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Document style setting"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  stylesCombo = new QComboBox;
  stylesCombo->setEditable(true);
  stylesCombo->setInsertPolicy(QComboBox::InsertAtTop);

  contentsWidget = new QListWidget;

  QListWidgetItem *pageColorButton = new QListWidgetItem(contentsWidget);
  pageColorButton->setText(tr("Page color"));

  QListWidgetItem *normalButton = new QListWidgetItem(contentsWidget);
  normalButton->setText(tr("Normal font"));

  QListWidgetItem *boldButton = new QListWidgetItem(contentsWidget);
  boldButton->setText(tr("Bold font"));

  QListWidgetItem *italicButton = new QListWidgetItem(contentsWidget);
  italicButton->setText(tr("Italic font"));

  QListWidgetItem *mathButton = new QListWidgetItem(contentsWidget);
  mathButton->setText(tr("Math font"));

  QListWidgetItem *typewriterButton = new QListWidgetItem(contentsWidget);
  typewriterButton->setText(tr("Typewriter font"));

  QListWidgetItem *commentButton = new QListWidgetItem(contentsWidget);
  commentButton->setText(tr("Comment font"));

  QListWidgetItem *csButton = new QListWidgetItem(contentsWidget);
  csButton->setText(tr("Control sequence font"));

  QListWidgetItem *paramButton = new QListWidgetItem(contentsWidget);
  paramButton->setText(tr("Parameter font"));

  QListWidgetItem *optionButton = new QListWidgetItem(contentsWidget);
  optionButton->setText(tr("Option font"));

  normalFont = new XWTexDocNormalFontWidget;
  normalFont->get();
  boldFont = new XWTexDocBoldFontWidget;
  boldFont->get();
  italicFont = new XWTexDocItalicFontWidget;
  italicFont->get();
  mathFont = new XWTexDocMathFontWidget;
  mathFont->get();
  typewriterFont = new XWTexDocTypewriterFontWidget;
  typewriterFont->get();
  commentFont = new XWTexDocCommentFontWidget;
  commentFont->get();
  csFont = new XWTexDocControlSequenceFontWidget;
  csFont->get();
  paramFont = new XWTexDocParameterFontWidget;
  paramFont->get();
  optionFont = new XWTexDocOptionFontWidget;
  optionFont->get();
  pageColor = new XWTexDocPageColorWidget;
  pageColor->get();

  pagesWidget = new QStackedWidget;
  pagesWidget->addWidget(pageColor);
  pagesWidget->addWidget(normalFont);
  pagesWidget->addWidget(boldFont);
  pagesWidget->addWidget(italicFont);
  pagesWidget->addWidget(mathFont);
  pagesWidget->addWidget(typewriterFont);
  pagesWidget->addWidget(commentFont);
  pagesWidget->addWidget(csFont);
  pagesWidget->addWidget(paramFont);
  pagesWidget->addWidget(optionFont);
  contentsWidget->setCurrentRow(0);

  QColor color = pageColor->getColor();
  setBgColor(color);

  QHBoxLayout *horizontalLayout = new QHBoxLayout;
  horizontalLayout->addWidget(contentsWidget);
  horizontalLayout->addWidget(pagesWidget, 1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(stylesCombo);
  mainLayout->addLayout(horizontalLayout);
  mainLayout->addStretch(1);
  mainLayout->addSpacing(12);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  QSettings * settings = xwApp->getSettings();
  settings->beginGroup("DocumentStyles");
  QStringList styles = settings->childGroups();
  stylesCombo->addItems(styles);
  settings->endGroup();

  QString curstyle = settings->value("CurrentDocumentStyle").toString();
  int idx = stylesCombo->findText(curstyle);
  stylesCombo->setCurrentIndex(idx);
  settings->endGroup();

  changeCurrent(curstyle);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(stylesCombo, SIGNAL(activated(const QString &)), this, SLOT(changeCurrent(const QString &)));
  connect(stylesCombo, SIGNAL(editTextChanged(const QString &)), this, SLOT(setCurrent(const QString &)));
  connect(contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
             this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
  connect(pageColor, SIGNAL(colorChanged(const QColor & )), this, SLOT(setBgColor(const QColor & )));
}

void XWTexDocFontSetting::accept()
{
  normalFont->set();
  boldFont->set();
  italicFont->set();
  mathFont->set();
  typewriterFont->set();
  commentFont->set();
  csFont->set();
  paramFont->set();
  optionFont->set();
  pageColor->set();
}

void XWTexDocFontSetting::changeCurrent(const QString & s)
{
  xwApp->saveSetting("CurrentDocumentStyle",s);
  normalFont->get();
  boldFont->get();
  italicFont->get();
  mathFont->get();
  typewriterFont->get();
  commentFont->get();
  csFont->get();
  paramFont->get();
  optionFont->get();
  pageColor->get();
}

void XWTexDocFontSetting::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
    current = previous;

  pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void XWTexDocFontSetting::setBgColor(const QColor & c)
{
  normalFont->setBgColor(c);
  boldFont->setBgColor(c);
  italicFont->setBgColor(c);
  mathFont->setBgColor(c);
  typewriterFont->setBgColor(c);
  commentFont->setBgColor(c);
  csFont->setBgColor(c);
  paramFont->setBgColor(c);
  optionFont->setBgColor(c);
}

void XWTexDocFontSetting::setCurrent(const QString & s)
{
  if (s.isEmpty())
    return ;

  xwApp->saveSetting("CurrentDocumentStyle",s);
}
