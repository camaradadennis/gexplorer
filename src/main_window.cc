#include "osm_parser.h"
#include "graph_drawing_area.h"
#include "main_window.h"

#include <gtkmm/alertdialog.h>
#include <gtkmm/error.h>
#include <gtkmm/menubutton.h>


#define THROW_INVALID_ID(id) \
    { throw Gtk::BuilderError(Gtk::BuilderError::INVALID_ID, \
            "no object named \"" id "\" in ui definition"); }


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

    m_search_bar = builder->get_widget<Gtk::Entry>("search-bar");
    m_search_bar->signal_activate().connect(
        sigc::mem_fun(*this, &MainWindow::on_search_activate));
}


void MainWindow::on_search_activate()
{
    auto buffer = m_search_bar->get_buffer();
    auto text = buffer->get_text();
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

        std::string fpath = file->get_path();

        auto g{ osm_parser::parse(fpath) };
        m_graph_area->set_graph(std::move(g));
    }
    catch (const osm_parser::ParserError& err)
    {
        auto alert = Gtk::AlertDialog::create();
        alert->set_message(
            "Could not parse file. Make sure it is in a proper "
            "OSM XML format.");

        alert->show(*this);
    }
    catch (const Gtk::DialogError& err)
    {
        if (err.code() != Gtk::DialogError::DISMISSED)
            throw err;
    }
}
