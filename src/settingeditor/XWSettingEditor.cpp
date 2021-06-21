/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include <QDir>

#include "XWSettingEditor.h"

XWVariantDelegate::XWVariantDelegate(QObject *parent)
: QItemDelegate(parent)
{
  boolExp.setPattern("true|false");
  boolExp.setCaseSensitivity(Qt::CaseInsensitive);
  byteArrayExp.setPattern("[\\x00-\\xff]*");
  charExp.setPattern(".");
  colorExp.setPattern("\\(([0-9]*),([0-9]*),([0-9]*),([0-9]*)\\)");
  doubleExp.setPattern("");
  pointExp.setPattern("\\((-?[0-9]*),(-?[0-9]*)\\)");
  rectExp.setPattern("\\((-?[0-9]*),(-?[0-9]*),(-?[0-9]*),(-?[0-9]*)\\)");
  signedIntegerExp.setPattern("-?[0-9]*");
  sizeExp = pointExp;
  unsignedIntegerExp.setPattern("[0-9]*");
  dateExp.setPattern("([0-9]{,4})-([0-9]{,2})-([0-9]{,2})");
  timeExp.setPattern("([0-9]{,2}):([0-9]{,2}):([0-9]{,2})");
  dateTimeExp.setPattern(dateExp.pattern() + "T" + timeExp.pattern());
}

QWidget * XWVariantDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
  if (index.column() != 2)
         return 0;

  QVariant originalValue = index.model()->data(index, Qt::UserRole);
  if (!isSupportedType(originalValue.type()))
    return 0;

  QLineEdit *lineEdit = new QLineEdit(parent);
  lineEdit->setFrame(false);

  QRegExp regExp;

  switch (originalValue.type())
  {
     case QVariant::Bool:
         regExp = boolExp;
         break;
     case QVariant::ByteArray:
         regExp = byteArrayExp;
         break;
     case QVariant::Char:
         regExp = charExp;
         break;
     case QVariant::Color:
         regExp = colorExp;
         break;
     case QVariant::Date:
         regExp = dateExp;
         break;
     case QVariant::DateTime:
         regExp = dateTimeExp;
         break;
     case QVariant::Double:
         regExp = doubleExp;
         break;
     case QVariant::Int:
     case QVariant::LongLong:
         regExp = signedIntegerExp;
         break;
     case QVariant::Point:
         regExp = pointExp;
         break;
     case QVariant::Rect:
         regExp = rectExp;
         break;
     case QVariant::Size:
         regExp = sizeExp;
         break;
     case QVariant::Time:
         regExp = timeExp;
         break;
     case QVariant::UInt:
     case QVariant::ULongLong:
         regExp = unsignedIntegerExp;
         break;
     default:
         ;
  }

  if (!regExp.isEmpty())
  {
    QValidator *validator = new QRegExpValidator(regExp, lineEdit);
    lineEdit->setValidator(validator);
  }

  return lineEdit;
}

bool XWVariantDelegate::isSupportedType(QVariant::Type type)
{
  switch (type)
  {
     case QVariant::Bool:
     case QVariant::ByteArray:
     case QVariant::Char:
     case QVariant::Color:
     case QVariant::Date:
     case QVariant::DateTime:
     case QVariant::Double:
     case QVariant::Int:
     case QVariant::LongLong:
     case QVariant::Point:
     case QVariant::Rect:
     case QVariant::Size:
     case QVariant::String:
     case QVariant::StringList:
     case QVariant::Time:
     case QVariant::UInt:
     case QVariant::ULongLong:
         return true;
     default:
         return false;
  }
}

QString XWVariantDelegate::displayText(const QVariant &value)
{
  switch (value.type())
  {
     case QVariant::Bool:
     case QVariant::ByteArray:
     case QVariant::Char:
     case QVariant::Double:
     case QVariant::Int:
     case QVariant::LongLong:
     case QVariant::String:
     case QVariant::UInt:
     case QVariant::ULongLong:
         return value.toString();
     case QVariant::Color:
         {
             QColor color = qvariant_cast<QColor>(value);
             return QString("(%1,%2,%3,%4)")
                    .arg(color.red()).arg(color.green())
                    .arg(color.blue()).arg(color.alpha());
         }
     case QVariant::Date:
         return value.toDate().toString(Qt::ISODate);
     case QVariant::DateTime:
         return value.toDateTime().toString(Qt::ISODate);
     case QVariant::Invalid:
         return "<Invalid>";
     case QVariant::Point:
         {
             QPoint point = value.toPoint();
             return QString("(%1,%2)").arg(point.x()).arg(point.y());
         }
     case QVariant::Rect:
         {
             QRect rect = value.toRect();
             return QString("(%1,%2,%3,%4)")
                    .arg(rect.x()).arg(rect.y())
                    .arg(rect.width()).arg(rect.height());
         }
     case QVariant::Size:
         {
             QSize size = value.toSize();
             return QString("(%1,%2)").arg(size.width()).arg(size.height());
         }
     case QVariant::StringList:
         return value.toStringList().join(",");
     case QVariant::Time:
         return value.toTime().toString(Qt::ISODate);
     default:
         break;
  }
  return QString("<%1>").arg(value.typeName());
}

void XWVariantDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
              const QModelIndex &index) const
{
  if (index.column() == 2)
  {
    QVariant value = index.model()->data(index, Qt::UserRole);
    if (!isSupportedType(value.type()))
    {
      QStyleOptionViewItem myOption = option;
      myOption.state &= ~QStyle::State_Enabled;
      QItemDelegate::paint(painter, myOption, index);
      return;
    }
  }

  QItemDelegate::paint(painter, option, index);
}

void XWVariantDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QVariant value = index.model()->data(index, Qt::UserRole);
  if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor))
    lineEdit->setText(displayText(value));
}

void XWVariantDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                     const QModelIndex &index) const
{
  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
  if (!lineEdit->isModified())
    return;

  QString text = lineEdit->text();
  const QValidator *validator = lineEdit->validator();
  if (validator)
  {
    int pos;
    if (validator->validate(text, pos) != QValidator::Acceptable)
      return;
  }

  QVariant originalValue = index.model()->data(index, Qt::UserRole);
  QVariant value;

  switch (originalValue.type())
  {
     case QVariant::Char:
         value = text.at(0);
         break;
     case QVariant::Color:
         colorExp.exactMatch(text);
         value = QColor(qMin(colorExp.cap(1).toInt(), 255),
                        qMin(colorExp.cap(2).toInt(), 255),
                        qMin(colorExp.cap(3).toInt(), 255),
                        qMin(colorExp.cap(4).toInt(), 255));
         break;
     case QVariant::Date:
         {
             QDate date = QDate::fromString(text, Qt::ISODate);
             if (!date.isValid())
                 return;
             value = date;
         }
         break;
     case QVariant::DateTime:
         {
             QDateTime dateTime = QDateTime::fromString(text, Qt::ISODate);
             if (!dateTime.isValid())
                 return;
             value = dateTime;
         }
         break;
     case QVariant::Point:
         pointExp.exactMatch(text);
         value = QPoint(pointExp.cap(1).toInt(), pointExp.cap(2).toInt());
         break;
     case QVariant::Rect:
         rectExp.exactMatch(text);
         value = QRect(rectExp.cap(1).toInt(), rectExp.cap(2).toInt(),
                       rectExp.cap(3).toInt(), rectExp.cap(4).toInt());
         break;
     case QVariant::Size:
         sizeExp.exactMatch(text);
         value = QSize(sizeExp.cap(1).toInt(), sizeExp.cap(2).toInt());
         break;
     case QVariant::StringList:
         value = text.split(",");
         break;
     case QVariant::Time:
         {
             QTime time = QTime::fromString(text, Qt::ISODate);
             if (!time.isValid())
                 return;
             value = time;
         }
         break;
     default:
         value = text;
         value.convert(originalValue.type());
  }

  model->setData(index, displayText(value), Qt::DisplayRole);
  model->setData(index, value, Qt::UserRole);
}

XWSettingEditor::XWSettingEditor(QWidget *parent)
: QTreeWidget(parent)
{
  setItemDelegate(new XWVariantDelegate(this));

  QStringList labels;
  labels << tr("Setting") << tr("Type") << tr("Value");
  setHeaderLabels(labels);
  header()->setResizeMode(0, QHeaderView::Stretch);
  header()->setResizeMode(2, QHeaderView::Stretch);

  setting = 0;
  iscnf = false;
}


void XWSettingEditor::setSetting(QSettings * sA)
{
  setting = sA;
  clear();
  if (setting)
  {
    QString filname = setting->fileName();
    QFileInfo info(filname);
    baseName = info.baseName();
    iscnf = filname.endsWith(".cnf",Qt::CaseInsensitive);
    refresh();
  }
}

QSize XWSettingEditor::sizeHint() const
{
  return QSize(800, 600);
}

void XWSettingEditor::add(const QString & name)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(this);
  if (baseName == "texmf")
  {
    QString key = QString("TEXINPUTS.%1").arg(name);
    item->setText(0, key);
    item->setText(1, "");
    item->setText(2, "$TEXTOPDIR/{xiuwen,latex,generic,}//");
  }
  else
  {
    item->setText(0, name);
    if (baseName == "formats")
    {
      QTreeWidgetItem * c = new QTreeWidgetItem(item);
      c->setText(0, "description");
      c->setText(1, "");
      c->setText(2, "some text");
      c = new QTreeWidgetItem(item);
      c->setText(0, "input");
      c->setText(1, "");
      QString key = QString("%1.ini").arg(name);
      c->setText(2, key);
      c = new QTreeWidgetItem(item);
      c->setText(0, "ocp_buf_size");
      c->setText(1, "");
      c->setText(2, "500000");
      c = new QTreeWidgetItem(item);
      c->setText(0, "ocp_stack_size");
      c->setText(1, "");
      c->setText(2, "10000");
      c = new QTreeWidgetItem(item);
      c->setText(0, "ocp_list_size");
      c->setText(1, "");
      c->setText(2, "1000");
    }
    else if (baseName == "texfonts")
    {
      QTreeWidgetItem * c = new QTreeWidgetItem(item);
      c->setText(0, "font");
      c->setText(1, "");
      c->setText(2, "font file name");
      c = new QTreeWidgetItem(item);
      c->setText(0, "inencname");
      c->setText(1, "");
      c->setText(2, "unicode");
      c = new QTreeWidgetItem(item);
      c->setText(0, "pid");
      c->setText(1, "");
      c->setText(2, "3");
      c = new QTreeWidgetItem(item);
      c->setText(0, "eid");
      c->setText(1, "");
      c->setText(2, "1");
      c = new QTreeWidgetItem(item);
      c->setText(0, "stemv");
      c->setText(1, "");
      c->setText(2, "50");
      c = new QTreeWidgetItem(item);
      c->setText(0, "slant");
      c->setText(1, "");
      c->setText(2, "0");
      c = new QTreeWidgetItem(item);
      c->setText(0, "efactor");
      c->setText(1, "");
      c->setText(2, "1");
      c = new QTreeWidgetItem(item);
      c->setText(0, "boldf");
      c->setText(1, "");
      c->setText(2, "0");
      c = new QTreeWidgetItem(item);
      c->setText(0, "index");
      c->setText(1, "");
      c->setText(2, "0");
      c = new QTreeWidgetItem(item);
      c->setText(0, "bold");
      c->setText(1, "");
      c->setText(2, "false");
      c = new QTreeWidgetItem(item);
      c->setText(0, "italic");
      c->setText(1, "");
      c->setText(2, "false");
      c = new QTreeWidgetItem(item);
      c->setText(0, "wmode");
      c->setText(1, "");
      c->setText(2, "0");
      c = new QTreeWidgetItem(item);
      c->setText(0, "noembed");
      c->setText(1, "");
      c->setText(2, "false");
    }
  }
}

void XWSettingEditor::del()
{
  QTreeWidgetItem *item = currentItem();
  if (!item)
    return ;

  if (item->parent())
    item = item->parent();

  QString key = item->text(0);
  setting->remove(key);
}

void XWSettingEditor::updateSetting(QTreeWidgetItem *item)
{
  QString key = item->text(0);
  QTreeWidgetItem *ancestor = item->parent();
  while (ancestor)
  {
    key.prepend(ancestor->text(0) + "/");
    ancestor = ancestor->parent();
  }

  setting->setValue(key, item->data(2, Qt::UserRole));
}

QTreeWidgetItem * XWSettingEditor::childAt(QTreeWidgetItem *parent, int index)
{
  if (parent)
    return parent->child(index);
  else
    return topLevelItem(index);
}

int XWSettingEditor::childCount(QTreeWidgetItem *parent)
{
  if (parent)
    return parent->childCount();
  else
    return topLevelItemCount();
}

QTreeWidgetItem  * XWSettingEditor::createItem(const QString &text, QTreeWidgetItem *parent,
                               int index)
{
  QTreeWidgetItem *after = 0;
  if (index != 0)
    after = childAt(parent, index - 1);

  QTreeWidgetItem *item;
  if (parent)
    item = new QTreeWidgetItem(parent, after);
  else
    item = new QTreeWidgetItem(this, after);

  item->setText(0, text);
  item->setFlags(item->flags() | Qt::ItemIsEditable);
  return item;
}

int XWSettingEditor::findChild(QTreeWidgetItem *parent, const QString &text, int startIndex)
{
  for (int i = startIndex; i < childCount(parent); ++i)
  {
    if (childAt(parent, i)->text(0) == text)
      return i;
  }
  return -1;
}

void XWSettingEditor::moveItemForward(QTreeWidgetItem *parent, int oldIndex, int newIndex)
{
  for (int i = 0; i < oldIndex - newIndex; ++i)
    delete childAt(parent, newIndex);
}

void XWSettingEditor::refresh()
{
  if (!setting)
    return;

  disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(updateSetting(QTreeWidgetItem*)));

  setting->sync();
  updateChildItems(0);

  connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(updateSetting(QTreeWidgetItem*)));
}

void XWSettingEditor::updateChildItems(QTreeWidgetItem *parent)
{
  int dividerIndex = 0;

  foreach (QString group, setting->childGroups())
  {
    QTreeWidgetItem *child;
    int childIndex = findChild(parent, group, dividerIndex);
    if (childIndex != -1)
    {
      child = childAt(parent, childIndex);
      child->setText(1, "");
      child->setText(2, "");
      child->setData(2, Qt::UserRole, QVariant());
      moveItemForward(parent, childIndex, dividerIndex);
    }
    else
    {
      child = createItem(group, parent, dividerIndex);
    }

    ++dividerIndex;

    setting->beginGroup(group);
    updateChildItems(child);
    setting->endGroup();
  }

  foreach (QString key, setting->childKeys())
  {
    QTreeWidgetItem *child;
    int childIndex = findChild(parent, key, 0);

    if (childIndex == -1 || childIndex >= dividerIndex)
    {
      if (childIndex != -1)
      {
        child = childAt(parent, childIndex);
        for (int i = 0; i < child->childCount(); ++i)
          delete childAt(child, i);
        moveItemForward(parent, childIndex, dividerIndex);
      }
      else
      {
        child = createItem(key, parent, dividerIndex);
      }

      ++dividerIndex;
    }
    else
    {
      child = childAt(parent, childIndex);
    }

    QVariant value = setting->value(key);
    if (value.type() == QVariant::Invalid)
    {
      child->setText(1, "Invalid");
    }
    else
    {
       child->setText(1, value.typeName());
    }
    child->setText(2, XWVariantDelegate::displayText(value));
    child->setData(2, Qt::UserRole, value);
  }

  while (dividerIndex < childCount(parent))
    delete childAt(parent, dividerIndex);
}

XWSettingDialog::XWSettingDialog(QWidget *parent)
:QDialog(parent)
{
  setWindowTitle(tr("Xiuwen settings editor"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  slabel = new QLabel(tr("setting"),this);
  settingsComboBox = new QComboBox(this);

  nlabel = new QLabel(tr("new name"),this);
  nameLineEdit = new QLineEdit(this);
  addButton = new QPushButton(tr("add"), this);
  delButton = new QPushButton(tr("delete"), this);

  settingEditor = new XWSettingEditor(this);

  QGridLayout *layout = new QGridLayout();
  layout->addWidget(slabel, 0, 0);
  layout->addWidget(settingsComboBox, 0, 1);
  layout->addWidget(nlabel, 1, 0);
  layout->addWidget(nameLineEdit, 1, 1);
  layout->addWidget(addButton, 1, 2);
  layout->addWidget(delButton, 1, 3);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(layout);
  mainLayout->addWidget(settingEditor);
  setLayout(mainLayout);

  QStringList filter;
  filter << "*.cnf" << "*.ini";
  QString path = QCoreApplication::applicationDirPath();
  QDir dir(path);
  QFileInfoList files = dir.entryInfoList(filter,QDir::Files);
  for (int i = 0; i < files.size(); i++)
  {
    QFileInfo file = files[i];
    QString bn = file.baseName();
    settingsComboBox->addItem(bn);
    QString filepath = file.absoluteFilePath();
    QSettings * setting = new QSettings(filepath,QSettings::IniFormat);
    settings[bn] = setting;
  }

  setSetting();

  connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
  connect(delButton, SIGNAL(clicked()), this, SLOT(del()));
  connect(settingsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setSetting()));
}

void XWSettingDialog::add()
{
  QString name = nameLineEdit->text();
  if (name.isEmpty())
  {
    QMessageBox msgBox(this);
    msgBox.setText(tr("Name is empty!"));
    msgBox.exec();
  }
  else
  {
    QString cur = settingEditor->fileName();
    if (cur == "formats")
    {
      cur = "texmf";
      QSettings * setting = settings[cur];
      QString key = QString("TEXINPUTS.%1").arg(name);
      setting->setValue(key,"$TEXTOPDIR/{xiuwen,latex,generic,}//");
    }
    settingEditor->add(name);
  }
}

void XWSettingDialog::del()
{
  settingEditor->del();
}

void XWSettingDialog::setSetting()
{
  QString cur = settingsComboBox->currentText();
  if (cur.isEmpty())
    return ;

  if (cur == "formats" || cur == "texfonts")
  {
    nlabel->setVisible(true);
    nameLineEdit->setVisible(true);
    addButton->setVisible(true);
    delButton->setVisible(true);
  }
  else
  {
    nlabel->setVisible(false);
    nameLineEdit->setVisible(false);
    addButton->setVisible(false);
    delButton->setVisible(false);
  }

  QSettings * setting = settings[cur];
  settingEditor->setSetting(setting);  
}
