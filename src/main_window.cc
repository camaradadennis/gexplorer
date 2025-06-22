#include "osm_parser.h"
#include "graph.h"
#include "graph_drawing_area.h"
#include "main_window.h"

#include <giomm/liststore.h>
#include <gtkmm/alertdialog.h>
#include <gtkmm/error.h>
#include <gtkmm/filefilter.h>


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

    auto button_new = builder->get_widget<Gtk::Button>("button-new");
    if (!button_new)
        THROW_INVALID_ID("button-new");

    button_new->signal_clicked().connect([this] () {
        this->m_graph_area->set_graph( Graph::create() );
        this->with_graph_opened(true);
    });

    auto button_open = builder->get_widget<Gtk::Button>("button-open");
    if (!button_open)
        THROW_INVALID_ID("button-open");

    button_open->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::open_file_dialog));

    m_button_save = builder->get_widget<Gtk::Button>("button-save");
    if (!m_button_save)
        THROW_INVALID_ID("button-save");

    m_button_save->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::save_file_dialog));

    m_button_close = builder->get_widget<Gtk::Button>("button-close");
    if (!m_button_close)
        THROW_INVALID_ID("button-close");

    m_button_close->signal_clicked().connect([this] () {
        this->m_graph_area->set_graph(nullptr);
        this->with_graph_opened(false);
    });

    m_toggle_edit = builder->get_widget<Gtk::CheckButton>("toggle-edit");
    if (!m_toggle_edit)
        THROW_INVALID_ID("toggle-edit");

    m_toggle_edit->signal_toggled().connect([this] () {
        this->m_graph_area->set_editable(this->m_toggle_edit->get_active());
    });

    m_toggle_show_arrows = builder->get_widget<Gtk::CheckButton>("toggle-view");
    if (!m_toggle_show_arrows)
        THROW_INVALID_ID("toggle-view");

    m_toggle_show_arrows->signal_toggled().connect([this] () {
        this->m_graph_area->set_show_arrows(
            this->m_toggle_show_arrows->get_active()
        );
    });

    m_toggle_show_weights = builder->get_widget<Gtk::CheckButton>("toggle-weights");
    if (!m_toggle_show_weights)
        THROW_INVALID_ID("toggle-weights");

    m_toggle_show_weights->signal_toggled().connect([this] () {
        this->m_graph_area->set_show_weights(
            this->m_toggle_show_weights->get_active()
        );
    });

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

    auto button_plot = builder->get_widget<Gtk::Button>("button-plot");
    if (!button_plot)
        THROW_INVALID_ID("button-plot");

    button_plot->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_plot_btn_clicked));
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

    auto filters = Gio::ListStore<Gtk::FileFilter>::create();

    auto osm_filter = Gtk::FileFilter::create();
    osm_filter->set_name("OSM files");
    osm_filter->add_pattern("*.osm");

    filters->append(osm_filter);

    file_dialog->set_filters(filters);

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

        with_graph_opened(true);
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


void MainWindow::save_file_dialog()
{
    auto file_dialog = Gtk::FileDialog::create();

    auto filters = Gio::ListStore<Gtk::FileFilter>::create();

    auto png_filter = Gtk::FileFilter::create();
    png_filter->set_name("PNG images");
    png_filter->add_pattern("*.png");

    filters->append(png_filter);

    file_dialog->set_filters(filters);

    file_dialog->save(*this, sigc::bind(
        sigc::mem_fun(*this, &MainWindow::on_save_selection),
        file_dialog
    ));
}


void MainWindow::on_save_selection(
        const Glib::RefPtr<Gio::AsyncResult>& result,
        const Glib::RefPtr<Gtk::FileDialog>& dialog)
{
    try
    {
        auto file = dialog->save_finish(result);
        std::string fpath = file->get_path();
        m_graph_area->save_to(fpath, 800, 600);
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
    m_info_field->set_num_path(m_graph_area->get_num_on_path());
    m_info_field->set_source(m_graph_area->get_src_vertex_id());
    m_info_field->set_target(m_graph_area->get_tgt_vertex_id());
    m_info_field->set_distance(m_graph_area->get_path_distance());
    m_info_field->set_elapsed_time(m_graph_area->get_elapsed_time());
}


void MainWindow::with_graph_opened(bool opened)
{
    m_button_save->set_sensitive(opened);
    m_button_close->set_sensitive(opened);
}
