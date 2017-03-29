#ifndef FIELD
#define FIELD

#include <vector>
#include "Common.h"
#include "Curve.h"

class Field 
{
  public:
    static std::vector<MyObject*> generate(double time, int num)
    {
      std::vector<MyObject*> ptrs;
      double x, y;
      double xx, yy;
      double xd, yd;
      GetRandPT(x, y);
      GetRandPT(x, y, xx, yy, 200);
      GetRandDir(x, y, xd, yd, 20);
      for(int i = 0; i < num; i++)
      {
        MyObject* last = new Stroke(time + step/num*i, step/2, x + xd*i, y + yd*i, xx + xd*i, yy + yd*i);
        ptrs.push_back(last);
      }
      return ptrs;
    };
};

#endif
