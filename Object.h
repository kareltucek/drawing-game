#ifndef OBJECT
#define OBJECT

#include <gtkmm/button.h>
#include <gtkmm/window.h>

class MyObject
{
  public:
    bool satisfied;
    bool dead;
    bool growing;

    virtual void satisfy(){};
    virtual bool animating(){ return (satisfied && !dead) || growing; };
    virtual void update(double time){};
    virtual void mouse(int x, int y){};
    virtual void draw(const Cairo::RefPtr<Cairo::Context>& context){};
};

#endif 
