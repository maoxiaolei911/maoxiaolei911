/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "XWRasterType.h"
#include "XWScreen.h"

static ScreenParams defaultParams = {
    SCREEN_TYPE_DISPERSED,	// type
    2,				// size
    2,				// dotRadius
    1.0,				// gamma
    0.0,				// blackThreshold
    1.0				// whiteThreshold
};

struct ScreenPoint 
{
    int x, y;
    int dist;
};

static int cmpDistances(const void *p0, const void *p1) 
{
    return ((ScreenPoint *)p0)->dist - ((ScreenPoint *)p1)->dist;
}

XWScreen::XWScreen(ScreenParams *params)
{
  uchar u;
  int black, white, i;

  if (!params) {
    params = &defaultParams;
  }

  // size must be a power of 2, and at least 2
  for (size = 2, log2Size = 1; size < params->size; size <<= 1, ++log2Size) ;

  switch (params->type) {

  case SCREEN_TYPE_DISPERSED:
    mat = (uchar *)malloc(size * size * sizeof(uchar));
    buildDispersedMatrix(size/2, size/2, 1, size/2, 1);
    break;

  case SCREEN_TYPE_CLUSTERED:
    mat = (uchar *)malloc(size * size * sizeof(uchar));
    buildClusteredMatrix();
    break;

  case SCREEN_TYPE_STOCHASTICCLUSTERED:
    // size must be at least 2*r
    while (size < (params->dotRadius << 1)) {
      size <<= 1;
      ++log2Size;
    }
    mat = (uchar *)malloc(size * size * sizeof(uchar));
    buildSCDMatrix(params->dotRadius);
    break;
  }

  sizeM1 = size - 1;

  // do gamma correction and compute minVal/maxVal
  minVal = 255;
  maxVal = 0;
  black = rasterRound((double)255.0 * params->blackThreshold);
  if (black < 1) {
    black = 1;
  }
  white = rasterRound((double)255.0 * params->whiteThreshold);
  if (white > 255) {
    white = 255;
  }
  for (i = 0; i < size * size; ++i) {
    u = rasterRound((double)255.0 *
		    rasterPow((double)mat[i] / 255.0, params->gamma));
    if (u < black) {
      u = (uchar)black;
    } else if (u >= white) {
      u = (uchar)white;
    }
    mat[i] = u;
    if (u < minVal) {
      minVal = u;
    } else if (u > maxVal) {
      maxVal = u;
    }
  }
}

XWScreen::XWScreen(XWScreen * screen)
{
  size = screen->size;
  sizeM1 = screen->sizeM1;
  log2Size = screen->log2Size;
  mat = (uchar *)malloc(size * size * sizeof(uchar));
  memcpy(mat, screen->mat, size * size * sizeof(uchar));
  minVal = screen->minVal;
  maxVal = screen->maxVal;
}

XWScreen::~XWScreen()
{
    if (mat)
       free(mat);
}

bool XWScreen::isStatic(uchar value)
{
    return value < minVal || value >= maxVal;
}

int XWScreen::test(int x, int y, uchar value)
{
    int xx, yy;
    xx = x & sizeM1;
    yy = y & sizeM1;
    return value < mat[(yy << log2Size) + xx] ? 0 : 1;
}

void XWScreen::buildClusteredMatrix()
{
  double *dist;
  double u, v, d;
  uchar val;
  int size2, x, y, x1, y1, i;

  size2 = size >> 1;

  // initialize the threshold matrix
  for (y = 0; y < size; ++y) {
    for (x = 0; x < size; ++x) {
      mat[(y << log2Size) + x] = 0;
    }
  }

  // build the distance matrix
  dist = (double *)malloc(size * size2 * sizeof(double));
  for (y = 0; y < size2; ++y) {
    for (x = 0; x < size2; ++x) {
      if (x + y < size2 - 1) {
	u = (double)x + 0.5 - 0;
	v = (double)y + 0.5 - 0;
      } else {
	u = (double)x + 0.5 - (double)size2;
	v = (double)y + 0.5 - (double)size2;
      }
      dist[y * size2 + x] = u*u + v*v;
    }
  }
  for (y = 0; y < size2; ++y) {
    for (x = 0; x < size2; ++x) {
      if (x < y) {
	u = (double)x + 0.5 - 0;
	v = (double)y + 0.5 - (double)size2;
      } else {
	u = (double)x + 0.5 - (double)size2;
	v = (double)y + 0.5 - 0;
      }
      dist[(size2 + y) * size2 + x] = u*u + v*v;
    }
  }

  // build the threshold matrix
  x1 = y1 = 0; // make gcc happy
  for (i = 0; i < size * size2; ++i) {
    d = -1;
    for (y = 0; y < size; ++y) {
      for (x = 0; x < size2; ++x) {
	if (mat[(y << log2Size) + x] == 0 &&
	    dist[y * size2 + x] > d) {
	  x1 = x;
	  y1 = y;
	  d = dist[y1 * size2 + x1];
	}
      }
    }
    // map values in [0, 2*size*size2-1] --> [1, 255]
    val = 1 + (254 * (2*i)) / (2*size*size2 - 1);
    mat[(y1 << log2Size) + x1] = val;
    val = 1 + (254 * (2*i+1)) / (2*size*size2 - 1);
    if (y1 < size2) {
      mat[((y1 + size2) << log2Size) + x1 + size2] = val;
    } else {
      mat[((y1 - size2) << log2Size) + x1 + size2] = val;
    }
  }

	if (dist)
  	free(dist);
}

void XWScreen::buildDispersedMatrix(int i, int j, int val, int delta, int offset)
{
  if (delta == 0) {
    // map values in [1, size^2] --> [1, 255]
    mat[(i << log2Size) + j] = 1 + (254 * (val - 1)) / (size * size - 1);
  } else {
    buildDispersedMatrix(i, j,
			 val, delta / 2, 4*offset);
    buildDispersedMatrix((i + delta) % size, (j + delta) % size,
			 val + offset, delta / 2, 4*offset);
    buildDispersedMatrix((i + delta) % size, j,
			 val + 2*offset, delta / 2, 4*offset);
    buildDispersedMatrix((i + 2*delta) % size, (j + delta) % size,
			 val + 3*offset, delta / 2, 4*offset);
  }
}

void XWScreen::buildSCDMatrix(int r)
{
  ScreenPoint *dots, *pts;
  int dotsLen, dotsSize;
  char *tmpl;
  char *grid;
  int *region, *dist;
  int x, y, xx, yy, x0, x1, y0, y1, i, j, d, iMin, dMin, n;

  //~ this should probably happen somewhere else
  srand(123);

  // generate the random space-filling curve
  pts = (ScreenPoint *)malloc(size * size * sizeof(ScreenPoint));
  i = 0;
  for (y = 0; y < size; ++y) {
    for (x = 0; x < size; ++x) {
      pts[i].x = x;
      pts[i].y = y;
      ++i;
    }
  }
  for (i = 0; i < size * size; ++i) {
    j = i + (int)((double)(size * size - i) *
		  (double)rand() / ((double)RAND_MAX + 1.0));
    x = pts[i].x;
    y = pts[i].y;
    pts[i].x = pts[j].x;
    pts[i].y = pts[j].y;
    pts[j].x = x;
    pts[j].y = y;
  }

  // construct the circle template
  tmpl = (char *)malloc((r+1)*(r+1) * sizeof(char));
  for (y = 0; y <= r; ++y) {
    for (x = 0; x <= r; ++x) {
      tmpl[y*(r+1) + x] = (x * y <= r * r) ? 1 : 0;
    }
  }

  // mark all grid cells as free
  grid = (char *)malloc(size * size * sizeof(char));
  for (y = 0; y < size; ++y) {
    for (x = 0; x < size; ++x) {
      grid[(y << log2Size) + x] = 0;
    }
  }

  // walk the space-filling curve, adding dots
  dotsLen = 0;
  dotsSize = 32;
  dots = (ScreenPoint *)malloc(dotsSize * sizeof(ScreenPoint));
  for (i = 0; i < size * size; ++i) {
    x = pts[i].x;
    y = pts[i].y;
    if (!grid[(y << log2Size) + x]) {
      if (dotsLen == dotsSize) {
	dotsSize *= 2;
	dots = (ScreenPoint *)realloc(dots, dotsSize * sizeof(ScreenPoint));
      }
      dots[dotsLen++] = pts[i];
      for (yy = 0; yy <= r; ++yy) {
	y0 = (y + yy) % size;
	y1 = (y - yy + size) % size;
	for (xx = 0; xx <= r; ++xx) {
	  if (tmpl[yy*(r+1) + xx]) {
	    x0 = (x + xx) % size;
	    x1 = (x - xx + size) % size;
	    grid[(y0 << log2Size) + x0] = 1;
	    grid[(y0 << log2Size) + x1] = 1;
	    grid[(y1 << log2Size) + x0] = 1;
	    grid[(y1 << log2Size) + x1] = 1;
	  }
	}
      }
    }
  }

	if (tmpl)
  free(tmpl);
  if (grid)
  free(grid);

  // assign each cell to a dot, compute distance to center of dot
  region = (int *)malloc(size * size * sizeof(int));
  dist = (int *)malloc(size * size * sizeof(int));
  for (y = 0; y < size; ++y) {
    for (x = 0; x < size; ++x) {
      iMin = 0;
      dMin = distance(dots[0].x, dots[0].y, x, y);
      for (i = 1; i < dotsLen; ++i) {
	d = distance(dots[i].x, dots[i].y, x, y);
	if (d < dMin) {
	  iMin = i;
	  dMin = d;
	}
      }
      region[(y << log2Size) + x] = iMin;
      dist[(y << log2Size) + x] = dMin;
    }
  }

  // compute threshold values
  for (i = 0; i < dotsLen; ++i) {
    n = 0;
    for (y = 0; y < size; ++y) {
      for (x = 0; x < size; ++x) {
	if (region[(y << log2Size) + x] == i) {
	  pts[n].x = x;
	  pts[n].y = y;
	  pts[n].dist = distance(dots[i].x, dots[i].y, x, y);
	  ++n;
	}
      }
    }
    qsort(pts, n, sizeof(ScreenPoint), &cmpDistances);
    for (j = 0; j < n; ++j) {
      // map values in [0 .. n-1] --> [255 .. 1]
      mat[(pts[j].y << log2Size) + pts[j].x] = 255 - (254 * j) / (n - 1);
    }
  }

	if (pts)
  free(pts);
  if (region)
  free(region);
  if (dist)
  free(dist);
	if (dots)
  free(dots);
}

int XWScreen::distance(int x0, int y0, int x1, int y1)
{
  int dx0, dx1, dx, dy0, dy1, dy;

  dx0 = abs(x0 - x1);
  dx1 = size - dx0;
  dx = dx0 < dx1 ? dx0 : dx1;
  dy0 = abs(y0 - y1);
  dy1 = size - dy0;
  dy = dy0 < dy1 ? dy0 : dy1;
  return dx * dx + dy * dy;
}
