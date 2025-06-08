#include "graph_drawing_area.h"

#include <gtkmm/eventcontrollerscroll.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/gesturedrag.h>

#include <cmath>       // for sqrt(), pow(), min(), max(), abs()
#include <limits>      // for numeric_limits<>::max(), numeric_limits<>::min()
#include <utility>     // for move()

#define VERTEX_PIXEL_RADIUS 5.0


GraphDrawingArea::GraphDrawingArea(BaseObjectType* cobject,
                                   const Glib::RefPtr<Gtk::Builder>& refBuilder)
    : Gtk::DrawingArea(cobject)
{
    set_draw_func(sigc::mem_fun(*this, &GraphDrawingArea::on_draw));
    set_focusable(true);

    auto click_gesture = Gtk::GestureClick::create();
    click_gesture->set_button(0);
    click_gesture->signal_pressed().connect(sigc::bind(
        sigc::mem_fun(*this, &GraphDrawingArea::on_click),
        click_gesture));
    add_controller(click_gesture);

    auto drag_gesture = Gtk::GestureDrag::create();
    drag_gesture->signal_drag_begin().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_drag_begin));
    drag_gesture->signal_drag_update().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_drag_update));
    add_controller(drag_gesture);

    auto scroll_gesture = Gtk::EventControllerScroll::create();
    scroll_gesture->set_flags(Gtk::EventControllerScroll::Flags::VERTICAL);
    scroll_gesture->signal_scroll().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_scroll), true);
    add_controller(scroll_gesture);

    auto key_press_gesture = Gtk::EventControllerKey::create();
    key_press_gesture->signal_key_pressed().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_key_pressed), true);
    add_controller(key_press_gesture);
}


void GraphDrawingArea::on_drag_begin(double x, double y)
{
    m_drag_start_x = m_offset_x;
    m_drag_start_y = m_offset_y;
}


void GraphDrawingArea::on_drag_update(double offset_x, double offset_y)
{
    m_offset_x = m_drag_start_x + offset_x;
    m_offset_y = m_drag_start_y + offset_y;
    queue_draw();
}


static inline double
distance(const Graph::VertexCoords& a, const Graph::VertexCoords& b)
{
    return std::sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}


void GraphDrawingArea::on_click(
    int n_press, double x, double y, Glib::RefPtr<Gtk::GestureClick>& click)
{
    if (!m_graph)
        return;

    grab_focus();

    const double translated_x = (x / m_scale_factor) - m_offset_x;
    const double translated_y = (y / m_scale_factor) - m_offset_y;

    auto selected = m_graph->find_vertex_with_coords(
        translated_x, translated_y, VERTEX_PIXEL_RADIUS);

    auto pressed = click->get_current_button();

    if (selected && pressed == GDK_BUTTON_PRIMARY)
        set_src_vertex(*selected);

    else if (!m_editable && selected && pressed == GDK_BUTTON_SECONDARY)
        set_tgt_vertex(*selected);

    else if (m_editable && selected && pressed == GDK_BUTTON_SECONDARY && m_src_vertex)
    {
        Graph::EdgeProperties newedge;

        newedge.name = "";
        newedge.oneway = false;
        newedge.weight = distance(
            m_graph->get_vertex_coords(*m_src_vertex),
            m_graph->get_vertex_coords(*selected));

        m_graph->add_edge(*m_src_vertex, *selected, newedge);

        auto modifier = click->get_current_event_state();

        // By default, create a two-way edge. Unless the 'Alt' key is pressed.
        if (modifier != Gdk::ModifierType::ALT_MASK)
            m_graph->add_edge(*selected, *m_src_vertex, newedge);

        set_tgt_vertex(*selected);
    }

    else if (m_editable && !selected && pressed == GDK_BUTTON_PRIMARY)
    {
        Graph::VertexProperties newvertex = {0, {translated_x, translated_y}};
        set_src_vertex(m_graph->add_vertex(newvertex));
    }


    queue_draw();
}


bool GraphDrawingArea::on_scroll(double dx, double dy)
{
    if (dy < 0 && m_scale_factor < 2.0)
    {
        m_scale_factor += 0.1;
        queue_draw();
    }
    else if (dy > 0 && m_scale_factor > 0.1)
    {
        m_scale_factor -= 0.1;
        queue_draw();
    }

    return true;
}


bool GraphDrawingArea::on_key_pressed(guint keyval, guint keycode,
                                      Gdk::ModifierType modifier)
{
    if (m_editable && keyval == GDK_KEY_Delete && m_src_vertex)
    {
        m_graph->remove_vertex(*m_src_vertex);

        // removing a vertex from Graph may cause other indices to shift.
        // This would invalidate all the references GraphDrawingArea has
        // for particular vertices on the graph.

        m_src_vertex = {};
        m_tgt_vertex = {};
        m_path_distance = {};
        m_path.clear();

        m_signal_changed_selection.emit();

        queue_draw();

        return true;
    }

    return false;
}


void GraphDrawingArea::set_graph(std::unique_ptr<Graph> graph)
{
    m_src_vertex = {};
    m_tgt_vertex = {};
    m_path_distance = {};
    m_path.clear();

    m_graph = std::move(graph);

    queue_draw();

    m_signal_changed_selection.emit();
}


bool GraphDrawingArea::has_graph() const
{
    return m_graph? true : false;
}


void GraphDrawingArea::save_to(const std::string& filename, int width, int height)
{
    if (!m_graph)
        return;

    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::min();
    double maxY = std::numeric_limits<double>::min();

    for (auto [vi, vend] = m_graph->iter_vertices(); vi != vend; ++vi)
    {
        auto point = m_graph->get_vertex_coords(*vi);

        if (point.x < minX)
            minX = point.x;
        if (point.x > maxX)
            maxX = point.x;
        if (point.y < minY)
            minY = point.y;
        if (point.y > maxY)
            maxY = point.y;
    }

    double max_dist = std::max(std::abs(maxX - minX), std::abs(maxY - minY));

    double save_scale_factor = m_scale_factor;
    m_scale_factor = static_cast<double>(std::max(width, height)) / max_dist;

    double save_offset_x = m_offset_x;
    double save_offset_y = m_offset_y;

    m_offset_x = (width / (2.0 * m_scale_factor)) - ((maxX + minX) / 2.0);
    m_offset_y = (height / (2.0 * m_scale_factor)) - ((maxY + minY) / 2.0);

    auto surface = Cairo::ImageSurface::create(
        Cairo::Surface::Format::RGB24, width, height);

    auto context = Cairo::Context::create(surface);

    on_draw(context, width, height);

    surface->write_to_png(filename);

    m_scale_factor = save_scale_factor;
    m_offset_x = save_offset_x;
    m_offset_y = save_offset_y;
}


void GraphDrawingArea::set_src_vertex(const Graph::VertexT& vertex)
{
    m_path.clear();
    m_tgt_vertex = {};
    m_path_distance = {};
    m_src_vertex = vertex;

    m_signal_changed_selection.emit();
}


bool GraphDrawingArea::set_src_vertex_id(std::size_t id)
{
    if (!m_graph)
        return false;

    auto [vi, vend] = m_graph->find_vertex_id(id);

    if (vi == vend)
        return false;

    set_src_vertex(*vi);

    return true;
}


void GraphDrawingArea::set_tgt_vertex(const Graph::VertexT& vertex)
{
    m_path.clear();
    m_tgt_vertex = vertex;
    m_path_distance =
        m_graph->plot_path(*m_src_vertex, *m_tgt_vertex, m_path);

    m_signal_changed_selection.emit();
}


bool GraphDrawingArea::set_tgt_vertex_id(std::size_t id)
{
    if (!m_graph)
        return false;

    auto [vi, vend] = m_graph->find_vertex_id(id);

    if (vi == vend)
        return false;

    set_tgt_vertex(*vi);

    return true;
}


void GraphDrawingArea::set_editable(bool state)
{
    m_editable = state;
}


std::optional<std::size_t> GraphDrawingArea::get_src_vertex_id() const
{
    if (m_src_vertex)
        return m_graph->get_vertex_id(*m_src_vertex);
    else
        return {};
}


std::optional<std::size_t> GraphDrawingArea::get_tgt_vertex_id() const
{
    if (m_tgt_vertex)
        return m_graph->get_vertex_id(*m_tgt_vertex);
    else
        return {};
}


std::optional<std::size_t> GraphDrawingArea::get_num_vertices() const
{
    if (m_graph)
        return m_graph->num_vertices();
    else
        return {};
}


std::optional<std::size_t> GraphDrawingArea::get_num_on_path() const
{
    if (!m_path.empty())
        return m_path.size();
    else
        return {};
}


std::optional<double> GraphDrawingArea::get_path_distance() const
{
    return m_path_distance;
}


GraphDrawingArea::SignalChangedSelection
GraphDrawingArea::signal_changed_selection()
{
    return m_signal_changed_selection;
}


void GraphDrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr,
                               int width, int height)
{
    if (!m_graph)
        return;

    cr->rectangle(0, 0, width, height);
    cr->clip();

    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->paint();

    cr->scale(m_scale_factor, m_scale_factor);
    cr->translate(m_offset_x, m_offset_y);

    cr->set_source_rgb(0.0, 0.0, 0.0);

    for (auto [vi, vend] = m_graph->iter_edges(); vi != vend; ++vi)
    {
        auto source = m_graph->get_edge_src(*vi);
        auto target = m_graph->get_edge_tgt(*vi);

        auto src_coords = m_graph->get_vertex_coords(source);
        auto tgt_coords = m_graph->get_vertex_coords(target);

        cr->move_to(src_coords.x, src_coords.y);
        cr->line_to(tgt_coords.x, tgt_coords.y);

        cr->stroke();
    }

    cr->set_source_rgb(0.4, 0.4, 0.4);

    for (auto [vi, vend] = m_graph->iter_vertices(); vi != vend; ++vi)
    {
        auto point = m_graph->get_vertex_coords(*vi);
        cr->arc(point.x, point.y, VERTEX_PIXEL_RADIUS, 0.0, 2 * M_PI);

        cr->fill();
    }

    cr->set_source_rgb(0.8, 0.0, 0.0);

    if (m_src_vertex)
    {
        auto point = m_graph->get_vertex_coords(*m_src_vertex);
        cr->arc(point.x, point.y, VERTEX_PIXEL_RADIUS, 0.0, 2 * M_PI);
        cr->fill();
    }

    if (m_tgt_vertex)
    {
        auto point = m_graph->get_vertex_coords(*m_tgt_vertex);
        cr->move_to(point.x, point.y);

        for (const auto& vd: m_path)
        {
            auto point = m_graph->get_vertex_coords(vd);

            cr->line_to(point.x, point.y);
            cr->stroke();

            cr->arc(point.x, point.y, VERTEX_PIXEL_RADIUS, 0.0, 2 * M_PI);
            cr->fill();

            cr->move_to(point.x, point.y);
        }
    }
}
