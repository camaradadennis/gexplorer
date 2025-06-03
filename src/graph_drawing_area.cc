#include "graph_drawing_area.h"

#include <gtkmm/eventcontrollerscroll.h>
#include <gtkmm/gesturedrag.h>

#include <limits>           // for numeric_limits<>::max()
#include <utility>          // for move()

#include <iostream>         // just for testing

#define VERTEX_PIXEL_RADIUS 5.0


GraphDrawingArea::GraphDrawingArea(BaseObjectType* cobject,
                                   const Glib::RefPtr<Gtk::Builder>& refBuilder)
    : Gtk::DrawingArea(cobject)
{
    set_draw_func(sigc::mem_fun(*this, &GraphDrawingArea::on_draw));

    m_click_gesture = Gtk::GestureClick::create();
    m_click_gesture->set_button(0);
    m_click_gesture->signal_pressed().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_gesture_pressed));
    add_controller(m_click_gesture);

    auto drag_gesture = Gtk::GestureDrag::create();
    drag_gesture->signal_drag_begin().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_gesture_drag_begin));
    drag_gesture->signal_drag_update().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_gesture_drag_update));
    add_controller(drag_gesture);

    auto scroll_gesture = Gtk::EventControllerScroll::create();
    scroll_gesture->set_flags(Gtk::EventControllerScroll::Flags::VERTICAL);
    scroll_gesture->signal_scroll().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_gesture_scroll), true);
    add_controller(scroll_gesture);
}


void GraphDrawingArea::on_gesture_drag_begin(double x, double y)
{
    m_drag_start_x = m_offset_x;
    m_drag_start_y = m_offset_y;
}


void GraphDrawingArea::on_gesture_drag_update(double offset_x, double offset_y)
{
    m_offset_x = m_drag_start_x + offset_x;
    m_offset_y = m_drag_start_y + offset_y;
    queue_draw();
}


void GraphDrawingArea::on_gesture_pressed(int n_press, double x, double y)
{
    if (!m_graph)
        return;

    const double translated_x = (x - m_offset_x) / m_scale_factor;
    const double translated_y = (y - m_offset_y) / m_scale_factor;

    for (auto [vi, vend] = m_graph->iter_vertices(); vi != vend; ++vi)
    {
        auto point = m_graph->get_vertex_coords(*vi);

        if (   translated_x <= point.x + VERTEX_PIXEL_RADIUS
            && translated_x >= point.x - VERTEX_PIXEL_RADIUS
            && translated_y <= point.y + VERTEX_PIXEL_RADIUS
            && translated_y >= point.y - VERTEX_PIXEL_RADIUS
        )
        {
            switch (m_click_gesture->get_current_button())
            {
                case GDK_BUTTON_PRIMARY:
                    this->set_src_vertex(*vi);
                    break;
                case GDK_BUTTON_SECONDARY:
                    this->set_tgt_vertex(*vi);
                    break;
            }
            break;
        }
    }

    queue_draw();
}


bool GraphDrawingArea::on_gesture_scroll(double dx, double dy)
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


void GraphDrawingArea::set_src_vertex(const Graph::VertexT& vertex)
{
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
        return std::nullopt;
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
    cr->rectangle(0, 0, width, height);
    cr->clip();

    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->paint();

    if (!m_graph)
        return;

    cr->translate(m_offset_x, m_offset_y);
    cr->scale(m_scale_factor, m_scale_factor);

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
