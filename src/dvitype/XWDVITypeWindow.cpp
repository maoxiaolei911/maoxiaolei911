/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWConst.h"
#include "XWDocSea.h"
#include "XWUtil.h"
#include "XWDVIType.h"
#include "XWDVITypeWindow.h"


XWDVITypeWindow::XWDVITypeWindow(QWidget * parent)
	:QPlainTextEdit(parent)
{
	setWindowIcon(QIcon(":/images/xiuwen24.png"));
	setReadOnly(true);
}


void XWDVITypeWindow::loadFile(const QString & filename)
{
	clear();
	fullFileName = filename;
	setWindowTitle(fullFileName);
	if (fullFileName.isEmpty())
		return ;
		
	XWDocSea sea;
	QIODevice * fileA = sea.openDvi(fullFileName);
	if (!fileA)
	{
		QString msg = QString(tr("Can not open file \"%1\".")).arg(fullFileName);
		QMessageBox::critical(this, tr("Open File Error"), msg);
		return ;
	}
		
	QTextCharFormat posformat;
	posformat.setFontWeight(QFont::Bold);
	posformat.setForeground(Qt::black);
		
	QTextCharFormat charformat;
	charformat.setFontWeight(QFont::Normal);
	posformat.setForeground(Qt::black);
		
	QTextCharFormat specialformat;
	specialformat.setFontWeight(QFont::Normal);
	specialformat.setForeground(Qt::red);
		
	QTextCharFormat fontformat;
	fontformat.setFontWeight(QFont::Normal);
	fontformat.setForeground(Qt::darkBlue);
		
	QTextCharFormat otherformat;
	otherformat.setFontWeight(QFont::Normal);
	otherformat.setForeground(Qt::darkCyan);
		
	QTextCharFormat pageformat;
	pageformat.setFontWeight(QFont::Bold);
	pageformat.setForeground(Qt::darkRed);
	
	QString title = QString(tr("DVI Parameters:\n"
	                           "h: Cartesian horizontal coordinate.\n"
	                           "v: Cartesian vertical coordinate.\n"
	                           "w,x: horizontal spacing amounts.\n"
	                           "y,z: vertical spacing amounts.\n"
	                           "f: current font.\n\n"));
	
	QTextCursor cursor = textCursor();
	cursor.setCharFormat(posformat);
	cursor.insertText(title);
	
	uchar opcode = 0;
  fileA->seek(0);  
  int p = 0;
  int i, k;
  QString tmp;
  char * buf = 0;
  while (!fileA->atEnd())
  {
  	cursor = textCursor();
  	cursor.setCharFormat(posformat);
  	QString str = QString("%1: ").arg(p, 8, 10, QChar('0'));
  	cursor.insertText(str);
  	
  	if (!fileA->getChar((char*)&opcode))
  		break;
  	
  	switch (opcode)
  	{
  		case DVI_PRE:  			
  			str = "pre 247";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getUnsignedByte(fileA);
  			str = QString("(file header,DVI format=%1,").arg(i);
  			i = getUnsignedQuad(fileA);
  			tmp = QString("numerator=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedQuad(fileA);
  			tmp = QString("denominator=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedQuad(fileA);
  			tmp = QString("magnification=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedByte(fileA);
  			tmp = QString("comment length=%1,comment=\"").arg(i);
  			str += tmp;
  			buf = new char[i + 1];
  			fileA->read(buf, i);
  			tmp = QString::fromUtf8(buf, i);
  			str += tmp;
  			str += "\")\n";
  			delete [] buf;
  			buf = 0;
  			break;
  			
  		case DVI_BOP:
  			str = "bop 139";
  			cursor.insertText(str);
  			cursor.setCharFormat(pageformat);
  			str = "(beginning of page,";
  			for (k = 0; k < 10; k++)
  			{
					i = getSignedQuad(fileA);
					tmp = QString("c%1=%2,").arg(k).arg(i);
					str += tmp;
				}
				i = getSignedQuad(fileA);
				tmp = QString("previous bop=%1)\n").arg(i);
  			str += tmp;
  			break;
  			
  		case DVI_EOP:
  			str = "eop 140";
  			cursor.insertText(str);
  			cursor.setCharFormat(pageformat);
  			str = "(ending of page)\n";  			
  			break;
  			
  		case DVI_POST:
  			str = "post 248";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "(beginning of postamble,";
  			i = getUnsignedQuad(fileA);
  			tmp = QString("final bop=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedQuad(fileA);
  			tmp = QString("numerator=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedQuad(fileA);
  			tmp = QString("denominator=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedQuad(fileA);
  			tmp = QString("magnification=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedQuad(fileA);
  			tmp = QString("media height=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedQuad(fileA);
  			tmp = QString("media width=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedPair(fileA);
  			tmp = QString("maximum stack depth=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedPair(fileA);
  			tmp = QString("total pages=%1)\n").arg(i);
  			str += tmp;
  			break;
  			
  		case DVI_POSTPOST:
  			str = "postpost 249";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "(ending of postamble,";
  			i = getUnsignedQuad(fileA);
  			tmp = QString("post=%1,").arg(i);
  			str += tmp;
  			i = getUnsignedByte(fileA);
  			tmp = QString("DVI format=%1,").arg(i);
  			str += tmp;
  			while (!fileA->atEnd())
  			{
  				fileA->getChar((char*)&opcode);
  				tmp = QString("%1").arg(opcode);
  				str += tmp;
  				if (!fileA->atEnd())
  					str += ",";
  			}
  			str += ")\n";
  			break;
  			
  		case DVI_SET1:
  			str = "set1 128";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			i = getUnsignedByte(fileA);
  			str = QString("(typeset char %1,h = h + width of char %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_SET2:
  			str = "set2 129";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			i = getUnsignedPair(fileA);
  			str = QString("(typeset char %1,h = h + width of char %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_SET3:
  			str = "set3 130";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			i = getUnsignedTriple(fileA);
  			str = QString("(typeset char %1,h = h + width of char %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_SET4:
  			str = "set4 131";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			i = getUnsignedQuad(fileA);
  			str = QString("(typeset char %1,h = h + width of char %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_SETRULE:
  			str = "set_rule 132";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			str = "(typeset rectangle,";
  			i = getSignedQuad(fileA);
  			tmp = QString("height=%1,h = h + height,").arg(i);
  			str += tmp;
  			i = getSignedQuad(fileA);
  			tmp = QString("width=%1)\n").arg(i);
  			str += tmp;
  			break;
  			
  		case DVI_PUT1:
  			str = "put1 133";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			i = getUnsignedByte(fileA);
  			str = QString("(typeset char %1)\n").arg(i);
  			break;
  			
  		case DVI_PUT2:
  			str = "put2 134";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			i = getUnsignedPair(fileA);
  			str = QString("(typeset char %1)\n").arg(i);
  			break;
  			
  		case DVI_PUT3:
  			str = "put3 135";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			i = getUnsignedTriple(fileA);
  			str = QString("(typeset char %1)\n").arg(i);
  			break;
  			
  		case DVI_PUT4:
  			str = "put4 136";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			i = getUnsignedQuad(fileA);
  			str = QString("(typeset char %1)\n").arg(i);
  			break;
  			
  		case DVI_PUTRULE:
  			str = "put_rule 137";
  			cursor.insertText(str);
  			cursor.setCharFormat(charformat);
  			str = "(typeset rectangle,";
  			i = getSignedQuad(fileA);
  			tmp = QString("height=%1,").arg(i);
  			str += tmp;
  			i = getSignedQuad(fileA);
  			tmp = QString("width=%1)\n").arg(i);
  			str += tmp;
  			break;
  			
  		case DVI_NOP:
  			str = "nop 138";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "(no operation)\n";
  			break;
  			
  		case DVI_PUSH:
  			str = "push 141";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "([h, v, w, x, y, z]->stack)\n";
  			break;
  			
  		case DVI_POP:
  			str = "pop 142";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "(stack->[h, v, w, x, y, z])\n";
  			break;
  			
  		case DVI_RIGHT1:
  			str = "right1 143";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedByte(fileA);
  			str = QString("(h = h + %1)\n").arg(i);
  			break;
  			
  		case DVI_RIGHT2:
  			str = "right2 144";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedPair(fileA);
  			str = QString("(h = h + %1)\n").arg(i);
  			break;
  			
  		case DVI_RIGHT3:
  			str = "right3 145";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedTriple(fileA);
  			str = QString("(h = h + %1)\n").arg(i);
  			break;
  			
  		case DVI_RIGHT4:
  			str = "right4 146";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedQuad(fileA);
  			str = QString("(h = h + %1)\n").arg(i);
  			break;
  			
  		case DVI_W0:
  			str = "w0 147";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "(h = h + w)\n";
  			break;
  			
  		case DVI_W1:
  			str = "w1 148";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedByte(fileA);
  			str = QString("(w = %1, h = h + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_W2:
  			str = "w2 149";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedPair(fileA);
  			str = QString("(w = %1, h = h + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_W3:
  			str = "w3 150";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);  			
  			i = getSignedTriple(fileA);
  			str = QString("(w = %1, h = h + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_W4:
  			str = "w4 151";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);  
  			i = getSignedQuad(fileA);
  			str = QString("(w = %1, h = h + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_X0:
  			str = "x0 152";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "( h = h + x)\n";
  			break;
  			
  		case DVI_X1:
  			str = "x1 153";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedByte(fileA);
  			str = QString("(x = %1, h = h + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_X2:
  			str = "x2 154";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedPair(fileA);
  			str = QString("(x = %1, h = h + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_X3:
  			str = "x3 155";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedTriple(fileA);
  			str = QString("(x = %1, h = h + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_X4:
  			str = "x4 156 ";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedQuad(fileA);  			
  			str = QString("(x = %1, h = h + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_DOWN1:
  			str = "down1 157";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedByte(fileA);
  			str = QString("(v = v + %1)\n").arg(i);
  			break;
  			
  		case DVI_DOWN2:
  			str = "down2 158";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedPair(fileA);
  			str = QString("(v = v + %1)\n").arg(i);
  			break;
  			
  		case DVI_DOWN3:
  			str = "down3 159";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedTriple(fileA);
  			str = QString("(v = v + %1)\n").arg(i);
  			break;
  			
  		case DVI_DOWN4:
  			str = "down4 160";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedQuad(fileA);
  			str = QString("(v = v + %1)\n").arg(i);
  			break;
  			
  		case DVI_Y0:
  			str = "y0 161";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "(v = v + y)\n";
  			break;
  			
  		case DVI_Y1:
  			str = "y1 162";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedByte(fileA);
  			str = QString("(y = %1, v = v + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_Y2:
  			str = "y2 163";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedPair(fileA);
  			str = QString("(y = %1, v = v + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_Y3:
  			str = "y3 164";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedTriple(fileA);
  			str = QString("(y = %1, v = v + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_Y4:
  			str = "y4 165";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedQuad(fileA);
  			str = QString("(y = %1, v = v + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_Z0:
  			str = "z0 166";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "(v = v + z)\n";
  			break;
  			
  		case DVI_Z1:
  			str = "z1 167";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedByte(fileA);
  			str = QString("(z = %1, v = v + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_Z2:
  			str = "z2 168";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedPair(fileA);
  			str = QString("(z = %1, v = v + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_Z3:
  			str = "z3 169";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedTriple(fileA);
  			str = QString("(z = %1, v = v + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_Z4:
  			str = "z4 170";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getSignedQuad(fileA);
  			str = QString("(z = %1, v = v + %2)\n").arg(i).arg(i);
  			break;
  			
  		case DVI_FNT1:
  			str = "fnt1 235";
  			cursor.insertText(str);
  			cursor.setCharFormat(fontformat);
  			i = getUnsignedByte(fileA);
  			str = QString("(f = %1)\n").arg(i);
  			break;
  			
  		case DVI_FNT2:
  			str = "fnt2 236";
  			cursor.insertText(str);
  			cursor.setCharFormat(fontformat);
  			i = getUnsignedPair(fileA);
  			str = QString("(f = %1)\n").arg(i);
  			break;
  			
  		case DVI_FNT3:  			
  			str = "fnt3 237";
  			cursor.insertText(str);
  			cursor.setCharFormat(fontformat);
  			i = getUnsignedTriple(fileA);
  			str = QString("(f = %1)\n").arg(i);
  			break;
  			
  		case DVI_FNT4:
  			str = "fnt4 238";
  			cursor.insertText(str);
  			cursor.setCharFormat(fontformat);
  			i = getSignedQuad(fileA);
  			str = QString("(f = %1)\n").arg(i);
  			break;
  			
  		case DVI_XXX1:
  			str = "xxx1 239";
  			cursor.insertText(str);
  			cursor.setCharFormat(specialformat);
  			i = getUnsignedByte(fileA);
  			str = QString("(len = %1, special=\"").arg(i);
  			buf = new char[i + 1];
  			fileA->read(buf, i);
  			tmp = QString::fromUtf8(buf, i);
  			str += tmp;
  			str += "\")\n";
  			delete [] buf;
  			buf = 0;
  			break;
  			
  		case DVI_XXX2:
  			str = "xxx2 240";
  			cursor.insertText(str);
  			cursor.setCharFormat(specialformat);
  			i = getUnsignedPair(fileA);
  			str = QString("(len = %1, special=\"").arg(i);
  			buf = new char[i + 1];
  			fileA->read(buf, i);
  			tmp = QString::fromUtf8(buf, i);
  			str += tmp;
  			str += "\")\n";
  			delete [] buf;
  			buf = 0;
  			break;
  			
  		case DVI_XXX3:
  			str = "xxx3 241";
  			cursor.insertText(str);
  			cursor.setCharFormat(specialformat);
  			i = getUnsignedTriple(fileA);
  			str = QString("(len = %1, special=\"").arg(i);
  			buf = new char[i + 1];
  			fileA->read(buf, i);
  			tmp = QString::fromUtf8(buf, i);
  			str += tmp;
  			str += "\")\n";
  			delete [] buf;
  			buf = 0;
  			break;
  			
  		case DVI_XXX4:
  			str = "xxx4 242";
  			cursor.insertText(str);
  			cursor.setCharFormat(specialformat);
  			i = getUnsignedQuad(fileA);
  			str = QString("(len = %1, special=\"").arg(i);
  			buf = new char[i + 1];
  			fileA->read(buf, i);
  			tmp = QString::fromUtf8(buf, i);
  			str += tmp;
  			str += "\")\n";
  			delete [] buf;
  			buf = 0;
  			break;
  			
  		case DVI_FNTDEF1:
  		case DVI_FNTDEF2:
  		case DVI_FNTDEF3:
  		case DVI_FNTDEF4:  			
  			if (opcode == DVI_FNTDEF1)
  			{
  				str = "fnt_def_1 243";  				
  				i = getUnsignedByte(fileA);
  			}
  			else if (opcode == DVI_FNTDEF2)
  			{
  				str = "fnt_def_2 244";
  				i = getUnsignedPair(fileA);
  			}
  			else if (opcode == DVI_FNTDEF2)
  			{
  				str = "fnt_def_3 245";
  				i = getUnsignedTriple(fileA);
  			}
  			else
  			{
  				str = "fnt_def_4 246";
  				i = getSignedQuad(fileA);
  			}  			
  			cursor.insertText(str);
  			cursor.setCharFormat(fontformat);
  			str = QString("(define font %1,").arg(i);
  			i = getUnsignedQuad(fileA);
  			tmp = QString("check sum = %1,").arg(i);
  			str += tmp;
  			i = getUnsignedQuad(fileA);
  			tmp = QString("point size = %1,").arg(i);
  			str += tmp;
  			i = getUnsignedQuad(fileA);
  			tmp = QString("design size = %1,").arg(i);
  			str += tmp;
  			i = getUnsignedByte(fileA);
  			tmp = QString("area length = %1,").arg(i);
  			str += tmp;
  			k = getUnsignedByte(fileA);
  			tmp = QString("name length = %1,").arg(k);
  			str += tmp;
  			if (i > 0)
  			{
  				buf = new char[i + 1];
  				fileA->read(buf, i);
  				buf[i] = '\0';
  				tmp = QString::fromUtf8(buf, i);
  				str += tmp;
  				str += ",";
  				delete [] buf;
  				buf = 0;
  			}
  			if (k > 0)
  			{
  				buf = new char[k + 1];
  				fileA->read(buf, k);
  				buf[k] = '\0';
  				tmp = QString::fromUtf8(buf, k);
  				str += tmp;
  				delete [] buf;
  				buf = 0;
  			}
  			str += ")\n";
  			break;
  			
  		case XDV_PIC_FILE:
  			str = "pic_file 251";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			str = "(file=\"";
  			getUnsignedByte(fileA);
  			getSignedQuad(fileA);
  			getSignedQuad(fileA);
  			getSignedQuad(fileA);
  			getSignedQuad(fileA);
  			getSignedQuad(fileA);
  			getSignedQuad(fileA);
  			i = getUnsignedPair(fileA);
  			buf = new char[i + 1];
  			fileA->read(buf, i);
  			buf[i] = '\0';
  			tmp = QString::fromUtf8(buf, i);
  			str += tmp;
  			str += "\")\n";
  			delete [] buf;
    		break;
  			
  		case XDV_NATIVE_FONT_DEF:
  			str = "native_fnt_def 252";
  			cursor.insertText(str);
  			cursor.setCharFormat(fontformat);
  			i = getUnsignedQuad(fileA);
  			str = QString("(point size = %1,").arg(i);
  			i = getUnsignedPair(fileA);
  			tmp = QString("flags = 0x%1,").arg(i,0, 16);
  			str += tmp;
  			if ((i & XDV_FLAG_FONTTYPE_ICU) || (i & XDV_FLAG_FONTTYPE_ATSUI))
  			{
  				k = getUnsignedByte(fileA);
  				tmp = QString("name length = %1, name=\"").arg(k);
  				str += tmp;
  				buf = new char[k + 1];
  				fileA->read(buf, k);
  				buf[i] = '\0';
  				tmp = QString::fromUtf8(buf, k);
  				str += tmp;
  				str += "\",";
  				delete [] buf;
  				k = getUnsignedByte(fileA);
  				tmp = QString("family length = %1, family=\"").arg(k);
  				str += tmp;
  				buf = new char[k + 1];
  				fileA->read(buf, k);
  				buf[i] = '\0';
  				tmp = QString::fromUtf8(buf, k);
  				str += tmp;
  				str += "\",";
  				delete [] buf;
  				k = getUnsignedByte(fileA);
  				tmp = QString("style length = %1, style=\"").arg(k);
  				str += tmp;
  				buf = new char[k + 1];
  				fileA->read(buf, k);
  				buf[i] = '\0';
  				tmp = QString::fromUtf8(buf, k);
  				str += tmp;
  				str += "\"";
  				delete [] buf;
  				if (i & XDV_FLAG_COLORED)
  					getUnsignedQuad(fileA);
  					
  				if (i & XDV_FLAG_VARIATIONS)
  				{
  					k = getUnsignedPair(fileA);
  					for (int j = 0; j < k * 2; j++)
  						getUnsignedQuad(fileA);
  				}
  			}
  			str += ")\n";
  			break;
  			
  		case XDV_GLYPH_ARRAY:
  			str = "glyph_array 253";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getUnsignedPair(fileA);
  			for (k = 0; k < i; k++)
  				getSignedQuad(fileA);
  			for (k = 0; k < i; k++)
  				getUnsignedPair(fileA);
  			str = "\n";
  			break;
  			
  		case XDV_GLYPH_STRING:
  			str = "glyph_string 254";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getUnsignedPair(fileA);
  			for (k = 0; k < i; k++)
  			{
  				getSignedQuad(fileA);
  				getSignedQuad(fileA);
  			}
  			for (k = 0; k < i; k++)
  				getUnsignedPair(fileA);
  			str = "\n";
  			break;
  			
  		case DVI_DIR:
  			str = "dir 255";
  			cursor.insertText(str);
  			cursor.setCharFormat(otherformat);
  			i = getUnsignedByte(fileA);
  			str = QString("(direction=%1)\n").arg(i);
  			break;
  			
  		default:
  			if (opcode >= 0 && opcode < DVI_SET1)
  			{
  				str = QString("set_char_%1").arg(opcode);
  				cursor.insertText(str);
  				cursor.setCharFormat(charformat);
  				str = QString("(typeset char %1,h = h + width of char %2)\n").arg(opcode).arg(opcode);
  			}
  			else if (opcode >= DVI_FNTNUM0 && opcode < DVI_FNT1)
  			{
  				str = QString("fnt_num_%1 %2").arg(opcode - 171).arg(opcode);
  				cursor.insertText(str);
  				cursor.setCharFormat(fontformat);
  				str = QString("(f = %1)\n").arg(opcode - 171);
  			}
  			else
  			{
  				fileA->close();
  				delete fileA;
  				QString msg = QString(tr("File \"%1\" is not a dvi file.")).arg(fullFileName);
					QMessageBox::critical(this, tr("File Type Error"), msg);
					return ;
  			}
  			break;
  	}
  	
  	cursor.insertText(str);
  	p = (int)(fileA->pos());
  }
  
  fileA->close();
  delete fileA;
}
