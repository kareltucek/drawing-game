#include "DArea.cpp"
#include <gtkmm/main.h>

int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  DArea area;
  Gtk::Window window;
  window.set_default_size(800, 600);
  window.set_title("Type");
  window.add(area);
  area.show();

  Gtk::Main::run(window);

  return 0;
}
