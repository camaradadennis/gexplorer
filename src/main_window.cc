#include "osm_parser.h"
#include "graph_drawing_area.h"
#include "main_window.h"

#include <gtkmm/alertdialog.h>
#include <gtkmm/error.h>
#include <gtkmm/menubutton.h>

#include <iostream>

#define THROW_INVALID_ID(id) \
    { throw Gtk::BuilderError(Gtk::BuilderError::INVALID_ID, \
            "no object named \"" id "\" in ui definition"); }


MainWindow::MainWindow(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::ApplicationWindow(cobject)
{
    m_graph_area = Gtk::Builder::get_widget_derived<GraphDrawingArea>(
        builder, "graph-area");
    if (!m_graph_area)
        THROW_INVALID_ID("graph-area");

    m_graph_area->signal_changed_selection().connect(
        sigc::mem_fun(*this, &MainWindow::on_selection_changed));

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

    m_src_field = Gtk::Builder::get_widget_derived<SearchField>(
        builder, "source-field");
    if (!m_src_field)
        THROW_INVALID_ID("source-field");

    m_src_field->set_placeholder_text("Source Vertex");

    m_tgt_field = Gtk::Builder::get_widget_derived<SearchField>(
        builder, "target-field");
    if (!m_tgt_field)
        THROW_INVALID_ID("target-field");

    m_tgt_field->set_placeholder_text("Target Vertex");

    m_info_field = Gtk::Builder::get_widget_derived<InfoField>(
        builder, "info-field");
    if (!m_info_field)
        THROW_INVALID_ID("info-field");

    m_plot_btn = builder->get_widget<Gtk::Button>("plot-btn");
    if (!m_plot_btn)
        THROW_INVALID_ID("plot-btn");

    m_plot_btn->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_plot_btn_clicked));

    add_action("load", sigc::mem_fun(*this, &MainWindow::open_file_dialog));
}


void MainWindow::on_plot_btn_clicked()
{
    auto src_id = m_src_field->get_selected();
    auto tgt_id = m_tgt_field->get_selected();

    if (src_id && tgt_id)
    {
        if (m_graph_area->set_src_vertex_id(*src_id) &&
            m_graph_area->set_tgt_vertex_id(*tgt_id))
            {
                m_graph_area->queue_draw();
            }
    }
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

        auto vertex_list{ g->get_vertex_id_list() };

        m_src_field->set_data(vertex_list);
        m_tgt_field->set_data(vertex_list);

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


void MainWindow::on_selection_changed()
{
    m_info_field->set_num(m_graph_area->get_num_vertices());
    m_info_field->set_source(m_graph_area->get_src_vertex_id());
    m_info_field->set_target(m_graph_area->get_tgt_vertex_id());
    m_info_field->set_distance(m_graph_area->get_path_distance());
}
