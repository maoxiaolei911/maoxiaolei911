/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWString.h"
#include "XWFontSea.h"
#include "XWTexSea.h"
#include "XWSubfont.h"
#include "XWFTFont.h"
#include "XWCharacterWidget.h"
#include "XWTexFontWindow.h"

XWTexFontWindow::XWTexFontWindow(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("TeX Font Setting"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    QWidget *centralWidget = new QWidget;
    
    texnameLabel = new QLabel(tr("TeX font name:"));
    texnameEdit = new QLineEdit;
    
    descriptionLabel = new QLabel(tr("Description:"));
    descriptionEdit = new QLineEdit;
    
    subfontLabel = new QLabel(tr("Subfont:"));
    subfontLabel->setToolTip(tr("Split multiple-byte Chinese/Japanese/Korean encoding\n"
                                "to one-byte encoding(subfont), so that 8-bit LaTeX\n"
                                "system can handle Chinese/Japanese/Korean."));
    subfontCombo = new QComboBox;
    subfontCombo->addItem(tr("No subfont"));
    subfontCombo->addItem("Big5");
    subfontCombo->addItem("EUC");
    subfontCombo->addItem("HKSCS");
    subfontCombo->addItem("KS-HLaTeX");
    subfontCombo->addItem("SJIS");
    subfontCombo->addItem("UBg5plus");
    subfontCombo->addItem("UBig5");
    subfontCombo->addItem("UGB");
    subfontCombo->addItem("UGBK");
    subfontCombo->addItem("UJIS");
    subfontCombo->addItem("UKS");
    subfontCombo->addItem("UKS-HLaTeX");
    subfontCombo->addItem("Unicode");
    subfontCombo->setCurrentIndex(0);
    
    subfontIDLabel = new QLabel(tr("Subfont IDs:"));
    subfontIDLabel->setToolTip(tr("Subfont ID, usually as infix of subfont file name\n"
                                  "(for 'gbksong05', 'gbksong' is prefix, '05' is infix)."));
    subfontIDCombo = new QComboBox;
    
    inencLabel = new QLabel(tr("Input encoding:"));
    inencLabel->setToolTip(tr("An .enc file for maping font to raw TeX font."));
    inencEdit = new QLineEdit;
    outencLabel = new QLabel(tr("Output encoding:"));
    outencLabel->setToolTip(tr("An .enc file for maping raw TeX font to another (virtual)\n"
                               "TeX font providing all kerning and ligature infomation."));
    outencEdit = new QLineEdit;
    
    tounicodeLabel = new QLabel(tr("Unicode cmap:"));
    tounicodeLabel->setToolTip(tr("A Cmap file translating character codes into unicode."));
    tounicodeEdit = new QLineEdit;
    collectionLabel = new QLabel(tr("CIDSystemInfo:"));
    collectionLabel->setToolTip(tr("CIDFont's charater collection(Adobe-Japan1-4, or UCS etc)."));
    collectionEdit = new QLineEdit;
    
    vfLabel = new QLabel(tr("Virtual font:"));
    vfLabel->setToolTip(tr("Virtual font file name for this TeX Font."));
    vfEdit = new QLineEdit;
    
    fontFileLabel = new QLabel(tr("Font File:"));
    fontFileLabel->setToolTip(tr("Physical font file name for this TeX Font."));
    fontFileCombo = new QComboBox;
    fontFileCombo->setEditable(true);
    fontFileCombo->setInsertPolicy(QComboBox::InsertAtTop);
    
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
    
    indexLabel = new QLabel(tr("Index:"));
    indexLabel->setToolTip(tr("The index of the face within the font file.\n"
                              "The first face has index 0."));
    indexCombo = new QComboBox;
    indexCombo->addItem("0");
    mapsLabel = new QLabel(tr("Character map:"));
    mapsLabel->setToolTip(tr("A charmap is used to translate character codes\n"
                             "in a given encoding into glyph indexes for its\n"
                             "parent's face."));
    mapsCombo = new QComboBox;
    mapsCombo->addItem("0");
    
    wmodeLabel = new QLabel(tr("Writing mode:"));
    wmodeLabel->setToolTip(tr("Writing mode (for unicode encoding), '0' indicate\n"
                              "horizontal and '1' indicate vertical."));
    wmodeCombo = new QComboBox;
    wmodeCombo->addItem("0");
    wmodeCombo->addItem("1");
    
    fontSizeLabel = new QLabel(tr("Size:"));
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
    boldfLabel->setToolTip(tr("Embolden font outline or bitmap in pixel. For outline,\n"
                              "The new outline will be at most 4 times the value pixels\n"
                              "wider and higher. For bitmap, The new bitmap will be about\n"
                              "the value wider and higher."));
    boldfBox = new QDoubleSpinBox;
    boldfBox->setRange(0.0, 10.0);
    boldfBox->setValue(0.0);
    
    capheightLabel = new QLabel(tr("Small caps height:"));
    capheightBox = new QDoubleSpinBox;
    capheightBox->setRange(0.0, 1.0);
    capheightBox->setValue(0.8);
    italicangleLabel = new QLabel(tr("Italic angle:"));
    italicangleBox = new QDoubleSpinBox;
    capheightBox->setRange(0.0, 1000.0);
    capheightBox->setValue(0.0);
    stemvLabel = new QLabel(tr("StemV:"));
    stemvBox = new QSpinBox;
    stemvBox->setRange(-1, 1000);
    stemvBox->setValue(-1);
    yoffsetLabel = new QLabel(tr("Y offset for rotated:"));
    yoffsetLabel->setToolTip(tr("Move rotated glyphs down by a factor of REAL [0.25]"));
    yoffsetBox = new QDoubleSpinBox;
    yoffsetBox->setRange(0.0, 3.0);
    yoffsetBox->setValue(0.25);
    
    smallcapsBox = new QCheckBox(tr("Use small caps"));
    boldBox = new QCheckBox(tr("Embolden font"));
    rotateBox = new QCheckBox(tr("Rotate"));
    italicBox = new QCheckBox(tr("Italic"));
    noembedBox = new QCheckBox(tr("Block embedding glyph data when convert .dvi to .pdf."));
    useVFBox = new QCheckBox(tr("Use virtual font"));
    
//    displayButton = new QPushButton(tr("Display"));
    addButton = new QPushButton(tr("Add"));
    
    QHBoxLayout * texnamelayout = new QHBoxLayout;
    texnamelayout->addWidget(texnameLabel);
    texnamelayout->addWidget(texnameEdit);
    
    QHBoxLayout * deslayout = new QHBoxLayout;
    deslayout->addWidget(descriptionLabel);
    deslayout->addWidget(descriptionEdit);
    
    QHBoxLayout * subfntlayout = new QHBoxLayout;
    subfntlayout->addWidget(subfontLabel);
    subfntlayout->addWidget(subfontCombo);
    subfntlayout->addWidget(subfontIDLabel);
    subfntlayout->addWidget(subfontIDCombo);
    
    QHBoxLayout * enclayout = new QHBoxLayout;
    enclayout->addWidget(inencLabel);
    enclayout->addWidget(inencEdit);
    enclayout->addWidget(outencLabel);
    enclayout->addWidget(outencEdit);
    
    QHBoxLayout * unilayout = new QHBoxLayout;
    unilayout->addWidget(tounicodeLabel);
    unilayout->addWidget(tounicodeEdit);
    unilayout->addWidget(collectionLabel);
    unilayout->addWidget(collectionEdit);
    
    QHBoxLayout * vflayout = new QHBoxLayout;
    vflayout->addWidget(useVFBox);
    vflayout->addWidget(vfLabel);
    vflayout->addWidget(vfEdit);
    
    QHBoxLayout * filelayout = new QHBoxLayout;
    filelayout->addWidget(fontFileLabel);
    filelayout->addWidget(fontFileCombo);
    
    QHBoxLayout * indexlayout = new QHBoxLayout;
    indexlayout->addWidget(indexLabel);
    indexlayout->addWidget(indexCombo);
    indexlayout->addWidget(mapsLabel);
    indexlayout->addWidget(mapsCombo);
    indexlayout->addWidget(wmodeLabel);
    indexlayout->addWidget(wmodeCombo);
        
    QHBoxLayout * sizelayout = new QHBoxLayout;
    sizelayout->addWidget(fontSizeLabel);
    sizelayout->addWidget(fontSizeBox);
    sizelayout->addWidget(efacterLabel);
    sizelayout->addWidget(efacterBox);
    sizelayout->addWidget(slantLabel);
    sizelayout->addWidget(slantBox);
        
    QHBoxLayout * boldlayout = new QHBoxLayout;
    boldlayout->addWidget(boldBox);
    boldlayout->addWidget(boldfLabel);
    boldlayout->addWidget(boldfBox);
    
    QHBoxLayout * caplayout = new QHBoxLayout;
    caplayout->addWidget(capheightLabel);
    caplayout->addWidget(capheightBox);
    caplayout->addWidget(italicangleLabel);
    caplayout->addWidget(italicangleBox);
    caplayout->addWidget(stemvLabel);
    caplayout->addWidget(stemvBox);
    
    QHBoxLayout * yolayout = new QHBoxLayout;
    yolayout->addWidget(yoffsetLabel);
    yolayout->addWidget(yoffsetBox);
    
    QHBoxLayout * smlayout = new QHBoxLayout;
    smlayout->addWidget(smallcapsBox);
    smlayout->addWidget(rotateBox);
    smlayout->addWidget(italicBox);
    
    QHBoxLayout * buttonLayout = new QHBoxLayout;
//    buttonLayout->addWidget(displayButton);
		buttonLayout->addStretch(1);
    buttonLayout->addWidget(addButton);
    
    QVBoxLayout * settinglayout = new QVBoxLayout;
    settinglayout->addLayout(texnamelayout);
    settinglayout->addLayout(deslayout);
    settinglayout->addLayout(subfntlayout);
    settinglayout->addLayout(enclayout);
    settinglayout->addLayout(unilayout);
    settinglayout->addLayout(vflayout);
    settinglayout->addLayout(filelayout);
    settinglayout->addLayout(indexlayout);
    settinglayout->addLayout(sizelayout);
    settinglayout->addLayout(boldlayout);
    settinglayout->addLayout(caplayout);
    settinglayout->addLayout(yolayout);
    settinglayout->addLayout(smlayout);
    settinglayout->addWidget(noembedBox);
    
    QGroupBox *settingBox = new QGroupBox(tr("TeX font setting"));
    settingBox->setLayout(settinglayout);
    
    texFonts = new QListWidget;
    texFonts->setFixedWidth(150);
    QVBoxLayout * listlayout = new QVBoxLayout;
    listlayout->addWidget(texFonts);
    
    QGroupBox *listBox = new QGroupBox(tr("TeX fonts"));
    listBox->setLayout(listlayout);
    
    XWTeXFontSetting texsetting;
    QStringList alltexfonts = texsetting.allFonts();
    alltexfonts.sort();
    texFonts->addItems(alltexfonts);
    
    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->addWidget(settingBox);
    leftLayout->addLayout(buttonLayout);
    
    scrollArea = new QScrollArea;
    characterWidget = new XWCharacterWidget;
    scrollArea->setWidget(characterWidget);
    
    QHBoxLayout * centralLayout = new QHBoxLayout;
    centralLayout->addWidget(listBox);
    centralLayout->addLayout(leftLayout);
    centralLayout->addSpacing(4);
    centralLayout->addWidget(scrollArea);
    
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
    connect(subfontIDCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFont()));
    
//    connect(displayButton, SIGNAL(clicked()), this, SLOT(displayFont()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
    connect(texFonts, SIGNAL(itemDoubleClicked(QListWidgetItem * )), this, SLOT(modifyList(QListWidgetItem * )));
    
    displayed = false;
    
    resize(1110, 500);
}

void XWTexFontWindow::add()
{
	QString texname = texnameEdit->text();
	if (!texname.isEmpty())
	{
		QString discription = descriptionEdit->text();
		QString subfont;
		if (subfontCombo->count() > 0 && subfontCombo->currentIndex() != 0)
			subfont = subfontCombo->currentText();
			
		QString inenc = inencEdit->text();
		QString outenc = outencEdit->text();
		QString tounicode = tounicodeEdit->text();
		QString collection = collectionEdit->text();
		QString vffont = vfEdit->text();
		QString font = fontFileCombo->currentText();
		QString istr = indexCombo->currentText();
	    int index = istr.toInt();
	    int pid = -1;
	    int eid = -1;
	    XWFTFont * ftfont = characterWidget->getFTFont();
	    if (ftfont)
	    {
	    	pid = ftfont->getPlatformID();
	    	eid = ftfont->getEncodingID();
	    }
	    
	    QString wmstr = wmodeCombo->currentText();
	    int wmode = wmstr.toInt();
	    double size = fontSizeBox->value();
	    double efacter = efacterBox->value();
	    double slant = slantBox->value();
	    double bold = boldfBox->value();
	    double capheight = capheightBox->value();
	    double italicangle = italicangleBox->value();
	    int stemv = stemvBox->value();
	    double y_offset = yoffsetBox->value();
	    
	    bool smallcaps = smallcapsBox->isChecked();
	    bool boldb = boldBox->isChecked();
	    bool rotate = rotateBox->isChecked();
	    bool italic = italicBox->isChecked();
	    bool noembed = noembedBox->isChecked();
	    bool usevf = useVFBox->isChecked();
	    
	    XWTeXFontSetting fontsetting;
	    fontsetting.setTexName(texname);
	    fontsetting.setBold(boldb);
	    fontsetting.setBoldF(bold);
	    fontsetting.setCapHeight(capheight);
	    fontsetting.setCMap(tounicode);
	    fontsetting.setCollection(collection);
	    fontsetting.setDescription(discription);
	    fontsetting.setEFactor(efacter);
	    fontsetting.setEncodingID(eid);
	    fontsetting.setFont(font);
	    fontsetting.setIndex(index);
	    fontsetting.setItalic(italic);
	    fontsetting.setInEncoding(inenc);
	    fontsetting.setItalicAngle(italicangle);
	    fontsetting.setNoEmbed(noembed);
	    fontsetting.setOutEncoding(outenc);
	    fontsetting.setPlatID(pid);
	    fontsetting.setRotate(rotate);
	    fontsetting.setSize(size);
	    fontsetting.setSlant(slant);
	    fontsetting.setSmallCaps(smallcaps);
	    fontsetting.setStemV(stemv);
	    fontsetting.setSubFont(subfont);
	    fontsetting.setVF(usevf, vffont);
	    fontsetting.setWMode(wmode);
	    fontsetting.setYOfsset(y_offset);
	    
	    QList<QListWidgetItem *> list = texFonts->findItems(texname, Qt::MatchExactly);
	    if (list.isEmpty())
	    	texFonts->addItem(texname);
	}
}

void XWTexFontWindow::displayFont()
{
	XWCharacterWidget::DevFontType type = XWCharacterWidget::FontFT;
	int idx = fontFileCombo->currentIndex();
	if (idx > maxFreeType)
	{
		if (idx < maxPK)
			type = XWCharacterWidget::FontPK;
		else
			type = XWCharacterWidget::FontVF;
	}
	
	int subfontid = -1;
	if (subfontCombo->count() > 0 && subfontCombo->currentIndex() != 0)
	{
		QString subfontname = subfontCombo->currentText();
		QString sunfontrec = subfontIDCombo->currentText();
		XWSubfont subfont(subfontname);
    	subfontid = subfont.loadRecord(sunfontrec);
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
	QString encname = outencEdit->text();
	
	switch (type)
	{
		case XWCharacterWidget::FontFT:
			characterWidget->displayFTFont(name, index, map, size, efactor, slant, boldf, bold, rotate, encname, subfontid);
			break;
			
		default:
			fontSizeBox->setValue(characterWidget->getSize());
			if (type == XWCharacterWidget::FontPK)
				characterWidget->displayPKFont(name);
			else
				characterWidget->displayVFFont(name, name);
			break;
	}
	
	updateIndexAndMaps();
	
	displayed = true;
}

void XWTexFontWindow::modifyList(QListWidgetItem * item)
{
	if (!item)
		return ;
			
	int subfontid = -1;		
	QString texname = item->text();
	texnameEdit->setText(texname);
	XWTeXFontSetting fontsetting;
	fontsetting.setTexName(texname);
	
	QString discription = fontsetting.getDescription();
	descriptionEdit->setText(discription);
	QString subfont = fontsetting.getSubFont();
	subfontIDCombo->disconnect(this);
	if (!subfont.isEmpty())
	{
		int sftid = subfontCombo->findText(subfont, Qt::MatchCaseSensitive);
		if (sftid >= 0)
			subfontCombo->setCurrentIndex(sftid);
			
		XWSubfont subf(subfont);
		int num_ids = 0;
		char ** ids = subf.getSubfontIds(&num_ids);
		subfontIDCombo->clear();
		for (int i = 0; i < num_ids; i++)
		{
			QString id(ids[i]);
			if (i == 0)
				subfontid = subf.loadRecord(id);
			subfontIDCombo->addItem(id);
		}
	}
	
	connect(subfontIDCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFont()));
			
	QString inenc = fontsetting.getInEncoding();
	inencEdit->setText(inenc);
	QString outenc = fontsetting.getOutEncoding();
	outencEdit->setText(outenc);
	QString tounicode = fontsetting.getCMap();
	tounicodeEdit->setText(tounicode);
	QString collection = fontsetting.getCollection();
	collectionEdit->setText(collection);
	QString vffont = fontsetting.getVF();
	vfEdit->setText(vffont);
	QString font = fontsetting.getFont();
	if (font.indexOf(".") < 0)
		font += ".";
		
	int fontidx = fontFileCombo->findText(font, Qt::MatchContains);
	if (fontidx >= 0)
		fontFileCombo->setCurrentIndex(fontidx);
		
	font = fontFileCombo->currentText();
		
	fontSizeBox->setValue(fontsetting.getSize());
	
	int wm = fontsetting.getWMode();
	if (wm != 0 || wm != 1)
		wm = 0;
	wmodeCombo->setCurrentIndex (wm);
	efacterBox->setValue(fontsetting.getEFactor());
	slantBox->setValue(fontsetting.getSlant());
	boldfBox->setValue(fontsetting.getBold());
	capheightBox->setValue(fontsetting.getCapHeight());
	italicangleBox->setValue(fontsetting.getItalicAngle());
	stemvBox->setValue(fontsetting.getStemV());
	yoffsetBox->setValue(fontsetting.getYOffset());
	smallcapsBox->setChecked(fontsetting.useSmallCaps());
	boldBox->setChecked(fontsetting.isBold());
	rotateBox->setChecked(fontsetting.isRotate());
	italicBox->setChecked(fontsetting.isItalic());
	noembedBox->setChecked(fontsetting.isNoEmbed());
	useVFBox->setChecked(fontsetting.useVF());
	
	int index = fontsetting.getIndex();
	int pid = fontsetting.getPlatID();
	int eid = fontsetting.getEncodingID();
	
	XWCharacterWidget::DevFontType type = XWCharacterWidget::FontFT;
	if (fontidx > maxFreeType)
	{
		if (fontidx < maxPK)
			type = XWCharacterWidget::FontPK;
		else
			type = XWCharacterWidget::FontVF;
	}
	switch (type)
	{
		case XWCharacterWidget::FontFT:
			characterWidget->displayFTFont(font, index, pid, eid, 
			                               fontsetting.getSize(), 
			                               fontsetting.getEFactor(), 
			                               fontsetting.getSlant(), 
			                               fontsetting.getBold(), 
			                               fontsetting.isBold(), 
			                               fontsetting.isRotate(), 
			                               outenc,
			                               subfontid);
			break;
			
		default:
			fontSizeBox->setValue(characterWidget->getSize());
			if (type == XWCharacterWidget::FontPK)
				characterWidget->displayPKFont(font);
			else
				characterWidget->displayVFFont(font, font);
			break;
	}
	
	updateIndexAndMaps();
	indexCombo->setCurrentIndex(index);
	XWFTFont * ftfont = characterWidget->getFTFont();
	if (ftfont)
	{
		index = ftfont->getCharMapIndex(pid, eid);
		mapsCombo->setCurrentIndex(index);
	}
}

void XWTexFontWindow::updateIndexAndMaps()
{
	indexCombo->disconnect(this);
	mapsCombo->disconnect(this);
	fontSizeBox->disconnect(this);
	
	XWFTFont * font = characterWidget->getFTFont();
	if (font)
	{
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
	else
	{
		indexCombo->clear();
		indexCombo->addItem("0");
		mapsCombo->clear();
		mapsCombo->addItem("0");
	}
	
	connect(indexCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFont()));
	connect(mapsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFont()));
	connect(fontSizeBox, SIGNAL(valueChanged(double)), this, SLOT(displayFont()));
}

