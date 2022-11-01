/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSTLABO_H
#define XWPSTLABO_H

#include <QString>

#define XW_TIKZ_TUBEESSAIS_CORPS "\\begin{scope}[xshift=0,yshift=0,line width=0.053]\n\
\\begin{scope}\n\
\\draw[clip] (-0.5,3.5) --(-0.5,0.5)\n\
  (0,0.5) arc[radius=0.5,start angle=180,end angle=0]\n\
  (0.5,0.5) --(0.5,3.5);\n\
\\fill[fill=green!50!blue] (-2,-2) rectangle (4,1.5);\n\
\\end{scope}\n\
\\draw (0,3.5) ellipse[x radius=0.5,y radius=0.1];\n\
\\end{scope}\n"

#define XW_TIKZ_BALLON "\\begin{scope}[xshift=0,yshift=0,line width=0.053]\n\
\\begin{scope}\n\
\\draw[clip] (-0.7,3.4) arc[radius=0.2,start angle=0,end angle=90]\n\
        (-0.5,3.4) --(-0.5,2.4)\n\
        (0,1.25) arc[radius=1.25,start angle=113,end angle=67]\n\
        (0.5,2.4) --(0.5,3.4)\n\
        (0.7,3.4) arc[radius=0.2,start angle=90,end angle=180];\n\
\\fill[fill=green!50!blue] (-2,-2) rectangle (4,1.5);\n\
\\end{scope}\n\
\\draw (0,3.6) ellipse[x radius=0.7,y radius=0.1];\n\
\\end{scope}\n"

#define XW_TIKZ_ERLEN_CORPS "\\begin{scope}[xshift=0,yshift=0,line width=0.053]\n\
\\begin{scope}\n\
\\draw[clip,rounded corners=0.3] (-0.5,3.5) --(-0.5,2.5) --(-1.5,0) --(1.5,0) --(0.5,2.5) --(0.5,3.5);\n\
\\fill[fill=green!50!blue] (-2,-2) rectangle (4,1.5);\n\
\\end{scope}\n\
\\draw (0,3.5) ellipse[x radius=0.6,y radius=0.1];\n\
\\end{scope}\n"

#define XW_TIKZ_BECHER_CORPS "\\begin{scope}[xshift=0,yshift=0,line width=0.053]\n\
\\begin{scope}\n\
\\draw[clip,rounded corners=0.5] (-1,2.5) --(-1,0) --(1,0) --(1,2.5);\n\
\\fill[fill=green!50!blue] (-2,-2) rectangle (4,1.5);\n\
\\end{scope}\n\
\\draw (0,2.55) ellipse [x radius=1.1, y radius=0.1];\n\
\\foreach \\x in {0.5, 1, 1.5, 2}\n\
    \\draw[line width=0.03] (-0.75,\\x) --(-0.4,\\x);\n\
\\foreach \\y in {0.5, 0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9}\n\
    \\draw[line width=0.02] (-0.75,\\y) --(-0.5,\\y);\n\
\\end{scope}\n"

#define XW_TIKZ_FLACON_CORPS "\\begin{scope}[xshift=0,yshift=0,line width=0.053]\n\
\\begin{scope}\n\
\\draw[clip] (-0.6,3.4) arc[radius=0.1,start angle=0,end angle=90]\n\
  (-0.5,3.4) --(-0.5,3.1)\n\
  (-0.6,3.1) arc[radius=0.1,start angle=-90,end angle=0]\n\
  (-0.6,2.6) arc[radius=0.4,start angle=90,end angle=180]\n\
  (-1,2.6) --(-1,0.2)\n\
  (-0.8,0.2) arc[radius=0.2,start angle=180,end angle=270]\n\
  (-0.8,0) --(0.8,0)\n\
  (0.8,0.2) arc[radius=0.2,start angle=-90,end angle=0]\n\
  (1,0.2) --(1,2.6)\n\
  (0.6,2.6) arc[radius=0.4,start angle=0,end angle=90]\n\
  (0.6,3.1) arc[radius=0.1,start angle=180,end angle=-90]\n\
  (0.5,3.1) --(0.5,3.4)\n\
  (0.6,3.4) arc[radius=0.1,start angle=90,end angle=180];\n\
\\fill[fill=green!50!blue] (-2,-2) rectangle (4,1.5);\n\
\\end{scope}\n\
\\draw (0,3.5) ellipse[x radius=0.6,y radius=0.1];\n\
\\end{scope}\n"

#define XW_TIKZ_FIOLEJAUGE "\\begin{scope}[xshift=0,yshift=0]\n\
\\begin{scope}\n\
\\draw[clip,rounded corners=0.1,line width=0.053] (-0.21,3.5) --(-0.1,3.5) --(-0.1,1.5) --(-0.75,0) --(0.75,0) --(0.1,1.5) --(0.1,3.5) --(0.21,3.5);\n\
\\fill[fill=green!50!blue] (-2,-2) rectangle (4,1.5);\n\
\\end{scope}\n\
\\draw[line width=0.02] (-0.1,2.5) --(0.1,2.5);\n\
\\end{scope}\n"

#define XW_TIKZ_ENTONNOIR_CORPS(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.053]\n\
\\begin{scope}\n\
\\draw[clip,rounded corners=0.25] (-0.07,0) --(-0.07,1) --(-1,2.5) (1,2.5) --(0.07,1) --(0.07,0.1);\n\
\\end{scope}\n\
\\draw (0,2.5) ellipse[x radius=1,y radius=0.1];\n\
\\end{scope}\n"

#define XW_TIKZ_EPROUVETTE_CORPS(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.053]\n\
\\begin{scope}\n\
\\draw[clip] (-0.5,5) --(-0.5,0) --(0.5,0) --(0.5,4.9) --(0.6,4.93);\n\
\\fill[fill=green!20!blue] (-1,-1) rectangle (1,1.5);\n\
\\end{scope}\n\
\\filldraw (-0.95,0.5) arc[radius=0.45, start angle=-90,end angle=0]\n\
       (-0.95,0.025) arc[radius=0.025, start angle=90,end angle=270]\n\
       (-0.95,0) --(0.95,0)\n\
       (0.95,0.025) arc[radius=0.025, start angle=-90,end angle=90]\n\
       (0.95,0.5) arc[radius=0.45, start angle=-90,end angle=180]\n\
       --plot coordinates[smooth] {(0.5,0.4)(0,0.25)(-0.5,0.4)};\n\
\\begin{scope}[line width=0.02]\n\
\\foreach \\x in {0.5,1,1.5,2,2.5,3,3.5,4,4.5,5}\n\
    \\draw (-0.4,\\x) --(-0.1,\\x);\n\
\\foreach \\x in {0.5,0.6,0.7,0.8,0.9,1.0,1,1,1.2,1.3,1.4,\n\
                  1.5,1.6,1.7,1.8,1.9,2.0,2.1,2.2,2.3,2.4,\n\
                  2.5,2.6,2.7,2.8,2.9,3.0,3.1,3.2,3.3,3.4,\n\
                  3.5,3.6,3.7,3.8,3.9,4.0,4.1,4.2,4.3,4.4}\n\
    \\draw (-0.4,\\x) --(-0.25,\\x);\n\
\\end{scope}\n\
\\begin{scope}[yshift=5]\n\
\\draw[smooth] plot coordinates {(0.6,-0.07) (0.498,0.008) (0.483,0.026) (0.433,0.05) (0.353,0.0707) (0.25,0.0866) (0.13,0.0966)(0,0.1)\n\
    (-0.13,0.0966) (-0.25,0.0866)(-0.353,0.0707) (-0.433,0.05) (-0.483,0.026) (-0.5,0) (-0.483,-0.026) (-0.353,-0.0707) (-0.25,-0.0866) (-0.13,-0.096)\n\
    (0,-0.1) (0.13,-0.096) (0.25,-0.0866) (0.353,-0.0707) (0.433,-0.05) (0.498,-0.026) (0.6,-0.07)};\n\
\\end{scope}\n\
\\end{scope}\n"

#define XW_TIKZ_TUBEESSAIS_BOUCHON_ "\\begin{scope}[fill=black!20,line width=0.053]\n\
\\filldraw (0,-0.3) ellipse[ x radius=0.4,y radius=0.1];\n\
\\filldraw (-0.4,-0.3) --(-0.6,0.5) --(0.6,0.5) --(0.4,-0.3) --cycle;\n\
\\filldraw[line width=0.07] (0,-0.3) ellipse[x radius=0.4,y radius=0.1];\n\
\\filldraw (0,0.5) ellipse[x radius=0.6,y radius=0.1];\n\
\\end{scope}\n\
\\draw[smooth] plot coordinates {(-0.5,0)(0,-0.07)(0.5,0)};\n"

#define XW_TIKZ_TUBEESSAIS_BOUCHON(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),fill=black!20,line width=0.053]\n\
\\begin{scope}[fill=black!20]\n\
\\filldraw (0,-0.3) ellipse[ x radius=0.4,y radius=0.1];\n\
\\filldraw (-0.4,-0.3) --(-0.6,0.5) --(0.6,0.5) --(0.4,-0.3) --cycle;\n\
\\filldraw[line width=0.07] (0,-0.3) ellipse[x radius=0.4,y radius=0.1];\n\
\\filldraw (0,0.5) ellipse[x radius=0.6,y radius=0.1];\n\
\\end{scope}\n\
\\draw[smooth] plot coordinates {(-0.5,0)(0,-0.07)(0.5,0)};\n\
\\end{scope}\n"

#define XW_TIKZ_BURETTE_CORPS(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.053]\n\
\\begin{scope}\n\
\\draw[clip] (-0.5,11.5) --(-0.25,11) --(-0.25,3) --(0,2.75)--(0,0.5)\n\
  (0.2,0.7) --(0.2,2.75) --(0.5,3) --(0.5,11) --(0.75,11.5);\n\
\\fill[fill=orange!20] (-1,-1) rectangle (1,5);\n\
\\end{scope}\n\
\\filldraw (-0.1,1.6) rectangle (0.3,2.1);\n\
\\draw (-0.1,1.5) rectangle (-0.2,2.2);\n\
\\draw (0.3,1.5) rectangle (0.4,2.2);\n\
\\draw (0.4,1.7) --(0.5,1.7) --(0.6,1.4) --(0.8,1.4) --(0.8,2.3) --(0.6,2.3) --(0.5,2) --(0.4,2) --cycle;\n\
\\begin{scope}[fill=cyan!20]\n\
\\draw (-0.5,1.5) --(-0.75,1.7) --(-0.75,2) --(-0.5,2.2) --(-0.3,2.2) --(-0.3,1.5) --cycle;\n\
\\draw (-0.5,1.5) rectangle (-0.2,2.2);\n\
\\end{scope}\n\
\\foreach \\x in {3.0,3.32,3.64,3.96,4.28,4.6,4.92,5.24,5.56,5.88,6.2,6.52,6.84,7.16,7.48,\n\
    7.8,8.12,8.44,8.76,9.08,9.4,9.72,10.04,10.36,10.68}\n\
    \\draw (-0.25,\\x) --(0,\\x);\n\
\\end{scope}\n"

#define XW_TIKZ_DOSAGE_AIMANT(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),rounded corners=0.5,line width=0.053]\n\
\\filldraw (-0.5,0.1) rectangle (0.5,0.4);\n\
\\filldraw (-0.05,0) rectangle (0.05,0.5);\n\
\\end{scope}\n\
\\begin{scope}\n\
\\draw (-1.5,0) rectangle (1.5,1.5);\n\
\\begin{scope}[double]\n\
\\filldraw[draw=red, double] (0,0.75) circle[radius=0.2];\n\
\\draw (-1.25,0.3) rectangle (-0.75,1.2);\n\
\\draw (1,0.75) circle[radius=0.25];\n\
\\end{scope}\n\
\\end{scope}\n"

#define XW_TIKZ_DOSAGE_PHMETRE(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.053]\n\
\\draw[rounded corners=0.25] (-1,0) rectangle (1,3);\n\
\\filldraw[fill=black!20] (-0.75,2) rectangle (0.75,2.75);\n\
\\draw (0,2.375) node {PH};\n\
\\draw (0,1.25) circle[radius=0.3];\n\
\\draw[->] (0,1.25) --(0,1.5);\n\
\\draw(0.65,1.5) node {$^\\circ$C};\n\
\\draw (-0.5,0.5) circle[radius=0.2];\n\
\\draw (0.5,0.5) circle[radius=0.2];\n\
\\draw[rounded corners=0.5] (2.25,2.5) rectangle (2.75,4.5);\n\
\\filldraw[fill=black!20] (2.25,4) rectangle (2.75,4.5);\n\
\\draw (2.5,2.25) circle[radius=0.25];\n\
\\draw[rounded corners=0.4] (2.5,4.5) --(2.5,5) --(1.25,4.75) --(0,5) --(0,3);\n\
\\filldraw[draw=green!50!blue,fill=black!20,rounded corners=0.4,line width=0.1] (2.5,4.5) --(2.5,5) --(1.25,4.75) --(0,5) --(0,3);\n\
\\end{scope}\n"

#define XW_TIKZ_TUBEESSAIS_PINCE(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.053]\n\
\\begin{scope}[fill=brown!20]\n\
\\filldraw plot coordinates {(0.5,0)(0,-0.1)(-0.5,0)}\n\
       (-0.5,0) --(-0.8,0)\n\
       (-0.5,0) arc[radius=0.3,start angle=180,end angle=270]\n\
       (-0.5,-0.3) --(2.5,-0.3) --(2.4,-0.2) --(1,0) --(0.5,0);\n\
\\filldraw (-0.8,0) --(-0.8,-0.3)\n\
       (-0.5,-0.3) arc[radius=0.3,start angle=180,end angle=270]\n\
       (-0.5,-0.6) --(2.5,-0.6) --(2.5,-0.3) --(-0.5,-0.3)\n\
       (-0.5,0) arc[radius=0.3,start angle=180,end angle=-90];\n\
\\filldraw (-0.5,0.05) --(-0.8,0.05)\n\
       (-0.5,0.05) arc[radius=0.25,start angle=90,end angle=180]\n\
       (-0.5,0.3) --(-0.5,0.05);\n\
\\filldraw (0.5,0.3) --(4.9,0.3) --(5,0.2) --(1,0) --(0.5,0) --(0.5,0.3);\n\
\\draw (0.5,0) --(5,0.2) --(5,0) --(2,-0.2) --(1,0);\n\
\\end{scope}\n\
\\draw[line width=0.6]  (1,0) ellipse[x radius=0.3, y radius=0.1];\n\
\\draw[line width=0.6] (0.7,0) --(0.4,-0.3) --(0.4,-0.6);\n\
\\end{scope}\n"

#define XW_TIKZ_BECBUNSEN(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.053]\n\
\\draw (-1.25,0) rectangle (1.25,0.25);\n\
\\draw(-0.5,1.25) rectangle (0.5,2.25);\n\
\\foreach \\x in {0.3,0.6,0.9}\n\
   \\draw (\\x,1.75) circle[radius=0.1];\n\
\\draw(-0.25,2.25) rectangle (0.25,4.25);\n\
\\draw (0.25,1.25) --(0.25,0.5) --(1.25,0.25);\n\
\\draw (-1.25,0.25) --(-0.25,0.5) --(-0.25,0.75);\n\
\\draw (-2.25,0.75) --(-0.25,0.75);\n\
\\draw (-2.25,1) --(-0.25,1);\n\
\\draw (-0.25,0.875) ellipse[x radius=0.1,y radius=0.125];\n\
\\filldraw (-2.25,0.75) rectangle (-0.25,1);\n\
\\draw (-2.25,0.75) --(-0.25,0.75);\n\
\\draw (-2.25,1) --(-0.25,1) --(-0.25,1.25);\n\
\\draw[smooth] plot coordinates {(-0.25,0.5)(0,0.4)(0.25,0.5)};\n\
\\begin{scope}\n\
\\shade[clip,yshift=4.25,top color=magenta, bottom color=yellow] (-0.25,0) .. controls (-0.35,0.5) and (-0.4,0.75)\n\
                                        (-0.35,1) .. controls (-0.25,1.5) and (0.5,2)  (0.25,1.5) .. controls (0.35,1) and (0.4,0.75)\n\
                                        (0.35,0.5) ..controls (0.25,0) (0,0);\n\
\\shade[top color=cyan,bottom color=white] (-0.25,0) --(0.25,0) --(0,1) --cycle;\n\
\\end{scope}\n\
\\end{scope}\n"

#define XW_TIKZ_BECBUNSENGRILLE(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.053]\n\
\\draw (-1.25,0) rectangle (1.25,0.25);\n\
\\draw (-0.5,1.25) rectangle (0.5,2.25);\n\
\\foreach \\x in {0.3,0.6,0.9}\n\
  \\draw (\\x,1.75) circle[radius=0.1];\n\
\\draw (-0.25,2.25) rectangle (0.25,4.25);\n\
\\draw (0.25,1.25) --(0.25,0.5) --(1.25,0.25);\n\
\\draw (-1.25,0.25) --(-0.25,0.5) --(-0.25,0.75);\n\
\\draw (-2.25,0.75) --(-0.25,0.75);\n\
\\draw (-2.25,1) --(-0.25,1);\n\
\\draw (-0.25,0.875) ellipse[x radius=0.1, y radius=,0.125];\n\
\\filldraw (-2.25,0.75) rectangle (-0.25,1);\n\
\\draw (-2.25,0.75) --(-0.25,0.75);\n\
\\draw (-2.25,1) --(-0.25,1) --(-0.25,1.25);\n\
\\draw[smooth] plot coordinates {(-0.25,0.5)(0,0.4)(0.25,0.5)};\n\
\\begin{scope}[yshift=4.25]\n\
\\shade[clip,top color=magenta, bottom color=yellow] (0,0.75) arc[radius=0.75,start angle=270,end angle=0]\n\
         (1.5,0.75) arc[radius=0.75,start angle=90,end angle=180]\n\
         (1.5,1.5) --(-1.5,1.5)\n\
         (-1.5,0.75) arc[radius=0.75,start angle=0,end angle=90]\n\
         (0,0.75) arc[radius=0.75,start angle=180,end angle=270];\n\
\\shade[top color=cyan,bottom color=white] (-0.25,0) --(0.25,0) --(0,1) --cycle;\n\
\\end{scope}\n\
\\draw[dashed] (-2,5.75) --(2,5.75);\n\
\\end{scope}\n"

#define XW_TIKZ_PIPETTE_CORPS(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.053,rounded corners=0.05]\n\
\\begin{scope}\n\
\\draw[clip] (-0.05,0) --(-0.15,0.2) --(-0.15,6) --(0.15,6) --(0.15,0.2) --(0.05,0) --cycle\n\
  (0,0) ..controls (-0.04,-0.083) and (-0.083,-0.22)  (0,-0.25) .. controls (0.083,-0.22) and (0.04,-0.083) (0,0);\n\
\\fill[fill=orange!20] (-1,-1) rectangle (1,2);\n\
\\end{scope}\n\
\\draw (0,6) ellipse[x radius=0.15,y radius=0.05]\n\
\\foreach \\x in {0.5,1,1.5,2,2.5,3,3.5,4,4.5,5}\n\
  \\draw[line width=0.02] (0,\\x) --(0.1,\\x);\n\
\\end{scope}\n"

#define XW_TIKZ_TUBEESSAIS_DROIT(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.053]\n\
\\draw[line width=0.2] (0.05,-1) --(0.05,5);\n\
\\draw[line width=0.2] (-0.05,-1) --(-0.05,5);\n\
XW_TIKZ_TUBEESSAIS_BOUCHON_\
\\filldraw[rounded corners=0.5,line width=0.2] (-0.05,5) rectangle (0.05,0.5);\n\
\\draw [line width=0.14] (-0.1,5) --(0.1,5);\n\
\\end{scope}\n"

#define XW_TIKZ_TUBEESSAIS_COUDE(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by)]\n\
\\draw[line width=0.2] (0.05,-1) --(0.05,0);\n\
\\draw[line width=0.2] (-0.05,-1) --(-0.05,0);\n\
XW_TIKZ_TUBEESSAIS_BOUCHON_\
\\filldraw[rounded corners=0.05,line width=0.2] (-0.05,0.5) --(-0.05,2) --(8,2) --(8,1.9) --(0.05,1.9) --(0.05,0.5) --cycle;\n\
\\draw[draw=white,line width=0.14] (7.95,1.9) --(8.05,2);\n\
\\end{scope}\n"

#define XW_TIKZ_TUBEESSAIS_COUDEU(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by)]\n\
\\draw[line width=0.2] (0.05,-1) --(0.05,0);\n\
\\draw[line width=0.2] (-0.05,-1) --(-0.05,0);\n\
XW_TIKZ_TUBEESSAIS_BOUCHON_\
\\filldraw[rounded corners=0.05,line width=0.2] (-0.05,0.5) --(-0.05,2) --(4.5,2) --(4.5,-3) --(4.4,-3) --(4.4,1.9) --(0.05,1.9) --(0.05,0.5) --cycle;\n\
\\draw[draw=white,line width=0.14] (4.3,-3) --(4.6,-3);\n\
\\end{scope}\n"

#define XW_TIKZ_TUBEESSAIS_COUDEUB(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by)]\n\
\\draw[line width=0.2] (0.05,-1) --(0.05,0);\n\
\\draw[line width=0.2] (-0.05,-1) --(-0.05,0);\n\
XW_TIKZ_TUBEESSAIS_BOUCHON_\
\\filldraw[rounded corners=0.05,line width=0.2] (-0.05,0.5) --(-0.05,2) --(4.5,2) --(4.5,-9) --(4.4,-9) --(4.4,1.9) --(0.05,1.9) --(0.05,0.5) --cycle;\n\
\\draw[draw=white,line width=0.14] (4.3,-9) --(4.6,-9);\n\
\\end{scope}"

#define XW_TIKZ_TUBEESSAIS_DOUBLETUBE(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by),line width=0.2]\n\
\\draw (-0.3,-3) --(-0.3,0);\n\
\\draw (-0.2,-3) --(-0.2,0);\n\
\\draw (0.25,-1) --(0.25,0);\n\
\\draw (0.15,-1) --(0.15,0)\n\
\\end{scope}\n\
XW_TIKZ_TUBEESSAIS_BOUCHON_\
\\begin{scope}[xshift=0.2]\n\
\\begin{scope}[line width=0.2]\n\
\\draw (-0.05,0.5) --(-0.05,1);\n\
\\draw (0.5,1) arc[radius=0.55,start angle=90,end angle=180];\n\
\\draw (0.5,1.55) --(3.5,1.55);\n\
\\draw (3.5,1) arc[radius=0.55,start angle=0,end angle=90];\n\
\\draw (4.05,1) --(4.05,-2.8);\n\
\\draw (4.5,-2.8) arc[radius=0.45,start angle=0,end angle=180];\n\
\\draw (4.95,-2.8) --(4.95,-2.3);\n\
\\draw (5.05,-2.3) --(5.05,-2.8);\n\
\\draw (4.5,-2.8) arc[radius=0.55,start angle=180,end angle=0];\n\
\\draw (3.95,-2.8) --(3.95,1);\n\
\\draw (3.5,1) arc[radius=0.45,start angle=0,end angle=90];\n\
\\draw (3.5,1.45) --(0.5,1.45);\n\
\\draw (0.5,1) arc[radius=0.45,start angle=90,end angle=180];\n\
\\draw (0.05,1) --(0.05,0.5);\n\
\\draw (-0.05,.5) --(0.05,0.5);\n\
\\end{scope}\n\
\\filldraw[draw=white] (-0.025,0.52) rectangle (0.018,0.8);\n\
\\end{scope}\n\
\\begin{scope}[yshift=0.5]\n\
\\begin{scope}[line width=0.2]\n\
\\filldraw [rounded corners=0.05] (-0.3,0) --(-0.3,2) --(-0.5,2.2) --(-0.5,3) --(0,3) --(0,2.2) --(-0.2,2) --(-0.2,0) --cycle;\n\
\\filldraw (0.1,1.55) --(-0.5,1.55) --(-0.5,1.7) --(-0.55,1.7) --(-0.55,1.3) --(-0.5,1.3) --(-0.5,1.55) --(-0.5,1.45) --(0.1,1.45) --cycle;\n\
\\filldraw[rounded corners=0.2] (-0.4,1.3) rectangle (-0.1,1.7);\n\
\\end{scope}\n\
\\filldraw (-0.25,3) ellipse[x radius=0.3,y radius=0.1];\n\
\\end{scope}\n"

#define XW_TIKZ_CRISTALLISOIR(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by)]\n\
\\begin{scope}\n\
\\draw[clip] (1.9,2) --(1.9,1)\n\
       (1,1) arc[radius=0.9,start angle=-90,end angle=0]\n\
       (1,0.1) --(-1,0.1)\n\
       (-1,1) arc[radius=0.9,start angle=180,end=270]\n\
       (-1.9,1) --(-1.9,2);\n\
\\fill[draw=cyan!20] (-3,-1) rectangle (3,1.5);\n\
\\end{scope}\n\
\\draw (-1.9,2) --(-2,2);\n\
\\draw (-2,1.95) arc[radius=0.05,start angle=90,end angle=270];\n\
\\draw (-2,1.9) --(-2,1);\n\
\\draw (-1,1) arc[radius=1,start angle=180,end angle=270];\n\
\\draw (-1,0) --(1,0);\n\
\\draw (1,1) arc[radius=1,start angle=270,end angle=0];\n\
\\draw (2,1) --(2,1.9);\n\
\\draw (2,1.95) arc[radius=0.05,start angle=270,end angle=90];\n\
\\draw (2,2) --(1.9,2);\n\
\\begin{scope}[draw=lightgray]\n\
\\draw (0,-0.25) arc[radius=1,start angle=100,140];\n\
\\draw (0,-0.25) arc[radius=1,start angle=150,end angle=160];\n\
\\draw (0,-0.25) arc[radius=1,start angle=20,end angle=80];\n\
\\end{scope}\n\
\\begin{scope}[xshift=0,yshift=0.7,line width=0.07]\n\
\\filldraw[fill=cyan!20] (-0.5,0) recrangle (0.5,5.5);\n\
\\draw (-0.5,0) --(-0.5,6);\n\
\\draw (0,6) arc[radius=0.5,start angle=0,end angle=180];\n\
\\draw (0.5,6) --(0.5,0);\n\
\\end{scope}\n\
\\end{scope}\n"

#define XW_TIKZ_TUBERECOURBE_COURT(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by)]\n\
\\draw[line width=0.2] (0.05,-1) --(0.05,0);\n\
\\draw[line width=0.2] (-0.05,-1) --(-0.05,0);\n\
XW_TIKZ_TUBEESSAIS_BOUCHON_\
\\begin{scope}[line width=0.2]\n\
\\draw (-0.05,0.5) --(-0.05,1);\n\
\\draw (0.5,1) arc[radius=0.55,start angle=90,end angle=180];\n\
\\draw (0.5,1.55) --(3.5,1.55);\n\
\\draw (3.5,1) arc[radius=0.55,start angle=0,end angle=90];\n\
\\draw (4.05,1) --(4.05,-2.8);\n\
\\draw (4.5,-2.8) arc[radius=0.45,start angle=180,end angle=0];\n\
\\draw (4.95,-2.8) --(4.95,-2.3);\n\
\\draw (5.05,-2.3) --(5.05,-2.8);\n\
\\draw (4.5,-2.8) arc[radius=0.55,start angle=180,end angle=0];\n\
\\draw (3.95,-2.8) --(3.95,1);\n\
\\draw (3.5,1) arc[radius=0.45,start angle=0,end angle=90];\n\
\\draw (3.5,1.45) --(0.5,1.45);\n\
\\draw (0.5,1) arc[radius=0.45,start angle=90,end angle=180];\n\
\\draw (0.05,1) --(0.05,0.5);\n\
\\draw (-0.05,.5) --(0.05,0.5);\n\
\\end{scope}\n\
\\filldraw[draw=white] (-0.025,0.52) rectangle (0.020,0.6);\n\
\\end{scope}\n"

#define XW_TIKZ_TUBERECOURBE(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by)]\n\
\\draw[line width=0.2] (0.05,-1) --(0.05,0);\n\
\\draw[line width=0.2] (-0.05,-1) --(-0.05,0);\n\
XW_TIKZ_TUBEESSAIS_BOUCHON_\
\\begin{scope}[line width=0.2]\n\
\\draw (-0.05,0.5) --(-0.05,1);\n\
\\draw (0.5,1) arc[radius=0.55,start angle=90,end angle=180];\n\
\\draw (0.5,1.55) --(5.5,1.55);\n\
\\draw (5.5,1) arc[radius=0.55,start angle=0,end angle=90];\n\
\\draw (6.05,1) --(6.05,-8.8);\n\
\\draw (6.5,-8.8) arc[radius=0.45,start angle=180,end angle=0];\n\
\\draw (6.95,-8.8) --(6.95,-8.3);\n\
\\draw (7.05,-8.3) --(7.05,-8.8);\n\
\\draw (6.5,-8.8) arc[radius=0.55,start angle=180,end angle=0];\n\
\\draw (5.95,-8.8) --(5.95,1);\n\
\\draw (5.5,1) arc[radius=0.45,start angle=0,end angle=90];\n\
\\draw (5.5,1.45) --(0.5,1.45);\n\
\\draw (0.5,1) arc[radius=0.45,start angle=90,end angle=180];\n\
\\draw (0.05,1) --(0.05,0.5);\n\
\\draw (-0.05,0.5) --(0.05,0.5);\n\
\\end{scope}\n\
\\filldraw[draw=white](-0.025,0.52) rectangle (0.020,0.6);\n\
\\end{scope}\n"

#define XW_TIKZ_BALLON_REFLUX(ax,by) "\\begin{scope}[xshift=(ax),yshift=(by)]\n\
\\draw[clip,line width=0.053] (-0.7,3.4) arc[radius=0.2,start angle=0,end angle=90]\n\
       (-0.5,3.4) --(-0.5,2.4)\n\
       (0,1.25) arc[radius=1.25,start angle=113,end angle=67]\n\
       (0.5,2.4) --(0.5,3.4)\n\
       (0.7,3.4) arc[radius=0.2,start angle=90,end angle=180]\n\
       (0,3.6) ellipse[x radius=0.7,y radius=0.1];\n\
\\shade[smooth,top color=cyan,bottom color=white,draw=cyan] plot coordinates {(-1.5,1.5)(-1.375,1.55)\n\
	(-1.25,1.5)(-1.125,1.45)(-1,1.5)(-0.875,1.55)(-0.75,1.5)(-0.625,1.45)(-0.5,1.5)(-0.375,1.55)\n\
	(-0.25,1.5)(-0.125,1.45)(0,1.5)(0.125,1.55)(0.25,1.5)(0.375,1.45)(0.5,1.5)(0.625,1.55)\n\
	(0.75,1.5)(0.875,1.45)(1,1.5)(1.125,1.55)	(1.25,1.5)(1.375,1.45)(1.5,1.5)}\n\
  (1.5,1.5) --(1.5,0) --(-1.5,0) --(-1.5,1.5);\n\
\\end{scope}\n\
\\begin{scope}[xshift=0,yshift=3.5]\n\
\\begin{scope}[yshift=0.2]\n\
\\shade[top color=cyan,bottom color=orange!50] (0,5.75) arc[radius=0.5,start angle=0,end angle=180]\n\
                  (-0.5,5.75) --(-0.5,5.5) --(-1.5,5.5)\n\
                  (-1.5,5.3) --(-0.5,5.3) --(-0.5,1.7) --(-1.5,1.7)\n\
                  (-1.5,1.5) --(-0.5,1.5) --(-0.5,1)\n\
                  (0,1) arc[radius=0.5,start angle=180,end angle=0]\n\
                  (0.5,1) --(0.5,5.75);\n\
\\begin{scope}[line width=0.44]\n\
\\draw (0,5.75) arc[radius=0.5,start angle=0,end angle=180];\n\
\\draw (-0.5,5.75) --(-0.5,5.5) --(-1.5,5.5);\n\
\\draw (-0.5,5.75) --(-0.5,5.5) --(-1.5,5.5);\n\
\\draw (-1.5,5.3) --(-0.5,5.3) --(-0.5,1.7) --(-1.5,1.7);\n\
\\draw (-1.5,1.5) --(-0.5,1.5) --(-0.5,1);\n\
\\draw (0,1) arc[radius=0.5,start angle=0,end angle=180];\n\
\\draw (0.5,1) --(0.5,5.75);\n\
\\draw[draw=cyan,->] (-2.5,1.6) --(-1.6,1.6);\n\
\\draw[draw=brown,->] (-2.5,5.4) --(-1.6,5.4);\n\
\\end{scope}\n\
\\filldraw (-0.35,7.5) arc[radius=0.1,start angle=0,end angle=90]\n\
       (-0.25,7.5) --(-0.25,6.6)\n\
       (-0.15,6.6) arc[radius=0.1,start angle=180,end angle=270]\n\
       (-0.15,6.4) arc[radius=0.1,start angle=0,end angle=90]\n\
       (-0.05,6.4) --(-0.05,6)\n\
       (-0.05,0.75) --(-0.05,-1.5)\n\
       (0.05,-1.5) --(0.05,1)\n\
       (0.05,6) --(0.05,6.4)\n\
       (0.15,6.4) arc[radius=0.1,start angle=90,end angle=180]\n\
       (0.15,6.6) arc[radius=0.1,start angle=-90,end angle=0]\n\
       (0.25,6.6) --(0.25,7.5)\n\
       (0.35,7.5) arc[radius=0.1, start angle=90,end angle=180];\n\
\\foreach \\x/\\y/\\z in {5.75/5.50/5.25, 5/4.75/4.5, 4.25/4/3.75, 3.5/3.25/3, 2.75/2.5/2.25, 2/1.75/1.5, 1.25/1/0.75}\n\
     {\\draw (-0.05,\\x) arc[radius=0.25,start angle=90,end angle=270];\n\
      \\draw (-0.05,\\y) --(-0.05,\\z);}\n\
\\foreach \\x/\\y/\\z in {1.25/1.50/1.75, 2/2.25/1.5, 2.75/3/2.25, 3.5/3.75/3, 4.25/4.5/3.75, 5/5.25/4.5, 5.75/6/5.25}\n\
     {\\draw (0.05,\\x) arc[radius=0.25,start angle=270,end angle=90];\n\
      \\draw (0.05,\\y) --(0.05,\\z);}\n\
\\draw[yshift=-1.6] (0,0) ..controls (-0.04,-0.083) and (-0.083,-0.22) (0,-0.25) .. controls (0.083,-0.22) and (0.04,-0.083) (0,0);\n\
\\end{scope}\n\
XW_TIKZ_TUBEESSAIS_BOUCHON_\
\\filldraw[rounded corners=0.025] (-0.05,0.6) --(-0.05,0.5) --(0.05,0.5) --(0.05,0.6) --cycle;\n\
\\end{scope}\n\
\\begin{scope}\n\
\\filldraw (-2,-1) rectangle (2,1);\n\
\\draw (-1.25,0) circle[radius=0.4];\n\
\\begin{scope}[xshift=-1.25]\n\
\\foreach \\i in {0,30,60,90,120,150,180}\n\
     \\draw (0.45:\\i) --(0.6:\\i);\n\
\\draw[->] (0,0) --(0.3:60);\n\
\\end{scope}\n\
\\filldraw[double,fill=gray] (1,-0.5) rectangle (1.5,0.5);\n\
\\filldraw[draw=red] (1.25,0.7) circle[radius=0.1];\n\
\\end{scope}\n"

#define XW_TIKZ_DISTILLATIONFRACTIONNEE "\\begin{scope}[yshift=-3]\n\
\\begin{scope}[xshift=2.75,yshift=13.5]\n\
\\draw[rounded corners=0.05] (-0.05,0.25) --(-0.05,2.75) --(0.05,2.75) --(0.05,0.25);\n\
\\filldraw[fill=red,rounded corner=0.8] (-0.15,-0.25) rectangle (0.15,0.25);\n\
\\draw[draw=red] (0.15,0.25) --(0,2);\n\
\\end{scope}\n\
\\begin{scope}[xshift=2.75,yshift=2.5]\n\
\\filldraw[pattern=horizontal lines] (-0.5,2.25) rectangle (-0.25,3);\n\
\\filldraw[pattern=horizontal lines] (0.5,2.25) rectangle (0.25,3);\n\
\\draw (0.5,1.5) --(0.5,3) (-0.5,1.5) --(-0.5,3);\n\
\\draw  (0,0) arc[radius=1.581, start angle=108.435, end angle=71.565];\n\
\\shade[top color=green,bottom color=cyan] plot coordinates[smooth] {(-1.581,0)(-1.25,0.1)(-0.75,-0.1)(0,0.1)(0.75,-0.1)(1.25,0.1)(1.581,0)}\n\
             (0,0) arc[radius=1.581, start angle=180, end engle=0];\n\
\\end{scope}\n\
\\begin{scope}[xshift=2.75,yshift=4]\n\
\\draw[rounded corners=0.3] (-0.25,10.5) --(-0.25,8.5) --(-0.75,8) --(-0.75,2.5) --(-.25,2) --(-0.25,0);\n\
\\draw[rounded corners=0.3] (0.25,0) --(0.25,2) --(0.75,2.5) --(0.75,8) --(0.25,8.5) --(0.25,9.36);\n\
\\foreach \\x in {3,4,5,6,7,8}\n\
  \\draw[xshift=-0.75,yshift=\\x,line width=0.16] (0,0) --(0.5,-0.5) --(0,-0.1);\n\
\\foreach \\x in {3,4,5,6,7,8}\n\
  \\draw[xshift=0.75,yshift=\\x,line width=0.16] (0,0) --(-0.5,-0.5) --(0,-0.1);\n\
\\filldraw[pattern=horizontal lines] (-0.25,10) rectangle (-0.05,10.5);\n\
\\filldraw[pattern=horizontal lines] (0.25,10) rectangle (0.05,10.5);\n\
\\foreach \\x in {2.5,3.5,4.5,5.5,6.5,7.5}\n\
  \\draw[xshift=-0.25,yshift=\\x,line width=0.17,scale=0.6]  (0,0) ..controls (0.25,-0.4) and (-0.25,-0.4) (0,0);\n\
\\foreach \\x in {2.5,3.5,4.5,5.5,6.5,7.5}\n\
  \\draw[xshift=0.25,yshift=\\x,line width=0.17,scale=0.6]  (0,0) ..controls (0.25,-0.4) and (-.25,-0.4) (0,0);\n\
\\draw[xshift=-0.25,yshift=-0.1,line width=0.17,scale=0.6]  (0,0) ..controls (0.25,-0.4) and (-.25,-0.4) (0,0);\n\
\\draw[xshift=0.25,yshift=-0.1,line width=0.17,scale=0.6]  (0,0) ..controls (0.25,-0.4) and (-.25,-0.4) (0,0);\n\
\\end{scope}\n\
\\begin{scope}[xshift=3,yshift=13.4]\n\
\\begin{scope}[rotate=-45]\n\
\\draw (3,0) arc[radius=0.5,start angle=90, end angle=270];\n\
\\draw (7,0) arc[radius=0.5,start angle=270, end angle=90];\n\
\\filldraw (0,-0.05) rectangle (9,0.05);\n\
\\draw[line width=0.2] (-.075,0.05) --(9,0.05) (0.02,-0.05) --(9,-0.05);\n\
\\draw (3,0.5) --(3.5,0.5);\n\
\\draw (3,-0.5) --(6.5,-0.5);\n\
\\draw (3.6,0.5) --(7,0.5);\n\
\\draw[line width=0.2] (3.5,0.5) --(3.5,1.5) (3.6,0.5) --(3.6,1.5) (6.6,-0.5) --(6.6,-1.5) (6.5,-0.5) --(6.5,-1.5);\n\
\\draw (6.6,-0.5) --(7,-0.5);\n\
\\draw[->] (6.55,-2.5) --(6.55,-1.5);\n\
\\draw[->] (3.55,1.5) --(3.55,2.5);\n\
\\end{scope}\n\
\\end{scope}\n\
\\begin{scope} \n\
\\draw (1.17,2.5) --(0,2.5) --(0,0) --(5.5,0) --(5.5,2.5) --(4.33,2.5);\n\
\\end{scope}\n\
\\begin{scope}[xshift=9.4] \n\
\\begin{scope}\n\
\\draw[clip,line width=0.6] (-0.5,6) --(-0.5,0.5) (-1,0.5) arc[radius=0.5,start angle=-90,end angle=0] \n\
              (-1,0) --(1,0) (1,0.5) arc[radius=0.5,start angle=180,end angle=270] (0.5,0.5) --(0.5,6);\n\
\\filldraw[draw=yellow] (-1.5,-1) rectangle (1.5,1);\n\
\\end{scope}\n\
\\begin{scope}[line width=0.2]\n\
\\foreach \\x in {1,2,3,4,5}\n\
  \\draw (-0.4,\\x) --(-0.1,\\x);\n\
\\foreach \\x in {0.2,0.4,0.6,0.8,1,1.2,1.4,1.6,1.8,2,2.2,2.4,2.6,2.8,3,3.2,3.4,3.6,3.8,4,4.2,4.4,4.6,4.8}\n\
  \\draw (-0.4,\\x) --(-0.2,\\x);\n\
\\end{scope}\n\
\\end{scope}\n\
\\begin{scope}[xshift=9.37,yshift=7]\n\
\\draw[line width=0.017,scale=0.6] (0,0) ..controls (0.25,-0.4) and (-0.25,-0.4) (0,0);\n\
\\end{scope}\n\
\\begin{scope}[xshift=9.37,yshift=6]\n\
\\draw[line width=0.017,scale=0.6] (0,0) ..controls (0.25,-0.4) and (-0.25,-0.4) (0,0);\n\
\\end{scope}\n\
\\begin{scope}[xshift=9.37,yshift=4]\n\
\\draw[line width=0.017,scale=0.6] (0,0) ..controls (0.25,-0.4) and (-0.25,-0.4) (0,0);\n\
\\end{scope}\n\
\\begin{scope}[xshift=9.37,yshift=1.2]\n\
\\draw[line width=0.017,scale=0.6] (0,0) ..controls (0.25,-0.4) and (-0.25,-0.4) (0,0);\n\
\\end{scope}\n\
\\begin{scope}[xshift=0.75,yshift=0.75]\n\
\\draw (0,0) circle[radius=0.5];\n\
\\foreach \\i in {0,30,60,90,120,150,180}\n\
  (0.55:\\i) --(0.65:\\i);\n\
\\draw[->] (0.65:180) --(0,0.45);\n\
\\end{scope}\n\
\\end{scope}\n"

QString Billes(int c=50);

QString bulles(int c=25);

QString BULLES(int c=50);

QString bullesChampagne(int c=25);

QString clouFer(int c=60);

QString filaments(int c=5);

QString grenailleZinc(int c=25);

QString tournureCuivre(int c=30);

#endif //XWPSTLABO_H