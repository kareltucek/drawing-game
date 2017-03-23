#ifndef MYDCOM
#define MYDCOM

#include <sys/time.h>
#include <stdlib.h>  
#include <math.h>

static  int width = 100;
static  int height = 100;
static const double step = 3.0;
static const double randStep = 3.0;
static const double radiusBig = 10.0;
static const double radiusMinor = radiusBig/2;
static const double radius = radiusBig * 1.5;
static const int samples = 10;
static const int wantLiving = 8;
static const int maxLiving = 15;
static const int tail = 500;
//static const double tailFallOff = 0.1; //computed implicitly as 1/num
static const double minorInvisible = true;
static const double endpointsHidden = false;
static const bool allowDying = true; //if true, objects die by themselves after lifeLength*step
static const double lifeLength = 1000.0; //times mystep - high value - turn dying off
static const bool animationLoop = false; //keeps things in motion via killing and spawning (if allowDying is true)
static const int maxPolyLen = 4;
static const bool curvesOnly = false;
static const bool bezierTailInterpolation = true;
static const double spawnDelayVariation = 1.0;

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

double Len(double x, double y)
{
  return sqrt(x*x + y*y);
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
  GetRandPT(xx, yy);

  while(Dist(x, y, xx, yy) > mr)
  {
    xx = (x + xx)/2;
    yy = (y + yy)/2;
  }

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
