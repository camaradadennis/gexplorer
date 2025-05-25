#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "graph_drawing_area.h"

#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <gtkmm/entry.h>
#include <gtkmm/filedialog.h>


class GraphDrawingArea;

class MainWindow final: public Gtk::ApplicationWindow
{
public:
    MainWindow(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

private:
    void open_file_dialog();

    void on_file_selection(const Glib::RefPtr<Gio::AsyncResult>&,
                           const Glib::RefPtr<Gtk::FileDialog>&);

    void on_search_activate();

    GraphDrawingArea* m_graph_area;
    Gtk::Entry* m_search_bar;
};

#endif // MAIN_WINDOW_H
