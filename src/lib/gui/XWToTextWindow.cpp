/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QCoreApplication>
#include <QDir>
#include "XWString.h"
#include "XWFileName.h"
#include "XWPaper.h"
#include "XWDocSetting.h"
#include "XWToTextWindow.h"

XWToTextWindow::XWToTextWindow(const QString & basename, 
	                           int pgnum, 
	                           QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Convert to Plain Text"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    setting = new XWDocSetting;
    
    tab = new QTabWidget;
    
    fileNameLabel = new QLabel(tr("Text file name:"));
    fileNameEdit = new QLineEdit;
    QString txtname = QString("%1.txt").arg(basename);
    fileNameEdit->setText(txtname);
    browseButton = new QPushButton(tr("Browse"));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(open()));
    
    allButton = new QRadioButton("All pages");
    allButton->setChecked(true);
    rangButton = new QRadioButton(tr("Page ranges(2-10, 12, 15-20):"));
    rangeEdit = new QLineEdit;
    QString range = QString("1-%1").arg(pgnum);
    rangeEdit->setText(range);
    
    QHBoxLayout * namelayout = new QHBoxLayout;
    namelayout->addWidget(fileNameLabel);
    namelayout->addWidget(fileNameEdit, 1);
    namelayout->addWidget(browseButton);
    
    QHBoxLayout * rlayout = new QHBoxLayout;  
    rlayout->addSpacing(20);  
    rlayout->addWidget(rangeEdit, 1);
    QVBoxLayout * rangelayout = new QVBoxLayout;
    rangelayout->addWidget(allButton);
    rangelayout->addWidget(rangButton);
    rangelayout->addLayout(rlayout);
    
    QVBoxLayout * outlayout = new QVBoxLayout;
    outlayout->addLayout(namelayout);
    outlayout->addLayout(rangelayout);
    outWidget = new QWidget;
    outWidget->setLayout(outlayout);
    tab->addTab(outWidget, tr("Output setting"));
    
    encodingLabel = new QLabel(tr("Unicode map:"));
    encodingEdit = new QLineEdit;
    XWString * str = setting->getTextEncodingName();
    if (str)
    	encodingEdit->setText(str->toQString());
    	
    eolLabel = new QLabel(tr("End of line:"));
    eolCombo = new QComboBox;
    eolCombo->addItem("Unix");
    eolCombo->addItem("DOS");
    eolCombo->addItem("Mac");
    eolCombo->setCurrentIndex(setting->getTextEOL());
    
    QGridLayout * tlayout = new QGridLayout;
    tlayout->addWidget(encodingLabel, 0, 0);
    tlayout->addWidget(encodingEdit, 0, 1);
    tlayout->addWidget(eolLabel, 1, 0);
    tlayout->addWidget(eolCombo, 1, 1);
    
    pageBreaksBox = new QCheckBox(tr("Insert  page breaks"));
    pageBreaksBox->setChecked(setting->getTextPageBreaks());
    
    keepTinyCharsBox = new QCheckBox(tr("Keep all characters"));
    keepTinyCharsBox->setChecked(setting->getTextKeepTinyChars());
    
   QVBoxLayout * misclayout = new QVBoxLayout;
	misclayout->addLayout(tlayout);
	misclayout->addWidget(pageBreaksBox);
	misclayout->addWidget(keepTinyCharsBox);
	miscWidget = new QWidget;
	miscWidget->setLayout(misclayout);
	tab->addTab(miscWidget, tr("Miscellaneous setting"));
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
	QVBoxLayout *mainLayout = new QVBoxLayout;
   mainLayout->addWidget(tab);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    
    path = QCoreApplication::applicationDirPath();
	int idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);
					
	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');
			
	path += "doc/txt";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);
		
	resize(420, 200);
}

XWToTextWindow::~XWToTextWindow()
{
	delete setting;
}

void  XWToTextWindow::accept()
{
	QString name = encodingEdit->text();
	if (!name.isEmpty())
		setting->setTextEncoding(name);
		
	EndOfLineKind k = (EndOfLineKind)(eolCombo->currentIndex());
	setting->setTextEOL(k);
	setting->setTextKeepTinyChars(keepTinyCharsBox->isChecked());
	setting->setTextPageBreaks(pageBreaksBox->isChecked());
	QDialog::accept();
}

bool  XWToTextWindow::allPages()
{
	return allButton->isChecked();
}

QString XWToTextWindow::getFileName()
{
	QString filename = fileNameEdit->text();
	if (filename.isEmpty())
		return QString();
		
	int idx = filename.indexOf(QChar('/'));
	if (idx == -1)
	{
		path += "/";
		path += filename;
	}
	else
	{
		XWFileName fn(filename);	
		if (!fn.isAbsolute())
		{
			path += "/";
			path += fn.dirPart();
			QDir d(path);
			if (!d.exists(path))
				d.mkdir(path);
			path += "/";
			path += fn.fileName();
		}
		else
			path = filename;
	}
	
	if (!path.endsWith(".txt", Qt::CaseInsensitive))
		path += ".txt";
	
	return path;
}

QList<int> XWToTextWindow::getPages(bool dvi)
{
	QString str = rangeEdit->text();
	QList<int> ret;
	QStringList list = str.split(",", QString::SkipEmptyParts);
	bool ok = true;
	for (int i = 0; i < list.size(); i++)
	{
		QString tmp = list[i];
		tmp.remove(QChar(' '));
		int idx = tmp.indexOf(QChar('-'));
		if (idx > 0)
		{
			QString minstr = tmp.left(idx);			
			int minv = minstr.toInt(&ok);
			if (ok)
			{
				idx++;
				QString maxstr = tmp.right(tmp.length() - idx);
				int maxv = maxstr.toInt(&ok);
				if (ok)
				{
					if (dvi)
					{
						minv--;
						maxv--;
					}
					while (minv <= maxv)
					{
						ret << minv;
						minv++;
					}
				}
			}
		}
		else
		{
			int v = tmp.toInt(&ok);
			if (ok)
			{
				if (dvi)
					ret << v - 1;
				else
					ret << v;
			}
		}
	}
	
	return ret;
}

void XWToTextWindow::open()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save file"),
                            path, tr("Plain text file (*.txt)"));
	fileNameEdit->setText(filename);
}

