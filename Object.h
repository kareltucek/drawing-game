#ifndef OBJECT
#define OBJECT

#include <gtkmm/button.h>
#include <gtkmm/window.h>

class MyObject
{
public:
    bool satisfied;
    bool dead;

    virtual void mouse(int x, int y){};
    virtual void draw(const Cairo::RefPtr<Cairo::Context>& context){};
};

#endif 
