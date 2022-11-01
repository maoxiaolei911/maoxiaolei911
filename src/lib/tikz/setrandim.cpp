/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <time.h>
#include "setrandim.h"

double pstMyRand()
{
  static int rand_state = 1;

#define A 16807
#define M 0x7fffffff
#define Q 127773		/* M / A */
#define R 2836			/* M % A */
    
  rand_state = A * (rand_state % Q) - R * (rand_state / Q);
  if (rand_state <= 0)
		rand_state += M;
#undef A
#undef M
#undef Q
#undef R

  return rand_state / 2147483648;
}

double setrandim(double x,double y)
{
  time_t timep;
  time(&timep);
  struct tm *p = gmtime(&timep);
  int randomi = p->tm_hour * 60 + p->tm_min;
  randomi = randomi * 388 + p->tm_year;
  randomi = randomi * 31 + p->tm_mday;
  randomi = randomi * 97 + p->tm_mon;
  int countii = randomi / 127773;
  int count = countii;
  countii = countii * 127773;
  randomi = (randomi - countii) * 16807;
  count = count * 2836;
  randomi = randomi - count;
  if (randomi < 0)
    randomi = 2147483647;

  int tempcnta = (int)(y - x + 1);
  int tempcntb = 2147483645 / tempcnta;
  int ranval = (randomi - 1) / tempcntb;
  while (ranval >= tempcnta)
  {
    time(&timep);
    p = gmtime(&timep);
    randomi = p->tm_hour * 60 + p->tm_min;
    randomi = randomi * 388 + p->tm_year;
    randomi = randomi * 31 + p->tm_mday;
    randomi = randomi * 97 + p->tm_mon;
    countii = randomi / 127773;
    count = countii;
    countii = countii * 127773;
    randomi = (randomi - countii) * 16807;
    count = count * 2836;
    randomi = randomi - count;
    if (randomi < 0)
      randomi = 2147483647;

    ranval = (randomi - 1) / tempcntb;
  }

  return ranval + x;
}