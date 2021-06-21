/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <QCoreApplication>
#include "XWNumberUtil.h"
#include "XWApplication.h"
#include "XWDVIType.h"

void
transform_info_clear(TransformInfo *info)
{
  	/* Physical dimensions */
  	info->width    = 0.0;
  	info->height   = 0.0;
  	info->depth    = 0.0;

  	info->bbox.llx = 0.0;
  	info->bbox.lly = 0.0;
  	info->bbox.urx = 0.0;
  	info->bbox.ury = 0.0;

  	/* Transformation matrix */
  	pdf_setmatrix(&(info->matrix), 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);

  	info->flags    = 0;
}

#define DVICOLOR_INVALID 0

#ifdef  rgb
#undef  rgb
#endif
#ifdef  cmyk
#undef  cmyk
#endif
#define gray(g)       {1, {g}}
#define rgb8(r,g,b)   {3, {((r)/255.0), ((g)/255.0), ((b)/255.0), 0.0}}
#define cmyk(c,m,y,k) {4, {(c), (m), (y), (k)}}

struct PDFColor
{
	int    num_components;
  	double values[PDF_COLOR_COMPONENT_MAX];
};

static struct colordef_
{
  const char  *key;
  PDFColor    color;
} colordefs[] = {
	{"GreenYellow",    cmyk(0.15, 0.00, 0.69, 0.00)},
  	{"Yellow",         cmyk(0.00, 0.00, 1.00, 0.00)},
  	{"Goldenrod",      cmyk(0.00, 0.10, 0.84, 0.00)},
  	{"Dandelion",      cmyk(0.00, 0.29, 0.84, 0.00)},
  	{"Apricot",        cmyk(0.00, 0.32, 0.52, 0.00)},
  	{"Peach",          cmyk(0.00, 0.50, 0.70, 0.00)},
  	{"Melon",          cmyk(0.00, 0.46, 0.50, 0.00)},
  	{"YellowOrange",   cmyk(0.00, 0.42, 1.00, 0.00)},
  	{"Orange",         cmyk(0.00, 0.61, 0.87, 0.00)},
  	{"BurntOrange",    cmyk(0.00, 0.51, 1.00, 0.00)},
  	{"Bittersweet",    cmyk(0.00, 0.75, 1.00, 0.24)},
  	{"RedOrange",      cmyk(0.00, 0.77, 0.87, 0.00)},
  	{"Mahogany",       cmyk(0.00, 0.85, 0.87, 0.35)},
  	{"Maroon",         cmyk(0.00, 0.87, 0.68, 0.32)},
  	{"BrickRed",       cmyk(0.00, 0.89, 0.94, 0.28)},
  	{"Red",            cmyk(0.00, 1.00, 1.00, 0.00)},
  	{"OrangeRed",      cmyk(0.00, 1.00, 0.50, 0.00)},
  	{"RubineRed",      cmyk(0.00, 1.00, 0.13, 0.00)},
  	{"WildStrawberry", cmyk(0.00, 0.96, 0.39, 0.00)},
  	{"Salmon",         cmyk(0.00, 0.53, 0.38, 0.00)},
  	{"CarnationPink",  cmyk(0.00, 0.63, 0.00, 0.00)},
  	{"Magenta",        cmyk(0.00, 1.00, 0.00, 0.00)},
  	{"VioletRed",      cmyk(0.00, 0.81, 0.00, 0.00)},
  	{"Rhodamine",      cmyk(0.00, 0.82, 0.00, 0.00)},
  	{"Mulberry",       cmyk(0.34, 0.90, 0.00, 0.02)},
  	{"RedViolet",      cmyk(0.07, 0.90, 0.00, 0.34)},
  	{"Fuchsia",        cmyk(0.47, 0.91, 0.00, 0.08)},
  	{"Lavender",       cmyk(0.00, 0.48, 0.00, 0.00)},
  	{"Thistle",        cmyk(0.12, 0.59, 0.00, 0.00)},
  	{"Orchid",         cmyk(0.32, 0.64, 0.00, 0.00)},
  	{"DarkOrchid",     cmyk(0.40, 0.80, 0.20, 0.00)},
  	{"Purple",         cmyk(0.45, 0.86, 0.00, 0.00)},
  	{"Plum",           cmyk(0.50, 1.00, 0.00, 0.00)},
  	{"Violet",         cmyk(0.79, 0.88, 0.00, 0.00)},
  	{"RoyalPurple",    cmyk(0.75, 0.90, 0.00, 0.00)},
  	{"BlueViolet",     cmyk(0.86, 0.91, 0.00, 0.04)},
  	{"Periwinkle",     cmyk(0.57, 0.55, 0.00, 0.00)},
  	{"CadetBlue",      cmyk(0.62, 0.57, 0.23, 0.00)},
  	{"CornflowerBlue", cmyk(0.65, 0.13, 0.00, 0.00)},
  	{"MidnightBlue",   cmyk(0.98, 0.13, 0.00, 0.43)},
  	{"NavyBlue",       cmyk(0.94, 0.54, 0.00, 0.00)},
  	{"RoyalBlue",      cmyk(1.00, 0.50, 0.00, 0.00)},
  	{"Blue",           cmyk(1.00, 1.00, 0.00, 0.00)},
  	{"Cerulean",       cmyk(0.94, 0.11, 0.00, 0.00)},
  	{"Cyan",           cmyk(1.00, 0.00, 0.00, 0.00)},
  	{"ProcessBlue",    cmyk(0.96, 0.00, 0.00, 0.00)},
  	{"SkyBlue",        cmyk(0.62, 0.00, 0.12, 0.00)},
  	{"Turquoise",      cmyk(0.85, 0.00, 0.20, 0.00)},
  	{"TealBlue",       cmyk(0.86, 0.00, 0.34, 0.02)},
  	{"Aquamarine",     cmyk(0.82, 0.00, 0.30, 0.00)},
  	{"BlueGreen",      cmyk(0.85, 0.00, 0.33, 0.00)},
  	{"Emerald",        cmyk(1.00, 0.00, 0.50, 0.00)},
  	{"JungleGreen",    cmyk(0.99, 0.00, 0.52, 0.00)},
  	{"SeaGreen",       cmyk(0.69, 0.00, 0.50, 0.00)},
  	{"Green",          cmyk(1.00, 0.00, 1.00, 0.00)},
  	{"ForestGreen",    cmyk(0.91, 0.00, 0.88, 0.12)},
  	{"PineGreen",      cmyk(0.92, 0.00, 0.59, 0.25)},
  	{"LimeGreen",      cmyk(0.50, 0.00, 1.00, 0.00)},
  	{"YellowGreen",    cmyk(0.44, 0.00, 0.74, 0.00)},
  	{"SpringGreen",    cmyk(0.26, 0.00, 0.76, 0.00)},
  	{"OliveGreen",     cmyk(0.64, 0.00, 0.95, 0.40)},
  	{"RawSienna",      cmyk(0.00, 0.72, 1.00, 0.45)},
  	{"Sepia",          cmyk(0.00, 0.83, 1.00, 0.70)},
  	{"Brown",          cmyk(0.00, 0.81, 1.00, 0.60)},
  	{"Tan",            cmyk(0.14, 0.42, 0.56, 0.00)},
  	/* Adobe Reader 7 and 8 had problem when gray and cmyk black colors
   	* are mixed. No problem with Previewer.app.
   	* It happens when \usepackage[dvipdfm]{graphicx} and then called
   	* \usepackage{color} without dvipdfm option. */
  	{"Gray",           gray(0.5)},
  	{"Black",          gray(0.0)},
  	{"White",          gray(1.0)},
  	{NULL}
};

static const char *colorerror_strings[] = {
	QT_TRANSLATE_NOOP("XWDVIColor", "Invalid color value specified: ")
};

static QString getClolorErrStr(int type)
{
	return qApp->translate("XWDVIColor", colorerror_strings[type]);
}

XWDVIColor::XWDVIColor()
{
	values[0] = 0.0;
	num_components = 0;
}

XWDVIColor::XWDVIColor(const char * name)
{
	values[0] = 0.0;
	num_components = 0;
	setName(name);
}

XWDVIColor::XWDVIColor(double g)
{
	setGray(g);
}

XWDVIColor::XWDVIColor(double r, double g, double b)
{
	setRGB(r, g, b);
}

XWDVIColor::XWDVIColor(double c, double m, double y, double k)
{
	setCMYK(c, m, y, k);
}

XWDVIColor::XWDVIColor(const XWDVIColor * other)
{
	copy(other);
}

XWDVIColor::XWDVIColor(const XWDVIColor * src, double f)
{
	brighten(src, f);
}

void XWDVIColor::brighten(const XWDVIColor * src, double f)
{
	if (f == 1.0)
		white();
	else
	{
		num_components = src->num_components;
		int n = num_components;
		double f1 = n == 4 ? 0.0 : f;
		double f0 = 1.0 - f;
		while (n--)
      		values[n] = f0 * src->values[n] + f1;
	}
}

int XWDVIColor::compare(const XWDVIColor *color2)
{
	int n = num_components;
  	if (n != color2->num_components)
    	return -1;

  	while (n--)
  	{
    	if (values[n] != color2->values[n])
      		return -1;
    }

  	return 0;
}

void XWDVIColor::copy(const XWDVIColor * other)
{
	num_components = other->num_components;
	values[0] = other->values[0];
  	values[1] = other->values[1];
  	values[2] = other->values[2];
  	values[3] = other->values[3];
}

bool XWDVIColor::isValid()
{
	int n = num_components;
  	if (n != 1 && n != 3 && n != 4)
    	return false;

  	while (n--)
  	{
    	if (values[n] < 0.0 || values[n] > 1.0)
      		return false;
    }

  	return true;
}

bool XWDVIColor::isWhite()
{
	int n =num_components;
  	double f = n == 4 ? 0.0 : 1.0;
  	while (n--)
  	{
    	if (values[n] != f)
      		return false;
  	}

  	return true;
}

void XWDVIColor::setCMYK(double c, double m, double y, double k)
{
	if (c < 0.0 || c > 1.0) 
	{
		QString msg = getClolorErrStr(DVICOLOR_INVALID);
		msg += QString("cyan=%1\n").arg(c);
		xwApp->warning(msg);
		c = 0.0;
  	}
  	
  	if (m < 0.0 || m > 1.0) 
  	{
  		QString msg = getClolorErrStr(DVICOLOR_INVALID);
		msg += QString("magenta=%1\n").arg(c);
		xwApp->warning(msg);
		m = 0.0;
  	}
  	
  	if (y < 0.0 || y > 1.0) 
  	{
  		QString msg = getClolorErrStr(DVICOLOR_INVALID);
		msg += QString("yellow=%1\n").arg(c);
		xwApp->warning(msg);
		y = 0.0;
  	}
  	
  	if (k < 0.0 || k > 1.0) 
  	{
  		QString msg = getClolorErrStr(DVICOLOR_INVALID);
		msg += QString("black=%1\n").arg(c);
		xwApp->warning(msg);
		k = 0.0;
  	}

  	values[0] = c;
  	values[1] = m;
  	values[2] = y;
  	values[3] = k;

  	num_components = 4;
}

void XWDVIColor::setGray(double g)
{
	if (g < 0.0 || g > 1.0) 
	{
		QString msg = getClolorErrStr(DVICOLOR_INVALID);
		msg += QString("gray=%1\n").arg(g);
		xwApp->warning(msg);
    	g = 0.0;
  	}

  	values[0] = g;

  	num_components = 1;
}

int XWDVIColor::setName(const char * name)
{
	for (int i = 0; colordefs[i].key; i++) 
	{
    	if (!strcmp(colordefs[i].key, name)) 
    	{
    		
      		PDFColor * color = &colordefs[i].color;
      		num_components = color->num_components;
      		for (int k = 0; k < num_components; k++)
      			values[k] = color->values[k];
      		return  0;
    	}
  	}
  	return  -1;
}

void XWDVIColor::setRGB(double r, double g, double b)
{
	if (r < 0.0 || r > 1.0) 
	{
		QString msg = getClolorErrStr(DVICOLOR_INVALID);
		msg += QString("red=%1\n").arg(r);
		xwApp->warning(msg);
		r = 0.0;
  	}
  	
  	if (g < 0.0 || g > 1.0) 
  	{
    	QString msg = getClolorErrStr(DVICOLOR_INVALID);
		msg += QString("green=%1\n").arg(g);
		xwApp->warning(msg);
		g = 0.0;
  	}
  	
  	if (b < 0.0 || b > 1.0) 
  	{
    	QString msg = getClolorErrStr(DVICOLOR_INVALID);
		msg += QString("blue=%1\n").arg(b);
		xwApp->warning(msg);
		b = 0.0;
  	}
  	
  	values[0] = r;
  	values[1] = g;
  	values[2] = b;

  	num_components = 3;
}

void XWDVIColor::setRGBFromHSV(double h, double s, double v)
{
	double  r, g, b;
	r = g = b = v;
	if (s != 0.0)
	{
		double h6 = h * 6; /* 360 / 60 */
    	int i  = (int) h6;
    	double f  = h6 - i;
    	double v1 = v * (1 - s);
    	double v2 = v * (1 - s * f);
    	double v3 = v * (1 - s * (1 - f));
    	switch (i) 
    	{
    		case  0: 
    			r = v ; 
    			g = v3; 
    			b = v1; 
    			break;
    			
    		case  1: 
    			r = v2; 
    			g = v ; 
    			b = v1; 
    			break;
    			
    		case  2: 
    			r = v1; 
    			g = v ; 
    			b = v3; 
    			break;
    			
    		case  3: 
    			r = v1; 
    			g = v2; 
    			b = v ; 
    			break;
    			
    		case  4: 
    			r = v3; 
    			g = v1; 
    			b = v ; 
    			break;
    			
    		case  5: 
    			r = v ; 
    			g = v1; 
    			b = v2; 
    			break;
    			
    		case  6: 
    			r = v ; 
    			g = v1; 
    			b = v2; 
    			break;
    			
    	}
	}
	
	setRGB(r,g,b);
}

int XWDVIColor::toString(char *buffer)
{
	int len = 0;

  	for (int i = 0; i < num_components; i++) 
    	len += sprintf(buffer + len, " %g", round(values[i], 0.001));
    	
  	return len;
}
