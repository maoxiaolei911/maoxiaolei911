/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "pstlabo.h"
#include "XWPDFDriver.h"
#include "XWTikzGraphic.h"
#include "XWTikzState.h"
#include "pstlaboWidget.h"

PSTLaboWidget::PSTLaboWidget(XWTikzGraphic * graphicA,QWidget * parent)
:QWidget(parent),
 graphic(graphicA)
{
  QGridLayout *layout = new QGridLayout;

  QString src = XW_TIKZ_TUBEESSAIS_CORPS;
  QToolButton * button = createButton(src,"tube");
  layout->addWidget(button,0,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertTubeEssais()));

  src = XW_TIKZ_BALLON;
  button = createButton(src,"ballon");
  layout->addWidget(button,0,1);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBallon()));

  src = XW_TIKZ_ERLEN_CORPS;
  button = createButton(src,"erlen");
  layout->addWidget(button,0,2);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertErlen()));

  src = XW_TIKZ_BECHER_CORPS;
  button = createButton(src,"becher");
  layout->addWidget(button,0,3);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBecher()));

  src = XW_TIKZ_FLACON_CORPS;
  button = createButton(src,"flacon");
  layout->addWidget(button,1,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertFlacon()));

  src = XW_TIKZ_FIOLEJAUGE;
  button = createButton(src,"fiolejauge");
  layout->addWidget(button,1,1);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertFiolejauge()));

  src = XW_TIKZ_ENTONNOIR_CORPS(0,0);
  button = createButton(src,"entonnoir",0.7);
  layout->addWidget(button,1,2);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertEntonnoir()));

  src = XW_TIKZ_EPROUVETTE_CORPS(0,0);
  button = createButton(src,"eprouvette",0.4);
  layout->addWidget(button,1,3);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertEprouvette()));

  src = XW_TIKZ_TUBEESSAIS_BOUCHON(0,0);
  button = createButton(src,"bouchon",1);
  layout->addWidget(button,2,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBouchon()));

  src = XW_TIKZ_BURETTE_CORPS(0,0);
  button = createButton(src,"burette",0.25);
  layout->addWidget(button,3,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBurette()));

  src = XW_TIKZ_DOSAGE_AIMANT(0,0);
  button = createButton(src,"aimant");
  layout->addWidget(button,3,1);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertDosageAimant()));

  src = XW_TIKZ_DOSAGE_PHMETRE(0,0);
  button = createButton(src,"PH metre");
  layout->addWidget(button,3,2);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertDosagePHmetre()));

  src = XW_TIKZ_TUBEESSAIS_PINCE(0,0);
  button = createButton(src,"pince",0.5,-60);
  layout->addWidget(button,4,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertPince()));

  src = XW_TIKZ_BECBUNSEN(0,0);
  button = createButton(src,"becbunsen");
  layout->addWidget(button,4,1);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBecBunsen()));

  src = XW_TIKZ_BECBUNSENGRILLE(0,0);
  button = createButton(src,"becbunsen grill");
  layout->addWidget(button,4,2);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBecBunsenGrille()));

  src = XW_TIKZ_PIPETTE_CORPS(0,0);
  button = createButton(src,"pipette");
  layout->addWidget(button,4,3);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertPipette()));
  
  src = XW_TIKZ_TUBEESSAIS_DROIT(0,0);
  button = createButton(src,"droit");
  layout->addWidget(button,5,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertDroit()));

  src = XW_TIKZ_TUBEESSAIS_COUDE(0,0);
  button = createButton(src,"coude");
  layout->addWidget(button,5,1);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertCoude()));

  src = XW_TIKZ_TUBEESSAIS_COUDEU(0,0);
  button = createButton(src,"coudeU");
  layout->addWidget(button,5,2);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertCoudeU()));

  src = XW_TIKZ_TUBEESSAIS_COUDEUB(0,0);
  button = createButton(src,"coudeUB");
  layout->addWidget(button,5,3);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertCoudeUB()));

  src = XW_TIKZ_TUBEESSAIS_DOUBLETUBE(0,0);
  button = createButton(src,"double tube");
  layout->addWidget(button,6,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertTubeEssaisDoubleTube()));

  src = XW_TIKZ_CRISTALLISOIR(0,0);
  button = createButton(src,"recuperation gaz");
  layout->addWidget(button,6,1);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertCristallisoir()));

  src = XW_TIKZ_TUBERECOURBE_COURT(0,0);
  button = createButton(src,"tuberecourbe court");
  layout->addWidget(button,6,2);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertTubeRecourbeCourt()));

  src = XW_TIKZ_TUBERECOURBE(0,0);
  button = createButton(src,"tuberecourbe",0.4);
  layout->addWidget(button,6,3);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertTubeRecourbe()));

  src = XW_TIKZ_BALLON_REFLUX(0,0);
  button = createButton(src,"ballon reflux",0.25);
  layout->addWidget(button,7,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBallonReflux()));

  src = XW_TIKZ_DISTILLATIONFRACTIONNEE;
  button = createButton(src,"distillation fractionnee",0.2);
  layout->addWidget(button,7,1);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertDistillationFractionnee()));

  src = tournureCuivre();
  button = createButton(src,"tournure cuivre",0.5,0,true);
  layout->addWidget(button,8,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertTournureCuivre()));

  src = clouFer();
  button = createButton(src,"cloufer",0.5,0,true);
  layout->addWidget(button,8,1);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertClouFer()));

  src = grenailleZinc();
  button = createButton(src,"grenaille zinc",0.5,0,true);
  layout->addWidget(button,8,2);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertGrenailleZinc()));

  src = bullesChampagne();
  button = createButton(src,"bulles champagne",0.5,0,true);
  layout->addWidget(button,9,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBullesChampagne()));

  src = filaments();
  button = createButton(src,"filaments",0.5,0,true);
  layout->addWidget(button,9,1);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertFilaments()));

  src = Billes();
  button = createButton(src,"billes",0.5,0,true);
  layout->addWidget(button,9,2);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBilles()));

  src = BULLES();
  button = createButton(src,"BULLES",0.5,0,true);
  layout->addWidget(button,9,3);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBULLES()));

  src = bulles();
  button = createButton(src,"bulles",0.5,0,true);
  layout->addWidget(button,10,0);
  connect(button, SIGNAL(clicked()), graphic, SLOT(insertBulles()));

  setLayout(layout);
}

QToolButton * PSTLaboWidget::createButton(const QString & src,const QString & txt,
                                          double scale,double rotate,bool bg)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString ssrc = QString("\\begin{tikzpicture}[rotate=%2]\n%3\\end{tikzpicture}\n").arg(rotate).arg(src);
  graphicA.scan(ssrc);
  graphicA.doGraphic(&driver);

  QPixmap pix(100,100);
  if (bg)
  {
    QColor b(0,124,255);
    pix.fill(b);
  }
    
  QPainter painter(&pix);
  driver.display(&painter,scale);

  QSize iconSize(100,100);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(txt);
  button->setToolTip(txt);

  button->setIcon(QIcon(pix));
  button->setAutoRaise(true);
  return button;
}
