#ifndef DAREA
#define DAREA

#include <set>
#include <list>
#include "Object.h"
#include "Pt.h"
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/drawingarea.h>

class DArea : public Gtk::DrawingArea
{
  private:
    bool isDown;
    double lastRedraw;
    double lastGenerated;
    double lastTailCut;
    int objId;
    std::set<MyObject*> objects;

    std::list<Pt> tail;

    int GetActive();
    void Spawn();
    void Kill();
    bool Animating();
    int GetGrowing();
    int CountDots();

    void Register(const std::vector<MyObject*>&);

    void ProcessMouse(double x, double y);
    void Casteljau(double x, double y, double x2, double y2, double x3, double y3);

  public:
    DArea();
    ~DArea();

    void Ready();

    void Resize(Gtk::Allocation& alloc);

    bool threadexit;

    bool KeyPress(GdkEventKey * event);
    bool Mouse(GdkEventMotion* motion_event);
    bool MouseDown(GdkEventButton *event);
    bool MouseUp(GdkEventButton *event);
    bool NotifyMouse(int x, int y);

    void ComputeGame();

  protected:
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

  private:
    //  boost::thread * TimeThread;
    void TimeLoop();

    void RedrawGame(const Cairo::RefPtr<Cairo::Context>& context);

};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H
