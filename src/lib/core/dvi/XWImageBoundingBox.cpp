/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <time.h>
#include <math.h>
#include <string.h>
#include <QImage>
#include <QTextCodec>
#include <QFile>
#include "XWUtil.h"
#include "XWFileName.h"
#include "XWPictureSea.h"
#include "XWDVIRef.h"
#include "XWJPGImage.h"
#include "XWPDFFile.h"
#include "XWPNGImage.h"
#include "XWMPost.h"
#include "XWImageBoundingBox.h"

#ifdef  ROUND
#undef  ROUND
#endif

#define ROUND(n,acc) (floor(((double)n)/(acc)+0.5)*(acc))

XWImageBoundingBox::XWImageBoundingBox(QCoreApplication * app)
	:XWApplication(app)
{
}

QString XWImageBoundingBox::getProductID()
{
	return "extractbb";
}

QString XWImageBoundingBox::getProductName()
{
	return tr("Extract BoundingBox");
}

const char * XWImageBoundingBox::getProductName8()
{
	return "extractbb";
}

QString XWImageBoundingBox::getVersion()
{
	return "0.9";
}

const char * XWImageBoundingBox::getVersion8()
{
	return "0.9";
}

void XWImageBoundingBox::start(int & argc, char**argv)
{
	char xbb_to_file = 1;
	char compat_mode = 0;
	argc--;
	if (argc <= 0)
	{
		showHelp();
		return ;
	}
	argv += 1;

	while (argc > 0 && *argv[0] == '-')
	{
		char *flag = argv[0] + 1;
		switch (*flag)
		{
			case '-':
				if (++flag)
				{
					if (!strcmp(flag, "help"))
					{
						showVersion();
						showHelp();
						return ;
					}
					else if (!strcmp(flag, "version"))
					{
						showVersion();
						return ;
					}
				}
				showHelp();
				break;

			case 'O':
      	xbb_to_file = 0;
      	argc -= 1;
      	argv += 1;
      	break;

      case 'm':
      	compat_mode = 1;
      	argc -= 1;
      	argv += 1;
      	break;

      case 'x':
      	compat_mode = 0;
      	argc -= 1;
      	argv += 1;
      	break;

      case 'h':
      	showHelp();
      	argc -= 1;
      	argv += 1;
      	break;

      default:
      	argc -= 1;
      	argv += 1;
      	break;
		}
	}

	QByteArray env = qgetenv("OUTPUT_PATH");
	QString outputpath;
  if (!env.isEmpty())
  {
		QTextCodec * codec = QTextCodec::codecForLocale();
		outputpath = codec->toUnicode(env);
	}

	XWPictureSea sea;
	QTextCodec * codec = QTextCodec::codecForLocale();
	bool is_tmp = false;
  bool ok = false;
  QFile * out = 0;
	for (; argc > 0; argc--, argv++)
	{
		QString fn = codec->toUnicode(argv[0]);
		if (fn.isEmpty())
			continue;

		QFile * fp = sea.openFile(fn, &is_tmp);
		if (!fp)
			continue;

		if (xbb_to_file == 1)
		{
			XWFileName ffn(fn);
			QString basename = ffn.baseName();
			QString filename;
			if (compat_mode)
			 	filename = QString("%1/%2.bb").arg(outputpath).arg(basename);
			else
				filename = QString("%1/%2.xbb").arg(outputpath).arg(basename);

			out = new QFile(filename);
			out->open(QIODevice::WriteOnly);
		}
		else
		{
			out = new QFile(0);
			out->open(stdout, QIODevice::WriteOnly);
		}

		ok = XWPNGImage::checkForPNG(fp) > 0;
		if (ok)
		{
			fp->seek(0);
			doPNG(fp, out, argv[0], compat_mode);
			continue;
		}
		fp->seek(0);
		ok = XWJPGImage::checkForJPEG(fp) > 0;
		if (ok)
		{
			fp->seek(0);
			doJPG(fp, out, argv[0], compat_mode);
			continue;
		}

		fp->seek(0);
		ok = XWPDFFile::checkForPDF(fp);
		if (ok)
		{
			fp->seek(0);
			doPDF(fp, out, argv[0], compat_mode);
			continue;
		}

		fp->seek(0);
		ok = XWMPost::checkForMP(fp) > 0;
		if (ok)
		{
			fp->seek(0);
			doMPS(fp, out, argv[0], compat_mode);
			continue;
		}

		fp->close();
		delete fp;
		out->close();
		delete out;
	}
}

void XWImageBoundingBox::doJPG(QIODevice * in,
	                             QIODevice * out,
	                             const char * fname,
	                             bool compat_mode)
{
	long  width = 0;
	long  height = 0;
  double xdensity = 0.0;
  double ydensity = 0.0;
  XWJPGImage::getBBox(in, &width, &height, &xdensity, &ydensity);
	in->close();
	delete in;
	writeXBb(out, fname, 0, 0, xdensity*width, ydensity*height, -1, -1, compat_mode);
	out->close();
	delete out;
}

void XWImageBoundingBox::doMPS(QIODevice * in,
	                             QIODevice * out,
	                             const char * fname,
	                             bool compat_mode)
{
	PDFRect  bbox;
	long len = in->size();
  if (len < 1)
  {
  	in->close();
		delete in;
		out->close();
		delete out;
    	return ;
  }

  char * buf = new char[len + 1];
  char * p      = buf;
  char * endptr = p + len;
  endptr[0] = '\0';
  while (len > 0)
  {
   	long nb_read = in->read(buf, len);
   	if (nb_read < 0)
   	{
     		delete [] buf;
     		delete [] buf;
  			in->close();
				delete in;
				out->close();
				delete out;
     		return ;
   	}
   	len -= nb_read;
  }

  int e = XWMPost::scanBBox(&p, endptr, &bbox);
  if (!e)
  	writeXBb(out, fname, bbox.lly, bbox.urx, bbox.ury, -1, -1, compat_mode);

  delete [] buf;
  in->close();
	delete in;
	out->close();
	delete out;
}

void XWImageBoundingBox::doPDF(QIODevice * in,
	                             QIODevice * out,
	                             const char * fname,
	                             bool compat_mode)
{
	XWDVIRef xref;
	xref.open();
	XWPDFFile * pf = XWPDFFile::open(&xref, fname, in);
	if (!pf)
	{
		in->close();
		delete in;
		out->close();
		delete out;
		return ;
	}

	XWObject pageA, resourcesA;
	PDFRect  bbox;
	long page_noA = 1;
  long count = 0;
 	if (pf->getPage(&page_noA, &count, &bbox, 0, &pageA))
 	{
 		in->seek(0);
 		writeXBb(out, fname, bbox.lly, bbox.urx, bbox.ury, XWPDFFile::checkVersion(in), count, compat_mode);
 	}

 	pageA.free();

 	in->close();
	delete in;
	out->close();
	delete out;
	XWPDFFile::closeAll();
}

void XWImageBoundingBox::doPNG(QIODevice * in,
	                             QIODevice * out,
	                             const char * fname,
	                             bool compat_mode)
{
	long  width = 0;
	long  height = 0;
  double xdensity = 0.0;
  double ydensity = 0.0;
  XWPNGImage::getBBox(in, &width, &height, &xdensity, &ydensity);
	in->close();
	delete in;
	writeXBb(out, fname, 0, 0, xdensity*width, ydensity*height, -1, -1, compat_mode);
	out->close();
	delete out;
}

void XWImageBoundingBox::doTime(QIODevice * fp)
{
	time_t current_time;
  struct tm *bd_time;

  time(&current_time);
  bd_time = localtime(&current_time);
  char buf[200];
  int len = sprintf(buf, "%%%%CreationDate: %s\n", asctime(bd_time));
  fp->write(buf, len);
}

void XWImageBoundingBox::showHelp()
{
	showMsg(tr("Usage: extractbb [-v|-q] [-O] [-m|-x] [files]\n"));
	showMsg(tr("  -O Write output to stdout\n"));
	showMsg(tr("  -m Output .bb  file\n"));
	showMsg(tr("  -x Output .xbb file\n"));
}

void XWImageBoundingBox::showMsg(const QString & msg)
{
	QFile file;
	file.open(stdout, QIODevice::WriteOnly);
	puts(&file, msg);
	file.close();
}

void XWImageBoundingBox::showVersion()
{
	QString v = getVersion();
	v = QString("extractbb version %1\n").arg(v);
	showMsg(v);
	showMsg(tr("Extract bounding box from PDF, PNG, JPEG and MPS images.\n"));
}

void XWImageBoundingBox::writeXBb(QIODevice * fp,
	                                const char * fname,
	                                double bbllx_f,
	                                double bblly_f,
		                              double bburx_f,
		                              double bbury_f,
		                              int pdf_version,
		                              long pagecount,
		                              bool compat_mode)
{
	long bbllx = ROUND(bbllx_f, 1.0);
	long bblly = ROUND(bblly_f, 1.0);
  long bburx = ROUND(bburx_f, 1.0);
  long bbury = ROUND(bbury_f, 1.0);

  char buf[500];
  int len = sprintf(buf, "%%%%Title: %s\n", fname);
  fp->write(buf, len);
  len = sprintf(buf, "%%%%Creator: extractbb 0.9\n");
  fp->write(buf, len);
  len = sprintf(buf, "%%%%BoundingBox: %ld %ld %ld %ld\n", bbllx, bblly, bburx, bbury);
  fp->write(buf, len);
  if (!compat_mode)
  {
  	len = sprintf(buf, "%%%%HiResBoundingBox: %f %f %f %f\n", bbllx_f, bblly_f, bburx_f, bbury_f);
  	fp->write(buf, len);
  	if (pdf_version >= 0)
  	{
  		len = sprintf(buf, "%%%%PDFVersion: 1.%d\n", pdf_version);
  		fp->write(buf, len);
  		len = sprintf(buf, "%%%%Pages: %ld\n", pagecount);
  		fp->write(buf, len);
  	}
  }

  doTime(fp);
}
