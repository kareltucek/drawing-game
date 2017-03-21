#ifndef PCURVE
#define PCURVE

#include <vector>
#include "Curve.h"

class PolyCurve 
{
  public:
    static std::vector<Curve*> generate(double time, int num)
    {
      std::vector<Curve*> ptrs;
      Curve* last = new Curve(time);
      ptrs.push_back(last);
      double lastTime = time;
      for(int i = 1; i < num; i++)
      {
        double rad = last->pointRadius;
        //rad = 0; //if we want segment size variation
        std::vector<Pt> direction = last->GetDir();
        //time += last->mystep;
        last = new Curve(time, last->mystep, direction, rad, i % 2);
        ptrs.push_back(last);
      }
      return ptrs;
    };
};

#endif
