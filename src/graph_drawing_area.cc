#include "graph_drawing_area.h"

#include <gtkmm/eventcontrollerscroll.h>
#include <gtkmm/gesturedrag.h>

#include <algorithm>
#include <utility>

#include <iostream>

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
    drag_gesture->signal_drag_update().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_gesture_drag_update));
    add_controller(drag_gesture);

    auto scroll_gesture = Gtk::EventControllerScroll::create();
    scroll_gesture->set_flags(Gtk::EventControllerScroll::Flags::VERTICAL);
    scroll_gesture->signal_scroll().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::on_gesture_scroll), true);
    add_controller(scroll_gesture);
}


void GraphDrawingArea::on_gesture_drag_update(double offset_x, double offset_y)
{
    m_offset_x = offset_x;
    m_offset_y = offset_y;
    queue_draw();
}


void GraphDrawingArea::on_gesture_pressed(int n_press, double x, double y)
{
    if (!m_graph)
        return;

    const double translated_x = (x - m_offset_x) / m_scale_factor;
    const double translated_y = (y - m_offset_y) / m_scale_factor;

    for (auto iter = m_graph->vertex_begin(); iter != m_graph->vertex_end(); ++iter)
    {
        const auto point = m_graph->get_vertex_coords(*iter);

        if (   translated_x <= point.x + VERTEX_PIXEL_RADIUS
            && translated_x >= point.x - VERTEX_PIXEL_RADIUS
            && translated_y <= point.y + VERTEX_PIXEL_RADIUS
            && translated_y >= point.y - VERTEX_PIXEL_RADIUS
        )
        {
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

    const int n_vertices = m_graph->num_vertices();
    int width;

    if (n_vertices < 10)
        width = 256;
    else if (n_vertices < 50)
        width = 512;
    else if (n_vertices < 100)
        width = 1024;
    else if (n_vertices < 500)
        width = 2048;
    else
        width = 4096;

    set_content_width(width);
    set_content_height(width);

    m_graph->compute_vertex_coords(width, width);
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
    m_graph->plot_path(m_src_tgt.first, m_src_tgt.second, m_path);
}


void GraphDrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr,
                               int width, int height)
{
    cr->save();

    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->paint();

    if (!m_graph)
    {
        cr->restore();
        return;
    }

    cr->translate(m_offset_x, m_offset_y);
    cr->scale(m_scale_factor, m_scale_factor);

    cr->set_source_rgb(0.0, 0.0, 0.0);

    for (auto iter = m_graph->edge_begin(); iter != m_graph->edge_end(); ++iter)
    {
        const auto source = m_graph->get_edge_source(*iter);
        const auto target = m_graph->get_edge_target(*iter);

        const auto src_coords = m_graph->get_vertex_coords(source);
        const auto tgt_coords = m_graph->get_vertex_coords(target);

        cr->move_to(src_coords.x, src_coords.y);
        cr->line_to(tgt_coords.x, tgt_coords.y);

        cr->stroke();
    }

    cr->set_source_rgb(0.4, 0.4, 0.4);

    for (auto iter = m_graph->vertex_begin(); iter != m_graph->vertex_end(); ++iter)
    {
        const auto point = m_graph->get_vertex_coords(*iter);
        cr->arc(point.x, point.y, VERTEX_PIXEL_RADIUS, 0.0, 2 * M_PI);

        cr->fill();
    }

    cr->set_source_rgb(0.8, 0.0, 0.0);

    const auto point = m_graph->get_vertex_coords(m_src_tgt.first);
    cr->arc(point.x, point.y, VERTEX_PIXEL_RADIUS, 0.0, 2 * M_PI);
    cr->fill();

    if (m_path_set)
    {
        const auto point = m_graph->get_vertex_coords(m_src_tgt.second);
        cr->move_to(point.x, point.y);

        for (auto iter = m_path.begin(); iter != m_path.end(); ++iter)
        {
            const auto point = m_graph->get_vertex_coords(*iter);

            cr->line_to(point.x, point.y);
            cr->stroke();

            cr->arc(point.x, point.y, VERTEX_PIXEL_RADIUS, 0.0, 2 * M_PI);
            cr->fill();

            cr->move_to(point.x, point.y);
        }
    }

    cr->restore();
}
