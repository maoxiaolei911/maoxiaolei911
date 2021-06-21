/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "PGFKeyWord.h"
#include "tikzcolor.h"

QColor tikzEnumToColor(int C)
{
  QColor ret;
  switch (C)
  {
    case PGFblack:
      ret = Qt::black;
      break;

    case PGFblue:
      ret = Qt::blue;
      break;

    case PGFbrown:
      ret.setRgb(212, 140, 95);
      break;

    case PGFcyan:
      ret = Qt::cyan;
      break;

    case PGFdarkgray:
      ret = Qt::darkGray;
      break;

    case PGFgray:
      ret = Qt::gray;
      break;

    case PGFgreen:
      ret = Qt::green;
      break;

    case PGFlightgray:
      ret = Qt::lightGray;
      break;

    case PGFlime:
      ret.setRgb(191, 255, 0);
      break;

    case PGFmagenta:
      ret = Qt::magenta;
      break;

    case PGFolive:
      ret.setRgb(128, 128, 0);
      break;

    case PGForange:
      ret.setRgb(255, 69, 0);
      break;

    case PGFpink:
      ret.setRgb(255, 192, 203);
      break;

    case PGFpurple:
      ret.setRgb(128, 0, 128);
      break;

    case PGFred:
      ret = Qt::red;
      break;

    case PGFteal:
      ret.setRgb(0, 128, 128);
      break;

    case PGFviolet:
      ret.setRgb(199, 21, 133);
      break;

    case PGFwhite:
      ret = Qt::white;
      break;

    case PGFyellow:
      ret = Qt::yellow;
      break;

    case PGFlightsteelblue:
      ret.setRgbF(0.6875,0.765625,0.8671875);
      break;

    case PGFdarklightsteelblue:
      ret.setRgbF(0.34375,0.3828125,0.43359375);
      break;
  }

  return ret;
}

QColor calulateColor(const QColor & c, double p)
{
  QColor color;
  color.setRedF(c.redF() * p + 1-p);
  color.setGreenF(c.greenF() * p + 1-p);
  color.setBlueF(c.blueF() * p + 1-p);

  return color;
}

QColor calulateColor(const QColor & c1, double p, const QColor & c2)
{
  QColor color;

  color.setRedF(c1.redF() * p + c2.redF() * (1-p));
  color.setGreenF(c1.greenF() * p + c2.redF() * (1-p));
  color.setBlueF(c1.blueF() * p + c2.redF() * (1-p));

  return color;
}
