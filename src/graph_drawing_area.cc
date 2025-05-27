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

    std::cout << "\n(x,   y) = (" << x << ", " << y << ")" << std::endl;

    for (auto iter = m_graph->vertex_begin(); iter != m_graph->vertex_end(); ++iter)
    {
        const auto point = m_graph->get_vertex_coords(*iter);

        if (   translated_x <= point.first + VERTEX_PIXEL_RADIUS
            && translated_x >= point.first - VERTEX_PIXEL_RADIUS
            && translated_y <= point.second + VERTEX_PIXEL_RADIUS
            && translated_y >= point.second - VERTEX_PIXEL_RADIUS
        )
        {
            std::cout << "\nSelected vertex (" << m_graph->get_vertex_id(*iter)
                      << ")" << std::endl;

            switch (m_click_gesture->get_current_button())
            {
                case GDK_BUTTON_PRIMARY:
                    this->set_src_vertex(*iter);
                    break;
                case GDK_BUTTON_SECONDARY:
                    this->set_tgt_vertex(*iter);
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
    m_graph = std::move(graph);
    queue_draw();
}


void GraphDrawingArea::set_src_vertex(Graph::vertex_t vertex)
{
    m_path_set = false;
    m_path.clear();
    m_src_tgt.first = vertex;
}


void GraphDrawingArea::set_tgt_vertex(Graph::vertex_t vertex)
{
    m_path_set = true;
    m_src_tgt.second = vertex;
    m_path.clear();
    double distance =
        m_graph->plot_path(m_src_tgt.first, m_src_tgt.second, m_path);

    if (distance == std::numeric_limits<double>::max())
        std::cout << "\nNo path between \""
                  << m_graph->get_vertex_id(m_src_tgt.first)
                  << "\" and \""
                  << m_graph->get_vertex_id(m_src_tgt.second)
                  << std::endl;
    else
        std::cout << "\nDistance between \""
                  << m_graph->get_vertex_id(m_src_tgt.first)
                  << "\" and \""
                  << m_graph->get_vertex_id(m_src_tgt.second)
                  << " is " << distance << " meters"
                  << std::endl;
}


void GraphDrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr,
                               int width, int height)
{
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->paint();

    if (!m_graph)
        return;

    cr->translate(m_offset_x, m_offset_y);
    cr->scale(m_scale_factor, m_scale_factor);

    cr->set_source_rgb(0.0, 0.0, 0.0);

    for (auto iter = m_graph->edge_begin(); iter != m_graph->edge_end(); ++iter)
    {
        const auto source = m_graph->get_edge_source(*iter);
        const auto target = m_graph->get_edge_target(*iter);

        const auto src_coords = m_graph->get_vertex_coords(source);
        const auto tgt_coords = m_graph->get_vertex_coords(target);

        cr->move_to(src_coords.first, src_coords.second);
        cr->line_to(tgt_coords.first, tgt_coords.second);

        cr->stroke();
    }

    cr->set_source_rgb(0.4, 0.4, 0.4);

    for (auto iter = m_graph->vertex_begin(); iter != m_graph->vertex_end(); ++iter)
    {
        const auto point = m_graph->get_vertex_coords(*iter);
        cr->arc(point.first, point.second, VERTEX_PIXEL_RADIUS, 0.0, 2 * M_PI);

        cr->fill();
    }

    cr->set_source_rgb(0.8, 0.0, 0.0);

    const auto point = m_graph->get_vertex_coords(m_src_tgt.first);
    cr->arc(point.first, point.second, VERTEX_PIXEL_RADIUS, 0.0, 2 * M_PI);
    cr->fill();

    if (m_path_set)
    {
        const auto point = m_graph->get_vertex_coords(m_src_tgt.second);
        cr->move_to(point.first, point.second);

        for (auto iter = m_path.begin(); iter != m_path.end(); ++iter)
        {
            const auto point = m_graph->get_vertex_coords(*iter);

            cr->line_to(point.first, point.second);
            cr->stroke();

            cr->arc(point.first, point.second, VERTEX_PIXEL_RADIUS, 0.0, 2 * M_PI);
            cr->fill();

            cr->move_to(point.first, point.second);
        }
    }
}
