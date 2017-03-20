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
  public:
    double mystep;
    double pointRadius;
  private:
    bool reversed;
    int segments;
    double born;
    double dying;
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
    Curve(double time, std::vector<Pt> pts = {}, int rad = 0, bool reverse = false) : points()
  {
    reversed = reverse;
    mystep = step + Rand(randStep);
    born = time;
    satisfied = false;
    dead = false;
    growing = true;
    double x, y;
    if(rad == 0)
    {
      rad = pow(Rand(4) + 1, 2) * 30;
    }
    pointRadius = rad;
    GetRandPT(x, y);
    for(auto pt : pts)
    {
      points.push_back(pt);
      x = pt.x;
      y = pt.y;
    }
    for(int i = points.size(); i < deg; i++)
    {
      GetRandPT(x, y, x, y, rad);
      points.push_back((struct Pt){x, y});
    }
    if(reverse)
      std::reverse(points.begin(), points.end());
    for(int i = 0; i < samples; i++)
    {
      double xx, yy;
      Compute(i*(1.0/(samples - 1.0)), xx, yy);
      handles.push_back(new Point(time + i*mystep/samples, xx, yy, mystep, !endpointsHidden && i != 0 && i != samples - 1));
    }
    segments = (int)Length()/8;
  };

    std::vector<Pt> GetDir()
    {
      std::vector<Pt> res;
      int lastIdx = reversed ? 0 : points.size() - 1;
      int prevIdx = reversed ? 1 : points.size() - 2;
      double x1 = points[prevIdx].x;
      double x2 = points[lastIdx].x;
      double y1 = points[prevIdx].y;
      double y2 = points[lastIdx].y;
      double nx = x2 - (x1 - x2);
      double ny = y2 - (y1 - y2);
      while(nx < 0 || ny < 0 || nx > width || ny > height)
      {
        nx = (nx + x2)/2;
        ny = (ny + y2)/2;
      }
      res.push_back(points[lastIdx]);
      res.push_back((struct Pt){nx, ny});
      return res;
    }

    virtual void satisfy()
    {
      if(satisfied)
        return;
      for(int i = 0; i < samples; i++)
        handles[i]->satisfy();
      satisfied = true;
      dying = GetTime();
      growing = false;
    }

    virtual void update(double time)
    {
      if(time > born + lifeLength*mystep && !satisfied && allowDying)
      {
        satisfy();
      }
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
        growing = false;
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
      {
        dead = true;
        return;
      }
      double begin = 0.0;
      double end = 1.0;
      if(!satisfied)
      {
        if(time - born > mystep)
        {
          end = 1.0;
          growing = false;
        }
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
