/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWSETTINGEDITOR_H
#define XWSETTINGEDITOR_H

#include <QDialog>
#include <QTreeWidget>
#include <QItemDelegate>
#include <QRegExp>
#include <QHash>

class QSettings;
class QComboBox;
class QLineEdit;
class QPushButton;
class QLabel;

class XWVariantDelegate: public QItemDelegate
{
  Q_OBJECT

public:
  XWVariantDelegate(QObject *parent = 0);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

  static bool isSupportedType(QVariant::Type type);
  static QString displayText(const QVariant &value);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;

  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

private:
  mutable QRegExp boolExp;
  mutable QRegExp byteArrayExp;
  mutable QRegExp charExp;
  mutable QRegExp colorExp;
  mutable QRegExp dateExp;
  mutable QRegExp dateTimeExp;
  mutable QRegExp doubleExp;
  mutable QRegExp pointExp;
  mutable QRegExp rectExp;
  mutable QRegExp signedIntegerExp;
  mutable QRegExp sizeExp;
  mutable QRegExp timeExp;
  mutable QRegExp unsignedIntegerExp;
};

class XWSettingEditor: public QTreeWidget
{
  Q_OBJECT

public:
  XWSettingEditor(QWidget *parent = 0);

  bool cnf() {return iscnf;}

  QString fileName() {return baseName;}

  void setSetting(QSettings * sA);
  QSize sizeHint() const;

public slots:
  void add(const QString & name);
  void del();

private slots:
  void updateSetting(QTreeWidgetItem *item);

private:
  QTreeWidgetItem *childAt(QTreeWidgetItem *parent, int index);
  int childCount(QTreeWidgetItem *parent);
  QTreeWidgetItem *createItem(const QString &text, QTreeWidgetItem *parent, int index);

  int findChild(QTreeWidgetItem *parent, const QString &text, int startIndex);

  void moveItemForward(QTreeWidgetItem *parent, int oldIndex, int newIndex);

  void refresh();

  void updateChildItems(QTreeWidgetItem *parent);

private:
  QSettings * setting;
  QString baseName;
  bool iscnf;
};

class XWSettingDialog: public QDialog
{
  Q_OBJECT

public:
  XWSettingDialog(QWidget *parent = 0);

private slots:
  void add();

  void del();

  void setSetting();

private:
  QLabel * slabel;
  QComboBox * settingsComboBox;
  QLabel * nlabel;
  QLineEdit * nameLineEdit;
  QPushButton * addButton;
  QPushButton * delButton;

  XWSettingEditor * settingEditor;

  QHash<QString, QSettings*> settings;
};

#endif /XWSETTINGEDITOR_H
