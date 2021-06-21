/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZARROWDIALOG_H
#define XWTIKZARROWDIALOG_H

#include <QDialog>
#include <QPixmap>

class QLabel;
class QComboBox;
class QDialogButtonBox;

class XWTikzArrowDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzArrowDialog(QWidget * parent = 0);

  int getEndArrow();
  int getStartArrow();

  void setEndArrow(int a);
  void setStartArrow(int a);

private:
  void createArrows();
  void createEndArrow(int index,int d,const QString & str);
  void createStartArrow(int index,int d,const QString & str);
  QIcon createArrow(const QString & arrow);

private:
  QLabel * endlabel;
  QComboBox * endarrows;
  QLabel * startlabel;
  QComboBox * startarrows;

  QDialogButtonBox *buttonBox;
};

#endif //XWTIKZARROWDIALOG_H
