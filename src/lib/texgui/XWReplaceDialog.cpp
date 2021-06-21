/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWReplaceDialog.h"

XWReplaceDialog::XWReplaceDialog(QWidget * parent)
:QDialog(parent)
{
    setWindowTitle(tr("Replace text"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));

    label = new QLabel(tr("Replace:"));
    lineEdit = new QLineEdit;
    label->setBuddy(lineEdit);
    bylabel = new QLabel(tr("By:"));
    bylineEdit = new QLineEdit;
    bylabel->setBuddy(bylineEdit);

    caseCheckBox = new QCheckBox(tr("Match &case"));
    wholeWordsCheckBox = new QCheckBox(tr("Match &Whole words"));
    regexpCheckBox = new QCheckBox(tr("&Regexps match"));
    allCheckBox = new QCheckBox(tr("replace all"));
    findButton = new QPushButton(tr("&Go"));
    findButton->setDefault(true);

    buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(findButton, QDialogButtonBox::ActionRole);

    QGridLayout * txtlayout = new QGridLayout;
    txtlayout->addWidget(label, 0, 0);
    txtlayout->addWidget(lineEdit,0, 1);
    txtlayout->addWidget(bylabel, 1, 0);
    txtlayout->addWidget(bylineEdit, 1, 1);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(txtlayout);
    leftLayout->addWidget(caseCheckBox);
    leftLayout->addWidget(wholeWordsCheckBox);
    leftLayout->addWidget(regexpCheckBox);
    leftLayout->addWidget(allCheckBox);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(leftLayout, 0, 0);
    mainLayout->addWidget(buttonBox, 0, 1);

    connect(findButton, SIGNAL(clicked()), this, SIGNAL(replaceClicked()));
    connect(bylineEdit, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
    connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SIGNAL(textChanged(const QString &)));

    setLayout(mainLayout);
}

bool XWReplaceDialog::allVisible()
{
    return allCheckBox->isVisible();
}

QString XWReplaceDialog::getByText()
{
    return bylineEdit->text();
}

QString XWReplaceDialog::getText()
{
    return lineEdit->text();
}

void XWReplaceDialog::hideAll(bool h)
{
    allCheckBox->setVisible(!h);
}

bool XWReplaceDialog::isAll()
{
    return allCheckBox->isChecked();
}

bool XWReplaceDialog::isCaseSensitive()
{
    return caseCheckBox->isChecked();
}

bool XWReplaceDialog::isRegexpMatch()
{
    return regexpCheckBox->isChecked();
}

bool XWReplaceDialog::isWholeWords()
{
    return wholeWordsCheckBox->isChecked();
}

void XWReplaceDialog::setText(const QString & str)
{
    lineEdit->setText(str);
}