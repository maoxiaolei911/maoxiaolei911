/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWSYSTEMFONTSWIDGET_H
#define XWSYSTEMFONTSWIDGET_H

#include <QWidget>
#include <QColor>
#include "XWTeXGuiType.h"

class QFontComboBox;
class QComboBox;
class QLabel;
class QTextEdit;
class QPushButton;
class QAction;
class QCheckBox;

class XW_TEXGUI_EXPORT XWSystemFontWidget : public QWidget
{
    Q_OBJECT

public:
    XWSystemFontWidget(QWidget * parent = 0);

    QColor getColor();
    QString getFamily();
    bool getItalic();
    int getSize();
    int getWeight();

    void setBgColor(const QColor & c);
    void setFont(const QString & fam, int weight, bool italic, int size, const QColor & c);

private slots:
    void setColor();
    void setFont(const QFont & font);
    void setItalic(int state);
    void setSize(int idx);
    void setWeight(int idx);

private:
  void showFont();

private:
  QLabel * fontLabel;
  QFontComboBox * fontCombox;

  QLabel * sizeLabel;
  QComboBox * sizeCombox;

  QLabel * weightLabel;
  QComboBox * weightCombox;

  QCheckBox * italicBox;

  QLabel * colorLabel;
  QPushButton * colorButton;
  QColor color;

  QTextEdit * textBrowser;
};

#endif //XWSYSTEMFONTSWIDGET_H
