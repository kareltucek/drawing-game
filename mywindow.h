#ifndef MYWINDOW_H
#define MYWINDOW_H
#include <gtkmm.h>

class MyWindow : public Gtk::Window
{
public:
	MyWindow();
	bool KeyPress(GdkEventKey * event);
	bool Mouse(GdkEventMotion * event);
};

#endif // MYWINDOW_H
