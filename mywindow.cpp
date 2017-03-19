#include "mywindow.h"
#include "DArea.h"

MyWindow::MyWindow()
{
	DArea area;
	this->add(area);
    //this->signal_key_press_event().connect(sigc::mem_fun(area, &DArea::KeyPress), false);
	this->show_all();
}

bool MyWindow::KeyMotion(GdkEventMotion * event)
{
	return true;
}

bool MyWindow::KeyPress(GdkEventKey * event)
{
	return true;
}
