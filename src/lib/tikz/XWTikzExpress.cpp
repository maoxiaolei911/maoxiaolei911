/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QHash>
#include <QStack>
#include "XWTikzGraphic.h"
#include "XWTikzState.h"
#include "XWTikzExpress.h"

#define XW_TIKZ_OP    0
#define XW_TIKZ_NUM   1
#define XW_TIKZ_DIM   2
#define XW_TIKZ_BOOL  3
#define XW_TIKZ_VAR   4
#define XW_TIKZ_CORD  5
#define XW_TIKZ_NAME  5

#define XW_TIKZ_ADD  0
#define XW_TIKZ_AND  1
#define XW_TIKZ_ABS  2
#define XW_TIKZ_ACOS  3
#define XW_TIKZ_ARRAY  4
#define XW_TIKZ_ASIN  5
#define XW_TIKZ_ATAN  6
#define XW_TIKZ_ATAN2  7

#define XW_TIKZ_BIN  8

#define XW_TIKZ_CEIL  9
#define XW_TIKZ_COND  10
#define XW_TIKZ_COS   11
#define XW_TIKZ_COSEC  12
#define XW_TIKZ_COSH  13
#define XW_TIKZ_COT  14

#define XW_TIKZ_DEG  15
#define XW_TIKZ_DIV  16
#define XW_TIKZ_DEPTH  17

#define XW_TIKZ_E  18
#define XW_TIKZ_EQU  19
#define XW_TIKZ_EXP  20


#define XW_TIKZ_FACT 21
#define XW_TIKZ_FALSE  22
#define XW_TIKZ_FLOOR  23
#define XW_TIKZ_FRAC  24

#define XW_TIKZ_GRE  25

#define XW_TIKZ_HEIGHT  26
#define XW_TIKZ_HEX  27

#define XW_TIKZ_INT  28
#define XW_TIKZ_IFTHENELSE  29

#define XW_TIKZ_LEFTBR  30
#define XW_TIKZ_LESS 31
#define XW_TIKZ_LN  32
#define XW_TIKZ_LOG10  33
#define XW_TIKZ_LOG2  34

#define XW_TIKZ_MAX  35
#define XW_TIKZ_MIN  36
#define XW_TIKZ_MOD  37
#define XW_TIKZ_MUL  38

#define XW_TIKZ_NEG  39
#define XW_TIKZ_NEQ  40
#define XW_TIKZ_NLESS 41
#define XW_TIKZ_NGRE  42
#define XW_TIKZ_NOT  43

#define XW_TIKZ_OCT  44
#define XW_TIKZ_OR   45

#define XW_TIKZ_PI  46
#define XW_TIKZ_POW  47

#define XW_TIKZ_RAD  48
#define XW_TIKZ_RAND  49
#define XW_TIKZ_RANDOM  50
#define XW_TIKZ_REAL  51
#define XW_TIKZ_RIGHTBR  52
#define XW_TIKZ_RND  53
#define XW_TIKZ_ROUND  54

#define XW_TIKZ_SEC  55
#define XW_TIKZ_SIN  56
#define XW_TIKZ_SINH  57
#define XW_TIKZ_SQRT  58
#define XW_TIKZ_SUB  59

#define XW_TIKZ_TAN  60
#define XW_TIKZ_TANH  61
#define XW_TIKZ_TRUE  62

#define XW_TIKZ_VECLEN  63

#define XW_TIKZ_WIDTH  64

#define XW_TIKZ_HV 65
#define XW_TIKZ_VH 66

#define XW_TIKZ_ROT 67


#define XW_TIKZ_STATE_INIT    0
#define XW_TIKZ_STATE_MID     1
#define XW_TIKZ_STATE_COND    2
#define XW_TIKZ_STATE_NOT     3
#define XW_TIKZ_STATE_COORD_1 4
#define XW_TIKZ_STATE_COORD_2 5
#define XW_TIKZ_STATE_COORD_3 6

XWTikzExpress::XWTikzExpress(XWTikzGraphic * graphicA,QObject * parent)
:QObject(parent),
graphic(graphicA),
rescan(false),
hasVar(false),
result(0.0)
{}

XWTikzExpress::XWTikzExpress(XWTikzGraphic * graphicA,const QString & str, QObject * parent)
:QObject(parent),
graphic(graphicA),
rescan(true),
hasVar(false),
result(0.0)
{
  text = str;
}

XWTikzExpress::XWTikzExpress(XWTikzExpress * other, QObject * parent)
:QObject(parent)
{
  graphic = other->graphic;
  rescan = other->rescan;
  hasVar = other->hasVar;
  result = other->result;
  postExpress = other->postExpress;
  text = other->text;
}

double XWTikzExpress::getResult(XWTikzState * state)
{
  if (rescan || postExpress.size() > 1)
    calculate(state);

  return result;
}

QStringList XWTikzExpress::getVarNames()
{
  return vars;
}

bool XWTikzExpress::isDim()
{
  if (postExpress.isEmpty()) 
    return false;

  QPointF p = postExpress[0];
  return ((int)(p.x()) == XW_TIKZ_DIM);
}

bool XWTikzExpress::isNum()
{
  if (postExpress.isEmpty()) 
    return false;

  QPointF p = postExpress[0];
  return ((int)(p.x()) == XW_TIKZ_NUM);
}

void XWTikzExpress::setText(const QString & str)
{
  rescan = true;
  hasVar = false;
  text = str;
}

void XWTikzExpress::setValue(double v)
{
  rescan = false;
  hasVar = false;
  result = v;
  QPointF p = postExpress[0];
  p.setY(v);
  postExpress[0] = p;
}

QString XWTikzExpress::getText()
{
  if (postExpress.size() > 1)
    return text;

  QPointF p = postExpress[0];
  if ((int)p.x() == XW_TIKZ_DIM)
  {
    double v = p.y();
    QString ret;
    if (v < 20)
      ret = QString("%1pt").arg(v);
    else if (v < 72)
    {
      v *= 10;
      v /= 72.27;
      ret = QString("%1mm").arg(v);
    }
    else
    {
      v /= 72.27;
      ret = QString("%1cm").arg(v);
    } 

    return ret;
  }

  return QString::number(result);
}

void XWTikzExpress::calculate(XWTikzState * state)
{
  if (rescan)
  {
    vars.clear();
    postExpress.clear();
    int pos = 0;
    int len = text.length();
    scan(text,len,pos);
  }

  QStack<QPointF> stack;
  QPointF oprd1, oprd2, oprd3;
  for (int i = 0; i < postExpress.size(); i++)
  {
    QPointF p = postExpress[i];
    if ((int)(p.x()) == XW_TIKZ_NUM || 
        (int)(p.x()) == XW_TIKZ_DIM || 
        (int)(p.x()) == XW_TIKZ_BOOL)
    {
      result = p.y();
      stack.push(p);
    }      
    else if((int)(p.x()) == XW_TIKZ_VAR)
    {
      oprd1.setX(XW_TIKZ_NUM);
      QString n = vars[(int)(p.y())];
      result = state->getValue(n);
      oprd1.setY(result);
      stack.push(oprd1);
    }
    else
    {
      int op = (int)(p.y());
      switch (op)
      {
        default:
          {
            result = p.x();
            oprd1.setX(XW_TIKZ_NUM);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_E:
          {
            result = 2.71828;
            oprd1.setX(XW_TIKZ_NUM);
            oprd1.setY(result);
          }
          break;

        case XW_TIKZ_ADD:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = oprd2.y() + oprd1.y();
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SUB:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = oprd2.y() - oprd1.y();
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_MUL:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = oprd2.y() * oprd1.y();
            oprd2.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_DIV:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = oprd2.y() / oprd1.y();
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_POW:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = pow(oprd2.y(), oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_EXP:
          {
            oprd1 = stack.pop();
            result = pow(2.71828, oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_FACT:
          {
            oprd1 = stack.pop();
            result = 1;
            int k = (int)(oprd1.y());
            for (; k > 0; k--)
              result *= k;
            oprd1.setY(result);
            stack.push(oprd1);
          }          
          break;

        case XW_TIKZ_RAD:
          {
            oprd1 = stack.pop();
            result = oprd1.y() * 3.1415926 / 180;
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COND:
          {
            i++;
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            oprd3 = stack.pop();
            result = (bool)(oprd3.y()) ? oprd2.y() : oprd1.y();
            oprd1.setY(result);
            stack.push(oprd1);   
          }
          break;

        case XW_TIKZ_EQU:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = (oprd2.y() == oprd1.y());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_GRE:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = (oprd2.y() > oprd1.y());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_LESS:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = (oprd2.y() < oprd1.y());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NEQ:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = (oprd2.y() != oprd1.y());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NLESS:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = (oprd2.y() >= oprd1.y());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NGRE:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = (oprd2.y() <= oprd1.y());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_AND:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = ((bool)(oprd2.y()) && (bool)(oprd1.y()));
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_OR:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = ((bool)(oprd2.y()) || (bool)(oprd2.y()));
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NOT:
          {
            oprd1 = stack.pop();
            result = !((bool)(oprd1.y()));
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ABS:
          {
            oprd1 = stack.pop();
            result = qAbs(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COSEC:
          {
            oprd1 = stack.pop();
            result = 1 / sin(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_FLOOR:
          {
            oprd1 = stack.pop();
            result = floor(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_LOG2:
          {
            oprd1 = stack.pop();
            result = (log10(oprd1.y())) / (log10((double)2));
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SIN:
          {
            oprd1 = stack.pop();
            result = sin(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ACOS:
          {
            oprd1 = stack.pop();
            result = acos(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COSH:
          {
            oprd1 = stack.pop();
            result = cosh(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SINH:
          {
            oprd1 = stack.pop();
            result = sinh(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COT:
          {
            oprd1 = stack.pop();
            result = 1 / tan(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_PI:
          {
            result = 3.1415926;
            oprd1.setX(XW_TIKZ_NUM);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SQRT:
          {
            oprd1 = stack.pop();
            result = sqrt(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_MOD:
          {
            oprd1 = stack.pop();
            double ipart = 0;
            result = modf(oprd1.y(),&ipart);
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_TAN:
          {
            oprd1 = stack.pop();
            result = tan(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ASIN:
          {
            oprd1 = stack.pop();
            result = asin(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_TANH:
          {
            oprd1 = stack.pop();
            result = tanh(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ATAN:
          {
            oprd1 = stack.pop();
            result = atan(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NEG:
          {
            oprd1 = stack.pop();
            result = -oprd1.y();
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ATAN2:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result = atan2(oprd2.x(),oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_CEIL:
          {
            oprd1 = stack.pop();
            result = ceil(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_LN:
          {
            oprd1 = stack.pop();
            result = log(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ROUND:
          {
            oprd1 = stack.pop();
            result = qRound(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COS:
          {
            oprd1 = stack.pop();
            result = cos(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_LOG10:
          {
            oprd1 = stack.pop();
            result = log10(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SEC:
          {
            oprd1 = stack.pop();
            result = 1 / cos(oprd1.y());
            oprd1.setY(result);
            stack.push(oprd1);
          }
          break;
      }
    }
  }
}

void XWTikzExpress::scan(const QString & str, int & len, int & pos)
{
  rescan = false;
  QList<QPointF> list;
  int state = XW_TIKZ_STATE_INIT;
  QPointF p;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos].isLetter())
    {      
      p.setX(XW_TIKZ_OP);
      int ch = str[pos].unicode();
      int i = pos;
      while (str[pos].isLetter() || str[pos].isDigit())
        pos++;
      QString key = str.mid(i, pos - i);
      switch (ch)
      {
        default:
hasaname:
          rescan = true; 
          break;

        case 'a':
          if (key == "abs")
            p.setY(XW_TIKZ_ABS);
          else if (key == "acos")
            p.setY(XW_TIKZ_ACOS);
          else if (key == "add")
            p.setY(XW_TIKZ_ADD);
          else if (key == "and")
            p.setY(XW_TIKZ_AND);
          else if (key == "array")
            p.setY(XW_TIKZ_ARRAY);
          else if (key == "asin")
            p.setY(XW_TIKZ_ASIN);
          else if (key == "atan")
            p.setY(XW_TIKZ_ATAN);
          else if (key == "atan2")
            p.setY(XW_TIKZ_ATAN2);
          else 
            goto hasaname;
          break;

        case 'b':
          if (key == "bin")
            p.setY(XW_TIKZ_BIN);
          else 
            goto hasaname;
          break;

         case 'c':
          if (key == "ceil")
            p.setY(XW_TIKZ_CEIL);
          else if (key == "cos")
            p.setY(XW_TIKZ_COS);
          else if (key == "cosec")
            p.setY(XW_TIKZ_COSEC);
          else if (key == "cosh")
            p.setY(XW_TIKZ_COSH);
          else if (key == "cot")
            p.setY(XW_TIKZ_COT);
          else if (key == "cm")
          {
            int i = list.size() - 1;
            if (i >= 0)
            {
              list[i].setX(XW_TIKZ_DIM);
              list[i].setY(list[i].y() * 72.27 / 2.54);
            }
          }
          else
            goto hasaname;
          break;

        case 'd':
          if (key == "deg")
            p.setY(XW_TIKZ_DEG);
          else if (key == "depth")
            p.setY(XW_TIKZ_DEPTH);
          else if (key == "div")
            p.setY(XW_TIKZ_DIV);
          else if (key == "divide")
            p.setY(XW_TIKZ_DIV);
          else
            goto hasaname;
          break;

        case 'e':
          if (key == "e")
            p.setY(XW_TIKZ_E);
          else if (key == "equal")
            p.setY(XW_TIKZ_EQU);
          else if (key == "exp")
            p.setY(XW_TIKZ_EXP);
          else
            goto hasaname;
          break;

        case 'f':
          if (key == "factorial")
            p.setY(XW_TIKZ_FACT);
          else if (key == "false")
          {
            p.setX(XW_TIKZ_BOOL);
            p.setY(0);
          }            
          else if (key == "floor")
            p.setY(XW_TIKZ_FLOOR);
          else if (key == "frac")
            p.setY(XW_TIKZ_FRAC);
          else
            goto hasaname;
          break;

        case 'g':
          if (key == "greater")
            p.setY(XW_TIKZ_GRE);
          else 
            goto hasaname;
          break;

        case 'h':
          if (key == "hex")
            p.setY(XW_TIKZ_HEX);
          else if (key == "height")
            p.setY(XW_TIKZ_HEIGHT);
          else 
            goto hasaname;
          break;

        case 'H':
          if (key == "Hex")
            p.setY(XW_TIKZ_HEX);
          else 
            goto hasaname;
          break;

        case 'i':
          if (key == "int")
            p.setY(XW_TIKZ_INT);
          else if (key == "ifthenelse")
            p.setY(XW_TIKZ_IFTHENELSE);
          else 
            goto hasaname;
          break;

        case 'l':
          if (key == "less")
            p.setY(XW_TIKZ_LESS);
          else if (key == "ln")
            p.setY(XW_TIKZ_LN);
          else if (key == "log10")
            p.setY(XW_TIKZ_LOG10);
          else if (key == "log2")
            p.setY(XW_TIKZ_LOG2);
          else
            goto hasaname;
          break;

        case 'm':
          if (key == "max")
            p.setY(XW_TIKZ_MAX);
          else if (key == "min")
            p.setY(XW_TIKZ_MIN);
          else if (key == "mod")
            p.setY(XW_TIKZ_MOD);
          else if (key == "multiply")
            p.setY(XW_TIKZ_MUL);
          else if (key == "mm")
          {
            int i = list.size() - 1;
            if (i >= 0)
            {
              list[i].setX(XW_TIKZ_DIM);
              list[i].setY(list[i].y() * 72.27 / 25.4);
            }              
          }
          else
            goto hasaname;
          break;

        case 'M':
          if (key == "Mod")
            p.setY(XW_TIKZ_MOD);
          else 
            goto hasaname;
          break;

        case 'n':
          if (key == "neg")
            p.setY(XW_TIKZ_NEG);
          else if (key == "not")
            p.setY(XW_TIKZ_NOT);
          else if (key == "notequal")
            p.setY(XW_TIKZ_NEQ);
          else if (key == "notgreater")
            p.setY(XW_TIKZ_NGRE);
          else if (key == "notless")
            p.setY(XW_TIKZ_NLESS);
          else
            goto hasaname;
          break;

        case 'o':
          if (key == "oct")
            p.setY(XW_TIKZ_OCT);
          else if (key == "or")
            p.setY(XW_TIKZ_OR);
          else 
            goto hasaname;
          break;

        case 'p':
          if (key == "pi")
            p.setY(XW_TIKZ_PI);
          else if (key == "pow")
            p.setY(XW_TIKZ_POW);
          else if (key == "pt")
          {
            int i = list.size() - 1;
            if (i >= 0)
              list[i].setX(XW_TIKZ_DIM);
          }            
          else
            goto hasaname;
          break;

        case 'r':
          if (key == "rad" || key == "r")
            p.setY(XW_TIKZ_RAD);
          else if (key == "rand")
            p.setY(XW_TIKZ_RAND);
          else if (key == "random")
            p.setY(XW_TIKZ_RANDOM);
          else if (key == "real")
            p.setY(XW_TIKZ_REAL);
          else if (key == "rnd")
            p.setY(XW_TIKZ_RND);
          else if (key == "round")
            p.setY(XW_TIKZ_ROUND);
          else
            goto hasaname;
          break;

        case 's':
          if (key == "sec")
            p.setY(XW_TIKZ_SEC);
          else if (key == "sin")
            p.setY(XW_TIKZ_SIN);
          else if (key == "sinh")
            p.setY(XW_TIKZ_SINH);
          else if (key == "sqrt")
            p.setY(XW_TIKZ_SQRT);
          else if (key == "subtract")
            p.setY(XW_TIKZ_SUB);
          else
            goto hasaname;
          break;

        case 't':
          if (key == "tan")
            p.setY(XW_TIKZ_TAN);
          else if (key == "tanh")
            p.setY(XW_TIKZ_TANH);
          else if (key == "true")
          {
            p.setX(XW_TIKZ_BOOL);
            p.setY(1);
          }            
          else 
            goto hasaname;
          break;

        case 'v':
          if (key == "veclen")
            p.setY(XW_TIKZ_VECLEN);
          else 
            goto hasaname;
          break;

        case 'w':
          if (key == "width")
            p.setY(XW_TIKZ_WIDTH);
          else 
            goto hasaname;
          break;
      }

      list << p; 
      state = XW_TIKZ_STATE_MID;

      if (str[pos] == QChar('!'))
      {
        pos++;
        p.setX(XW_TIKZ_OP);
        p.setY(XW_TIKZ_FACT);      
        list << p;        
      }
    }
    else if (str[pos] == QChar('\\'))
    {
      hasVar = true;
      int i = pos;
      pos++;
      while (str[pos].isLetter())
        pos++;

      p.setX(XW_TIKZ_VAR);
      QString var = str.mid(i, pos - i);
      p.setY(vars.size());
      vars << var;
      list << p;
      if (str[pos] == QChar('!'))
      {
        pos++;
        p.setX(XW_TIKZ_OP);
        p.setY(XW_TIKZ_FACT);      
        list << p;      
      }
      if (str[pos] == QChar('\\') || 
          str[pos] == QChar('(') ||
          str[pos] == QChar('[') ||
          str[pos] == QChar('{'))
      {
        p.setX(XW_TIKZ_OP);
        p.setY(XW_TIKZ_MUL);
        list << p; 
      }
    }
    else if (str[pos] == QChar('.') || str[pos].isDigit())
    {
      int i = pos;
      if (str[pos] == QChar('.'))
        pos++;
      while (str[pos].isDigit() || str[pos] == QChar('.'))
        pos++;
      QString tmp = str.mid(i, pos - i);
      result = tmp.toDouble();
      if (str[pos].isSpace())
        pos++;
      p.setX(XW_TIKZ_NUM);
      p.setY(result);
      list << p; 
      if (str[pos].isLetter() || 
          str[pos] == QChar('\\') || 
          str[pos] == QChar('(') ||
          str[pos] == QChar('[') ||
          str[pos] == QChar('{'))
      {
        p.setX(XW_TIKZ_OP);
        p.setY(XW_TIKZ_MUL);
        list << p; 
      }

      if (str[pos] == QChar('!'))
      {
        pos++;
        p.setX(XW_TIKZ_OP);
        p.setY(XW_TIKZ_FACT);      
        list << p;        
      }
        
      state = XW_TIKZ_STATE_MID;
    }
    else if (str[pos] == QChar('+') || str[pos] == QChar('-'))
    {
      if (state == XW_TIKZ_STATE_INIT)
      {
        if (str[pos + 1].isDigit() || str[pos] == QChar('.'))
        {
          int i = pos;
          pos++;
          while (str[pos].isDigit() || str[pos] == QChar('.'))
            pos++;
          QString tmp = str.mid(i, pos - i);
          result = tmp.toDouble();
          if (str[pos].isSpace())
            pos++;
          p.setX(XW_TIKZ_NUM);
          p.setY(result);
          list << p;

          if (str[pos].isLetter() || 
              str[pos] == QChar('\\') || 
              str[pos] == QChar('(') ||
              str[pos] == QChar('[') ||
              str[pos] == QChar('{'))
          {
            p.setX(XW_TIKZ_OP);
            p.setY(XW_TIKZ_MUL);
            list << p; 
          }

          if (str[pos] == QChar('!'))
          {
            pos++;
            p.setX(XW_TIKZ_OP);
            p.setY(XW_TIKZ_FACT);      
            list << p;        
          }
        }
        else
        {
          if (str[pos] == QChar('+'))
            result = 1;
          else
            result = -1;
          pos++;
          p.setX(XW_TIKZ_NUM);
          p.setY(result);
          list << p;          
          if (pos < (len - 1))
          {
            p.setX(XW_TIKZ_OP);
            p.setY(XW_TIKZ_MUL);
            list << p;
          }          
        }  
      }
      else
      {
        if (str[pos] == QChar('+'))
          result = XW_TIKZ_ADD;
        else
          result = XW_TIKZ_SUB;
        pos++;
        p.setX(XW_TIKZ_OP);
        p.setY(result);
        list << p;      
      }      
    }
    else
    {
      p.setX(XW_TIKZ_OP);
      int ch = str[pos].unicode();      
      switch (ch)
      {
        default:          
          state = XW_TIKZ_STATE_MID;
          pos++;
          break;

        case '/':
          state = XW_TIKZ_STATE_MID;
          p.setY(XW_TIKZ_DIV);
          pos++;
          list << p;
          break;

        case '^':
          state = XW_TIKZ_STATE_MID;
          p.setY(XW_TIKZ_POW);
          pos++;
          list << p;
          break;

        case '?':
        case ':':
          state = XW_TIKZ_STATE_MID;
          p.setY(XW_TIKZ_COND);
          pos++;
          list << p;
          break;

        case '=':
          state = XW_TIKZ_STATE_MID;
          p.setY(XW_TIKZ_EQU);
          pos += 2;
          list << p;
          break;

        case '>':
          state = XW_TIKZ_STATE_MID;
          if (str[pos + 1] == QChar('='))
          {
            pos++;
            p.setY(XW_TIKZ_NLESS);
          }
          else
            p.setY(XW_TIKZ_GRE);
          pos++;
          list << p;
          break;

        case '<':
          state = XW_TIKZ_STATE_MID;
          if (str[pos + 1] == QChar('='))
          {
            pos++;
            p.setY(XW_TIKZ_NGRE);
          }
          else
            p.setY(XW_TIKZ_LESS);
          pos++;
          list << p;
          break;

        case '!':
          state = XW_TIKZ_STATE_MID;
          if (str[pos + 1] == QChar('='))
          {
            pos++;
            p.setY(XW_TIKZ_NEQ);
          }
          else
            p.setY(XW_TIKZ_NOT);
          pos++;
          list << p;
          break;

        case '&':
          state = XW_TIKZ_STATE_MID;
          pos += 2;
          p.setY(XW_TIKZ_AND);
          list << p;
          break;

        case '|':
          state = XW_TIKZ_STATE_MID;
          pos += 2;
          p.setY(XW_TIKZ_OR);
          list << p;
          break;

        case '(':
        case '{':
        case '[':
          p.setY(XW_TIKZ_LEFTBR);
          state = XW_TIKZ_STATE_INIT;
          pos++;
          list << p;
          break;

        case ')':
        case '}':
        case ']':
          p.setY(XW_TIKZ_RIGHTBR);
          pos++;
          list << p; 
          if (str[pos].isSpace()) 
            pos++;

          if (str[pos].isLetter() || 
              str[pos].isDigit() || 
              str[pos] == QChar('\\') ||
              str[pos] == QChar('(') ||
              str[pos] == QChar('[') ||
              str[pos] == QChar('{'))
          {
            p.setX(XW_TIKZ_OP);
            p.setY(XW_TIKZ_MUL);
            list << p;  
          }
          break;
      }      
    }
  }

  if (list.size() <= 0)
    return ;

  if (list.size() == 1)
  {
    postExpress = list;
    if (list[0].x() == XW_TIKZ_NUM || 
        list[0].x() == XW_TIKZ_DIM ||
        list[0].x() == XW_TIKZ_BOOL)
    {
      result = list[0].y();
    }
    
    return ;
  }

  QHash<int,int> orders;
  orders[XW_TIKZ_COND] = 0;
  orders[XW_TIKZ_OR] = 1;
  orders[XW_TIKZ_AND] = 2;
  orders[XW_TIKZ_EQU] = 3;
  orders[XW_TIKZ_NEQ] = 3;
  orders[XW_TIKZ_GRE] = 4;
  orders[XW_TIKZ_NLESS] = 4;
  orders[XW_TIKZ_LESS] = 4;
  orders[XW_TIKZ_NGRE] = 4;
  orders[XW_TIKZ_ADD] = 5;
  orders[XW_TIKZ_SUB] = 5;
  orders[XW_TIKZ_DIV] = 6;
  orders[XW_TIKZ_MOD] = 6;
  orders[XW_TIKZ_MUL] = 6;
  orders[XW_TIKZ_NOT] = 7;

  orders[XW_TIKZ_NEG] = 8;

  orders[XW_TIKZ_ABS] = 8;
  orders[XW_TIKZ_ACOS] = 8;
  orders[XW_TIKZ_ARRAY] = 8;
  orders[XW_TIKZ_ASIN] = 8;
  orders[XW_TIKZ_ATAN] = 8;
  orders[XW_TIKZ_ATAN2] = 8;

  orders[XW_TIKZ_BIN] = 8;

  orders[XW_TIKZ_CEIL] = 8;
  orders[XW_TIKZ_COS] = 8;
  orders[XW_TIKZ_COSEC] = 8;
  orders[XW_TIKZ_COSH] = 8;
  orders[XW_TIKZ_COT] = 8;

  orders[XW_TIKZ_DEG] = 8;
  orders[XW_TIKZ_DEPTH] = 8;

  orders[XW_TIKZ_FACT] = 8;
  orders[XW_TIKZ_FALSE] = 8;
  orders[XW_TIKZ_FLOOR] = 8;
  orders[XW_TIKZ_FRAC] = 8;

  orders[XW_TIKZ_HEIGHT] = 8;
  orders[XW_TIKZ_HEX] = 8;

  orders[XW_TIKZ_LN] = 8;
  orders[XW_TIKZ_LOG10] = 8;
  orders[XW_TIKZ_LOG2] = 8;

  orders[XW_TIKZ_MAX] = 8;
  orders[XW_TIKZ_MIN] = 8;

  orders[XW_TIKZ_OCT] = 8;
  orders[XW_TIKZ_PI] = 8; 

  orders[XW_TIKZ_RAD] = 8;
  orders[XW_TIKZ_RAND] = 8;
  orders[XW_TIKZ_RANDOM] = 8;
  orders[XW_TIKZ_REAL] = 8;
  orders[XW_TIKZ_RND] = 8;
  orders[XW_TIKZ_ROUND] = 8;

  orders[XW_TIKZ_SEC] = 8;
  orders[XW_TIKZ_SIN] = 8;
  orders[XW_TIKZ_SINH] = 8;
  orders[XW_TIKZ_SQRT] = 8;

  orders[XW_TIKZ_TAN] = 8;
  orders[XW_TIKZ_TANH] = 8;
  orders[XW_TIKZ_TRUE] = 8;

  orders[XW_TIKZ_VECLEN] = 8;
  orders[XW_TIKZ_WIDTH] = 8;

  orders[XW_TIKZ_POW] = 9;
  orders[XW_TIKZ_EXP] = 9;

  QStack<QPointF> stack;
  for (int i = 0; i < list.size(); i++)
  {
    p = list[i];
    if ((int)(p.x()) == XW_TIKZ_NUM || 
        (int)(p.x()) == XW_TIKZ_VAR ||
        (int)(p.x()) == XW_TIKZ_DIM ||
        (int)(p.x()) == XW_TIKZ_BOOL)
      postExpress << p;
    else
    {
      int op = (int)(p.y());
      switch (op)
      {
        default:
          {
            while (!stack.isEmpty())
            {
              QPointF tmp = stack.top();
              int o = (int)(tmp.y());
              if (orders[o] < orders[op])
                break;

              tmp = stack.pop();
              postExpress << tmp;
            }

            stack.push(p);
          }
          
          break;

        case XW_TIKZ_LEFTBR:
          stack.push(p);
          break;

        case XW_TIKZ_RIGHTBR:
          {
            while (!stack.isEmpty())
            {
              p = stack.pop();              
              op = (int)(p.y());
              if (op == XW_TIKZ_LEFTBR)
                break;
              
              postExpress << p;
            }
          }
          break;
      }
    }
  }

  while (!stack.isEmpty())
  {
    p = stack.pop();
    postExpress << p;
  }
}

XWTikzCoordExpress::XWTikzCoordExpress(XWTikzGraphic * graphicA,QObject * parent)
:QObject(parent),
graphic(graphicA),
rescan(false),
hasVar(false)
{}

XWTikzCoordExpress::XWTikzCoordExpress(XWTikzGraphic * graphicA,const QString & str, QObject * parent)
:QObject(parent),
graphic(graphicA),
rescan(true),
hasVar(false)
{
  text = str;
}

XWTikzCoordExpress::XWTikzCoordExpress(XWTikzCoordExpress * other,QObject * parent)
:QObject(parent)
{
  graphic = other->graphic;
  rescan = other->rescan;
  hasVar = other->hasVar;
  result = other->result;
  postExpress = other->postExpress;
  text = other->text;
}

QVector3D XWTikzCoordExpress::getResult(XWTikzState * state)
{
  if (rescan || !postExpress.isEmpty())
    calculate(state);

  return result;
}

QString XWTikzCoordExpress::getText()
{
  return text;
}

void XWTikzCoordExpress::setText(const QString & str)
{
  rescan = true;
  hasVar = false;
  text = str;
}

void XWTikzCoordExpress::calculate(XWTikzState * state)
{
  if (rescan)
  {
    vars.clear();
    postExpress.clear();
    int pos = 0;
    int len = text.length();
    scan(text,len,pos);
  }

  QStack<QVector4D> stack;
  QVector4D oprd1, oprd2, oprd3;
  for (int i = 0; i < postExpress.size(); i++)
  {
    QVector4D p = postExpress[i];
    if ((int)(p.w()) == XW_TIKZ_NUM || 
       (int)(p.w()) == XW_TIKZ_DIM || 
       (int)(p.w()) == XW_TIKZ_BOOL || 
       (int)(p.w()) == XW_TIKZ_CORD)
    {
      result.setX(p.x());
      result.setY(p.y());
      result.setZ(p.z());
      stack.push(p);
    }      
    else if((int)(p.w()) == XW_TIKZ_VAR)
    {
      QString n = vars[(int)(p.x())];
      result.setX(state->getValue(n));
      oprd1.setX(result.x());
      stack.push(oprd1);
    }
    else if((int)(p.w()) == XW_TIKZ_NAME)
    {
      result = graphic->getPoint3D(text,state);
      stack.push(result);
    }
    else
    {
      int op = (int)(p.x());
      switch (op)
      {
        default:
          break;

        case XW_TIKZ_E:
          {
            result.setX(2.71828);
            oprd1.setX(XW_TIKZ_NUM);
            oprd1.setY(2.71828);
          }
          break;

        case XW_TIKZ_ADD:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() + oprd1.x());
            result.setY(oprd2.y() + oprd1.y());
            result.setZ(oprd2.z() + oprd1.z());
            oprd1.setX(result.x());
            oprd1.setY(result.y());
            oprd1.setZ(result.z());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SUB:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() - oprd1.x());
            result.setY(oprd2.y() - oprd1.y());
            result.setZ(oprd2.z() - oprd1.z());
            oprd1.setX(result.x());
            oprd1.setY(result.y());
            oprd1.setZ(result.z());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_MUL:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() * oprd1.x());
            result.setY(oprd2.x() * oprd1.y());
            result.setZ(oprd2.x() * oprd1.z());
            oprd1.setW(oprd2.w());
            oprd1.setX(result.x());
            oprd1.setY(result.y());
            oprd1.setZ(result.z());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_DIV:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() / oprd1.x());
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_POW:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(pow(oprd2.x(), oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_EXP:
          {
            oprd1 = stack.pop();
            result.setX(pow(2.71828, oprd1.y()));
            oprd1.setY(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_FACT:
          {
            oprd1 = stack.pop();
            int f = 1;
            int k = (int)(oprd1.x());
            for (; k > 0; k--)
              f *= k;
            result.setX(f);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }          
          break;

        case XW_TIKZ_RAD:
          {
            oprd1 = stack.pop();
            result.setX(oprd1.x() * 3.1415926 / 180);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COND:
          {
            i++;
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            oprd3 = stack.pop();
            result.setX((bool)(oprd3.x()) ? oprd2.x() : oprd1.x());
            oprd1.setX(result.x());
            stack.push(oprd1);   
          }
          break;

        case XW_TIKZ_EQU:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() == oprd1.x());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_GRE:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() > oprd1.x());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_LESS:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() < oprd1.x());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NEQ:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() != oprd1.x());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NLESS:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() >= oprd1.x());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NGRE:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x() <= oprd1.x());
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_AND:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(((bool)(oprd2.x()) && (bool)(oprd1.x())));
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_OR:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(((bool)(oprd2.x()) || (bool)(oprd1.x())));
            oprd1.setX(XW_TIKZ_BOOL);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NOT:
          {
            oprd1 = stack.pop();
            if ((int)(oprd1.w()) == XW_TIKZ_BOOL)
            {
              result.setX(!((bool)(oprd1.x())));
              oprd1.setW(XW_TIKZ_BOOL);
              oprd1.setX(result.x());
            }
            else
            {
              i++;
              oprd2 = stack.pop();
              oprd3 = stack.pop();
              
              switch ((int)(oprd2.w()))
              {
                case XW_TIKZ_NUM:
                  result.setX((1-oprd2.x())*oprd3.x()+oprd2.x()*oprd1.x());
                  result.setY((1-oprd2.x())*oprd3.y()+oprd2.x()*oprd1.y());
                  result.setY((1-oprd2.x())*oprd3.z()+oprd2.x()*oprd1.z());
                  break;

                case XW_TIKZ_DIM:
                  {
                    double x2 = (oprd1.x()-oprd3.x())*(oprd1.x()-oprd3.x());
                    double y2 = (oprd1.y()-oprd3.y())*(oprd1.y()-oprd3.y());
                    double z2 = (oprd1.z()-oprd3.z())*(oprd1.z()-oprd3.z());
                    double r = sqrt(x2 + y2 + z2);
                    result.setX(oprd2.x() * (oprd1.x() - oprd3.x()) / r + oprd3.x());
                    result.setY(oprd2.x() * (oprd1.y() - oprd3.y()) / r + oprd3.y());
                    result.setZ(oprd2.x() * (oprd1.z() - oprd3.z()) / r + oprd3.z());
                  }                  
                  break;

                default:
                  {
                    double x2 = (oprd1.x()-oprd3.x())*(oprd1.x()-oprd3.x());
                    double y2 = (oprd1.y()-oprd3.y())*(oprd1.y()-oprd3.y());
                    double z2 = (oprd1.z()-oprd3.z())*(oprd1.z()-oprd3.z());
                    double x = (oprd3.x()-oprd2.x())*(oprd1.x()-oprd3.x());
                    double y = (oprd3.y()-oprd2.y())*(oprd1.y()-oprd3.y());
                    double z = (oprd3.z()-oprd2.z())*(oprd1.z()-oprd3.z());
                    double k = (x + y + z) / (x2 + y2 + z2);
                    result.setX(oprd3.x() + k * (oprd1.x() - oprd3.x()));
                    result.setY(oprd3.y() + k * (oprd1.y() - oprd3.y()));
                    result.setZ(oprd3.z() + k * (oprd1.z() - oprd3.z()));
                  }
                  break;
              }

              oprd1.setX(result.x());
              oprd1.setY(result.y());
              oprd1.setZ(result.z());
            }
            
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ABS:
          {
            oprd1 = stack.pop();
            result.setX(qAbs(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COSEC:
          {
            oprd1 = stack.pop();
            result.setX(1 / sin(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_FLOOR:
          {
            oprd1 = stack.pop();
            result.setX(floor(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_LOG2:
          {
            oprd1 = stack.pop();
            result.setX((log10(oprd1.x()))/ (log10((double)2)));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SIN:
          {
            oprd1 = stack.pop();
            result.setX(sin(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ACOS:
          {
            oprd1 = stack.pop();
            result.setX(acos(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COSH:
          {
            oprd1 = stack.pop();
            result.setX(cosh(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SINH:
          {
            oprd1 = stack.pop();
            result.setX(sinh(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COT:
          {
            oprd1 = stack.pop();
            result.setX(1 / tan(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_PI:
          {
            result.setX(3.1415926);
            oprd1.setW(XW_TIKZ_NUM);
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SQRT:
          {
            oprd1 = stack.pop();
            result.setX(sqrt(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_MOD:
          {
            oprd1 = stack.pop();
            double ipart = 0;
            result.setX(modf(oprd1.x(),&ipart));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_TAN:
          {
            oprd1 = stack.pop();
            result.setX(tan(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ASIN:
          {
            oprd1 = stack.pop();
            result.setX(asin(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_TANH:
          {
            oprd1 = stack.pop();
            result.setX(tanh(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ATAN:
          {
            oprd1 = stack.pop();
            result.setX(atan(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_NEG:
          {
            oprd1 = stack.pop();
            result.setX(-oprd1.x());
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ATAN2:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(atan2(oprd2.x(),oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_CEIL:
          {
            oprd1 = stack.pop();
            result.setX(ceil(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_LN:
          {
            oprd1 = stack.pop();
            result.setX(log(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ROUND:
          {
            oprd1 = stack.pop();
            result.setX(qRound(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_COS:
          {
            oprd1 = stack.pop();
            result.setX(cos(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_LOG10:
          {
            oprd1 = stack.pop();
            result.setX(log10(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_SEC:
          {
            oprd1 = stack.pop();
            result.setX(1 / cos(oprd1.x()));
            oprd1.setX(result.x());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_ROT:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            oprd3 = stack.pop();
            QVector4D oprd4 = stack.top();
            result.setX((oprd1.x() - oprd4.x()) * cos(oprd2.x()) - (oprd1.y() - oprd4.y()) * sin(oprd2.x()) + oprd4.x());
            result.setY((oprd1.x() - oprd4.x()) * sin(oprd2.x()) + (oprd1.y() - oprd4.y()) * cos(oprd2.x()) + oprd4.y());
            oprd2.setW(XW_TIKZ_CORD);
            oprd2.setX(result.x());
            oprd2.setY(result.y());
            stack.push(oprd3);
            stack.push(oprd2);
          }
          break;

        case XW_TIKZ_HV:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd1.x());
            result.setY(oprd2.y());
            oprd1.setX(result.x());
            oprd1.setY(result.y());
            stack.push(oprd1);
          }
          break;

        case XW_TIKZ_VH:
          {
            oprd1 = stack.pop();
            oprd2 = stack.pop();
            result.setX(oprd2.x());
            result.setY(oprd1.y());
            oprd1.setX(result.x());
            oprd1.setY(result.y());
            stack.push(oprd1);
          }
          break;
      }
    }
  }
}

void XWTikzCoordExpress::scan(const QString & str, int & len, int & pos)
{
  rescan = false;
  QList<QVector4D> list;
  int state = XW_TIKZ_STATE_INIT;
  QVector4D p;
  QPointF np;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos].isLetter())
    {      
      p.setW(XW_TIKZ_OP);
      int ch = str[pos].unicode();
      int i = pos;
      while (str[pos].isLetter() || 
             str[pos].isDigit() || 
             str[pos].isSpace() || 
             str[pos] == QChar('.') || 
             str[pos] == QChar('-'))
        pos++;
      QString key = str.mid(i, pos - i);
      key = key.simplified();
      switch (ch)
      {
        default:
thisisname:     
          p.setW(XW_TIKZ_NAME);
          text = key;
          break;

        case 'a':
          if (key == "abs")
            p.setX(XW_TIKZ_ABS);
          else if (key == "acos")
            p.setX(XW_TIKZ_ACOS);
          else if (key == "add")
            p.setX(XW_TIKZ_ADD);
          else if (key == "and")
            p.setX(XW_TIKZ_AND);
          else if (key == "array")
            p.setX(XW_TIKZ_ARRAY);
          else if (key == "asin")
            p.setX(XW_TIKZ_ASIN);
          else if (key == "atan")
            p.setX(XW_TIKZ_ATAN);
          else if (key == "atan2")
            p.setX(XW_TIKZ_ATAN2);
          else 
            goto thisisname;
          break;

        case 'b':
          if (key == "bin")
            p.setX(XW_TIKZ_BIN);
          else 
            goto thisisname;
          break;

         case 'c':
          if (key == "ceil")
            p.setX(XW_TIKZ_CEIL);
          else if (key == "cos")
            p.setX(XW_TIKZ_COS);
          else if (key == "cosec")
            p.setX(XW_TIKZ_COSEC);
          else if (key == "cosh")
            p.setX(XW_TIKZ_COSH);
          else if (key == "cot")
            p.setX(XW_TIKZ_COT);
          else if (key == "cm")
          {
            int i = list.size() - 1;
            if (i >= 0)
            {
              list[i].setW(XW_TIKZ_DIM);
              list[i].setX(list[i].x() * 72.27 / 2.54);
            }              
          }
          else
            goto thisisname;
          break;

        case 'd':
          if (key == "deg")
            p.setX(XW_TIKZ_DEG);
          else if (key == "depth")
            p.setX(XW_TIKZ_DEPTH);
          else if (key == "div")
            p.setX(XW_TIKZ_DIV);
          else if (key == "divide")
            p.setX(XW_TIKZ_DIV);
          else
            goto thisisname;
          break;

        case 'e':
          if (key == "e")
            p.setX(XW_TIKZ_E);
          else if (key == "equal")
            p.setX(XW_TIKZ_EQU);
          else if (key == "exp")
            p.setY(XW_TIKZ_EXP);
          else
            goto thisisname;
          break;

        case 'f':
          if (key == "factorial")
            p.setX(XW_TIKZ_FACT);
          else if (key == "false")
            p.setX(XW_TIKZ_FALSE);
          else if (key == "floor")
            p.setX(XW_TIKZ_FLOOR);
          else if (key == "frac")
            p.setX(XW_TIKZ_FRAC);
          else
            goto thisisname;
          break;

        case 'g':
          if (key == "greater")
            p.setX(XW_TIKZ_GRE);
          else 
            goto thisisname;
          break;

        case 'h':
          if (key == "hex")
            p.setX(XW_TIKZ_HEX);
          else if (key == "height")
            p.setX(XW_TIKZ_HEIGHT);
          else 
            goto thisisname;
          break;

        case 'H':
          if (key == "Hex")
            p.setX(XW_TIKZ_HEX);
          else 
            goto thisisname;
          break;

        case 'i':
          if (key == "int")
            p.setX(XW_TIKZ_INT);
          else if (key == "ifthenelse")
            p.setX(XW_TIKZ_IFTHENELSE);
          else if (key == "intersection")
          {
            if (str[pos] == QChar('-') && str[pos + 1].isDigit())
            {
              pos++;
              while (str[pos].isDigit())
                pos++;
              key = str.mid(i, pos - i);
            }

            goto thisisname;
          }
          else
            goto thisisname;
          break;

        case 'l':
          if (key == "less")
            p.setX(XW_TIKZ_LESS);
          else if (key == "ln")
            p.setX(XW_TIKZ_LN);
          else if (key == "log10")
            p.setX(XW_TIKZ_LOG10);
          else if (key == "log2")
            p.setX(XW_TIKZ_LOG2);
          else
            goto thisisname;
          break;

        case 'm':
          if (key == "max")
            p.setX(XW_TIKZ_MAX);
          else if (key == "min")
            p.setX(XW_TIKZ_MIN);
          else if (key == "mod")
            p.setX(XW_TIKZ_MOD);
          else if (key == "multiply")
            p.setX(XW_TIKZ_MUL);
          else if (key == "mm")
          {
            int i = list.size() - 1;
            if (i >= 0)
            {
              list[i].setW(XW_TIKZ_DIM);
              list[i].setX(list[i].x() * 72.27 / 25.4);
            }              
          }
          else
            goto thisisname;
          break;

        case 'M':
          if (key == "Mod")
            p.setX(XW_TIKZ_MOD);
          else 
            goto thisisname;
          break;

        case 'n':
          if (key == "neg")
            p.setX(XW_TIKZ_NEG);
          else if (key == "not")
            p.setX(XW_TIKZ_NOT);
          else if (key == "notequal")
            p.setX(XW_TIKZ_NEQ);
          else if (key == "notgreater")
            p.setX(XW_TIKZ_NGRE);
          else if (key == "notless")
            p.setX(XW_TIKZ_NLESS);
          else
            goto thisisname;
          break;

        case 'o':
          if (key == "oct")
            p.setX(XW_TIKZ_OCT);
          else if (key == "or")
            p.setX(XW_TIKZ_OR);
          else 
            goto thisisname;
          break;

        case 'p':
          if (key == "pi")
            p.setX(XW_TIKZ_PI);
          else if (key == "pow")
            p.setX(XW_TIKZ_POW);
          else if (key == "pt")
          {
            int i = list.size() - 1;
            if (i >= 0)
              list[i].setX(XW_TIKZ_DIM);
          }
          else
            goto thisisname;
          break;

        case 'r':
          if (key == "rad" || key == "r")
            p.setX(XW_TIKZ_RAD);
          else if (key == "rand")
            p.setX(XW_TIKZ_RAND);
          else if (key == "random")
            p.setX(XW_TIKZ_RANDOM);
          else if (key == "real")
            p.setX(XW_TIKZ_REAL);
          else if (key == "rnd")
            p.setX(XW_TIKZ_RND);
          else if (key == "round")
            p.setX(XW_TIKZ_ROUND);
          else
            goto thisisname;
          break;

        case 's':
          if (key == "sec")
            p.setX(XW_TIKZ_SEC);
          else if (key == "sin")
            p.setX(XW_TIKZ_SIN);
          else if (key == "sinh")
            p.setX(XW_TIKZ_SINH);
          else if (key == "sqrt")
            p.setX(XW_TIKZ_SQRT);
          else if (key == "subtract")
            p.setX(XW_TIKZ_SUB);
          else
            goto thisisname;
          break;

        case 't':
          if (key == "tan")
            p.setX(XW_TIKZ_TAN);
          else if (key == "tanh")
            p.setX(XW_TIKZ_TANH);
          else if (key == "true")
            p.setX(XW_TIKZ_TRUE);
          else 
            goto thisisname;
          break;

        case 'v':
          if (key == "veclen")
            p.setX(XW_TIKZ_VECLEN);
          else 
            goto thisisname;
          break;

        case 'w':
          if (key == "width")
            p.setX(XW_TIKZ_WIDTH);
          else 
            goto thisisname;
          break;
      }

      list << p; 
    }
    else if (str[pos] == QChar('\\'))
    {
      hasVar = true;
      int i = pos;
      pos++;
      while (str[pos].isLetter())
        pos++;

      QString var = str.mid(i, pos - i);
      p.setW(XW_TIKZ_VAR);        
      p.setX(vars.size());
      vars << var;
      list << p; 
      if (str[pos] == QChar('(') ||
          str[pos] == QChar('[') ||
          str[pos] == QChar('{'))
      {
        p.setW(XW_TIKZ_OP);
        p.setX(XW_TIKZ_MUL);
        list << p;
      }
    }
    else if (str[pos] == QChar('.') || str[pos].isDigit())
    {
      int i = pos;
      while (str[pos].isDigit() || str[pos] == QChar('.'))
        pos++;
      QString tmp = str.mid(i, pos - i);
      result.setX(tmp.toDouble());
      if (state == XW_TIKZ_STATE_COORD_1)
      {
        p = list.takeLast();
        p.setW(XW_TIKZ_CORD);
        p.setY(result.x());
      }
      else if (state == XW_TIKZ_STATE_COORD_2)
      {
        p = list.takeLast();
        p.setZ(result.x());
      }
      else
      {
        p.setW(XW_TIKZ_NUM);
        p.setX(result.x()); 
      }

      list << p;
      if (str[pos].isSpace())
        pos++;

      if (str[pos].isLetter() || 
          str[pos] == QChar('\\') || 
          str[pos] == QChar('(') ||
          str[pos] == QChar('[') ||
          str[pos] == QChar('{'))
      {
        p.setW(XW_TIKZ_OP);
        p.setX(XW_TIKZ_MUL);
        list << p;
      }
    }
    else if (str[pos] == QChar('+') || str[pos] == QChar('-'))
    {
      if (state == XW_TIKZ_STATE_INIT || 
          state == XW_TIKZ_STATE_COORD_1 || 
          state == XW_TIKZ_STATE_COORD_2)
      {
        if (str[pos + 1].isDigit() || str[pos + 1] == QChar('.'))
        {
          int i = pos;
          pos++;
          while (str[pos].isDigit() || str[pos] == QChar('.'))
            pos++;
          QString tmp = str.mid(i, pos - i);
          result.setX(tmp.toDouble());
          if (str[pos].isSpace())
            pos++;
          if (state == XW_TIKZ_STATE_COORD_1)
          {
            p = list.takeLast();
            p.setW(XW_TIKZ_CORD);
            p.setY(result.x());
            list << p;
          }
          else if (state == XW_TIKZ_STATE_COORD_2)
          {
            p = list.takeLast();
            p.setZ(result.x());
            list << p;
          }
          else
          {
            p.setW(XW_TIKZ_NUM);
            p.setX(result.x()); 
            list << p;
            if (str[pos].isLetter() || 
                str[pos] == QChar('\\') || 
                str[pos] == QChar('(') ||
                str[pos] == QChar('[') ||
                str[pos] == QChar('{'))
            {
              p.setW(XW_TIKZ_OP);
              p.setX(XW_TIKZ_MUL);
              list << p;
            }
          }
        }
        else
        {
          if (str[pos] == QChar('+'))
            result.setX(1);
          else
            result.setX(-1);
          pos++;
          p.setW(XW_TIKZ_NUM);
          p.setX(result.x());
          list << p;          
          if (pos < (len - 1))
          {
            p.setW(XW_TIKZ_OP);
            p.setX(XW_TIKZ_MUL);
            list << p;
          }          
        }  
      }
      else
      {
        p.setW(XW_TIKZ_OP);
        if (str[pos] == QChar('+'))
          result.setX(XW_TIKZ_ADD);
        else
        {
          if (str[pos + 1] == QChar('|'))
          {
            pos++;
            result.setX(XW_TIKZ_HV);
          }
          else
            result.setX(XW_TIKZ_SUB);
        }          
        pos++;        
        p.setX(result.x());
        list << p;      
      }      
    }
    else
    {
      p.setW(XW_TIKZ_OP);
      int ch = str[pos].unicode();      
      switch (ch)
      {
        default:
          state = XW_TIKZ_STATE_MID;
          pos++;
          break;

        case ',':
          if (state == XW_TIKZ_STATE_COORD_1)
            state = XW_TIKZ_STATE_COORD_2;
          else if (state == XW_TIKZ_STATE_COORD_2)
            state = XW_TIKZ_STATE_COORD_3;
          else
            state = XW_TIKZ_STATE_COORD_1;
          pos++;
          break;

        case '/':
          state = XW_TIKZ_STATE_MID;
          p.setX(XW_TIKZ_DIV);
          pos++;
          list << p;
          break;

        case '^':
          state = XW_TIKZ_STATE_MID;
          p.setX(XW_TIKZ_POW);
          pos++;
          list << p;
          break;

        case '?':        
          p.setX(XW_TIKZ_COND);
          state = XW_TIKZ_STATE_COND;
          pos++;
          list << p;
          break;

        case ':':
          if (state == XW_TIKZ_STATE_COND)
          {
            p.setX(XW_TIKZ_COND);
            state = XW_TIKZ_STATE_COND;
            list << p;
          }            
          else if (state == XW_TIKZ_STATE_NOT)
          {
            state = XW_TIKZ_STATE_MID;
            p.setX(XW_TIKZ_ROT);
            list << p;
          }
          else if (state == XW_TIKZ_STATE_COORD_1)
            state = XW_TIKZ_STATE_COORD_2;
          else if (state == XW_TIKZ_STATE_COORD_2)
            state = XW_TIKZ_STATE_COORD_3;
          else
            state = XW_TIKZ_STATE_COORD_1;    
          pos++; 
          break;

        case '=':
          state = XW_TIKZ_STATE_MID;
          pos += 2;
          p.setX(XW_TIKZ_EQU);
          list << p;
          break;

        case '>':
          state = XW_TIKZ_STATE_MID;
          if (str[pos + 1] == QChar('='))
          {
            pos++;
            p.setX(XW_TIKZ_NLESS);
          }
          else
            p.setX(XW_TIKZ_GRE);
          pos++;
          list << p;
          break;

        case '<':
          state = XW_TIKZ_STATE_MID;
          if (str[pos + 1] == QChar('='))
          {
            pos++;
            p.setX(XW_TIKZ_NGRE);
          }
          else
            p.setX(XW_TIKZ_LESS);
          pos++;
          list << p;
          break;

        case '!':
          if (str[pos + 1] == QChar('='))
          {
            state = XW_TIKZ_STATE_MID;
            p.setX(XW_TIKZ_NEQ);
            pos++;
          }
          else
          {
            p.setX(XW_TIKZ_NOT);
            state = XW_TIKZ_STATE_NOT;
          }
          pos++;
          list << p;
          break;

        case '&':
          state = XW_TIKZ_STATE_MID;
          p.setX(XW_TIKZ_AND);
          pos += 2;
          list << p;
          break;

        case '|':
          state = XW_TIKZ_STATE_MID;
          if (str[pos + 1] == QChar('-'))
            p.setX(XW_TIKZ_VH);
          else
            p.setX(XW_TIKZ_OR);
          pos += 2;
          list << p;
          break;

        case '(':
        case '{':
        case '[':
          p.setX(XW_TIKZ_LEFTBR);
          state = XW_TIKZ_STATE_INIT;
          pos++;
          list << p;
          break;

        case ')':
        case '}':
        case ']':
          state = XW_TIKZ_STATE_MID;
          p.setX(XW_TIKZ_RIGHTBR);
          pos++;
          list << p;
          if (str[pos].isSpace()) 
            pos++;
          if (str[pos].isLetter() || 
              str[pos].isDigit() || 
              str[pos] == QChar('\\') ||
              str[pos] == QChar('(') ||
              str[pos] == QChar('[') ||
              str[pos] == QChar('{'))
          {
            p.setW(XW_TIKZ_OP);
            p.setX(XW_TIKZ_MUL);
            list << p;  
          }
          break;
      }
    }
  }

  if (list.size() <= 2)
  {
    postExpress = list;
    return ;
  }

  QHash<int,int> orders;
  orders[XW_TIKZ_COND] = 0;
  orders[XW_TIKZ_OR] = 1;
  orders[XW_TIKZ_AND] = 2;
  orders[XW_TIKZ_EQU] = 3;
  orders[XW_TIKZ_NEQ] = 3;
  orders[XW_TIKZ_GRE] = 4;
  orders[XW_TIKZ_NLESS] = 4;
  orders[XW_TIKZ_LESS] = 4;
  orders[XW_TIKZ_NGRE] = 4;
  orders[XW_TIKZ_ADD] = 5;
  orders[XW_TIKZ_SUB] = 5;
  orders[XW_TIKZ_DIV] = 6;
  orders[XW_TIKZ_MOD] = 6;
  orders[XW_TIKZ_MUL] = 6;
  orders[XW_TIKZ_NOT] = 7;

  orders[XW_TIKZ_NEG] = 8;

  orders[XW_TIKZ_ABS] = 8;
  orders[XW_TIKZ_ACOS] = 8;
  orders[XW_TIKZ_ARRAY] = 8;
  orders[XW_TIKZ_ASIN] = 8;
  orders[XW_TIKZ_ATAN] = 8;
  orders[XW_TIKZ_ATAN2] = 8;

  orders[XW_TIKZ_BIN] = 8;

  orders[XW_TIKZ_CEIL] = 8;
  orders[XW_TIKZ_COS] = 8;
  orders[XW_TIKZ_COSEC] = 8;
  orders[XW_TIKZ_COSH] = 8;
  orders[XW_TIKZ_COT] = 8;

  orders[XW_TIKZ_DEG] = 8;
  orders[XW_TIKZ_DEPTH] = 8;

  orders[XW_TIKZ_FACT] = 8;
  orders[XW_TIKZ_FALSE] = 8;
  orders[XW_TIKZ_FLOOR] = 8;
  orders[XW_TIKZ_FRAC] = 8;

  orders[XW_TIKZ_HEIGHT] = 8;
  orders[XW_TIKZ_HEX] = 8;

  orders[XW_TIKZ_LN] = 8;
  orders[XW_TIKZ_LOG10] = 8;
  orders[XW_TIKZ_LOG2] = 8;

  orders[XW_TIKZ_MAX] = 8;
  orders[XW_TIKZ_MIN] = 8;

  orders[XW_TIKZ_OCT] = 8;
  orders[XW_TIKZ_PI] = 8; 

  orders[XW_TIKZ_RAD] = 8;
  orders[XW_TIKZ_RAND] = 8;
  orders[XW_TIKZ_RANDOM] = 8;
  orders[XW_TIKZ_REAL] = 8;
  orders[XW_TIKZ_RND] = 8;
  orders[XW_TIKZ_ROUND] = 8;

  orders[XW_TIKZ_SEC] = 8;
  orders[XW_TIKZ_SIN] = 8;
  orders[XW_TIKZ_SINH] = 8;
  orders[XW_TIKZ_SQRT] = 8;

  orders[XW_TIKZ_TAN] = 8;
  orders[XW_TIKZ_TANH] = 8;
  orders[XW_TIKZ_TRUE] = 8;

  orders[XW_TIKZ_VECLEN] = 8;
  orders[XW_TIKZ_WIDTH] = 8;

  orders[XW_TIKZ_ROT] = 8;

  orders[XW_TIKZ_HV] = 5;
  orders[XW_TIKZ_VH] = 5;

  orders[XW_TIKZ_POW] = 9;

  QStack<QVector4D> stack;
  for (int i = 0; i < list.size(); i++)
  {
    p = list[i];
    if ((int)(p.w()) == XW_TIKZ_NUM || 
        (int)(p.w()) == XW_TIKZ_VAR || 
        (int)(p.w()) == XW_TIKZ_DIM ||
        (int)(p.w()) == XW_TIKZ_BOOL ||
        (int)(p.w()) == XW_TIKZ_CORD)
      postExpress << p;
    else
    {
      int op = (int)(p.x());
      switch (op)
      {
        default:
          {
            while (!stack.isEmpty())
            {
              QVector4D tmp = stack.top();
              int o = (int)(tmp.x());
              if (orders[o] < orders[op])
                break;

              tmp = stack.pop();
              postExpress << tmp;
            }

            stack.push(p);
          }
          
          break;

        case XW_TIKZ_LEFTBR:
          stack.push(p);
          break;

        case XW_TIKZ_RIGHTBR:
          {
            while (!stack.isEmpty())
            {
              p = stack.pop();              
              op = (int)(p.x());
              if (op == XW_TIKZ_LEFTBR)
                break;
              
              postExpress << p;
            }
          }
          break;
      }
    }
  }

  while (!stack.isEmpty())
  {
    p = stack.pop();
    postExpress << p;
  }
}