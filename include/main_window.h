#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "graph_drawing_area.h"

#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>

class MainWindow final: public Gtk::ApplicationWindow
{
public:
    MainWindow(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

private:
    GraphDrawingArea m_graph_drawing_area;
};

#endif // MAIN_WINDOW_H
