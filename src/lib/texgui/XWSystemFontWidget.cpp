/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
 #include <QtGui>
 #include "XWSystemFontWidget.h"

 XWSystemFontWidget::XWSystemFontWidget(QWidget * parent)
  :QWidget(parent)
 {
   fontLabel = new QLabel(tr("Font"),this);
   fontCombox = new QFontComboBox(this);

   sizeLabel = new QLabel(tr("Font size"),this);
   sizeCombox = new QComboBox(this);

   weightLabel = new QLabel(tr("Font weight"),this);
   weightCombox = new QComboBox(this);
   weightCombox->addItem(tr("Normal"),QFont::Normal);
   weightCombox->addItem(tr("Bold"),QFont::Bold);
   weightCombox->addItem(tr("DemiBold"),QFont::DemiBold);
   weightCombox->addItem(tr("Light"),QFont::Light);
   weightCombox->addItem(tr("Black"),QFont::Black);

   italicBox = new QCheckBox(tr("Italic"));

   colorLabel = new QLabel(tr("Font color"),this);
   colorButton = new QPushButton;
   colorButton->setFlat(true);
   colorButton->setAutoFillBackground(true);
   color = QColor(Qt::black);
   QPalette pa(color);
   QBrush bru(color);
   pa.setBrush(QPalette::Button,bru);
   colorButton->setPalette(pa);

   QGroupBox * topbox = new QGroupBox(tr("Font setting"));
   QGridLayout * toplayout = new QGridLayout;
   toplayout->addWidget(fontLabel,0,0);
   toplayout->addWidget(fontCombox,0,1);
   toplayout->addWidget(sizeLabel,1,0);
   toplayout->addWidget(sizeCombox,1,1);
   toplayout->addWidget(weightLabel,2,0);
   toplayout->addWidget(weightCombox,2,1);
   toplayout->addWidget(italicBox,3,0);
   toplayout->addWidget(colorLabel,4,0);
   toplayout->addWidget(colorButton,4,1);
   topbox->setLayout(toplayout);

   textBrowser = new QTextEdit(this);

   QVBoxLayout * mainlayout = new QVBoxLayout;
   mainlayout->addWidget(topbox);
   mainlayout->addWidget(textBrowser);
   setLayout(mainlayout);

   QFont font = fontCombox->currentFont();
   setFont(font);
   connect(fontCombox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(setFont(const QFont &)));
   connect(sizeCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(setSize(int)));
   connect(weightCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(setWeight(int)));
   connect(italicBox, SIGNAL(stateChanged(int)), this, SLOT(setItalic(int)));
   connect(colorButton, SIGNAL(clicked()), this, SLOT(setColor()));
}

QColor XWSystemFontWidget::getColor()
{
  return color;
}

QString XWSystemFontWidget::getFamily()
{
  return fontCombox->currentText();
}

bool XWSystemFontWidget::getItalic()
{
  return italicBox->isChecked();
}

int XWSystemFontWidget::getSize()
{
  return sizeCombox->currentText().toInt();
}

int XWSystemFontWidget::getWeight()
{
  return weightCombox->itemData(weightCombox->currentIndex()).toInt();
}

void XWSystemFontWidget::setBgColor(const QColor & c)
{
  QPalette p = textBrowser->palette();
  p.setColor(QPalette::Base, c);
  textBrowser->setPalette(p);
}

void XWSystemFontWidget::setFont(const QString & fam, int weight, bool italic,int size, const QColor & c)
{
  QFont font(fam, size, weight, italic);
  color = c;
  QPalette pa(color);
  QBrush bru(color);
  pa.setBrush(QPalette::Button,bru);
  colorButton->setPalette(pa);
  textBrowser->setTextColor(color);
  fontCombox->setCurrentFont(font);
}

void XWSystemFontWidget::setItalic(int /*state*/)
{
  QFont font = textBrowser->document()->defaultFont();
  font.setItalic(italicBox->isChecked());
  textBrowser->document()->setDefaultFont(font);
  showFont();
}

void XWSystemFontWidget::setColor()
{
  color = QColorDialog::getColor(color, this);
  QPalette pa(color);
  QBrush bru(color);
  pa.setBrush(QPalette::Button,bru);
  colorButton->setPalette(pa);
  textBrowser->setTextColor(color);
  showFont();
}

 void XWSystemFontWidget::setFont(const QFont & font)
 {
   sizeCombox->clear();
   QFontDatabase database;
   QList<int> sizes = database.pointSizes(font.family(), font.styleName());
   int i = 0;
   int k = 0;
   for (; i < sizes.size(); i++)
   {
     if (sizes[i] == font.pointSize())
       k = i;
     QString s = QString::number(sizes[i]);
     sizeCombox->addItem(s,sizes[i]);
   }

   sizeCombox->setCurrentIndex(k);
   int idx = weightCombox->findData(font.weight());
   if (idx < 0 || idx > 4)
    idx = 0;
   weightCombox->setCurrentIndex(idx);
   if (font.italic())
     italicBox->setCheckState(Qt::Checked);
   else
     italicBox->setCheckState(Qt::Unchecked);

  textBrowser->document()->setDefaultFont(font);
  showFont();
}

void XWSystemFontWidget::setSize(int idx)
{
  int size = sizeCombox->itemData(idx).toInt();
  if (size <= 0 || size > 72)
	  size = 10;
  QFont font = textBrowser->document()->defaultFont();
  font.setPointSize(size);
  textBrowser->document()->setDefaultFont(font);
  showFont();
}

void XWSystemFontWidget::setWeight(int idx)
{
  QFont font = textBrowser->document()->defaultFont();
  font.setWeight(weightCombox->itemData(idx).toInt());
  textBrowser->document()->setDefaultFont(font);
  showFont();
}

void XWSystemFontWidget::showFont()
{
  textBrowser->clear();
  QFont font = textBrowser->document()->defaultFont();
  QString family = font.family();
  QTextCursor cursor = textBrowser->textCursor();
  QTextBlockFormat blockFormat;
  blockFormat.setAlignment(Qt::AlignCenter);
  cursor.insertBlock(blockFormat);
  cursor.insertText(QString("%1").arg(family));
}
