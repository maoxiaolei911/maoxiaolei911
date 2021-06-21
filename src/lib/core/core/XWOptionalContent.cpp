/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWString.h"
#include "XWList.h"
#include "XWApplication.h"
#include "XWObject.h"
#include "XWDoc.h"
#include "XWDocEncoding.h"
#include "XWOptionalContent.h"


#define ocPolicyAllOn  1
#define ocPolicyAnyOn  2
#define ocPolicyAnyOff 3
#define ocPolicyAllOff 4

// Max depth of nested visibility expressions.  This is used to catch
// infinite loops in the visibility expression object structure.
#define visibilityExprRecursionLimit 50

// Max depth of nested display nodes.  This is used to catch infinite
// loops in the "Order" object structure.
#define displayNodeRecursionLimit 50


XWOptionalContent::XWOptionalContent(XWDoc *doc)
{
	XWObject *ocProps;
  XWObject ocgList, defView, obj1, obj2;
  ObjRef ref1;
  XWOptionalContentGroup *ocg;
  int i;
  
  xref = doc->getXRef();
  ocgs = new XWList();
  display = NULL;
  
  if ((ocProps = doc->getCatalog()->getOCProperties())->isDict())
  {
  	if (ocProps->dictLookup("OCGs", &ocgList)->isArray())
  	{
  		for (i = 0; i < ocgList.arrayGetLength(); ++i)
  		{
  			if (ocgList.arrayGetNF(i, &obj1)->isRef()) 
  			{
	  			ref1 = obj1.getRef();
	  			obj1.fetch(xref, &obj2);
	  			if ((ocg = XWOptionalContentGroup::parse(&ref1, &obj2))) 
	  			{
	    			ocgs->append(ocg);
	  			}
	  			obj2.free();
				}
				obj1.free();
  		}
  		
  		if (ocProps->dictLookup("D", &defView)->isDict())
  		{
  			if (defView.dictLookup("OFF", &obj1)->isArray()) 
  			{
	  			for (i = 0; i < obj1.arrayGetLength(); ++i) 
	  			{
	    			if (obj1.arrayGetNF(i, &obj2)->isRef()) 
	    			{
	      			ref1 = obj2.getRef();
	      			if ((ocg = findOCG(&ref1))) 
	      			{
								ocg->setState(false);
	      			} 
	      			else 
	      			{
								xwApp->error("Invalid OCG reference in OFF array in default viewing OCCD");
	      			}
	    			}
	    			obj2.free();
	  			}
				}
				obj1.free();

				//----- display order
				if (defView.dictLookup("Order", &obj1)->isArray()) 
				{
	  			display = XWOCDisplayNode::parse(&obj1, this, xref);
				}
				obj1.free();
  		}
  		else
  			xwApp->error("Missing or invalid default viewing OCCD");
  			
  		defView.free();
  	}
  	
  	ocgList.free();
  }
  
  if (!display) 
  {
    display = new XWOCDisplayNode();
  }
}

XWOptionalContent::~XWOptionalContent()
{
	deleteXWList(ocgs, XWOptionalContentGroup);
  delete display;
}


int XWOptionalContent::getNumOCGs() 
{
  return ocgs->getLength();
}

XWOptionalContentGroup *XWOptionalContent::getOCG(int idx) 
{
  return (XWOptionalContentGroup *)ocgs->get(idx);
}

XWOptionalContentGroup *XWOptionalContent::findOCG(ObjRef *ref) 
{
  XWOptionalContentGroup *ocg;
  int i;

  for (i = 0; i < ocgs->getLength(); ++i) 
  {
    ocg = (XWOptionalContentGroup *)ocgs->get(i);
    if (ocg->matches(ref)) 
    {
      return ocg;
    }
  }
  return NULL;
}

bool XWOptionalContent::evalOCObject(XWObject *obj, bool *visible)
{
	XWOptionalContentGroup *ocg;
  int policy;
  ObjRef ref;
  XWObject obj2, obj3, obj4, obj5;
  int i;
  
  if (obj->isNull()) 
  {
    return false;
  }
  if (obj->isRef()) 
  {
    ref = obj->getRef();
    if ((ocg = findOCG(&ref))) 
    {
      *visible = ocg->getState();
      return true;
    }
  }
  
  obj->fetch(xref, &obj2);
  if (obj2.isDict("OCMD")) 
  {
    if (obj2.dictLookup("VE", &obj3)->isArray()) 
    {
      *visible = evalOCVisibilityExpr(&obj3, 0);
      obj3.free();
    } 
    else 
    {
      obj3.free();
      policy = ocPolicyAnyOn;
      if (obj2.dictLookup("P", &obj3)->isName()) 
      {
				if (obj3.isName("AllOn")) 
				{
	  			policy = ocPolicyAllOn;
				} 
				else if (obj3.isName("AnyOn")) 
				{
	  			policy = ocPolicyAnyOn;
				} 
				else if (obj3.isName("AnyOff")) 
				{
	  			policy = ocPolicyAnyOff;
				} 
				else if (obj3.isName("AllOff")) 
				{
	  			policy = ocPolicyAllOff;
				}
      }
      obj3.free();
      obj2.dictLookupNF("OCGs", &obj3);
      ocg = NULL;
      if (obj3.isRef()) 
      {
				ref = obj3.getRef();
				ocg = findOCG(&ref);
      }
      if (ocg) 
      {
				*visible = (policy == ocPolicyAllOn || policy == ocPolicyAnyOn) ? ocg->getState() : !ocg->getState();
      } 
      else 
      {
				*visible = true;
				if (obj3.fetch(xref, &obj4)->isArray()) 
				{
	  			for (i = 0; i < obj4.arrayGetLength(); ++i) 
	  			{
	    			obj4.arrayGetNF(i, &obj5);
	    			if (obj5.isRef()) 
	    			{
	      			ref = obj5.getRef();
	      			if ((ocg = findOCG(&ref))) 
	      			{
								switch (policy) 
								{
									case ocPolicyAllOn:
		  							*visible = *visible && ocg->getState();
		  							break;
		  							
									case ocPolicyAnyOn:
		  							*visible = *visible || ocg->getState();
		  							break;
		  							
									case ocPolicyAnyOff:
		  							*visible = *visible || !ocg->getState();
		  							break;
		  							
									case ocPolicyAllOff:
		  							*visible = *visible && !ocg->getState();
		  							break;
								}
	      			}
	    			}
	    			obj5.free();
	  			}
				}
				obj4.free();
      }
      obj3.free();
    }
    obj2.free();
    return true;
  }
  obj2.free();
  return false;
}


bool XWOptionalContent::evalOCVisibilityExpr(XWObject *expr, int recursion) 
{
  XWOptionalContentGroup *ocg;
  XWObject expr2, op, obj;
  ObjRef ref;
  bool ret;
  int i;

  if (recursion > visibilityExprRecursionLimit) 
  {
    xwApp->error("Loop detected in optional content visibility expression");
    return true;
  }
  if (expr->isRef()) 
  {
    ref = expr->getRef();
    if ((ocg = findOCG(&ref))) 
    {
      return ocg->getState();
    }
  }
  expr->fetch(xref, &expr2);
  if (!expr2.isArray() || expr2.arrayGetLength() < 1) 
  {
    xwApp->error("Invalid optional content visibility expression");
    expr2.free();
    return true;
  }
  expr2.arrayGet(0, &op);
  if (op.isName("Not")) 
  {
    if (expr2.arrayGetLength() == 2) 
    {
      expr2.arrayGetNF(1, &obj);
      ret = !evalOCVisibilityExpr(&obj, recursion + 1);
      obj.free();
    } 
    else 
    {
      xwApp->error("Invalid optional content visibility expression");
      ret = true;
    }
  } 
  else if (op.isName("And")) 
  {
    ret = true;
    for (i = 1; i < expr2.arrayGetLength() && ret; ++i) 
   	{
      expr2.arrayGetNF(i, &obj);
      ret = evalOCVisibilityExpr(&obj, recursion + 1);
      obj.free();
    }
  } 
  else if (op.isName("Or")) 
  {
    ret = false;
    for (i = 1; i < expr2.arrayGetLength() && !ret; ++i) 
    {
      expr2.arrayGetNF(i, &obj);
      ret = evalOCVisibilityExpr(&obj, recursion + 1);
      obj.free();
    }
  } 
  else 
  {
    xwApp->error("Invalid optional content visibility expression");
    ret = true;
  }
  op.free();
  expr2.free();
  return ret;
}

XWOptionalContentGroup::XWOptionalContentGroup(ObjRef *refA, 
	                                             int *nameA, 
	                                             int nameLenA,
		       							                       OCUsageState viewStateA, 
		       							                       OCUsageState printStateA)
{
	ref = *refA;
  name = nameA;
  nameLen = nameLenA;
  viewState = viewStateA;
  printState = printStateA;
  state = true;
}

XWOptionalContentGroup * XWOptionalContentGroup::parse(ObjRef *refA, XWObject *obj)
{
	int *nameA;
  int nameLenA;
  XWObject obj1, obj2, obj3;
  XWString *s;
  OCUsageState viewStateA, printStateA;
  int i;
  
  if (!obj->isDict()) 
  {
    return NULL;
  }
  if (!obj->dictLookup("Name", &obj1)->isString()) 
  {
    xwApp->error("Missing or invalid Name in OCG");
    obj1.free();
    return NULL;
  }
  s = obj1.getString();
  if ((s->getChar(0) & 0xff) == 0xfe &&
      (s->getChar(1) & 0xff) == 0xff) 
  {
    nameLenA = (s->getLength() - 2) / 2;
    nameA = (int *)malloc(nameLenA * sizeof(int));
    for (i = 0; i < nameLenA; ++i) 
    {
      nameA[i] = ((s->getChar(2 + 2*i) & 0xff) << 8) | (s->getChar(3 + 2*i) & 0xff);
    }
  } 
  else 
  {
    nameLenA = s->getLength();
    nameA = (int *)malloc(nameLenA * sizeof(int));
    for (i = 0; i < nameLenA; ++i) 
    {
      nameA[i] = pdfDocEncoding[s->getChar(i) & 0xff];
    }
  }
  obj1.free();

  viewStateA = printStateA = ocUsageUnset;
  if (obj->dictLookup("Usage", &obj1)->isDict()) 
  {
    if (obj1.dictLookup("View", &obj2)->isDict()) 
    {
      if (obj2.dictLookup("ViewState", &obj3)->isName()) 
      {
				if (obj3.isName("ON")) 
				{
	  			viewStateA = ocUsageOn;
				} 
				else 
				{
	  			viewStateA = ocUsageOff;
				}
      }
      obj3.free();
    }
    obj2.free();
    if (obj1.dictLookup("Print", &obj2)->isDict()) 
    {
      if (obj2.dictLookup("PrintState", &obj3)->isName()) 
      {
				if (obj3.isName("ON")) 
				{
	  			printStateA = ocUsageOn;
				} 
				else 
				{
	  			printStateA = ocUsageOff;
				}
      }
      obj3.free();
    }
    obj2.free();
  }
  obj1.free();

  return new XWOptionalContentGroup(refA, nameA, nameLenA, viewStateA, printStateA);
}

XWOptionalContentGroup::~XWOptionalContentGroup() 
{
	if (name)
  	free(name);
}

bool XWOptionalContentGroup::matches(ObjRef *refA) 
{
  return refA->num == ref.num && refA->gen == ref.gen;
}


XWOCDisplayNode::XWOCDisplayNode() 
{
  name = NULL;
  nameLen = 0;
  ocg = NULL;
  children = NULL;
}

XWOCDisplayNode::XWOCDisplayNode(XWString *nameA) 
{
  int i;

  if ((nameA->getChar(0) & 0xff) == 0xfe &&
      (nameA->getChar(1) & 0xff) == 0xff) 
  {
    nameLen = (nameA->getLength() - 2) / 2;
    name = (int *)malloc(nameLen * sizeof(int));
    for (i = 0; i < nameLen; ++i) 
    {
      name[i] = ((nameA->getChar(2 + 2*i) & 0xff) << 8) | (nameA->getChar(3 + 2*i) & 0xff);
    }
  } 
  else 
  {
    nameLen = nameA->getLength();
    name = (int *)malloc(nameLen * sizeof(int));
    for (i = 0; i < nameLen; ++i) 
    {
      name[i] = pdfDocEncoding[nameA->getChar(i) & 0xff];
    }
  }
  ocg = NULL;
  children = NULL;
}

XWOCDisplayNode::XWOCDisplayNode(XWOptionalContentGroup *ocgA) 
{
  nameLen = ocgA->getNameLength();
  if (nameLen) 
  {
    name = (int *)malloc(nameLen * sizeof(int));
    memcpy(name, ocgA->getName(), nameLen * sizeof(int));
  } 
  else 
  {
    name = NULL;
  }
  ocg = ocgA;
  children = NULL;
}

XWOCDisplayNode::~XWOCDisplayNode() 
{
	if (name)
  	free(name);
  if (children) 
  {
    deleteXWList(children, XWOCDisplayNode);
  }
}

XWOCDisplayNode * XWOCDisplayNode::parse(XWObject *obj, XWOptionalContent *oc, XWRef *xref, int recursion)
{
	XWObject obj2, obj3;
  ObjRef ref;
  XWOptionalContentGroup *ocgA;
  XWOCDisplayNode *node, *child;
  int i;

  if (recursion > displayNodeRecursionLimit) 
  {
    xwApp->error("Loop detected in optional content order");
    return NULL;
  }
  if (obj->isRef()) 
  {
    ref = obj->getRef();
    if ((ocgA = oc->findOCG(&ref))) 
    {
      return new XWOCDisplayNode(ocgA);
    }
  }
  obj->fetch(xref, &obj2);
  if (!obj2.isArray()) 
  {
    obj2.free();
    return NULL;
  }
  i = 0;
  if (obj2.arrayGetLength() >= 1) 
  {
    if (obj2.arrayGet(0, &obj3)->isString()) 
    {
      node = new XWOCDisplayNode(obj3.getString());
      i = 1;
    } 
    else 
    {
      node = new XWOCDisplayNode();
    }
    obj3.free();
  } 
  else 
  {
    node = new XWOCDisplayNode();
  }
  for (; i < obj2.arrayGetLength(); ++i) 
  {
    obj2.arrayGetNF(i, &obj3);
    if ((child = XWOCDisplayNode::parse(&obj3, oc, xref, recursion + 1))) 
    {
      if (!child->ocg && !child->name && node->getNumChildren() > 0) 
      {
				node->getChild(node->getNumChildren() - 1)->addChildren(child->takeChildren());
				delete child;
      } 
      else 
      {
				node->addChild(child);
      }
    }
    obj3.free();
  }
  obj2.free();
  return node;
}


void XWOCDisplayNode::addChild(XWOCDisplayNode *child) 
{
  if (!children) 
  {
    children = new XWList();
  }
  children->append(child);
}

void XWOCDisplayNode::addChildren(XWList *childrenA) 
{
  if (!children) 
  {
    children = new XWList();
  }
  children->append(childrenA);
  delete childrenA;
}

XWList *XWOCDisplayNode::takeChildren() 
{
  XWList *childrenA;

  childrenA = children;
  children = NULL;
  return childrenA;
}

int XWOCDisplayNode::getNumChildren() 
{
  if (!children) {
    return 0;
  }
  return children->getLength();
}

XWOCDisplayNode *XWOCDisplayNode::getChild(int idx) 
{
  return (XWOCDisplayNode *)children->get(idx);
}
