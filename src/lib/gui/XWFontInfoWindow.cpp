/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWString.h"
#include "XWFontSea.h"
#include "XWFTFont.h"
#include "XWCharacterWidget.h"
#include "XWFontInfoWindow.h"

XWFontInfoWindow::XWFontInfoWindow(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Font Infomation"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    QWidget *centralWidget = new QWidget;
    
    fontFileLabel = new QLabel(tr("Font file:"));
    fontFileCombo = new QComboBox;
    
    XWFontSea sea;
    QStringList listft = sea.getSystemFiles();
    listft.sort();
    QStringList fts = sea.allFreeType();
    fts.sort();
    listft += fts;
    maxFreeType = listft.size() - 1;
    if (!listft.isEmpty())
    	fontFileCombo->addItems(listft);
    QStringList listpk = sea.allPk();
    if (listpk.isEmpty())
    	maxPK = -1;
    else
    {
    	listpk.sort();
    	maxPK = listft.size() + listpk.size() - 1;
    	fontFileCombo->addItems(listpk);
    }
    	
    QStringList listvf = sea.allVf();
    if (listvf.isEmpty())
    	maxVF = -1;
    else
    {
    	listvf.sort();
    	maxVF = listft.size() + listpk.size() + listvf.size() - 1;
    	fontFileCombo->addItems(listvf);
    }
    
    QStringList listovf = sea.allOvf();
    if (listovf.isEmpty())
    	maxOVF = -1;
    else
    {
    	listovf.sort();
    	maxOVF = listft.size() + listpk.size() + listvf.size() + listovf.size() - 1;
    	fontFileCombo->addItems(listovf);
    }
    
    indexLabel = new QLabel(tr("Face index:"));
    indexLabel->setToolTip(tr("The index of the face within the font file. "
                              "The first face has index 0."));
    indexCombo = new QComboBox;
    indexCombo->addItem("0");
    mapsLabel = new QLabel(tr("Character map:"));
    mapsLabel->setToolTip(tr("A charmap is used to translate character codes "
                             "in a given encoding into glyph indexes for its "
                             "parent's face."));
    mapsCombo = new QComboBox;
    mapsCombo->addItem("0");
    
    fontSizeLabel = new QLabel(tr("Font size:"));
    fontSizeBox = new QDoubleSpinBox;
    fontSizeBox->setRange(5.0, 72);
    fontSizeBox->setSingleStep(0.5);
    fontSizeBox->setValue(10.0);
    
    efacterLabel = new QLabel(tr("Extend:"));
    efacterLabel->setToolTip(tr("Widen (extend) characters by a factor of real [1.0]."));
    efacterBox = new QDoubleSpinBox;
    efacterBox->setRange(0.01, 1.0);
    efacterBox->setValue(1.0);
    slantLabel = new QLabel(tr("Slant:"));
    slantLabel->setToolTip(tr("Oblique (slant) characters by real, usually < 1 [0.0]."));
    slantBox = new QDoubleSpinBox;
    slantBox->setRange(0.0, 1.0);
    slantBox->setValue(0.0);
    boldfLabel = new QLabel(tr("Embolden value:"));
    boldfLabel->setToolTip(tr("Embolden font outline or bitmap in pixel. For outline, "
                              "The new outline will be at most 4 times the value pixels "
                              "wider and higher. For bitmap, The new bitmap will be about "
                              "the value wider and higher."));
    boldfBox = new QDoubleSpinBox;
    boldfBox->setRange(0.0, 10.0);
    boldfBox->setValue(0.0);
    boldBox = new QCheckBox(tr("Embolden font"));
    boldBox->setToolTip(tr("Enable embolden font."));
    rotateBox = new QCheckBox(tr("Rotate"));
    rotateBox->setToolTip(tr("Rotate subfont glyphs by 90 degrees."));
    
    typeLabel = new QLabel;
    familyLabel = new QLabel;
    styleLabel = new QLabel;
    numGlyphsLabel = new QLabel;
    codingSchemeLabel = new QLabel;
    peLabel = new QLabel;
    scalableBox = new QCheckBox(tr("Scalable"));
//    scalableBox->setEnabled(false);
    fixedWidthBox = new QCheckBox(tr("Contains fixed-width characters"));
//    fixedWidthBox->setEnabled(false);
    sfntBox = new QCheckBox(tr("Uses the \"sfnt\" storage scheme"));
//    sfntBox->setEnabled(false);
    cidBox = new QCheckBox(tr("Is CID-keyed"));
//    cidBox->setEnabled(false);
    glyphNameBox = new QCheckBox(tr("Contains glyph names"));
//    glyphNameBox->setEnabled(false);
    psnameBox = new QCheckBox(tr("Provides reliable PostScript glyph names"));
//    psnameBox->setEnabled(false);
    horizontalBox = new QCheckBox(tr("Contains horizontal metrics"));
//    horizontalBox->setEnabled(false);
    verticalBox = new QCheckBox(tr("Contains vertical metrics"));
//    verticalBox->setEnabled(false);
    kerningBox = new QCheckBox(tr("Contains kerning data"));
//    kerningBox->setEnabled(false);
    
    QHBoxLayout * filelayout = new QHBoxLayout;
    filelayout->addWidget(fontFileLabel);
    filelayout->addWidget(fontFileCombo);
    
    QHBoxLayout * indexlayout = new QHBoxLayout;
    indexlayout->addWidget(indexLabel);
    indexlayout->addWidget(indexCombo);
    
    QHBoxLayout * maplayout = new QHBoxLayout;
    maplayout->addWidget(mapsLabel);
    maplayout->addWidget(mapsCombo);
    
    QHBoxLayout * sizelayout = new QHBoxLayout;
    sizelayout->addWidget(fontSizeLabel);
    sizelayout->addWidget(fontSizeBox);
    
    QHBoxLayout * efactorlayout = new QHBoxLayout;
    efactorlayout->addWidget(efacterLabel);
    efactorlayout->addWidget(efacterBox);
    
    QHBoxLayout * slantlayout = new QHBoxLayout;
    slantlayout->addWidget(slantLabel);
    slantlayout->addWidget(slantBox);
    
    QHBoxLayout * boldlayout = new QHBoxLayout;
    boldlayout->addWidget(boldBox);
    boldlayout->addWidget(boldfLabel);
    boldlayout->addWidget(boldfBox);
        
    QVBoxLayout * settinglayout = new QVBoxLayout;
    settinglayout->addLayout(filelayout);
    settinglayout->addLayout(indexlayout);
    settinglayout->addLayout(maplayout);
    settinglayout->addLayout(sizelayout);
    settinglayout->addLayout(efactorlayout);
    settinglayout->addLayout(slantlayout);
    settinglayout->addLayout(boldlayout);
    settinglayout->addWidget(rotateBox);
    
    QGroupBox *settingBox = new QGroupBox(tr("Font select"));
    settingBox->setLayout(settinglayout);
    
    QVBoxLayout * info1layout = new QVBoxLayout;
    info1layout->addWidget(typeLabel);
    info1layout->addWidget(familyLabel);
    info1layout->addWidget(styleLabel);
    info1layout->addWidget(numGlyphsLabel);
    info1layout->addWidget(codingSchemeLabel);
    info1layout->addWidget(peLabel);
    info1layout->addWidget(scalableBox); 
    info1layout->addWidget(fixedWidthBox);
    
    QVBoxLayout * info2layout = new QVBoxLayout;
    info2layout->addWidget(sfntBox);
    info2layout->addWidget(cidBox);
    info2layout->addWidget(glyphNameBox);
    info2layout->addWidget(psnameBox);
    info2layout->addWidget(horizontalBox);
    info2layout->addWidget(verticalBox);
    info2layout->addWidget(kerningBox);
    
    QHBoxLayout * infolayout = new QHBoxLayout;
    infolayout->addLayout(info1layout);
    infolayout->addLayout(info2layout);
    
    QGroupBox *infoBox = new QGroupBox(tr("Font infomation"));
    infoBox->setLayout(infolayout);
    
    QVBoxLayout * leftlayout = new QVBoxLayout;
    leftlayout->addWidget(settingBox);
    leftlayout->addSpacing(4);
    leftlayout->addWidget(infoBox);
    
    scrollArea = new QScrollArea;
    characterWidget = new XWCharacterWidget;
    scrollArea->setWidget(characterWidget);
    
//    QScrollBar * hbar = scrollArea->horizontalScrollBar();
//    QScrollBar * vbar = scrollArea->verticalScrollBar();
//    connect(hbar, SIGNAL(valueChanged(int)), characterWidget, SLOT(update()));
//    connect(vbar, SIGNAL(valueChanged(int)), characterWidget, SLOT(update()));
    
    QHBoxLayout * centralLayout = new QHBoxLayout;
    centralLayout->addLayout(leftlayout);
    centralLayout->addWidget(scrollArea, 1);
    centralWidget->setLayout(centralLayout);
    
    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
		
		QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(centralWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    
    connect(fontFileCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFont()));
    connect(indexCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFont()));
    connect(mapsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFont()));
    connect(fontSizeBox, SIGNAL(valueChanged(double)), this, SLOT(displayFont()));
    connect(efacterBox, SIGNAL(valueChanged(double)), this, SLOT(displayFont()));
    connect(slantBox, SIGNAL(valueChanged(double)), this, SLOT(displayFont()));
    connect(boldfBox, SIGNAL(valueChanged(double)), this, SLOT(displayFont()));
    connect(boldBox, SIGNAL(toggled(bool)), this, SLOT(displayFont()));
    connect(rotateBox, SIGNAL(toggled(bool)), this, SLOT(displayFont()));
    
    resize(960, 500);
    
    displayFont();
}

void XWFontInfoWindow::displayFont()
{
	indexCombo->disconnect(this);
	mapsCombo->disconnect(this);
	fontSizeBox->disconnect(this);
	
	XWCharacterWidget::DevFontType type = XWCharacterWidget::FontFT;
	int idx = fontFileCombo->currentIndex();
	if (idx > maxFreeType)
	{
		if (idx < maxPK)
			type = XWCharacterWidget::FontPK;
		else
			type = XWCharacterWidget::FontVF;
	}
	
	QString name = fontFileCombo->currentText();
	QString istr = indexCombo->currentText();
	int index = istr.toInt();
	double size = fontSizeBox->value();
	QString mapstr = mapsCombo->currentText();
	int map = mapstr.toInt();
	double efactor = efacterBox->value();
	double slant = slantBox->value();
	double boldf = boldfBox->value();
	bool bold = boldBox->isChecked();
	bool rotate = rotateBox->isChecked();
	QString encname;
	QString unknown = tr("Unknown");
	QString typetext = tr("Font type: ");
	QString familytext = tr("Font family: ");
	QString styletext = tr("Font style: ");
	QString numglyphtext = tr("Number of glyphs: ");
	QString codingtext = tr("Coding scheme: ");
	QString petext = tr("Platform ID and encoding number: ");
	bool scalable = false;
	bool fixedwidth = false;
	bool sfnt = false;
	bool cid = false;
	bool glyphname = false;
	bool psname = false;
	bool hori = false;
	bool vert = false;
	bool kerning = false;
	switch (type)
	{
		case XWCharacterWidget::FontFT:
			characterWidget->displayFTFont(name, index, map, size, efactor, slant, boldf, bold, rotate, encname, -1);
			{
				XWFTFont * font = characterWidget->getFTFont();
				if (!font)
				{
					familytext += unknown;
					styletext += unknown;
					codingtext += unknown;
					petext += unknown;
				}
				else
				{
					char * family = font->getFamilyName();
					if (family)
					{
						XWString tmpstr(family, strlen(family));
						QString strtmp = tmpstr.toQString();
						familytext += strtmp;
					}
					else
						familytext += unknown;
						
					char * style = font->getStyleName();
					if (style)
					{
						XWString tmpstr(style, strlen(style));
						QString strtmp = tmpstr.toQString();
						styletext += strtmp;
					}
					else
						styletext += unknown;
						
					{
						const char * coding = font->getCodingScheme();
						XWString tmpstr((char*)coding, strlen(coding));
						QString strtmp = tmpstr.toQString();
						codingtext += strtmp;
					}
					
					int pid = font->getPlatformID();
					int eid = font->getEncodingID();
					petext += QString("%1  %2").arg(pid).arg(eid);
					scalable = font->isScalable();
					fixedwidth = font->isFixedWidth();
					sfnt = font->isSFNT();
					cid = font->isCIDKeyed();
					glyphname = font->hasGlyphNames();
					psname = font->hasPSGlyphNames();
					hori = font->hasHorizontal();
					vert = font->hasVertical();
					kerning = font->hasKerning();
					int numface = font->getNumFaces();
					if (numface != indexCombo->count())
					{
						
						indexCombo->clear();
						for (int i = 0; i < numface; i++)
						{
							QString t = QString("%1").arg(i);
							indexCombo->addItem(t);
						}
						
						indexCombo->setCurrentIndex(0);
						
					}
					
					int nummaps = font->getNumCmaps();
					if (nummaps != mapsCombo->count())
					{
						mapsCombo->clear();
						for (int i = 0; i < nummaps; i++)
						{
							QString t = QString("%1").arg(i);
							mapsCombo->addItem(t);
						}
						
						mapsCombo->setCurrentIndex(0);
						
					}
				}
			}			
			break;
			
		default:
			fontSizeBox->setValue(characterWidget->getSize());
			if (type == XWCharacterWidget::FontPK)
				characterWidget->displayPKFont(name);
			else
				characterWidget->displayVFFont(name, name);
				
			if (indexCombo->count() != 1)
			{
				indexCombo->disconnect(this);
				indexCombo->clear();
				indexCombo->addItem("0");
				indexCombo->setCurrentIndex(0);
			}
			
			if (mapsCombo->count() != 1)
			{
				mapsCombo->clear();
				mapsCombo->addItem("0");
				mapsCombo->setCurrentIndex(0);
			}
			
			familytext += unknown;
			styletext += unknown;
			codingtext += unknown;
			petext += unknown;
			break;
	}
	
	QString text = characterWidget->getType();
	typetext += text;
	typeLabel->setText(typetext);
	familyLabel->setText(familytext);
	styleLabel->setText(styletext);
	int numg = characterWidget->getNumGlyphs();
	numglyphtext += QString("%1").arg(numg);
	numGlyphsLabel->setText(numglyphtext);
	codingSchemeLabel->setText(codingtext);
	peLabel->setText(petext);
	scalableBox->setChecked(scalable);
	fixedWidthBox->setChecked(fixedwidth);
	sfntBox->setChecked(sfnt);
	
	cidBox->setChecked(cid);
	glyphNameBox->setChecked(glyphname);
	psnameBox->setChecked(psname);
	horizontalBox->setChecked(hori);
	verticalBox->setChecked(vert);
	kerningBox->setChecked(kerning);
	
	connect(indexCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFont()));
	connect(mapsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFont()));
	connect(fontSizeBox, SIGNAL(valueChanged(double)), this, SLOT(displayFont()));
}

