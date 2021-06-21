/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include <QCoreApplication>
#include <QDir>
#include <QImageWriter>
#include "XWFileName.h"
#include "XWPaper.h"
#include "XWPictureSea.h"
#include "XWToImageWindow.h"

XWToImageWindow::XWToImageWindow(const QString & basename,
	                             int pgnum, 
	                             QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Convert Page To Image"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
	fileNameLabel = new QLabel(tr("Base name:"));
    fileNameEdit = new QLineEdit;
    fileNameEdit->setText(basename);
    browseButton = new QPushButton(tr("Browse"));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(open()));
    
    allButton = new QRadioButton("All pages");
    allButton->setChecked(true);
    rangButton = new QRadioButton(tr("Page ranges(2-10, 12, 15-20):"));
    rangeEdit = new QLineEdit;
    QString range = QString("1-%1").arg(pgnum);
    rangeEdit->setText(range);
    
    createFmtCombo();
    
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
    
    QHBoxLayout * fmtlayout = new QHBoxLayout;
    fmtlayout->addWidget(fmtLabel);
    fmtlayout->addWidget(fmtCombo);
    
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(namelayout);
    mainLayout->addLayout(rangelayout);
    mainLayout->addLayout(fmtlayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    
    createPath();
    
    resize(420, 190);
}

XWToImageWindow::XWToImageWindow(const QString & basename, 
	                             QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Convert to image"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
	fileNameLabel = new QLabel(tr("File name:"));
    fileNameEdit = new QLineEdit;
    fileNameEdit->setText(basename);
    browseButton = new QPushButton(tr("Browse"));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(open()));
            
    bgColor = Qt::white;
    bgLabel = new QLabel(tr("Background color:"));
    bgButton = new QToolButton;
    QAction * a = new QAction(this);
    connect(a, SIGNAL(triggered()), this, SLOT(setBgColor()));
    bgButton->setDefaultAction(a);
    bgButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    bgButton->setIconSize(QSize(70, 25));
    changeColor(bgColor);
    
    reverseVideoBox = new QCheckBox(tr("Reverses colors"));
    reverseVideoBox->setChecked(false);
    
    createFmtCombo();
    
    QHBoxLayout * namelayout = new QHBoxLayout;
    namelayout->addWidget(fileNameLabel);
    namelayout->addWidget(fileNameEdit, 1);
    namelayout->addWidget(browseButton);
        
    QHBoxLayout * bglayout = new QHBoxLayout;
    bglayout->addWidget(bgLabel);
    bglayout->addWidget(bgButton);
    	  
	  QHBoxLayout * fmtlayout = new QHBoxLayout;
    fmtlayout->addWidget(fmtLabel);
    fmtlayout->addWidget(fmtCombo);
    
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(namelayout);
    mainLayout->addLayout(bglayout);    
    mainLayout->addWidget(reverseVideoBox);
    mainLayout->addLayout(fmtlayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    
    createPath();
    
    resize(420, 220);
}

QColor XWToImageWindow::getBgColor()
{
	return bgColor;
}

QString XWToImageWindow::getFileName()
{
	QString filename = fileNameEdit->text();
	if (filename.isEmpty())
		return QString();
	
	QString ss = fmtCombo->currentText();
	ss = ss.toLower();
	int idx = filename.indexOf(QChar('/'));
	if (idx == -1)
	{
		path += QString("/%1").arg(ss);
		QDir d(path);
		if (!d.exists(path))
			d.mkdir(path);
		path += "/";
		path += filename;
	}
	else
	{
		XWFileName fn(filename);	
		if (!fn.isAbsolute())
		{
			path += QString("/%1/").arg(ss);
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
		
	return path;
}

QString XWToImageWindow::getFormat()
{
	return fmtCombo->currentText();
}

QList<int> XWToImageWindow::getPages(bool dvi)
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

bool XWToImageWindow::getReverseVideo()
{
	return reverseVideoBox->isChecked();
}

void XWToImageWindow::changeColor(const QColor &c)
{
	QAction * action = bgButton->defaultAction();
	QPixmap pix(70, 25);
	pix.fill(c);
  action->setIcon(pix);
}

void XWToImageWindow::createFmtCombo()
{
	fmtLabel = new QLabel(tr("Image format:"));
	fmtCombo = new QComboBox;
	foreach (QByteArray format, QImageWriter::supportedImageFormats())
	{
		QString text = tr("%1").arg(QString(format).toUpper());
		fmtCombo->addItem(text);
	}
}

void XWToImageWindow::createPath()
{
	path = QCoreApplication::applicationDirPath();
	int idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);
					
	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');
			
	path += "images";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);
}

void XWToImageWindow::open()
{
	QStringList formats;
	foreach (QByteArray format, QImageReader::supportedImageFormats())
	{
		formats.append("*." + format);
	}
	
	QString filename = QFileDialog::getSaveFileName(this, tr("Save file"),
                            path, tr("Image file (%1)").arg(formats.join(" ")));
	fileNameEdit->setText(filename);
}

void XWToImageWindow::setBgColor()
{
	bgColor = QColorDialog::getColor(Qt::white, this);
	changeColor(bgColor);
}
