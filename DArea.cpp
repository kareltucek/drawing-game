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
#include "Field.h"

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
      //step += 0.1;
      break;
    case GDK_KEY_minus:
      //step -= 0.1;
      break;
  }
  printf("step is %f\n", step);
  return true;
}

bool DArea::MouseDown(GdkEventButton *event)
{
  tail.clear();
  isDown = true;
  NotifyMouse(event->x, event->y);
  return true;
}

bool DArea::MouseUp(GdkEventButton *event)
{
  isDown = false;
  //tail.clear();
  return true;
}

bool DArea::NotifyMouse(int x, int y)
{
  static int prevx = x;
  static int prevy = y;
  static int lastx = x;
  static int lasty = y;
  static bool lastDown = isDown;
  static int notDown = 0;

  if(bezierTailInterpolation)
  {
    if(isDown || lastDown)
      Casteljau((prevx+lastx)/2, (prevy+lasty)/2, lastx, lasty, (lastx+x)/2, (lasty+y)/2);
  }
  else
  {
    if(isDown)
    {
      if(Dist(x,y, lastx, lasty) > radius/2)
        NotifyMouse((lastx + x)/2, (lasty + y)/2);
      else
        ProcessMouse(x, y);
    }
  }

  prevx = lastx;
  prevy = lasty;
  lastx = x;
  lasty = y;
  lastDown = isDown;
  if(isDown)
    notDown = 0;
  else
    notDown++;
  if(notDown > 1)
    tail.clear();
}

void DArea::Casteljau(double x, double y, double x2, double y2, double x3, double y3)
{
  if(Dist(x,y, x3, y3) < radius/2)
  {
    ProcessMouse((x+x3)/2, (y+y3)/2);
  }
  else
  {
    double secx = (x+x2)/2;
    double secy = (y+y2)/2;
    double fourthx = (x2+x3)/2;
    double fourthy = (y2+y3)/2;
    double midx = (secx+fourthx)/2;
    double midy = (secy+fourthy)/2;
    Casteljau(x, y, secx, secy, midx, midy);
    Casteljau(midx, midy, fourthx, fourthy, x3, y3);
  }
}

void DArea::ProcessMouse(double x, double y)
{
  std::vector<MyObject*> rem;
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

  tail.push_front((struct Pt){(double)x, (double)y});
  if(tail.size() > maxTailLen)
  {
    tail.pop_back();
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

int DArea::GetGrowing()
{
  int cnt = 0; 
  for(std::set<MyObject*>::iterator itr = objects.begin(); itr != objects.end(); itr++)
  {
    if((*itr)->satisfied == false && (*itr)->animating())
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
    if((*itr)->satisfied == false && dynamic_cast<Point*>(*itr))
      cnt++;
  }
  return cnt;
}

void DArea::Register(const std::vector<MyObject*>& pts)
{
  for(MyObject* ptr : pts)
    objects.insert(ptr);
}

void DArea::Spawn()
{
  if(GetActive() > maxLiving)
    return;
  double time = GetTime();
  switch(Rand(5))
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
      Register(Field::generate(time, 4+Rand(10)));
      break;
    case 4:
      static bool g = false;
      int r1 = Rand(maxPolyLen);
      int r2 = Rand(maxPolyLen);
      int deg = MIN(r1, r2);
      Register(PolyCurve::generate(time, deg));
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
  static double spawnNext = GetTime();
  static bool spawnPlanned = false;
  double time = GetTime();

  bool animating = Animating();

  if( !animating && animationLoop)
  {
    if( GetActive() < maxLiving )
      Spawn();
    else
      Kill();
  }

  if(spawnNext < time)
  {
    Spawn();
    spawnNext = lastGenerated + 8 * step;
    spawnPlanned = false;
  }

  int active = GetActive();
  int dotCount = CountDots();
  bool growing = GetGrowing();
  if( active < wantLiving/2 || (active < wantLiving && !growing))
  {
    if(!spawnPlanned)
    {
      spawnPlanned = true;
      double delay = spawnDelayVariation*0.001*Rand(1000);
      MINOP(spawnNext, time+delay);
    }
  }
  if( dotCount < 3*wantLiving/4 && !growing)
  {
    objects.insert(new Point(time));
    lastGenerated = time;
  }
  else if( dotCount < wantLiving/3 )
  {
    objects.insert(new Point(time));
    lastGenerated = time;
  }

  static double tailDelBuffer = 0;
  int tailLen = 10 < tail.size() ? tail.size() : 10;
  if(tail.size() > maxTailLen || (GetTime() > lastTailCut + 1.0/tailLen && tail.size() > 1))
  {
    
    for(tailDelBuffer += tail.size()/30+1; tailDelBuffer > 1.0; tailDelBuffer -= 1)
      tail.pop_back();
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

  if(!tail.empty())
  {
    Pt pt = *tail.begin();
    context->move_to(pt.x, pt.y);
    for(std::list<Pt>::iterator itr = tail.begin(); itr != tail.end(); itr++)
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


