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
#include "XWToPSWindow.h"

XWToPSWindow::XWToPSWindow(const QString & basename, 
	                       int pgnum, 
	                       QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Convert to PostScript"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    setting = new XWDocSetting;
    dirty = false;
    
    tab = new QTabWidget;
    
    fileNameLabel = new QLabel(tr("PS file name:"));
    fileNameEdit = new QLineEdit;
    QString psname = QString("%1.ps").arg(basename);
    fileNameEdit->setText(psname);
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
    
    cropBox = new QCheckBox(tr("Cropped  to pdf cropbox"));
    cropBox->setChecked(setting->getPSCrop());
    
    expandSmallerBox = new QCheckBox(tr("Expand smaller PDF pages"));
    expandSmallerBox->setChecked(setting->getPSExpandSmaller());
    
    shrinkLargerBox = new QCheckBox(tr("Shrink larger PDF pages"));
    shrinkLargerBox->setChecked(setting->getPSShrinkLarger());
    
    centerBox = new QCheckBox(tr("Center smaller PDF pages"));
    centerBox->setChecked(setting->getPSCenter());
    
    duplexBox = new QCheckBox(tr("Duplex"));
    duplexBox->setChecked(setting->getPSDuplex());
    
    QVBoxLayout *pageLayout = new QVBoxLayout;
    pageLayout->addWidget(cropBox);
    pageLayout->addWidget(expandSmallerBox);
    pageLayout->addWidget(shrinkLargerBox);
    pageLayout->addWidget(centerBox);
    pageLayout->addWidget(duplexBox);
    pageWidget = new QWidget;
    pageWidget->setLayout(pageLayout);
    tab->addTab(pageWidget, tr("Page setting"));
    
    levelLabel = new QLabel(tr("PostScript  level:"));
    levelCombo = new QComboBox;
    levelCombo->addItem("Level1");
    levelCombo->addItem("Level1Sep");
    levelCombo->addItem("Level2");
    levelCombo->addItem("Level2Sep");
    levelCombo->addItem("Level3");
    levelCombo->addItem("Level3Sep");
    levelCombo->setCurrentIndex(setting->getPSLevel());
    
    QHBoxLayout * combolayout = new QHBoxLayout;
    combolayout->addWidget(levelLabel);
    combolayout->addWidget(levelCombo);
    embedType1Box = new QCheckBox(tr("Embed Type1 fonts"));
    embedType1Box->setChecked(setting->getPSEmbedType1());
    
    embedTrueTypeBox = new QCheckBox(tr("Embed TrueType fonts"));
    embedTrueTypeBox->setChecked(setting->getPSEmbedTrueType());
    
    embedCIDPostScriptBox = new QCheckBox(tr("Embed PostScript fonts"));
    embedCIDPostScriptBox->setChecked(setting->getPSEmbedCIDPostScript());
    
    embedCIDTrueTypeBox = new QCheckBox(tr("Embed CID  TrueType fonts"));
    embedCIDTrueTypeBox->setChecked(setting->getPSEmbedCIDTrueType());
    
    QVBoxLayout *fontlayout = new QVBoxLayout;
    fontlayout->addLayout(combolayout);
    fontlayout->addWidget(embedType1Box);
    fontlayout->addWidget(embedTrueTypeBox);
    fontlayout->addWidget(embedCIDPostScriptBox);
    fontlayout->addWidget(embedCIDTrueTypeBox);
    fontWidget = new QWidget;
    fontWidget->setLayout(fontlayout);
    tab->addTab(fontWidget, tr("Font setting"));
    
    paperSizeLabel = new QLabel(tr("Paper size:"));
	paperCombo = new QComboBox;
	QStringList specs = XWPaper::getPaperSpecs();
	specs << tr("Custom");
	paperCombo->addItems(specs);
	XWPaper paper(setting->getPSPaperWidth(), setting->getPSPaperHeight());
	QString spec = paper.getSpec();
	int idx = specs.size() - 1;
	if (!spec.isEmpty())
		idx = specs.indexOf(spec);
	paperCombo->setCurrentIndex(idx);
	connect(paperCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setPaperSize(int)));
	
	paperWidthLabel = new QLabel(tr("Paper width:"));
	paperWidthBox = new QDoubleSpinBox;
	paperWidthBox->setRange(72.0, 72000.0);
	paperWidthBox->setValue(setting->getPSPaperWidth());
	paperHeightLabel = new QLabel(tr("Paper height:"));
	paperHeightBox = new QDoubleSpinBox;
	paperHeightBox->setRange(72.0, 72000.0);
	paperHeightBox->setValue(setting->getPSPaperHeight());
	
	
	QGridLayout * paperlayout = new QGridLayout;
	paperlayout->addWidget(paperSizeLabel, 0, 0);
	paperlayout->addWidget(paperCombo, 0, 1);
	paperlayout->addWidget(paperWidthLabel, 2, 0);
	paperlayout->addWidget(paperWidthBox, 2, 1);
	paperlayout->addWidget(paperHeightLabel, 3, 0);
	paperlayout->addWidget(paperHeightBox, 3, 1);
	paperWidget = new QWidget;
  paperWidget->setLayout(paperlayout);
  tab->addTab(paperWidget, tr("Paper setting"));
	
	int llx, lly, urx, ury;
	setting->getPSImageableArea(&llx, &lly, &urx, &ury);
	
	imageableLLXLabel = new QLabel(tr("Lower-left X:"));
	imageableLLXBox = new QSpinBox;
	imageableLLXBox->setRange(0, 10000);
	imageableLLXBox->setValue(llx);
	imageableLLYLabel = new QLabel(tr("Lower-left Y:"));
	imageableLLYBox = new QSpinBox;
	imageableLLYBox->setRange(0, 10000);
	imageableLLYBox->setValue(lly);
	imageableURXLabel = new QLabel(tr("Upper-right X:"));
	imageableURXBox = new QSpinBox;
	imageableURXBox->setRange(0, 10000);
	imageableURXBox->setValue(urx);
	imageableURYLabel = new QLabel(tr("Upper-right Y:"));
	imageableURYBox = new QSpinBox;
	imageableURYBox->setRange(0, 10000);
	imageableURYBox->setValue(ury);
	connect(imageableLLXBox, SIGNAL(valueChanged(int)), this, SLOT(setImageableDirty()));
	connect(imageableLLYBox, SIGNAL(valueChanged(int)), this, SLOT(setImageableDirty()));
	connect(imageableURXBox, SIGNAL(valueChanged(int)), this, SLOT(setImageableDirty()));
	connect(imageableURYBox, SIGNAL(valueChanged(int)), this, SLOT(setImageableDirty()));
	
	QGridLayout * imageablelayout = new QGridLayout;
	imageablelayout->addWidget(imageableLLXLabel, 0, 0);
	imageablelayout->addWidget(imageableLLXBox, 0, 1);
	imageablelayout->addWidget(imageableLLYLabel, 1, 0);
	imageablelayout->addWidget(imageableLLYBox, 1, 1);
	imageablelayout->addWidget(imageableURXLabel, 2, 0);
	imageablelayout->addWidget(imageableURXBox, 2, 1);
	imageablelayout->addWidget(imageableURYLabel, 3, 0);
	imageablelayout->addWidget(imageableURYBox, 3, 1);
	imageableWidget = new QWidget;
	imageableWidget->setLayout(imageablelayout);
	tab->addTab(imageableWidget, tr("Imageable area"));
		
	preloadBox = new QCheckBox(tr("Preload image data"));
	preloadBox->setChecked(setting->getPSPreload());
    
	opiBox = new QCheckBox(tr("Generates PostScript OPI comments"));
	opiBox->setChecked(setting->getPSOPI());
    
	asciiHexBox = new QCheckBox(tr("Use ASCIIHexEncode filter"));
	asciiHexBox->setChecked(setting->getPSASCIIHex());
    
	QVBoxLayout *misclayout = new QVBoxLayout;
	misclayout->addWidget(preloadBox);
	misclayout->addWidget(opiBox);
	misclayout->addWidget(asciiHexBox);
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
			
	path += "doc/ps";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);
		
	resize(685, 200);
}

XWToPSWindow::~XWToPSWindow()
{
	delete setting;
}

void XWToPSWindow::accept()
{
	if (dirty)
	{
		setting->setPSImageableArea(imageableLLXBox->value(), 
		                   imageableLLYBox->value(),
		                   imageableURXBox->value(),
		                   imageableURYBox->value());
	}
	
	int idx = paperCombo->currentIndex();
	if (dirty && idx == (paperCombo->count() - 1))
	{
		QString p = QString("%1 %2")
		                   .arg(paperWidthBox->value())
		                   .arg(paperHeightBox->value());
		setting->setPSPaperSize(p);
	}
	else if (dirty)
	{
		QString spec = paperCombo->itemText(idx);
		setting->setPSPaperSize(spec);
	}
	
	setting->setPSASCIIHex(asciiHexBox->isChecked());
	setting->setPSCenter(centerBox->isChecked());
	setting->setPSEmbedCIDPostScript(embedCIDPostScriptBox->isChecked());
	setting->setPSEmbedCIDTrueType(embedCIDTrueTypeBox->isChecked());
	setting->setPSCrop(cropBox->isChecked());
	setting->setPSDuplex(duplexBox->isChecked());
	setting->setPSEmbedTrueType(embedTrueTypeBox->isChecked());
	setting->setPSEmbedType1(embedType1Box->isChecked());
	setting->setPSOPI(opiBox->isChecked());
	setting->setPSExpandSmaller(expandSmallerBox->isChecked());
	setting->setPSPreload(preloadBox->isChecked());
	PSLevel level = (PSLevel)(levelCombo->currentIndex());
	setting->setPSLevel(level);
	setting->setPSShrinkLarger(shrinkLargerBox->isChecked());
	
	QDialog::accept();
}

bool  XWToPSWindow::allPages()
{
	return allButton->isChecked();
}

QString XWToPSWindow::getFileName()
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
	
	if (!path.endsWith(".ps", Qt::CaseInsensitive))
		path += ".ps";
		
	return path;
}

QList<int> XWToPSWindow::getPages(bool dvi)
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

void XWToPSWindow::open()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save file"),
                            path, tr("PS file (*.ps)"));
	fileNameEdit->setText(filename);
}

void XWToPSWindow::setImageableDirty()
{
	dirty = true;
}

void XWToPSWindow::setPaperSize(int idx)
{
	if (idx != (paperCombo->count() - 1))
	{
		QString spec = paperCombo->itemText(idx);
		XWPaper paper(spec);
		paperWidthBox->setValue(paper.ptWidth());
		paperHeightBox->setValue(paper.ptHeight());
	}
	
	dirty = true;
}
