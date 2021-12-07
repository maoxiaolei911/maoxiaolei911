/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWFINDTEXTDIALOG_H
#define XWFINDTEXTDIALOG_H

#include <QDialog>
#include "XWTeXGuiType.h"

class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

class XW_TEXGUI_EXPORT XWFindTextDialog : public QDialog
{
    Q_OBJECT
	
public:       
    XWFindTextDialog(QWidget * parent = 0);

    QString getText();

    bool isCaseSensitive();
    bool isRegexpMatch();
    bool isWholeWords();

    void setText(const QString & str);

signals:
    void findClicked();
    void editingFinished();
    void textChanged(const QString & str);

private:
    QLabel *label;
    QLineEdit *lineEdit;
    QCheckBox *caseCheckBox;
    QCheckBox *wholeWordsCheckBox;
    QCheckBox *regexpCheckBox;
    QDialogButtonBox *buttonBox;
    QPushButton *findButton;
};

#endif // XWFINDTEXTDIALOG_H
