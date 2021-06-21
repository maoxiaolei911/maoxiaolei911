/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWOPTIONALCONTENT_H
#define XWOPTIONALCONTENT_H

#include "XWGlobal.h"
#include "XWObject.h"


class XWString;
class XWList;
class XWDoc;
class XWRef;
class XWOptionalContentGroup;
class XWOCDisplayNode;

class XW_CORE_EXPORT XWOptionalContent
{
public:

  XWOptionalContent(XWDoc *doc);
  ~XWOptionalContent();

  // Walk the list of optional content groups.
  int getNumOCGs();
  XWOptionalContentGroup *getOCG(int idx);

  // Find an OCG by indirect reference.
  XWOptionalContentGroup *findOCG(ObjRef *ref);

  // Get the root node of the optional content group display tree
  // (which does not necessarily include all of the OCGs).
  XWOCDisplayNode *getDisplayRoot() { return display; }

  // Evaluate an optional content object -- either an OCG or an OCMD.
  // If <obj> is a valid OCG or OCMD, sets *<visible> and returns
  // true; otherwise returns false.
  bool evalOCObject(XWObject *obj, bool *visible);

private:

  bool evalOCVisibilityExpr(XWObject *expr, int recursion);

  XWRef *xref;
  XWList *ocgs;			// all OCGs [OptionalContentGroup]
  XWOCDisplayNode *display;	// root node of display tree 
};

enum OCUsageState {
  ocUsageOn,
  ocUsageOff,
  ocUsageUnset
};

class XWOptionalContentGroup
{
public:

  static XWOptionalContentGroup *parse(ObjRef *refA, XWObject *obj);
  ~XWOptionalContentGroup();

  bool matches(ObjRef *refA);

  int *getName() { return name; }
  int getNameLength() { return nameLen; }
  OCUsageState getViewState() { return viewState; }
  OCUsageState getPrintState() { return printState; }
  bool getState() { return state; }
  void setState(bool stateA) { state = stateA; }

private:

  XWOptionalContentGroup(ObjRef *refA, int *nameA, int nameLenA,
		       							 OCUsageState viewStateA, OCUsageState printStateA);

  ObjRef ref;
  int *name;
  int nameLen;
  OCUsageState viewState,	// suggested state when viewing
               printState;	// suggested state when printing
  bool state;			// current state (on/off)
};

class XWOCDisplayNode
{
public:

  static XWOCDisplayNode *parse(XWObject *obj, XWOptionalContent *oc, XWRef *xref, int recursion = 0);
  XWOCDisplayNode();
  ~XWOCDisplayNode();

  int *getName() { return name; }
  int getNameLength() { return nameLen; }
  XWOptionalContentGroup *getOCG() { return ocg; }
  int getNumChildren();
  XWOCDisplayNode *getChild(int idx);

private:

  XWOCDisplayNode(XWString *nameA);
  XWOCDisplayNode(XWOptionalContentGroup *ocgA);
  void addChild(XWOCDisplayNode *child);
  void addChildren(XWList *childrenA);
  XWList *takeChildren();

  int *name;		// display name (may be NULL)
  int nameLen;
  XWOptionalContentGroup *ocg;	// NULL for display labels
  XWList *children;		// NULL if there are no children
				//   [OCDisplayNode]
};

#endif //XWOPTIONALCONTENT_H
