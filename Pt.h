#ifndef POINT
#define POINT

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include "Common.h"
#include "Object.h"
#include <stdio.h>

struct Pt {
  double x;
  double y;
};

class Point : public MyObject
{
  private:
    double born;
    double dying;
    double mystep;
    bool isMinor;
    double myRadius;
  public:
    double x;
    double y;

    Point()
    {
    }

    Point(double time) 
    {
      myRadius = radius;
      mystep = step+Rand(randStep);
      satisfied = false;
      dead = false;
      growing = true;
      born = time;
      isMinor = false;
      GetRandPT(x, y);
    };

    Point(double time, double xx, double yy, double s = 0.0, bool isM = true) 
    {
      myRadius = radiusMinor;
      isMinor = isM;
      if(s == 0.0)
        mystep = step+Rand(randStep);
      else
        mystep = s;
      satisfied = false;
      dead = false;
      growing = true;
      born = time;
      x = xx;
      y = yy;
    };

    virtual void satisfy()
    {
      if(satisfied)
        return;
      this->satisfied = true;
      growing = false;
      dying = GetTime();
    };

    virtual void mouse(int xx, int yy)
    {
      if(Dist(x, y, xx, yy) < radius && !satisfied)
      {
        satisfy();
      }
    };

    virtual void update(double time)
    {
      if(time > born + lifeLength*mystep && !satisfied && allowDying)
      {
        satisfy();
      }
    };

    virtual void draw(const Cairo::RefPtr<Cairo::Context>& context)
    {
      double time = GetTime();
      if(time < born)
        return;
      double cf = 0.0;
      if(!satisfied)
      {
        if(time - born > mystep)
        {
          cf = 1.0;
          growing = false;
        }
        else
          cf = (time - born) / mystep;
      }
      else
      {
        if(time - dying > mystep)
        {
          cf = 0;
          dead = true;
        }
        else
          cf = 1.0 - (time - dying) / mystep;
      }
      if(!minorInvisible || !isMinor)
      {
        context->arc(x, y, myRadius*cf, 0, 2*M_PI);
        context->fill();
      }
    };
};

double Dist(const Point& a, const Point& b)
{
  return Dist(a.x, a.y, b.x, b.y);
}

double Dist(const Pt& a, const Pt& b)
{
  return Dist(a.x, a.y, b.x, b.y);
}


#endif 
