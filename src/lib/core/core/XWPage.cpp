/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stddef.h>
#include "XWApplication.h"
#include "XWFontSetting.h"
#include "XWDocSetting.h"
#include "XWObject.h"
#include "XWArray.h"
#include "XWDict.h"
#include "XWDoc.h"
#include "XWRef.h"
#include "XWLink.h"
#include "XWOutputDev.h"
#include "XWGraphix.h"
#include "XWGraphixState.h"
#include "XWAnnot.h"
#include "XWCatalog.h"
#include "XWPage.h"


void XWPDFRectangle::clipTo(XWPDFRectangle *rect)
{
  if (x1 < rect->x1) {
    x1 = rect->x1;
  } else if (x1 > rect->x2) {
    x1 = rect->x2;
  }
  if (x2 < rect->x1) {
    x2 = rect->x1;
  } else if (x2 > rect->x2) {
    x2 = rect->x2;
  }
  if (y1 < rect->y1) {
    y1 = rect->y1;
  } else if (y1 > rect->y2) {
    y1 = rect->y2;
  }
  if (y2 < rect->y1) {
    y2 = rect->y1;
  } else if (y2 > rect->y2) {
    y2 = rect->y2;
  }
}

XWPageAttrs::XWPageAttrs()
{
	mediaBox.x1 = mediaBox.y1 = 0;
  mediaBox.x2 = mediaBox.y2 = 50;
  cropBox = mediaBox;
  haveCropBox = false;
  bleedBox = cropBox;
  trimBox = cropBox;
  artBox = cropBox;
  rotate = 0;
  lastModified.initNull();
  boxColorInfo.initNull();
  group.initNull();
  metadata.initNull();
  pieceInfo.initNull();
  separationInfo.initNull();
  resources.initNull();
}

XWPageAttrs::XWPageAttrs(XWPageAttrs *attrs, XWDict *dict)
{
  XWObject obj1;

  // get old/default values
  if (attrs) {
    mediaBox = attrs->mediaBox;
    cropBox = attrs->cropBox;
    haveCropBox = attrs->haveCropBox;
    rotate = attrs->rotate;
    attrs->resources.copy(&resources);
  } else {
    // set default MediaBox to 8.5" x 11" -- this shouldn't be necessary
    // but some (non-compliant) PDF files don't specify a MediaBox
    mediaBox.x1 = 0;
    mediaBox.y1 = 0;
    mediaBox.x2 = 612;
    mediaBox.y2 = 792;
    cropBox.x1 = cropBox.y1 = cropBox.x2 = cropBox.y2 = 0;
    haveCropBox = false;
    rotate = 0;
    resources.initNull();
  }

  // media box
  readBox(dict, "MediaBox", &mediaBox);

  // crop box
  if (readBox(dict, "CropBox", &cropBox)) {
    haveCropBox = true;
  }
  if (!haveCropBox) {
    cropBox = mediaBox;
  }

  // other boxes
  bleedBox = cropBox;
  readBox(dict, "BleedBox", &bleedBox);
  trimBox = cropBox;
  readBox(dict, "TrimBox", &trimBox);
  artBox = cropBox;
  readBox(dict, "ArtBox", &artBox);

  // rotate
  dict->lookup("Rotate", &obj1);
  if (obj1.isInt()) {
    rotate = obj1.getInt();
  }
  obj1.free();
  while (rotate < 0) {
    rotate += 360;
  }
  while (rotate >= 360) {
    rotate -= 360;
  }

  // misc attributes
  dict->lookup("LastModified", &lastModified);
  dict->lookup("BoxColorInfo", &boxColorInfo);
  dict->lookup("Group", &group);
  dict->lookup("Metadata", &metadata);
  dict->lookup("PieceInfo", &pieceInfo);
  dict->lookup("SeparationInfo", &separationInfo);

  // resource dictionary
  dict->lookup("Resources", &obj1);
  if (obj1.isDict()) {
    resources.free();
    obj1.copy(&resources);
  }
  obj1.free();
}

XWPageAttrs::~XWPageAttrs()
{
    lastModified.free();
    boxColorInfo.free();
    group.free();
    metadata.free();
    pieceInfo.free();
    separationInfo.free();
    resources.free();
}

void XWPageAttrs::clipBoxes()
{
	cropBox.clipTo(&mediaBox);
  bleedBox.clipTo(&mediaBox);
  trimBox.clipTo(&mediaBox);
  artBox.clipTo(&mediaBox);
}

bool XWPageAttrs::readBox(XWDict *dict, char *key, XWPDFRectangle *box)
{
  XWPDFRectangle tmp;
  double t;
  XWObject obj1, obj2;
  bool ok;

  dict->lookup(key, &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 4) {
    ok = true;
    obj1.arrayGet(0, &obj2);
    if (obj2.isNum()) {
      tmp.x1 = obj2.getNum();
    } else {
      ok = false;
    }
    obj2.free();
    obj1.arrayGet(1, &obj2);
    if (obj2.isNum()) {
      tmp.y1 = obj2.getNum();
    } else {
      ok = false;
    }
    obj2.free();
    obj1.arrayGet(2, &obj2);
    if (obj2.isNum()) {
      tmp.x2 = obj2.getNum();
    } else {
      ok = false;
    }
    obj2.free();
    obj1.arrayGet(3, &obj2);
    if (obj2.isNum()) {
      tmp.y2 = obj2.getNum();
    } else {
      ok = false;
    }
    obj2.free();
    if (ok) {
      if (tmp.x1 > tmp.x2) {
	t = tmp.x1; tmp.x1 = tmp.x2; tmp.x2 = t;
      }
      if (tmp.y1 > tmp.y2) {
	t = tmp.y1; tmp.y1 = tmp.y2; tmp.y2 = t;
      }
      *box = tmp;
    }
  } else {
    ok = false;
  }
  obj1.free();
  return ok;
}

XWPage::XWPage(XWDoc *docA, int numA, XWDict *pageDict, XWPageAttrs *attrsA)
{
  ok = true;
  doc = docA;
  xref = doc->getXRef();
  num = numA;

  // get attributes
  attrs = attrsA;
  attrs->clipBoxes();

  // annotations
  pageDict->lookupNF("Annots", &annots);
  if (!(annots.isRef() || annots.isArray() || annots.isNull())) {
    xwApp->error("Page annotations object (page {0:d}) is wrong type ({1:s})");
    annots.free();
    goto err2;
  }

  // contents
  pageDict->lookupNF("Contents", &contents);
  if (!(contents.isRef() || contents.isArray() ||
	contents.isNull())) {
    xwApp->error("Page contents object (page {0:d}) is wrong type ({1:s})");
    contents.free();
    goto err1;
  }

  return;

 err2:
  annots.initNull();
 err1:
  contents.initNull();
  ok = false;
}

XWPage::XWPage(XWDoc *docA, int numA)
{
	doc = docA;
  xref = doc->getXRef();
  num = numA;
  attrs = new XWPageAttrs();
  annots.initNull();
  contents.initNull();
  ok = true;
}

XWPage::~XWPage()
{
    delete attrs;
  	annots.free();
  	contents.free();
}

void XWPage::display(XWOutputDev *out, 
                     double hDPI, 
                     double vDPI,
	                 int rotate, 
	                 bool useMediaBox, 
	                 bool crop,
	                 bool printing, 
	                 bool (*abortCheckCbk)(void *data),
	                 void *abortCheckCbkData)
{
  displaySlice(out, hDPI, vDPI, rotate, useMediaBox, crop,
	       -1, -1, -1, -1, printing,
	       abortCheckCbk, abortCheckCbkData);
}

void XWPage::displaySlice(XWOutputDev *out, 
	                      double hDPI, 
	                      double vDPI,
		                  int rotate, 
		                  bool useMediaBox, 
		                  bool crop,
		                  int sliceX, 
		                  int sliceY, 
		                  int sliceW, 
		                  int sliceH,
		                  bool printing, 
		                  bool (*abortCheckCbk)(void *data),
		                  void *abortCheckCbkData)
{
  XWPDFRectangle *cropBox;
  XWPDFRectangle box;
  XWGraphix *gfx;
  XWObject obj;
  XWAnnots *annotList;
  XWDict *acroForm;
  int i;

  if (!out->checkPageSlice(this, hDPI, vDPI, rotate, useMediaBox, crop,
			   sliceX, sliceY, sliceW, sliceH,
			   printing, abortCheckCbk, abortCheckCbkData)) {
    return;
  }

  rotate += getRotate();
  if (rotate >= 360) {
    rotate -= 360;
  } else if (rotate < 0) {
    rotate += 360;
  }

  makeBox(hDPI, vDPI, rotate, useMediaBox, out->upsideDown(),
	  sliceX, sliceY, sliceW, sliceH, &box, &crop);
  cropBox = getCropBox();

  gfx = new XWGraphix(doc, out, num, attrs->getResourceDict(),
		hDPI, vDPI, &box, crop ? cropBox : (XWPDFRectangle *)NULL,
		rotate, abortCheckCbk, abortCheckCbkData);
  contents.fetch(xref, &obj);
  if (!obj.isNull()) {
    gfx->saveState();
    gfx->display(&obj);
    gfx->restoreState();
  } else {
    // empty pages need to call dump to do any setup required by the
    // OutputDev
    out->dump();
  }
  obj.free();

  // draw annotations
    annotList = new XWAnnots(doc, getAnnots(&obj));
    obj.free();
    acroForm = doc->getCatalog()->getAcroForm()->isDict() ?
               doc->getCatalog()->getAcroForm()->getDict() : NULL;
    if (acroForm) {
      if (acroForm->lookup("NeedAppearances", &obj)) {
	if (obj.isBool() && obj.getBool()) {
	  annotList->generateAppearances();
	}
      }
      obj.free();
    }
    if (annotList->getNumAnnots() > 0) {
      for (i = 0; i < annotList->getNumAnnots(); ++i) {
	annotList->getAnnot(i)->draw(gfx, printing);
      }
      out->dump();
    }
    delete annotList;

  delete gfx;
}

void XWPage::getDefaultCTM(double *ctm, 
                       double hDPI, 
                       double vDPI,
		               int rotate, 
		               bool useMediaBox, 
		               bool upsideDown)
{
  XWGraphixState *state;
  int i;

  rotate += getRotate();
  if (rotate >= 360) {
    rotate -= 360;
  } else if (rotate < 0) {
    rotate += 360;
  }
  state = new XWGraphixState(hDPI, vDPI,
		       useMediaBox ? getMediaBox() : getCropBox(),
		       rotate, upsideDown);
  for (i = 0; i < 6; ++i) {
    ctm[i] = state->getCTM()[i];
  }
  delete state;
}

XWLinks  * XWPage::getLinks(XWCatalog *catalog)
{
    XWObject obj;
    
    XWLinks *links = new XWLinks(getAnnots(&obj), catalog->getBaseURI());
    obj.free();
    return links;
}

void XWPage::makeBox(double hDPI, 
                     double vDPI, 
                     int rotate,
	                 bool useMediaBox, 
	                 bool upsideDown,
	                 double sliceX, 
	                 double sliceY, 
	                 double sliceW, 
	                 double sliceH,
	                 XWPDFRectangle *box, 
	                 bool *crop)
{
  XWPDFRectangle *mediaBox, *cropBox, *baseBox;
  double kx, ky;

  mediaBox = getMediaBox();
  cropBox = getCropBox();
  if (sliceW >= 0 && sliceH >= 0) {
    baseBox = useMediaBox ? mediaBox : cropBox;
    kx = 72.0 / hDPI;
    ky = 72.0 / vDPI;
    if (rotate == 90) {
      if (upsideDown) {
	box->x1 = baseBox->x1 + ky * sliceY;
	box->x2 = baseBox->x1 + ky * (sliceY + sliceH);
      } else {
	box->x1 = baseBox->x2 - ky * (sliceY + sliceH);
	box->x2 = baseBox->x2 - ky * sliceY;
      }
      box->y1 = baseBox->y1 + kx * sliceX;
      box->y2 = baseBox->y1 + kx * (sliceX + sliceW);
    } else if (rotate == 180) {
      box->x1 = baseBox->x2 - kx * (sliceX + sliceW);
      box->x2 = baseBox->x2 - kx * sliceX;
      if (upsideDown) {
	box->y1 = baseBox->y1 + ky * sliceY;
	box->y2 = baseBox->y1 + ky * (sliceY + sliceH);
      } else {
	box->y1 = baseBox->y2 - ky * (sliceY + sliceH);
	box->y2 = baseBox->y2 - ky * sliceY;
      }
    } else if (rotate == 270) {
      if (upsideDown) {
	box->x1 = baseBox->x2 - ky * (sliceY + sliceH);
	box->x2 = baseBox->x2 - ky * sliceY;
      } else {
	box->x1 = baseBox->x1 + ky * sliceY;
	box->x2 = baseBox->x1 + ky * (sliceY + sliceH);
      }
      box->y1 = baseBox->y2 - kx * (sliceX + sliceW);
      box->y2 = baseBox->y2 - kx * sliceX;
    } else {
      box->x1 = baseBox->x1 + kx * sliceX;
      box->x2 = baseBox->x1 + kx * (sliceX + sliceW);
      if (upsideDown) {
	box->y1 = baseBox->y2 - ky * (sliceY + sliceH);
	box->y2 = baseBox->y2 - ky * sliceY;
      } else {
	box->y1 = baseBox->y1 + ky * sliceY;
	box->y2 = baseBox->y1 + ky * (sliceY + sliceH);
      }
    }
  } else if (useMediaBox) {
    *box = *mediaBox;
  } else {
    *box = *cropBox;
    *crop = false;
  }
}

void XWPage::processLinks(XWOutputDev *out, XWCatalog *catalog)
{
    XWLinks * links = getLinks(catalog);
    for (int i = 0; i < links->getNumLinks(); ++i) 
        out->processLink(links->getLink(i), catalog);
    delete links;
}
