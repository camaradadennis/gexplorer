#include "graph.h"
#include "graph_drawing_area.h"
#include "main_window.h"

#include <gtkmm/menubutton.h>
#include <gtkmm/error.h>

#include <gtkmm/eventcontrollerscroll.h>

#include <iostream>

#define THROW_INVALID_ID(id) \
    { throw Gtk::BuilderError(Gtk::BuilderError::INVALID_ID, \
            "no object named \"" id "\" in ui definition"); }


static bool called_scroll(double dx, double dy)
{
    std::cout << "Scroll (dx, dy): (" << dx << ", " << dy << ")" << std::endl;
    return true;
}


MainWindow::MainWindow(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& builder)
    : Gtk::ApplicationWindow(cobject),
    m_graph_area{
        Gtk::Builder::get_widget_derived<GraphDrawingArea>(builder, "graph-area")
    }
{
    if (!m_graph_area)
        THROW_INVALID_ID("graph_area");

    auto menu_builder = Gtk::Builder::create_from_resource(
        "/io/github/camaradadennis/gexplorer/menu.ui");

    auto menu_model = std::dynamic_pointer_cast<Gio::MenuModel>(
        menu_builder->get_object("menu"));

    if (!menu_model)
        THROW_INVALID_ID("menu");

    auto menu_btn = builder->get_widget<Gtk::MenuButton>("menu-btn");

    if (!menu_btn)
        THROW_INVALID_ID("menu-btn");

    menu_btn->set_menu_model(menu_model);

    add_action("load", sigc::mem_fun(*this, &MainWindow::open_file_dialog));

    auto mouse_scroll = Gtk::EventControllerScroll::create();
    mouse_scroll->signal_scroll().connect(
        sigc::ptr_fun(called_scroll), true);
    add_controller(mouse_scroll);
}


void MainWindow::open_file_dialog()
{
    auto file_dialog = Gtk::FileDialog::create();
    file_dialog->set_title("Select new graph file");

    file_dialog->open(*this, sigc::bind(
        sigc::mem_fun(*this, &MainWindow::on_file_selection),
        file_dialog
    ));
}


void MainWindow::on_file_selection(
        const Glib::RefPtr<Gio::AsyncResult>& result,
        const Glib::RefPtr<Gtk::FileDialog>& dialog)
{
    try
    {
        auto file = dialog->open_finish(result);
        file->load_contents_async(sigc::bind(sigc::mem_fun(
            *this, &MainWindow::load_file_contents),
            file
        ));
    }
    catch (const Gtk::DialogError& err)
    {
        // not actually an error
        std::cout << "No file selected: " << err.what() << std::endl;
    }
}


void MainWindow::load_file_contents(
        const Glib::RefPtr<Gio::AsyncResult>& result,
        const Glib::RefPtr<Gio::File>& file)
{
    char* contents = nullptr;
    gsize length = 0;

    if (file->load_contents_finish(result, contents, length))
    {
        auto g = Graph::from(contents);
        m_graph_area->set_graph(std::move(g));
    }
    else
        std::cout << "Not ok" << std::endl;

    g_free(contents);
}
