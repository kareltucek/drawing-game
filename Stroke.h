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
    double lastActivity;
    Point p1;
    Point p2;

    void Compute(double t, double& x, double& y)
    {
      x = p1.x*(1-t) + p2.x*t;
      y = p1.y*(1-t) + p2.y*t;
    }

  public:
    Stroke(double time, double ms = -1, double x1 = -1, double y1 = -1, double x2 = -1, double y2 = -1) 
    {
      mystep = ms == -1 ? step+Rand(randStep) : ms;
      if(x1 == -1)
        GetRandPT(x1, y1);
      p1 = Point(time, x1, y1, mystep, endpointsHidden);
      if(x2 == -1)
        GetRandPT(x1, y1, x2, y2);
      p2 = Point(time, x2, y2, mystep, endpointsHidden);

      born = GetTime();
      satisfied = false;
      dead = false;
      growing = true;
    };

    virtual bool satisfy()
    {
      if(satisfied)
        return false;
      p1.satisfy();
      p2.satisfy();
      satisfied = true;
      growing = false;
      dying = GetTime();
      return true;
    }

    virtual void update(double time)
    {
      if(time > born + lifeLength*mystep && !satisfied && allowDying)
      {
        satisfy();
      }
      if(((p1.satisfied && !p2.satisfied) || (!p1.satisfied && p2.satisfied)) && time > lastActivity + 1.0)
      {
        if(p1.satisfied)
          p1.reset(time);
        if(p2.satisfied)
          p2.reset(time);
      }
    };

    virtual bool mouse(int x, int y)
    {
      if(satisfied)
        return false;

      bool flipped = false;
      flipped |= p1.mouse(x, y);
      flipped |= p2.mouse(x, y);
      if(flipped)
        lastActivity = GetTime();
      satisfied = p1.satisfied && p2.satisfied;
      if(satisfied)
      {
        satisfied = true;
        dying = GetTime();
        return true;
      }
      return false;
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
      {
        dead = true;
        return;
      }
      double begin = 0.0;
      double end = 1.0;
      if(!satisfied)
      {
        if(time - born > strokestep)
        {
          end = 1.0;
          growing = false;
        }
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
