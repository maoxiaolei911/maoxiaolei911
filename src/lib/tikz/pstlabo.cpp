/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "setrandim.h"
#include "pstlabo.h"

QString Billes(int c)
{
  QString s;
  int k = 22;
  for (int i = 0; i < c; i++)
  {
    double x = setrandim(-1.5,1.5);
    double y = setrandim(0,3);
    double z = setrandim(0.01,0.1);
    if (y > 2)
      z = z * 3;
    else
    {
      if (y > 1)
        z = z * 2;
    }
    QString tmp = QString("%1/%2/%3").arg(x).arg(y).arg(z);
    k += tmp.length();
    s.append(tmp);
    if (i < (c - 1))
      s.append(QChar(','));
    if (k > 45)
    {
      s.append("\n  ");
      k = 0;
    }
  }

  QString b = QString("\\begin{scope}[line width=0.08,shading=ball,ball color=blue!10]\n"
                      "\\foreach \\x/\\y/\\z in {%1}\n"
                      "  \\shade (\\x,\\y) circle[radius=\\z];\n"
                      "\\end{scope}\n" ).arg(s);

  return b;
}

QString bulles(int c)
{
  QString s;
  int k = 22;
  for (int i = 0; i < c; i++)
  {
    double x = setrandim(-0.1,0.1);
    double y = setrandim(0.15,3);
    double z = y;
    if (y < 1)
      z = z * 3;
    else
    {
      if (y < 2)
        z = z * 2;
    }
    QString tmp = QString("%1/%2/%3").arg(x).arg(y).arg(z);
    k += tmp.length();
    s.append(tmp);
    if (i < (c - 1))
      s.append(QChar(','));
    if (k > 45)
    {
      s.append("\n  ");
      k = 0;
    }
  }

  QString b = QString("\\begin{scope}[fill=white]\n"
                      "\\foreach \\x/\\y/\\z in {%1}\n"
                      "  \\filldraw (\\x pt, \\y pt * 1.5) circle[radius=\\z pt];\n"
                      "\\end{scope}\n").arg(s);
  return b;
}

QString BULLES(int c)
{
  QString s;
  int k = 22;
  for (int i = 0; i < c; i++)
  {
    double x = setrandim(-1.5,1.5);
    double y = setrandim(0,3);
    double z = setrandim(0.01,0.1);
    if (y > 2)
      z = z * 3;
    else
    {
      if (y > 1)
        z = z * 2;
    }
    QString tmp = QString("%1/%2/%3").arg(x).arg(y).arg(z);
    k += tmp.length();
    s.append(tmp);
    if (i < (c - 1))
      s.append(QChar(','));
    if (k > 45)
    {
      s.append("\n  ");
      k = 0;
    }
  }

  QString b = QString("\\begin{scope}[fill=white,line width=0.08]\n"
                      "\\foreach \\x/\\y/\\z in {%1}\n"
                      "  \\fill (\\x,\\y) circle [radius=\\z];\n"
                      "\\end{scope}\n").arg(s);
  return b;
}

QString bullesChampagne(int c)
{
  QString s;
  int k = 20;
  for (int i = 0; i < c; i++)
  {
    double x = setrandim(-1,1);
    double y = setrandim(0,2);
    QString tmp = QString("%1/%2").arg(x).arg(y);
    k += tmp.length();
    s.append(tmp);
    if (i < (c - 1))
      s.append(QChar(','));
    if (k > 45)
    {
      s.append("\n  ");
      k = 0;
    }
  }

  QString b = QString("\\begin{scope}\n"
                      "\\foreach \\x/\\y in {%1}\n"
                      "  \\draw (\\x pt,\\y pt * 2) circle [radius=\\y];\n"
                      "\\end{scope}\n").arg(s);

  return b;
}

QString clouFer(int c)
{
  QString s;
  int k = 22;
  for (int i = 0; i < c; i++)
  {
    double x = setrandim(-1,1);
    double y = setrandim(-0.2,0.4);
    double z = setrandim(-90,90);
    QString tmp = QString("%1/%2/%3").arg(x).arg(y).arg(z);
    k += tmp.length();
    s.append(tmp);
    if (i < (c - 1))
      s.append(QChar(','));
    if (k > 45)
    {
      s.append("\n  ");
      k = 0;
    }
  }

  QString fer = QString("\\begin{scope}[scale=0.5,draw=blue]\n"
                        "\\foreach \\x/\\y/\\z in {%1}\n"
                        "  {\\draw[rorate=\\z,xshift=\\x,yshift=\\y,line width=0.1]\n"
                        "     (0,0.5) arc [radius=0.4,start angle=10,end angle=260];\n"
                        "   \\draw[rorate=\\z,xshift=\\x,yshift=\\y] (-0.28,0.2) --(0.5,1);}\n"
                        "\\end{scope}\n").arg(s);

 return fer;
}

QString filaments(int c)
{
  QString f = QString("\\begin{scope}[draw=red,smooth]\n");
  for (int i = 0; i < c; i++)
  {
    double x = setrandim(0,0.05);
    f += QString("\\draw[line width=%1 pt] plot coordinates {").arg(x);

    double m = pstMyRand() - 1;
    double n = pstMyRand() * 2 - 1;
    f += QString("(%1,%2) ").arg(m).arg(n);
    m = pstMyRand() + 0.2;
    n = pstMyRand() + 2;
    f += QString("(%1,%2) ").arg(m).arg(n);
    m = pstMyRand();
    n = pstMyRand() - 1;
    f += QString("(%1,%2)").arg(m).arg(n);
    m = pstMyRand() + 0.5;
    n = pstMyRand() + 4;
    f += QString("(%1,%2)").arg(m).arg(n);
    f += "};\n";
  }

  f += "\\end{scope}\n";

  return f;
}

QString grenailleZinc(int c)
{
  QString s;
  int k = 22;
  for (int i = 0; i < c; i++)
  {
    double x = setrandim(-1,1);
    double y = setrandim(-0.2,0.3);
    double z = setrandim(-180,180);
    QString tmp = QString("%1/%2/%3").arg(x).arg(y).arg(z);
    k += tmp.length();
    s.append(tmp);
    if (i < (c - 1))
      s.append(QChar(','));
    if (k > 45)
    {
      s.append("\n  ");
      k = 0;
    }
  }

  QString zinc = QString("\\begin{scope}[scale=0.5,fill=black!20]\n"
                         "\\foreach \\x/\\y/\\z in {%1}\n"
                         "  \\filldraw[rorate=\\z,xshift=\\x,yshift=\\y]\n"
                         "      plot coordinates {(0,0) (-0.25,0.25) (0,.5) (0.25,0.75) (0.5,0.5) (0.15,0.4) (0.,0)};\n"
                         "\\end{scope}\n").arg(s);

  return zinc;
}

QString tournureCuivre(int c)
{
  QString s;
  int k = 22;
  for (int i = 0; i < c; i++)
  {
    double x = setrandim(-1,1);
    double y = setrandim(-0.2,0.3);
    double z = setrandim(-180,180);
    QString tmp = QString("%1/%2/%3").arg(x).arg(y).arg(z);
    k += tmp.length();
    s.append(tmp);
    if (i < (c - 1))
      s.append(QChar(','));
    if (k > 45)
    {
      s.append("\n  ");
      k = 0;
    }
  }

  QString cuivre = QString("\\begin{scope}[scale=0.5,draw=brown,line width=0.1]\n"
                           "\\foreach \\x/\\y/\\z in {%1}\n"
                           "  \\draw[rorate=\\z,xshift=\\x,yshift=\\y]\n"
                           "      (0.25,0.25)  controls (-0,0) and (-0.25,0.25) (0.25,0.75)\n"
                           "      ..controls (0,0.5) and (-0.25,0.75) (0.25,1.25)\n"
                           "      ..controls (0,1) and (-0.25,1.25) (0.25,1.75);\n"
                           "\\end{scope}\n").arg(s);

  return cuivre;
}
