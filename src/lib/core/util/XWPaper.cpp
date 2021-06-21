/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPaper.h"

struct PaperSpecification
{
	char * spec;
	QPrinter::PageSize qtSize;
	qreal widthPt;
    qreal heightPt;
};

static PaperSpecification paperSpec[] = 
{
	{"A0", QPrinter::A0, 2383.94, 3370.40},
	{"A1", QPrinter::A1, 1683.78, 2383.94},
	{"A2", QPrinter::A2, 1190.55, 1683.78},
	{"A3", QPrinter::A3, 841.89,  1190.55},
	{"A4", QPrinter::A4, 595.28,  841.89},
	{"A5", QPrinter::A5, 419.53,  595.28},
	{"A6", QPrinter::A6, 297.64,  419.53},
	{"A7", QPrinter::A7, 209.76,  297.64},
	{"A8", QPrinter::A8, 147.40,  209.76},
	{"A9", QPrinter::A9, 104.88,  147.40},
	{"B0", QPrinter::B0, 2834.65, 4008.19},
	{"B1", QPrinter::B1, 2004.09, 2834.65},
	{"B2", QPrinter::B2, 1417.32, 2004.09},
	{"B3", QPrinter::B3, 1000.63, 1417.32},
	{"B4", QPrinter::B4, 708.66,  1000.63},
	{"B5", QPrinter::B5, 498.90,  708.66},
	{"B6", QPrinter::B6, 354.33,  498.90},
	{"B7", QPrinter::B7, 249.45,  354.33},
	{"B8", QPrinter::B8, 175.75,  249.45},
	{"B9", QPrinter::B9, 93.54,  175.75},
	{"B10", QPrinter::B10, 87.87,   124.72},
	{"C5E", QPrinter::C5E, 462.05,  649.13},
	{"Comm10E", QPrinter::Comm10E, 297.64,  683.15},
	{"DLE", QPrinter::DLE, 311.81,  623.62},
	{"Executive", QPrinter::Executive, 540.00,  720.00},
	{"Folio", QPrinter::Folio, 595.28,  935.43},
	{"Ledger", QPrinter::Ledger, 1224.00, 792.00},
	{"Legal", QPrinter::Legal, 612.00,  1008.00},
	{"Letter", QPrinter::Letter, 612.00,  792.00},
	{"Tabloid", QPrinter::Tabloid, 792.00, 1224.00},
	{0, QPrinter::Custom, 0, 0}
};

XWPaper::XWPaper(QObject * parent)
    :QObject(parent)
{
	width = 597.5078740;
	height = 845.0468504;
	xOffset = 0.0;
    yOffset = 0.0;
	format = QPrinter::A4;
	orientation = QPrinter::Portrait;
}

XWPaper::XWPaper(const QString & specA, QObject * parent)
    :QObject(parent)
{
	width = 0.0;
	height = 0.0;
	xOffset = 0.0;
    yOffset = 0.0;
    format = QPrinter::Custom;
    orientation = QPrinter::Portrait;
    spec = specA;
    findSize();
}

XWPaper::XWPaper(qreal w, qreal h, QObject * parent)
    :QObject(parent)     
{
    width = w;
    height = h;
    xOffset = 0.0;
    yOffset = 0.0;
    format = QPrinter::Custom;
    orientation = QPrinter::Portrait;
    findSpec();
}

QStringList XWPaper::getPaperSpecs()
{
	int j = 0;
	PaperSpecification * pspec = &paperSpec[j];
	QStringList ret;
    while (pspec->spec)
    {
    	ret << QString::fromAscii(pspec->spec);
    	j++;
    	pspec = &paperSpec[j];
    }
    
    return ret;
}

void XWPaper::setPaperSize(const char * specA)
{
    spec = specA;
    findSize();
}

void XWPaper::setPaperSize(const QString & specA)
{
	spec = specA;
    findSize();
}

void XWPaper::setPaperSize(qreal w, qreal h)
{
	width = w;
    height = h;
    findSpec();
}

void XWPaper::findOrientation()
{
	int j = 0;
    PaperSpecification * pspec = &paperSpec[j];
    while (pspec->spec)
    {
    	if ((qRound(width) == qRound(pspec->widthPt)) && 
            (qRound(height) == qRound(pspec->heightPt)))
        {
            orientation = QPrinter::Portrait;
            break;
        }
        else if ((qRound(width) == qRound(pspec->heightPt)) && 
                 (qRound(height) == qRound(pspec->widthPt)))
        {
            orientation = QPrinter::Landscape;
            break;
        }
    	
    	j++;
    	pspec = &paperSpec[j];
    }
}

void XWPaper::findSize()
{
	int i = 0;
    while ((i < spec.length()) && (spec[i].isSpace()))
        i++;
        
    if (spec.startsWith("jis", Qt::CaseInsensitive) || spec.startsWith("iso", Qt::CaseInsensitive))
        i += 3;
        
    char c = spec[i].toUpper().toAscii();
    QString sp = "A4";
    switch (c)
    {
        case 'A':
            i++;
            sp = "A" + spec.mid(i, 1);
            break;
            
        case 'B':
            {
                i++;
                int k = i;
                while ((i < spec.length()) && (spec[i].isDigit()))
                    i++;
                    
                sp = "B" + spec.mid(k, i - k);
            }
            break;
            
        case 'C':
            if (spec[i + 1] == QChar('5'))
                sp = "C5E";
            else if (spec[i + 1] == QChar('o'))
                sp = "Comm10E";
            break;
            
        case 'D':
            sp = "DLE";
            break;
            
        case 'E':
            sp = "Executive";
            break;
           
        case 'F':
            sp = "Folio";
            break;
            
        case 'L':
            if ((i + 3) < spec.length())
            {
                c = spec[i + 3].toUpper().toAscii();
                if (c == 'D')
                    sp = "Ledger";
                else if (c == 'G')
                    sp = "Legal";
                else 
                    sp = "Letter";
            }
            else
                sp = "Legal";
            break;
            
        case 'T':
            sp = "Tabloid";
            break;
            
        default:
            break;
    }
    
    int j = 0;
    PaperSpecification * pspec = &paperSpec[j];
    while (pspec->spec)
    {
    	if (sp == pspec->spec)
    	{
    		format = pspec->qtSize;
    		width = pspec->widthPt;
    		height = pspec->heightPt;
    		break;
    	}
    	
    	j++;
    	pspec = &paperSpec[j];
    }
}

void XWPaper::findSpec()
{
	int j = 0;
    PaperSpecification * pspec = &paperSpec[j];
    while (pspec->spec)
    {
    	if ((qRound(width) == qRound(pspec->widthPt)) && 
            (qRound(height) == qRound(pspec->heightPt)))
        {
        	spec = QString::fromAscii(pspec->spec);
        	format = pspec->qtSize;
            orientation = QPrinter::Portrait;
            break;
        }
        else if ((qRound(width) == qRound(pspec->heightPt)) && 
                 (qRound(height) == qRound(pspec->widthPt)))
        {
        	spec = QString::fromAscii(pspec->spec);
        	format = pspec->qtSize;
            orientation = QPrinter::Landscape;
            break;
        }
    	
    	j++;
    	pspec = &paperSpec[j];
    }
}

