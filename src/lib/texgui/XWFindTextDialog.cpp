/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWFindTextDialog.h"

XWFindTextDialog::XWFindTextDialog(QWidget * parent)
:QDialog(parent)
{
    setWindowTitle(tr("Find text"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));

    label = new QLabel(tr("Find:"));
    lineEdit = new QLineEdit;
    label->setBuddy(lineEdit);
    caseCheckBox = new QCheckBox(tr("Match &case"));
    wholeWordsCheckBox = new QCheckBox(tr("Match &Whole words"));
    regexpCheckBox = new QCheckBox(tr("&Regexps match"));
    findButton = new QPushButton(tr("&Go"));
    findButton->setDefault(true);

    buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(findButton, QDialogButtonBox::ActionRole);

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(label);
    topLeftLayout->addWidget(lineEdit);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    leftLayout->addWidget(caseCheckBox);
    leftLayout->addWidget(wholeWordsCheckBox);
    leftLayout->addWidget(regexpCheckBox);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(leftLayout, 0, 0);
    mainLayout->addWidget(buttonBox, 0, 1);

    connect(findButton, SIGNAL(clicked()), this, SIGNAL(findClicked()));
    connect(lineEdit, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
    connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SIGNAL(textChanged(const QString &)));

    setLayout(mainLayout);
}

QString XWFindTextDialog::getText()
{
    return lineEdit->text();
}

bool XWFindTextDialog::isCaseSensitive()
{
    return caseCheckBox->isChecked();
}

bool XWFindTextDialog::isRegexpMatch()
{
    return regexpCheckBox->isChecked();
}

bool XWFindTextDialog::isWholeWords()
{
    return wholeWordsCheckBox->isChecked();
}

void XWFindTextDialog::setText(const QString & str)
{
    lineEdit->setText(str);
}
