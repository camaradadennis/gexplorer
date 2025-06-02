#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "graph_drawing_area.h"
#include "searchfield.h"

#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/filedialog.h>


class GraphDrawingArea;

class MainWindow final: public Gtk::ApplicationWindow
{
public:
    MainWindow(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

private:
    void on_plot_btn_clicked();
    void open_file_dialog();

    void on_file_selection(const Glib::RefPtr<Gio::AsyncResult>&,
                           const Glib::RefPtr<Gtk::FileDialog>&);

    GraphDrawingArea* m_graph_area;
    SearchField* m_src_field;
    SearchField* m_tgt_field;

    Gtk::Button* m_plot_btn;
};

#endif // MAIN_WINDOW_H
