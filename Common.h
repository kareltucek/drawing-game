#ifndef MYDCOM
#define MYDCOM

#include <sys/time.h>
#include <stdlib.h>  
#include <math.h>

static int width = 100;
static int height = 100;
static double step = 3.0;
static double randStep = 0.0;
static double radius = 20.0;
static double radiusMinor = 20.0;
static int samples = 4;
static int wantLiving = 3;
static int tail = 50;
static double tailFallOff = 0.1;
static double minorInvisible = true;
static double endpointsHidden = false;

int Rand(int max)
{
  if(max == 0)
    max = 1;
  return rand()%max;
}

double Dist(double x, double y, double xx, double yy)
{
  return sqrt((x - xx)*(x - xx) + (y - yy)*(y - yy));
}

double GetTime()
{
	struct timeval tim;
	gettimeofday(&tim, NULL);
	return tim.tv_sec+(tim.tv_usec/1000000.0);
}

void GetRandPT(double& x, double& y)
{
  x = Rand(width);
  y = Rand(height);
}

void SetAlpha(const Cairo::RefPtr<Cairo::Context>& context, double alpha)
{
  context->set_source_rgba(0.5, 1.0, 0.5, alpha);
}

#define MINOP(a, b) a = (a) < (b) ? (a) : (b);

void GetRandPT(double x, double y, double& xx, double& yy, double mr = 500)
{
  int w = Rand(2)*5;
  int v = 6 - w;
  GetRandPT(xx, yy);

    xx = (w*x + v*xx)/6;
    yy = (w*y + v*yy)/6;
  /*
  MINOP(mr, x);
  MINOP(mr, y);
  MINOP(mr, width-x);
  MINOP(mr, height-y);
  mr -= radius;
  if(mr < radius) mr = radius;

  xx = x + Rand(2*mr) - mr;
  yy = y + Rand(2*mr) - mr;*/
}

#endif
