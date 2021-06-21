/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QTextCodec>
#include "XWGraphixState.h"
#include "XWObject.h"
#include "XWStream.h"
#include "XWImageOutputDev.h"

XWImageOutputDev::XWImageOutputDev(const char *fileRootA, 
	                               bool dumpJPEGA)
{
	fileRoot = qstrdup(fileRootA);
  	fileName = new char[strlen(fileRoot) + 20];
  	dumpJPEG = dumpJPEGA;
  	imgNum = 0;
  	ok = true;
}

XWImageOutputDev::~XWImageOutputDev()
{
	if (fileName)
		delete [] fileName;
		
	if (fileRoot)
  		delete [] fileRoot;
}

void XWImageOutputDev::drawImage(XWGraphixState *, 
                                 XWObject *, 
                                 XWStream *str,
			 			         int width, 
			 			         int height, 
			 			         XWImageColorMap *colorMap,
			                     int *, 
			                     bool inlineImg)
{
	QTextCodec * codec = QTextCodec::codecForLocale();
		
	if (dumpJPEG && str->getKind() == STREAM_DCT && 
       (colorMap->getNumPixelComps() == 1 ||
        colorMap->getNumPixelComps() == 3) &&
       !inlineImg) 
    {
    	sprintf(fileName, "%s-%03d.jpg", fileRoot, imgNum);
    	++imgNum;
    	QString fn = codec->toUnicode(fileName);
    	QFile f(fn);
    	if (!f.open(QIODevice::WriteOnly))
    		return ;

    	str = ((XWDCTStream *)str)->getRawStream();
    	str->reset();
		int c = EOF;
    	while ((c = str->getChar()) != EOF)
      		f.putChar((char)c);

    	str->close();
    	f.close();
  	} 
  	else if (colorMap->getNumPixelComps() == 1 && colorMap->getBits() == 1) 
  	{
    	sprintf(fileName, "%s-%03d.pbm", fileRoot, imgNum);
    	++imgNum;
    	QString fn = codec->toUnicode(fileName);
    	QFile f(fn);
    	if (!f.open(QIODevice::WriteOnly))
    		return ;
    	
    	f.write("P4\n");
    	
    	QByteArray ba = QByteArray::number(width);
    	ba += " ";
    	ba += QByteArray::number(height);
    	ba += "\n";
		f.write(ba);

    	str->reset();

    	int size = height * ((width + 7) / 8);
    	for (int i = 0; i < size; ++i) 
      		f.putChar((char)(str->getChar() ^ 0xff));

    	str->close();
    	f.close();
  	} 
  	else 
  	{
    	sprintf(fileName, "%s-%03d.ppm", fileRoot, imgNum);
    	++imgNum;
    	QString fn = codec->toUnicode(fileName);
    	QFile f(fn);
    	if (!f.open(QIODevice::WriteOnly))
    		return ;
    		
    	f.write("P6\n");
    	QByteArray ba = QByteArray::number(width);
    	ba += " ";
    	ba += QByteArray::number(height);
    	ba += "\n";
		f.write(ba);
    	f.write("255\n");

    	XWImageStream * imgStr = new XWImageStream(str, width, colorMap->getNumPixelComps(),
			     colorMap->getBits());
    	imgStr->reset();

		CoreRGB rgb;
    	for (int y = 0; y < height; ++y) 
    	{
      		uchar * p = imgStr->getLine();
      		for (int x = 0; x < width; ++x) 
      		{
				colorMap->getRGB(p, &rgb);
				f.putChar((char)(colToByte(rgb.r)));
				f.putChar((char)(colToByte(rgb.g)));
				f.putChar((char)(colToByte(rgb.b)));
				p += colorMap->getNumPixelComps();
      		}
    	}
    	delete imgStr;

    	f.close();
  	}
}

void XWImageOutputDev::drawImageMask(XWGraphixState *, 
                                     XWObject *, 
                                     XWStream *str,
			     			         int width, 
			     			         int height, 
			     			         bool ,
			                         bool inlineImg)
{
	QTextCodec * codec = QTextCodec::codecForLocale();
	if (dumpJPEG && str->getKind() == STREAM_DCT && !inlineImg) 
	{
    	sprintf(fileName, "%s-%03d.jpg", fileRoot, imgNum);
    	++imgNum;
    	QString fn = codec->toUnicode(fileName);
    	QFile f(fn);
    	if (!f.open(QIODevice::WriteOnly))
    		return ;

    	str = ((XWDCTStream *)str)->getRawStream();
    	str->reset();
		int c = EOF;
    	while ((c = str->getChar()) != EOF)
      		f.putChar((char)c);

    	str->close();
    	f.close();
  	}
  	else
  	{
  		sprintf(fileName, "%s-%03d.pbm", fileRoot, imgNum);
    	++imgNum;
    	QString fn = codec->toUnicode(fileName);
    	QFile f(fn);
    	if (!f.open(QIODevice::WriteOnly))
    		return ;
    		
    	f.write("P4\n");
    	
    	QByteArray ba = QByteArray::number(width);
    	ba += " ";
    	ba += QByteArray::number(height);
    	ba += "\n";
		f.write(ba);
		
    	str->reset();

    	int size = height * ((width + 7) / 8);
    	for (int i = 0; i < size; ++i) 
      		f.putChar((char)(str->getChar()));

    	str->close();
    	f.close();
  	}
}

void XWImageOutputDev::tilingPatternFill(XWGraphixState *, 
                                   XWGraphix *, 
                                   XWObject *,
				 													 int , 
				 													 XWDict *,
				 													 double *, 
				 													 double *,
				                           int , 
				                           int , 
				                           int , 
				                           int ,
				 											     double , 
				 											     double )
{
}
