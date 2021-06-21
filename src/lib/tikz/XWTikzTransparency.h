/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZTRANSPARENCY_H
#define XWTIKZTRANSPARENCY_H

class XWPDFDriver;
class XWTikzState;

class XWTikzTransparency
{
public:
  XWTikzTransparency(XWPDFDriver * driverA);

public:
  XWPDFDriver * driver;
};

#endif //XWTIKZTRANSPARENCY_H
