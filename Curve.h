#ifndef CURVE
#define CURVE

#include <cairomm/context.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <math.h>
#include "Pt.h"

static const int deg = 5;

class Curve : public MyObject
{
  private:
    int segments;
    double born;
    double dying;
    double mystep;
    std::vector<Pt> points;
    std::vector<Point*> handles;

  static double Base(int idx, double t)
  {
    static const int cfs[deg] = {1, 4, 6, 4, 1};
    return cfs[idx]*pow(t, idx)*pow(1-t, deg-1-idx);
  };

  double Length()
  {
    double acum = 0;
    for(int i = 0; i < points.size()-1; i++)
      acum += Dist(points[i], points[i+1]);
  };

  void Compute(double t, double& x, double& y)
  {
    x = 0.0;
    y = 0.0;
    for(int i = 0; i < deg; i++)
    {
      x += Base(i, t)*points[i].x;
      y += Base(i, t)*points[i].y;
    }
  };

public:
  Curve(double time) : points()
  {
    mystep = step + Rand(randStep);
    born = GetTime();
    satisfied = false;
    dead = false;
    double x, y;
    int rad = Rand(2000);
    GetRandPT(x, y);
    for(int i = 0; i < deg; i++)
    {
      GetRandPT(x, y, x, y, rad);
      points.push_back((struct Pt){x, y});
    }
    for(int i = 0; i < samples; i++)
    {
      double xx, yy;
      Compute(i*(1.0/(samples - 1.0)), xx, yy);
      handles.push_back(new Point(time + i*mystep/samples, xx, yy, mystep, !endpointsHidden && i != 0 && i != samples - 1));
    }
    segments = (int)Length()/8;
  };

  virtual void mouse(int x, int y)
  {
    if(satisfied)
      return;
    bool unsatisfied = false;
    for(int i = 0; i < samples; i++)
    {
      handles[i]->mouse(x, y);
      unsatisfied |= !handles[i]->satisfied;
    }
    if(!unsatisfied)
    {
      satisfied = true;
      dying = GetTime();
    }
  };

  void drawCurve(const Cairo::RefPtr<Cairo::Context>& context, double begin, double end, double segments)
  {
    Pt p;
    double s = 1.0/(double)segments;
    double t = begin;
    Compute(t, p.x, p.y);
    context->move_to(p.x, p.y);
    while(t < end - s)
    {
      t += s;
      Compute(t, p.x, p.y);
      context->line_to(p.x, p.y);
    }
    Compute(end, p.x, p.y);
    context->line_to(p.x, p.y);
    context->stroke();
  };

  virtual void draw(const Cairo::RefPtr<Cairo::Context>& context)
  {
    double time = GetTime();
    for(int i = 0; i < samples; i++)
    {
      /*
      context->arc(points[i].x, points[i].y, 4, 0, 2*M_PI);
      context->fill();
      printf("point %d is %d %d\n",i, points[i].x, points[i].y);
      */
      handles[i]->draw(context);
    }


    if(time < born)
      return;
    if(satisfied && time > dying + mystep)
      return;
    double begin = 0.0;
    double end = 1.0;
    if(!satisfied)
    {
      if(time - born > mystep)
        end = 1.0;
      else
        end = (time - born) / mystep;
    }
    else
    {
      if(time - dying > mystep)
      {
        begin = 1.0;
        dead = true;
      }
      else
        begin = (time - dying) / mystep;
    }
    drawCurve(context, begin, end, segments);
  };
};

#endif 
