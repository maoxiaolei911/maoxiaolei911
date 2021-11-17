/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QPointF>
#include "PGFKeyWord.h"
#include "XWTikzGraphic.h"
#include "XWTikzShape.h"
#include "XWTikzExpress.h"
#include "XWTikzState.h"
#include "XWTikzOption.h"
#include "XWTikzDecoration.h"

void XWTikzDecoration::doFootPrintsLeftState(XWTikzState * stateA)
{
  stateA->shift(0, stateA->footSep / 2);
  stateA->scale(stateA->footLength, stateA->footLength);
  stateA->rotate(stateA->footAngle);
  switch (stateA->footOf)
  {
    default:
      doFootPrintsHuman(stateA);
      break;

    case PGFgnome:
      doFootPrintsGnome(stateA);
      break;

    case PGFbird:
      doFootPrintsBird(stateA);
      break;

    case PGFfelissilvestris:
      doFootPrintsFelisSilvestris(stateA);
      break;
  }
  current_state = &XWTikzDecoration::doFootPrintsRightState;
}

void XWTikzDecoration::doFootPrintsRightState(XWTikzState * stateA)
{
  stateA->scale(1, -1);
  stateA->shift(0, stateA->footSep / 2);
  stateA->scale(stateA->footLength, stateA->footLength);
  stateA->rotate(stateA->footAngle);
  switch (stateA->footOf)
  {
    default:
      doFootPrintsHuman(stateA);
      break;

    case PGFgnome:
      doFootPrintsGnome(stateA);
      break;

    case PGFbird:
      doFootPrintsBird(stateA);
      break;

    case PGFfelissilvestris:
      doFootPrintsFelisSilvestris(stateA);
      break;
  }
  current_state = &XWTikzDecoration::doFootPrintsLeftState;
}

void XWTikzDecoration::doFootPrintsBird(XWTikzState * stateA)
{
  stateA->concat(0,0.11,0.11,0,0.37,0.52);
  stateA->moveTo(-0.886, 1.652);
  stateA->curveTo(-0.4632, 2.669, -0.5092, 4.3712, -0.4512, 5.6734);
  stateA->curveTo(-0.4374, 5.9842, -0.3182, 6.0686, -0.2484, 5.644);
  stateA->curveTo(-0.0534, 4.459, 0.087, 3.0132, 0.5206, 1.8464);
  stateA->curveTo(0.714, 1.326, 0.692, 1.416, 1.0864, 1.759);
  stateA->curveTo(1.7662, 2.3504, 2.665, 3.1756, 3.2028, 4.02);
  stateA->curveTo(3.4708, 4.4408, 3.4258, 4.0814, 3.3366, 3.8708);
  stateA->curveTo(2.9068, 2.8562, 0.8502, 0.645, 0.4808, -0.0864);
  stateA->curveTo(0.0808, -0.8788, 0.1664, -2.1246, 0.0024, -2.7868);
  stateA->curveTo(-0.172, -3.4908, -0.2726, -3.704, -0.3812, -2.9558);
  stateA->curveTo(-0.5316, -1.9202, -0.88, -0.256, -1.332, 0.128);
  stateA->curveTo(-1.716, 0.4542, -3.7928, 2.3136, -4.5044, 3.3524);
  stateA->curveTo(-4.7618, 3.728, -4.709, 3.7864, -4.4212, 3.5818);
  stateA->curveTo(-3.4844, 2.9162, -2.6164, 1.654, -1.454, 1.3886);
  stateA->curveTo(-1.0876, 1.305, -1.0296, 1.3066, -0.886, 1.652);
  stateA->closePath();
}

void XWTikzDecoration::doFootPrintsFelisSilvestris(XWTikzState * stateA)
{
  stateA->concat(0, 0.1, 0.1, 0, 0.48, 0.5);
  stateA->moveTo(2.7312, -1.992);
  stateA->curveTo(1.8716, -1.4226, 2.1404, 0.0436, 0.9202, 0.0436);
  stateA->curveTo(-0.1146, 0.0436, -0.7386, 0.0178, -1.319, -1.0288);
  stateA->curveTo(-1.7922, -1.8822, -2.072, -1.5488, -2.9638, -1.968);
  stateA->curveTo(-3.9244, -2.4198, -4.4394, -3.348, -4.1136, -4.041);
  stateA->curveTo(-3.3862, -5.5882, -1.6156, -3.83, -1.1684, -4.312);
  stateA->curveTo(-0.5806, -4.9454, 0.5032, -4.8822, 1.1748, -4.2878);
  stateA->curveTo(1.5822, -3.9272, 2.6524, -5.0652, 3.3506, -4.1066);
  stateA->curveTo(3.839, -3.4356, 3.1192, -2.249, 2.7312, -1.992);
  stateA->closePath();

  stateA->moveTo(5.0576, 0.4996);
  stateA->curveTo(4.6056, 0.6536, 4.0596, 0.2516, 3.8382, -0.398);
  stateA->curveTo(3.6168, -1.0478, 3.8038, -1.6996, 4.2558, -1.8536);
  stateA->curveTo(4.7078, -2.0076, 5.2538, -1.6056, 5.4752, -0.9558);
  stateA->curveTo(5.6966, -0.3062, 5.5096, 0.3456, 5.0576, 0.4996);
  stateA->closePath();

  stateA->moveTo(3.1212, 3.5542);
  stateA->curveTo(2.5874, 3.6726, 1.975, 3.1756, 1.7538, 2.4442);
  stateA->curveTo(1.5324, 1.7128, 1.7858, 1.0238, 2.3196, 0.9052);
  stateA->curveTo(2.8534, 0.7868, 3.4658, 1.2838, 3.6872, 2.0152);
  stateA->curveTo(3.9084, 2.7466, 3.655, 3.4358, 3.1212, 3.5542);
  stateA->closePath();

  stateA->moveTo(-0.6182, 5.0456);
  stateA->curveTo(-1.2132, 5.0456, -1.6956, 4.3104, -1.6956, 3.4038);
  stateA->curveTo(-1.6956, 2.4972, -1.2132, 1.7618, -.06182, 1.7618);
  stateA->curveTo(-0.0232, 1.7618, 0.4592, 2.4972, 0.4592, 3.4038);
  stateA->curveTo(0.4592, 4.3104, -0.0232, 5.0456, -0.6182, 5.0456);
  stateA->closePath();

  stateA->moveTo(-4.1176, 1.8248);
  stateA->curveTo(-4.6334, 1.7534, -4.9806, 1.0312, -4.8928, 0.212);
  stateA->curveTo(-4.8052, -0.6072, -4.3158, -1.2136, -3.8, -1.1422);
  stateA->curveTo(-3.2842, -1.0708, -2.9372, -0.3486, -3.0248, 0.4706);
  stateA->curveTo(-3.1126, 1.2898, -3.6018, 1.8962, -4.1176, 1.8248);
  stateA->closePath();
}

void XWTikzDecoration::doFootPrintsGnome(XWTikzState * stateA)
{
  stateA->concat(0,-1,-1,0,1,0.8);
  stateA->scale(0.83333,0.83333);
  stateA->moveTo(0.86068, 0);
  stateA->curveTo(0.61466, 0, 0.56851, 0.35041, 0.70691, 0.35041);
  stateA->curveTo(0.84529, 0.35041, 1.10671, 0, 0.86068, 0);
  stateA->closePath();

  stateA->moveTo(0.45217, 0.30699);
  stateA->curveTo(0.52586, 0.31149, 0.60671, 0.02577, 0.46821, .04374);
  stateA->curveTo(0.32976, 0.06171, 0.37845, 0.30249, 0.45217, 0.30699);
  stateA->closePath();

  stateA->moveTo(0.11445, 0.48453);
  stateA->curveTo(0.16686, 0.46146, 0.1212, 0.23581, 0.03208, 0.29735);
  stateA->curveTo(-0.057, 0.3589, 0.06204, 0.50759, 0.11445, 0.48453);
  stateA->closePath();

  stateA->moveTo(0.26212, 0.36642);
  stateA->curveTo(0.32451, 0.3537, 0.32793, 0.09778, 0.21667, 0.14369);
  stateA->curveTo(0.10539, 0.18961, 0.19978, 0.37916, 0.26212, .36642);
  stateA->lineTo(0.26212, 0.36642);
  stateA->closePath();

  stateA->moveTo(0.58791, 0.93913);
  stateA->curveTo(0.59898, 1.02367, 0.52589, 1.06542, 0.45431, 1.01092);
  stateA->curveTo(0.22644, 0.83743, 0.8316, 0.75088, 0.79171, 0.51386);
  stateA->curveTo(0.7586, 0.31712, 0.15495, 0.37769, 0.08621, 0.68553);
  stateA->curveTo(0.03968, 0.89374, 0.27774, 1.1826, 0.52614, 1.1826);
  stateA->curveTo(0.64834, 1.1826, 0.78929, 1.07226, 0.81566, 0.93248);
  stateA->curveTo(0.8358, 0.82589, 0.57867, 0.8686, 0.58791, 0.93913);
  stateA->lineTo(0.58791, 0.93913);
  stateA->closePath();
}

void XWTikzDecoration::doFootPrintsHuman(XWTikzState * stateA)
{
  stateA->concat(0, 0.50, 0.5, 0, 0.48, 0.2);

  stateA->moveTo(0.20175, -0.72633);
  stateA->curveTo(0.16314, -0.88194, 0.07386, -1.0167, -0.08385, -1.0167);
  stateA->curveTo(-0.24153, -1.0167, -0.36942, -0.84897, -0.36942, -0.72633);
  stateA->curveTo(-0.36942, -0.60372, -0.32505, -0.54825, -0.27633, -0.51192);
  stateA->curveTo(-0.23937, -0.48435, -0.17031, -0.46539, -0.10611, -0.40323);
  stateA->curveTo(-0.04191, -0.3411, 0.00495, -0.27783, 0.00495, -0.14787);
  stateA->curveTo(0.00495, -0.01794, -0.08526, 0.12648, -0.22194, 0.22467);
  stateA->curveTo(-0.35862, 0.32286, -0.40863, 0.50226, -0.32703, 0.5979);
  stateA->curveTo(-0.24543, 0.69354, -0.00564, 0.66543, 0.17172, 0.53484);
  stateA->curveTo(0.34908, 0.40422, 0.39363, 0.27192, 0.37416, 0.10971);
  stateA->curveTo(0.35472, -0.0525, 0.26736, -0.46203, 0.20175, -0.72633);
  stateA->closePath();

  stateA->moveTo(-0.27138, 1.01451);
  stateA->curveTo(-0.21198, 1.01451, -0.1638, 0.94509, -0.1638, 0.8595);
  stateA->curveTo(-0.1638, 0.77388, -0.21198, 0.70446, -0.27138, 0.70446);
  stateA->curveTo(-0.33078, 0.70446, -0.37896, 0.77388, -0.37896, 0.8595);
  stateA->curveTo(-0.37896, 0.94509, -0.33078, 1.01451, -0.27138, 1.01451);
  stateA->closePath();

  stateA->moveTo(-0.05769, 0.90573);
  stateA->curveTo(-0.01659, 0.90573, 0.01674, 0.86445, 0.01674, 0.81354);
  stateA->curveTo(0.01674, 0.7626, -0.01659, 0.72132, -0.05769, 0.72132);
  stateA->curveTo(-0.09879, 0.72132, -0.13209, 0.7626, -0.13209, 0.81354);
  stateA->curveTo(-0.13209, 0.86445, -0.09879, 0.90573, -0.05769, 0.90573);
  stateA->closePath();

  stateA->moveTo(0.10497, 0.81636);
  stateA->curveTo(0.14214, 0.81096, 0.16722, 0.77193, 0.16104, 0.72918);
  stateA->curveTo(0.15483, 0.68646, 0.11964, 0.65619, 0.08247, 0.66159);
  stateA->curveTo(0.0453, 0.66699, 0.02019, 0.70602, 0.0264, 0.74874);
  stateA->curveTo(0.03261, 0.79149, 0.0678, 0.82176, 0.10497, 0.81636);
  stateA->closePath();

  stateA->moveTo(0.24054, 0.70842);
  stateA->curveTo(0.27513, 0.70842, 0.30318, 0.67686, 0.30318, 0.63795);
  stateA->curveTo(0.30318, 0.59904, 0.27513, 0.56751, 0.24054, 0.56751);
  stateA->curveTo(0.20598, 0.56751, 0.17793, 0.59904, 0.17793, 0.63795);
  stateA->curveTo(0.17793, 0.67686, 0.20598, 0.70842, 0.24054, 0.70842);
  stateA->closePath();

  stateA->moveTo(0.35973, 0.57501);
  stateA->curveTo(0.3918, 0.57501, 0.41781, 0.54852, 0.41781, 0.51585);
  stateA->curveTo(0.41781, 0.48318, 0.3918, 0.45669, 0.35973, 0.45669);
  stateA->curveTo(0.32769, 0.45669, 0.30168, 0.48318, 0.30168, 0.51585);
  stateA->curveTo(0.30168, 0.54852, 0.32769, 0.57501, 0.35973, 0.57501);
  stateA->closePath();
}
