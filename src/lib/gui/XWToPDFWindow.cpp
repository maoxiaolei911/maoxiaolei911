/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QCoreApplication>
#include <QDir>
#include "XWFileName.h"
#include "XWPaper.h"
#include "XWDocSetting.h"
#include "XWToPDFWindow.h"

XWToPDFWindow::XWToPDFWindow(const QString & basename, 
	                         int pgnum, 
	                         QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Convert to PDF"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    setting = new XWDocSetting;
    dirty = false;
    
    tab = new QTabWidget;
    fileNameLabel = new QLabel(tr("PDF file name:"));
    fileNameEdit = new QLineEdit;
    QString pdfname = QString("%1.pdf").arg(basename);
    fileNameEdit->setText(pdfname);
    browseButton = new QPushButton(tr("Browse"));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(open()));
    
    allButton = new QRadioButton("All pages");
    allButton->setChecked(true);
    rangButton = new QRadioButton(tr("Page ranges(2-10, 12, 15-20):"));
    rangeEdit = new QLineEdit;
    QString range = QString("1-%1").arg(pgnum);
    rangeEdit->setText(range);
    thumbBox = new QCheckBox(tr("Embed thumbnail images of PNG format"));
    thumbLabel = new QLabel(tr("Image base name:"));
    thumbEdit = new QLineEdit;
    
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
    
    QHBoxLayout * tnamelayout = new QHBoxLayout;
    tnamelayout->addSpacing(20);
    tnamelayout->addWidget(thumbLabel);
    tnamelayout->addWidget(thumbEdit);
    
    QVBoxLayout * thumblayout = new QVBoxLayout;
    thumblayout->addWidget(thumbBox);
    thumblayout->addLayout(tnamelayout);
    
    QVBoxLayout * outlayout = new QVBoxLayout;
    outlayout->addLayout(namelayout);
    outlayout->addLayout(rangelayout);
    outlayout->addLayout(thumblayout);
    outWidget = new QWidget;
    outWidget->setLayout(outlayout);
    tab->addTab(outWidget, tr("Output setting"));
    
    versionLabel = new QLabel(tr("PDF minor version:"));
    versionBox = new QSpinBox;
    versionBox->setRange(3, 7);
	versionBox->setValue(setting->getPDFVersion());	
	
	compressLevelLabel = new QLabel(tr("Compress level:"));
	compressLevelLabel->setToolTip(tr("ZLib compression level (0-9) [9]."
	                                  "9 corresponding to the greatest compression"
	                                  "(i.e. smaller compressed data.)"));
	compressLevelBox = new QSpinBox;
	compressLevelBox->setRange(0, 9);
	compressLevelBox->setValue(setting->getPDFCompressLevel());	
	
	precisionLabel = new QLabel(tr("Decimal precision:"));
	precisionLabel->setToolTip(tr("Number of fractional digits"));
	precisionBox = new QSpinBox;
	precisionBox->setRange(0, 8);
	precisionBox->setValue(setting->getPDFPrecision());	
	
	encryptBox = new QCheckBox(tr("Enable PDF encryption"));
	encryptBox->setChecked(setting->getPDFEncrypt());
	
	owpwdLabel = new QLabel(tr("Owner passward:"));
	owpwdEdit = new QLineEdit;
  owpwdEdit->setEchoMode(QLineEdit::Password);
  	
  uspwdLabel = new QLabel(tr("User passward:"));
	uspwdEdit = new QLineEdit;
  uspwdEdit->setEchoMode(QLineEdit::Password);
	
	keyBitsLabel = new QLabel(tr("Encryption key bits:"));
	keyBitsBox = new QSpinBox;
	keyBitsBox->setRange(40, 128);
	keyBitsBox->setValue(setting->getPDFKeyBits());	
	
	permissionLabel = new QLabel(tr("Encryption permission:"));
	permissionBox = new QSpinBox;
	permissionBox->setMinimum(0);
	permissionBox->setValue(setting->getPDFPermission());
	
	QGridLayout * playout = new QGridLayout;
	playout->addWidget(versionLabel, 0, 0);
	playout->addWidget(versionBox, 0, 1);
	playout->addWidget(compressLevelLabel, 1, 0);
	playout->addWidget(compressLevelBox, 1, 1);
	playout->addWidget(precisionLabel, 2, 0);
	playout->addWidget(precisionBox, 2, 1);
	
	QHBoxLayout * owpwdlayout = new QHBoxLayout;
	owpwdlayout->addSpacing(20);
	owpwdlayout->addWidget(owpwdLabel);
	owpwdlayout->addWidget(owpwdEdit);
	
	QHBoxLayout * uspwdlayout = new QHBoxLayout;
	uspwdlayout->addSpacing(20);
	uspwdlayout->addWidget(uspwdLabel);
	uspwdlayout->addWidget(uspwdEdit);
	
	QHBoxLayout * keylayout = new QHBoxLayout;
	keylayout->addSpacing(20);
	keylayout->addWidget(keyBitsLabel);
	keylayout->addWidget(keyBitsBox);
	
	QHBoxLayout * permlayout = new QHBoxLayout;
	permlayout->addSpacing(20);
	permlayout->addWidget(permissionLabel);
	permlayout->addWidget(permissionBox);
	
	QVBoxLayout * pdflayout = new QVBoxLayout;
	pdflayout->addLayout(playout);
	pdflayout->addWidget(encryptBox);
	pdflayout->addLayout(owpwdlayout);
	pdflayout->addLayout(uspwdlayout);
	pdflayout->addLayout(keylayout);
	pdflayout->addLayout(permlayout);
	pdfWidget = new QWidget;
	pdfWidget->setLayout(pdflayout);
	tab->addTab(pdfWidget, tr("PDF setting"));
	
	paperSizeLabel = new QLabel(tr("Paper size:"));
	paperCombo = new QComboBox;
	QStringList specs = XWPaper::getPaperSpecs();
	specs << tr("Custom");
	paperCombo->addItems(specs);
	
	XWPaper paper(setting->getPDFPaperWidth(), setting->getPDFPaperHeight());
	QString spec = paper.getSpec();
	int idx = specs.size() - 1;
	if (!spec.isEmpty())
		idx = specs.indexOf(spec);
	paperCombo->setCurrentIndex(idx);
	
	paperWidthLabel = new QLabel(tr("Paper width:"));
	paperWidthBox = new QDoubleSpinBox;
	paperWidthBox->setRange(72.0, 72000.0);
	paperWidthBox->setValue(setting->getPDFPaperWidth());
	paperHeightLabel = new QLabel(tr("Paper height:"));
	paperHeightBox = new QDoubleSpinBox;
	paperHeightBox->setRange(72.0, 72000.0);
	paperHeightBox->setValue(setting->getPDFPaperHeight());
	
	hoffLabel = new QLabel(tr("Horizontal offset:"));
	hoffBox = new QDoubleSpinBox;
	hoffBox->setRange(1.0, 720.0);
	hoffBox->setValue(setting->getPDFHoff());
	voffLabel = new QLabel(tr("Vertical offset:"));
	voffBox = new QDoubleSpinBox;
	voffBox->setRange(1.0, 720.0);
	voffBox->setValue(setting->getPDFVoff());
	
	connect(paperCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setPaperSize(int)));
	connect(hoffBox, SIGNAL(valueChanged(double)), this, SLOT(setPaperDirty()));
	connect(voffBox, SIGNAL(valueChanged(double)), this, SLOT(setPaperDirty()));
	
	QGridLayout * paperlayout = new QGridLayout;
	paperlayout->addWidget(paperSizeLabel, 0, 0);
	paperlayout->addWidget(paperCombo, 0, 1);
	paperlayout->addWidget(paperWidthLabel, 1, 0);
	paperlayout->addWidget(paperWidthBox, 1, 1);
	paperlayout->addWidget(paperHeightLabel, 2, 0);
	paperlayout->addWidget(paperHeightBox, 2, 1);
	paperlayout->addWidget(hoffLabel, 3, 0);
	paperlayout->addWidget(hoffBox, 3, 1);
	paperlayout->addWidget(voffLabel, 4, 0);
	paperlayout->addWidget(voffBox, 4, 1);
	paperWidget = new QWidget;
	paperWidget->setLayout(paperlayout);
	tab->addTab(paperWidget, tr("Paper setting"));
	
	magLabel = new QLabel(tr("Magnification:"));
	magBox = new QDoubleSpinBox;
	magBox->setRange(0.001, 4.0);
	magBox->setValue(setting->getPDFMag());	
	QHBoxLayout * maglayout = new QHBoxLayout;
	maglayout->addWidget(magLabel);
	maglayout->addWidget(magBox);
	
	annotGrowLabel = new QLabel(tr("Annotation \"grow\" amount:"));
	annotGrowBox = new QDoubleSpinBox;
	annotGrowBox->setRange(0.0, 1.0);
	annotGrowBox->setValue(setting->getPDFAnnotGrow());	
	QHBoxLayout * annlayout = new QHBoxLayout;
	annlayout->addWidget(annotGrowLabel);
	annlayout->addWidget(annotGrowBox);
	
	ignoreColorsBox = new QCheckBox(tr("Ignore color specials"));
	ignoreColorsBox->setChecked(setting->getPDFIgnoreColors());
	
	CIDFontFixedPitchBox = new QCheckBox(tr("CIDFont fixed pitch"));
	CIDFontFixedPitchBox->setChecked(setting->getPDFCIDFontFixedPitch());
	
	noDestRemoveBox = new QCheckBox(tr("Remove destination that not exists"));
	noDestRemoveBox->setChecked(setting->getPDFNoDestRemove());
	
	tpicTransparentBox = new QCheckBox(tr("TPIC transparent"));
	tpicTransparentBox->setChecked(setting->getPDFTPICTransparent());
	
	QVBoxLayout * misclayout = new QVBoxLayout;
	misclayout->addLayout(maglayout);
	misclayout->addLayout(annlayout);
	misclayout->addWidget(ignoreColorsBox);
	misclayout->addWidget(CIDFontFixedPitchBox);
	misclayout->addWidget(noDestRemoveBox);
	misclayout->addWidget(tpicTransparentBox);
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
	idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);
					
	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');
			
	path += "doc/pdf";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);
		
	resize(474, 275);
}

XWToPDFWindow::~XWToPDFWindow()
{
	delete setting;
}

void XWToPDFWindow::accept()
{
	int idx = paperCombo->currentIndex();
	if (dirty && idx == (paperCombo->count() - 1))
	{
		QString p = QString("%1 %2 %3 %4")
		                   .arg(paperWidthBox->value())
		                   .arg(paperHeightBox->value())
		                   .arg(hoffBox->value())
		                   .arg(voffBox->value());
		setting->setPDFPaperSize(p);
	}
	else if (dirty)
	{
		QString spec = paperCombo->itemText(idx);
		QString p = QString("%1 %2 %3")
		                   .arg(spec)
		                   .arg(hoffBox->value())
		                   .arg(voffBox->value());
		setting->setPDFPaperSize(p);
	}
	
	setting->setPDFAnnotGrow(annotGrowBox->value());
	setting->setPDFCIDFontFixedPitch(CIDFontFixedPitchBox->isChecked());
	setting->setPDFCompressLevel(compressLevelBox->value());
	setting->setPDFEncrypt(encryptBox->isChecked());
	setting->setPDFKeyBits(keyBitsBox->value());
	setting->setPDFIgnoreColors(ignoreColorsBox->isChecked());
	setting->setPDFMag(magBox->value());
	setting->setPDFNoDestRemove(noDestRemoveBox->isChecked());
	setting->setPDFPermission(permissionBox->value());
	setting->setPDFPrecision(precisionBox->value());
	setting->setPDFTPICTransparent(tpicTransparentBox->isChecked());
	setting->setPDFVersion(versionBox->value());
	
	QDialog::accept();
}

bool  XWToPDFWindow::allPages()
{
	return allButton->isChecked();
}

bool XWToPDFWindow::enableThumbnail()
{
	return thumbBox->isChecked();
}

QString XWToPDFWindow::getFileName()
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
	
	if (!path.endsWith(".pdf", Qt::CaseInsensitive))
		path += ".pdf";
	
	return path;
}

QString XWToPDFWindow::getOwnerPassward()
{
	return owpwdEdit->text();
}

QList<int> XWToPDFWindow::getPages()
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
					minv--;
					maxv--;
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
				ret << v - 1;
		}
	}
	
	return ret;
}

QString XWToPDFWindow::getThumbBase()
{
	return thumbEdit->text();
}

QString XWToPDFWindow::getUserPassward()
{
	return uspwdEdit->text();
}

void XWToPDFWindow::open()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save file"),
                            path, tr("PDF file (*.pdf)"));
	fileNameEdit->setText(filename);
}

void XWToPDFWindow::setPaperDirty()
{
	dirty = true;
}

void XWToPDFWindow::setPaperSize(int idx)
{
	QString spec = paperCombo->itemText(idx);
	if (idx != paperCombo->count())
	{
		XWPaper paper(spec);
		paperWidthBox->setValue(paper.ptWidth());
		paperHeightBox->setValue(paper.ptHeight());
	}
	
	dirty = true;
}
