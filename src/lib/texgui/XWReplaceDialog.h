/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWREPLACEDIALOG_H
#define XWREPLACEDIALOG_H

#include <QDialog>
#include "XWTeXGuiType.h"

class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

class XW_TEXGUI_EXPORT XWReplaceDialog : public QDialog
{
    Q_OBJECT
	
public:       
    XWReplaceDialog(QWidget * parent = 0);

    bool allVisible();

    QString getByText();
    QString getText();

    void hideAll(bool h);

    bool isAll();
    bool isCaseSensitive();
    bool isRegexpMatch();
    bool isWholeWords();

    void setText(const QString & str);

signals:
    void replaceClicked();
    void editingFinished();
    void textChanged(const QString & str);

private:
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *bylabel;
    QLineEdit *bylineEdit;
    QCheckBox *caseCheckBox;
    QCheckBox *wholeWordsCheckBox;
    QCheckBox *regexpCheckBox;
    QCheckBox *allCheckBox;
    QDialogButtonBox *buttonBox;
    QPushButton *findButton;
};

#endif // XWREPLACEDIALOG_H
