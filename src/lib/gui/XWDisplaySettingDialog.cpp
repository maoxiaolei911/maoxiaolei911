/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWDisplaySettingDialog.h"

XWDisplaySettingDialog::XWDisplaySettingDialog(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Display Setting"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
//    QString bgcolor = tr("Background Color:");
    QString opacity = tr("Opacity:");
    QString pgcolor = tr("Paper color:");
    QString reverse = tr("Reverse video:");
    
//    QString bgtip = tr("The background of the paper.");
    QString optip = tr("The level of opacity for the background of "
                        "the paper, from 1.0 (completely opaque) to 0.0 "
                        "(completely transparent).");
	QString pcvtip = tr("The background(paper) of the page display.");
	QString revtip = tr("Reverses the colors of everything except images.");
    
    mainBgColor = palette().base().color();
/*    if (xwApp->hasSetting("MainWinBackgroundColor"))
    {
    	QVariant v = xwApp->getSetting("MainWinBackgroundColor");
		mainBgColor = v.value<QColor>();
    }*/
    
    mainPaperColor = Qt::white;
   	if (xwApp->hasSetting("PageColor"))
   	{
   		QVariant v = xwApp->getSetting("PageColor");
		mainPaperColor = v.value<QColor>();
   	}
   	
    refBgColor = palette().base().color();
/*    if (xwApp->hasSetting("RefWinBackgroundColor"))
    {
    	QVariant v = xwApp->getSetting("RefWinBackgroundColor");
		mainBgColor = v.value<QColor>();
    }*/
    
    refPaperColor = Qt::white;
    if (xwApp->hasSetting("PageColorR"))
   	{
   		QVariant v = xwApp->getSetting("PageColorR");
		refPaperColor = v.value<QColor>();
   	}
   	
   	tab = new QTabWidget;
    
/*    mainWinBgColorLabel = new QLabel(bgcolor);
    mainWinBgColorLabel->setToolTip(bgtip);
    mainWinBgColorButton = new QToolButton;    
    mainWinBgColorButton->setIconSize(QSize(50, 20));
    QAction * action = new QAction(this);
    connect(action, SIGNAL(triggered()), this, SLOT(setMainBgColor()));
    mainWinBgColorButton->setDefaultAction(action);
    changeColor(mainWinBgColorButton, mainBgColor);*/
    
    mainWinOpacityLabel = new QLabel(opacity);
    mainWinOpacityLabel->setToolTip(optip);
    mainWinOpacityBox = new QDoubleSpinBox;
    mainWinOpacityBox->setRange(0.0, 1.0);
    mainWinOpacityBox->setSingleStep(0.01);
    mainWinOpacityBox->setValue(1.0);
    if (xwApp->hasSetting("MainWinOpacity"))
    	mainWinOpacityBox->setValue(xwApp->getSetting("MainWinOpacity").toDouble());
    	
    mainPaperColorLabel = new QLabel(pgcolor);
    mainPaperColorLabel->setToolTip(pcvtip);
    mainPaperColorButton = new QToolButton;
    mainPaperColorButton->setIconSize(QSize(50, 20));
    QAction * action = new QAction(this);
    connect(action, SIGNAL(triggered()), this, SLOT(setMainPaperColor()));
    mainPaperColorButton->setDefaultAction(action);
    changeColor(mainPaperColorButton, mainPaperColor);
    
    mainReverseVideoBox = new QCheckBox(reverse);
    mainReverseVideoBox->setToolTip(revtip);
    mainReverseVideoBox->setChecked(false);
    if (xwApp->hasSetting("ReverseVideo"))
    	mainReverseVideoBox->setChecked(xwApp->getSetting("ReverseVideo").toBool());
    	
    QGridLayout * mainlayout = new QGridLayout;
//    mainlayout->addWidget(mainWinBgColorLabel, 0, 0);
//    mainlayout->addWidget(mainWinBgColorButton, 0, 1);
    mainlayout->addWidget(mainWinOpacityLabel, 0, 0);
    mainlayout->addWidget(mainWinOpacityBox, 0, 1);
    mainlayout->addWidget(mainPaperColorLabel, 1, 0);
    mainlayout->addWidget(mainPaperColorButton, 1, 1);
    mainlayout->addWidget(mainReverseVideoBox, 2, 0);
    
    mainWidget = new QWidget;
    mainWidget->setLayout(mainlayout);
    tab->addTab(mainWidget, tr("Main document window"));
    
/*    refWinBgColorLabel = new QLabel(bgcolor);
    refWinBgColorLabel->setToolTip(bgtip);
    refWinBgColorButton = new QToolButton;
    refWinBgColorButton->setIconSize(QSize(50, 20));
    action = new QAction(this);
    connect(action, SIGNAL(triggered()), this, SLOT(setRefBgColor()));
    refWinBgColorButton->setDefaultAction(action);
    changeColor(refWinBgColorButton, refBgColor);*/
    
    refWinOpacityLabel = new QLabel(opacity);
    refWinOpacityLabel->setToolTip(optip);
    refWinOpacityBox = new QDoubleSpinBox;
    refWinOpacityBox->setRange(0.0, 1.0);
    refWinOpacityBox->setSingleStep(0.01);
    refWinOpacityBox->setValue(1.0);
    if (xwApp->hasSetting("RefWinOpacity"))
    	refWinOpacityBox->setValue(xwApp->getSetting("RefWinOpacity").toDouble());
    	
    refPaperColorLabel = new QLabel(pgcolor);
    refPaperColorLabel->setToolTip(pcvtip);
    refPaperColorButton = new QToolButton;
    refPaperColorButton->setIconSize(QSize(50, 20));
    action = new QAction(this);
    connect(action, SIGNAL(triggered()), this, SLOT(setRefPaperColor()));
    refPaperColorButton->setDefaultAction(action);
    changeColor(refPaperColorButton, refPaperColor);
    
    refReverseVideoBox = new QCheckBox(reverse);
    refReverseVideoBox->setToolTip(revtip);
    refReverseVideoBox->setChecked(false);
    if (xwApp->hasSetting("ReverseVideoR"))
    	refReverseVideoBox->setChecked(xwApp->getSetting("ReverseVideoR").toBool());
    	
    QGridLayout * reflayout = new QGridLayout;
//    reflayout->addWidget(refWinBgColorLabel, 0, 0);
//    reflayout->addWidget(refWinBgColorButton, 0, 1);
    reflayout->addWidget(refWinOpacityLabel, 0, 0);
    reflayout->addWidget(refWinOpacityBox, 0, 1);
    reflayout->addWidget(refPaperColorLabel, 1, 0);
    reflayout->addWidget(refPaperColorButton, 1, 1);
    reflayout->addWidget(refReverseVideoBox, 2, 0);
    
    refWidget = new QWidget;
    refWidget->setLayout(reflayout);
    tab->addTab(refWidget, tr("Reference document window"));
    
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tab);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    
    resize(340, 180);
}

void XWDisplaySettingDialog::accept()
{
//	xwApp->saveSetting("MainWinBackgroundColor", mainBgColor);
	xwApp->saveSetting("MainWinOpacity", mainWinOpacityBox->value());
	xwApp->saveSetting("PageColor", mainPaperColor);
	xwApp->saveSetting("ReverseVideo", mainReverseVideoBox->isChecked());
	
//	xwApp->saveSetting("RefWinBackgroundColor", refBgColor);
	xwApp->saveSetting("RefWinOpacity", refWinOpacityBox->value());
	xwApp->saveSetting("PageColorR", refPaperColor);
	xwApp->saveSetting("ReverseVideoR", refReverseVideoBox->isChecked());
	QDialog::accept();
}

void XWDisplaySettingDialog::changeColor(QToolButton * button, const QColor &c)
{
	QAction * action = button->defaultAction();
	QPixmap pix(50, 20);
	pix.fill(c);
  action->setIcon(pix);
}
/*
void XWDisplaySettingDialog::setMainBgColor()
{
	QColor col = QColorDialog::getColor(mainBgColor, this);
	if (!col.isValid())
        return;
        
    mainBgColor = col;
    changeColor(mainWinBgColorButton, mainBgColor);
}
*/
void XWDisplaySettingDialog::setMainPaperColor()
{
	QColor col = QColorDialog::getColor(mainPaperColor, this);
	if (!col.isValid())
        return;
        
    mainPaperColor = col;
    changeColor(mainPaperColorButton, mainPaperColor);
}
/*
void XWDisplaySettingDialog::setRefBgColor()
{
	QColor col = QColorDialog::getColor(refBgColor, this);
	if (!col.isValid())
        return;
        
    refBgColor = col;
    changeColor(refWinBgColorButton, refBgColor);
}
*/
void XWDisplaySettingDialog::setRefPaperColor()
{
	QColor col = QColorDialog::getColor(refPaperColor, this);
	if (!col.isValid())
        return;
        
    refPaperColor = col;
    changeColor(refPaperColorButton, refPaperColor);
}

