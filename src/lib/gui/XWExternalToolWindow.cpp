/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWString.h"
#include "XWDocSetting.h"
#include "XWPictureSea.h"
#include "XWExternalToolWindow.h"

XWExternalToolWindow::XWExternalToolWindow(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("External Tools"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    setting = new XWDocSetting;
    sea = new XWPictureSea;
    tab = new QTabWidget;
    mulabel = new QLabel(tr("Tools for playing movie and opening URL. "
                            "Use \"%s\" to instead input file name."));
	movieLabel = new QLabel(tr("Play Movie With:"));
	movieEdit = new QLineEdit;
	XWString * cmdstr = setting->getMovieCommand();
	if (cmdstr)
	{
		QString s = cmdstr->toQString();
		movieEdit->setText(s);
	}
	urlLabel = new QLabel(tr("Open URL with:"));
	urlEdit = new QLineEdit;
	cmdstr = setting->getURLCommand();
	if (cmdstr)
	{
		QString s = cmdstr->toQString();
		urlEdit->setText(s);
	}
	
	QHBoxLayout * mlayout = new QHBoxLayout;
	mlayout->addWidget(movieLabel);
	mlayout->addWidget(movieEdit, 1);
	
	QHBoxLayout * ulayout = new QHBoxLayout;
	ulayout->addWidget(urlLabel);
	ulayout->addWidget(urlEdit, 1);
	
	QVBoxLayout * mulayout = new QVBoxLayout;
	mulayout->addWidget(mulabel);
	mulayout->addSpacing(15);
  mulayout->addLayout(mlayout);
  mulayout->addLayout(ulayout);
  mulayout->addStretch(1);
  muWidget = new QWidget;
  muWidget->setLayout(mulayout);
  tab->addTab(muWidget, tr("Movie and URL tools"));
    
  imageLabel = new QLabel(tr("Tools for converting image format."));
  fmtLabel = new QLabel(tr("Image Format:"));
  fmtCombo = new QComboBox;
  QStringList fmts;
  for (int i = 0; i < XWPictureSea::Last; i++)
  {
   	QStringList tmplist = sea->getSuffixes((XWPictureSea::FileFormat)i);
   	QString tmp = tmplist.join(";");
   	fmts << tmp;
  }
    
    fmtCombo->addItems(fmts);
    fmtCombo->setCurrentIndex(XWPictureSea::EPS);
    
    imageCmdLabel = new QLabel(tr("Convert with:"));
    cmdEdit = new QLineEdit;
    infileLabel = new QLabel(tr("Instead input with:"));
    infileEdit = new QLineEdit;
    outfileLabel = new QLabel(tr("Instead uutput with:"));
    outfileEdit = new QLineEdit;
    outextLabel = new QLabel(tr("Output extension[.pdf]:"));
    outextEdit = new QLineEdit;
    
    QString epscmd;
	QString epsinfilep;
	QString epsoutfilep;
	QString epsoutext;
	sea->getFilter(XWPictureSea::EPS, epscmd, epsinfilep, epsoutfilep, epsoutext);
	if (!epscmd.isEmpty())
	{
		cmdEdit->setText(epscmd);
		infileEdit->setText(epsinfilep);
		outfileEdit->setText(epsoutfilep);
		outextEdit->setText(epsoutext);
	}
	else
	{
		cmdEdit->setText("gs -P- -dSAFER -dCompatibilityLevel=1.4 -q -dNOPAUSE "
		                 "-dBATCH -sDEVICE=pdfwrite -sOutputFile=%1 -c "
		                 ".setpdfwrite -f %2");
		infileEdit->setText("%1");
		outfileEdit->setText("%2");
		outextEdit->setText(".pdf");
	}
	
	QHBoxLayout * fmtlayout = new QHBoxLayout;
	fmtlayout->addWidget(fmtLabel);
	fmtlayout->addWidget(fmtCombo);
	
	QHBoxLayout * cmdlayout = new QHBoxLayout;
	cmdlayout->addWidget(imageCmdLabel);
	cmdlayout->addWidget(cmdEdit);
	
	QGridLayout * iolayout = new QGridLayout;
	iolayout->addWidget(infileLabel, 0, 0);
	iolayout->addWidget(infileEdit, 0, 1);
	iolayout->addWidget(outfileLabel, 1, 0);
	iolayout->addWidget(outfileEdit, 1, 1);
	iolayout->addWidget(outextLabel, 2, 0);
	iolayout->addWidget(outextEdit, 2, 1);
	                      
    QVBoxLayout * imglayout = new QVBoxLayout;
    imglayout->addWidget(imageLabel);
    imglayout->addSpacing(15);
    imglayout->addLayout(fmtlayout);
    imglayout->addLayout(cmdlayout);
    imglayout->addLayout(iolayout);
    
    imageWidget = new QWidget;
    imageWidget->setLayout(imglayout);
    tab->addTab(imageWidget, tr("Image convert tools"));
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tab);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    
    connect(cmdEdit, SIGNAL(editingFinished()), this, SLOT(setImageConvert()));
    connect(infileEdit, SIGNAL(editingFinished()), this, SLOT(setImageConvert()));
    connect(outfileEdit, SIGNAL(editingFinished()), this, SLOT(setImageConvert()));
    connect(outextEdit, SIGNAL(editingFinished()), this, SLOT(setImageConvert()));
    
    connect(fmtCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setImage(int)));
    
    resize(400, 220);
}

XWExternalToolWindow::~XWExternalToolWindow()
{
	delete setting;
	delete sea;
}

void XWExternalToolWindow::accept()
{
	QString m = movieEdit->text();
	if (!m.isEmpty() && m.indexOf("%s") != -1)
		setting->setMovieCommand(m);
		
	QString u = urlEdit->text();
	if (!u.isEmpty() && u.indexOf("%s") != -1)
		setting->setMovieCommand(u);
		
	QHash<int, QStringList>::iterator i = hash.begin();
	while (i != hash.end())
	{
		XWPictureSea::FileFormat fmt = (XWPictureSea::FileFormat)(i.key());
		QStringList s = i.value();
		if (s.size() == 4)
		{
			QString cmd = s[0];
			QString insep = s[1];
			QString outsep = s[2];
			QString outext = s[3];
			sea->setFilter(fmt, cmd, insep, outsep, outext);
		}
		i++;
	}
			
	QDialog::accept();
}

void XWExternalToolWindow::setImage(int idx)
{
	QString cmd;
	QString insep;
	QString outsep;
	QString outext;
	if (hash.contains(idx))
	{
		QStringList s = hash[idx];
		if (s.size() == 4)
		{
			cmd = s[0];
			insep = s[1];
			outsep = s[2];
			outext = s[3];
		}
	}
	else if (idx >= 0)
	{
		
		XWPictureSea::FileFormat fmt = (XWPictureSea::FileFormat)(idx);
		sea->getFilter(fmt, cmd, insep, outsep, outext);
	}
	
	cmdEdit->setText(cmd);
	infileEdit->setText(insep);
	outfileEdit->setText(outsep);
	outextEdit->setText(outext);
}

void XWExternalToolWindow::setImageConvert()
{
	QString cmd = cmdEdit->text();
	QString insep = infileEdit->text();
	QString outsep = outfileEdit->text();
	QString outext = outextEdit->text();
	int idx = fmtCombo->currentIndex();
	if (!cmd.isEmpty() && 
		!insep.isEmpty() && 
		!outsep.isEmpty() && 
		!outext.isEmpty() && 
		idx >= 0)
	{
		QStringList s;
		s << cmd << insep << outsep << outext;
		hash[idx] = s;
	}
}

