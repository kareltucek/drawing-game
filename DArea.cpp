//#include <iostream>
#include <cairomm/context.h>
#include <gtkmm.h>
#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <dirent.h>
#include <sys/time.h>
#include "Common.h"
#include "DArea.h"
#include "Curve.h"
#include "Stroke.h"
#include "Pt.h"
#include "PolyCurve.h"

DArea::DArea() : objects()
{
  isDown = false;
  srand(time(NULL));
  lastRedraw = GetTime();
  lastGenerated = lastRedraw;

  this->add_events( Gdk::KEY_PRESS_MASK );
  this->add_events( Gdk::BUTTON_PRESS_MASK );
  this->add_events( Gdk::BUTTON_RELEASE_MASK );
  this->add_events( Gdk::POINTER_MOTION_MASK );

  this->signal_key_press_event().connect(sigc::mem_fun(*this, &DArea::KeyPress), false);
  this->signal_motion_notify_event().connect(sigc::mem_fun(*this, &DArea::Mouse), false);
  this->signal_button_press_event().connect(sigc::mem_fun(*this, &DArea::MouseDown), false);
  this->signal_button_release_event().connect(sigc::mem_fun(*this, &DArea::MouseUp), false);

  this->show();
}


DArea::~DArea()
{
  threadexit = true;
}

bool DArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  RedrawGame(cr);
  this->lastRedraw = GetTime();
  return true;
}

bool DArea::KeyPress(GdkEventKey *event)
{
  printf("key caught!\n");
  switch (event->keyval)
  {
    case GDK_KEY_plus:
      step += 0.1;
      break;
    case GDK_KEY_minus:
      step -= 0.1;
      break;
  }
  printf("step is %f\n", step);
  return true;
}

bool DArea::MouseDown(GdkEventButton *event)
{
  isDown = true;
  NotifyMouse(event->x, event->y);
  return true;
}

bool DArea::MouseUp(GdkEventButton *event)
{
  isDown = false;
  stroke.clear();
  return true;
}

bool DArea::NotifyMouse(int x, int y)
{
  std::vector<MyObject*> rem;
  if(isDown)
  {
    for(std::set<MyObject*>::iterator itr = objects.begin(); itr != objects.end(); itr++)
    {
      (*itr)->mouse(x, y);
      if((*itr)->dead)
        rem.push_back(*itr);
    }
    for(std::vector<MyObject*>::iterator itr = rem.begin(); itr != rem.end(); itr++)
    {
      objects.erase(*itr);
    }
  }

  if(isDown)
  {
    stroke.push_front((struct Pt){x, y});
    if(stroke.size() > tail)
    {
      stroke.pop_back();
    }
  }
}

bool DArea::Mouse(GdkEventMotion *event)
{
  NotifyMouse(event->x, event->y);
}

int DArea::GetActive()
{
  int cnt = 0; 
  for(std::set<MyObject*>::iterator itr = objects.begin(); itr != objects.end(); itr++)
  {
    if((*itr)->satisfied == false)
      cnt++;
  }
  return cnt;
}

bool DArea::Animating()
{
  int cnt = 0; 
  for(std::set<MyObject*>::iterator itr = objects.begin(); itr != objects.end(); itr++)
  {
    if((*itr)->animating())
      return true;
  }
  return false;
}

int DArea::CountDots()
{
  int cnt = 0; 
  for(std::set<MyObject*>::iterator itr = objects.begin(); itr != objects.end(); itr++)
  {
    if(dynamic_cast<Point*>(*itr))
      cnt++;
  }
  return cnt;
}

void DArea::Spawn()
{
  if(GetActive() > maxLiving)
    return;
  double time = GetTime();
  switch(Rand(4))
  {
    case 0:
    case 1:
      if(!curvesOnly)
      {
        objects.insert(new Point(time));
        break;
      }
    case 2:
      if(!curvesOnly)
      {
        objects.insert(new Stroke(time));
        break;
      }
    case 3:
      static bool g = false;
      int r1 = Rand(maxPolyLen);
      int r2 = Rand(maxPolyLen);
      int deg = MIN(r1, r2);
      std::vector<Curve*> ptrs = PolyCurve::generate(time, deg);
      for(Curve* ptr : ptrs)
      {
        objects.insert(ptr);
      }
      break;
  }
  lastGenerated = time;
}

void DArea::Kill()
{
  if(allowDying)
  {
    for(std::set<MyObject*>::iterator itr = objects.begin(); itr != objects.end(); itr++)
    {
      if(!(*itr)->satisfied)
      {
        (*itr)->satisfy();
        return;
      }
    }
  }
}

void DArea::ComputeGame()
{
  double time = GetTime();

  if( !Animating() && animationLoop)
  {
    if( GetActive() < maxLiving )
      Spawn();
    else
      Kill();
  }

  if( GetActive() < wantLiving || (GetTime() > lastGenerated + 8 * step))
  {
    Spawn();
  }

  if( CountDots() < 2 )
  {
    objects.insert(new Point(time));
    lastGenerated = time;
  }

  if(stroke.size() > tail || (GetTime() > lastTailCut + tailFallOff && stroke.size() > 2))
  {
    stroke.pop_back();
    lastTailCut = time;
  }
}

void DArea::RedrawGame(const Cairo::RefPtr<Cairo::Context>& context)
{
  double time = GetTime();
  Gtk::Allocation allocation = get_allocation();
  width = allocation.get_width();
  height = allocation.get_height();

  /*
     static bool g = true;
     if(g)
     {
     g = false;
     objects.insert(new Curve(lastGenerated + step));
     }
     */

  context->set_source_rgb(0.0, 0.0, 0.0);
  context->paint();

  SetAlpha(context, 1.0);

  for(std::set<MyObject*>::iterator itr = objects.begin(); itr != objects.end(); itr++)
  {
    (*itr)->update(time);
    (*itr)->draw(context);
  }
  //DrawBackground(context);
  //SetDefContext(context);
  //SetContext(context);

  context->stroke();

  if(!stroke.empty())
  {
    Pt pt = *stroke.begin();
    context->move_to(pt.x, pt.y);
    for(std::list<Pt>::iterator itr = stroke.begin(); itr != stroke.end(); itr++)
    {
      pt = (*itr);
      context->line_to(pt.x, pt.y);
    }
    context->stroke();
  }

  this->ComputeGame();
  this->queue_draw();
}

/*
   void DArea::DrawDots(double fade, double addmod, bool pulse, const Cairo::RefPtr<Cairo::Context>& context, bool ealpha, int mask)
   {
   Gtk::Allocation allocation = get_allocation();
   int line = GetCalibrationHeight(addmod);
//context->line_to(allocation.get_width()-5, line);
//context->stroke();
double alpha = hfs->fadecol*pow((sin(GetTime()*hfs->pulsemod)+1)/2, 0.25)*fade;
if(!pulse)
alpha = fade;
if(!ealpha)
context->set_source_rgb(alpha, alpha, alpha);
else
context->set_source_rgba(1,1,1,alpha);
if(mask & 1)
{
context->move_to(5, line);
context->arc(5, line, 3, 0, 2*M_PI);
context->fill();
}
if(mask & 2)
{
context->move_to(allocation.get_width()-7, line);
context->arc(allocation.get_width()-7, line, 3, 0, 2*M_PI);
context->fill();
}
}

*/


