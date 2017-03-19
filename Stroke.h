#ifndef STROKE
#define STROKE

#include <cairomm/context.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <math.h>
#include "Pt.h"

class Stroke : public MyObject
{
  private:
    double mystep;
    double born;
    double dying;
    Point p1;
    Point p2;

    void Compute(double t, double& x, double& y)
    {
      x = p1.x*(1-t) + p2.x*t;
      y = p1.y*(1-t) + p2.y*t;
    }

public:
  Stroke(double time) 
  {
    mystep = step+Rand(randStep);
    double x, y;
    GetRandPT(x, y);
    p1 = Point(time, x, y, mystep, endpointsHidden);
    GetRandPT(x, y, x, y);
    p2 = Point(time, x, y, mystep, endpointsHidden);
    born = GetTime();
    satisfied = false;
    dead = false;
  };

  virtual void mouse(int x, int y)
  {
    if(satisfied)
      return;

    p1.mouse(x, y);
    p2.mouse(x, y);
    satisfied = p1.satisfied && p2.satisfied;
    if(satisfied)
    {
      satisfied = true;
      dying = GetTime();
    }
  };

  void drawStroke(const Cairo::RefPtr<Cairo::Context>& context, double begin, double end)
  {
    Pt p;
    Compute(begin, p.x, p.y);
    context->move_to(p.x, p.y);
    Compute(end, p.x, p.y);
    context->line_to(p.x, p.y);
    context->stroke();
  };

  virtual void draw(const Cairo::RefPtr<Cairo::Context>& context)
  {
    double strokestep = mystep/4;
    double time = GetTime();
    p1.draw(context);
    p2.draw(context);

    if(time < born)
      return;
    if(satisfied && time > dying + strokestep)
      return;
    double begin = 0.0;
    double end = 1.0;
    if(!satisfied)
    {
      if(time - born > strokestep)
        end = 1.0;
      else
        end = (time - born) / strokestep;
    }
    else
    {
      if(time - dying > strokestep)
      {
        begin = 1.0;
        dead = true;
      }
      else
        begin = (time - dying) / strokestep;
    }
    drawStroke(context, begin, end);
  };
};

#endif 
