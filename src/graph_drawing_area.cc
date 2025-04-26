#include "graph_drawing_area.h"

#include <gtkmm/gestureclick.h>

#include <stdio.h>


GraphDrawingArea::GraphDrawingArea() {}


GraphDrawingArea::GraphDrawingArea(BaseObjectType* cobject,
                                   const Glib::RefPtr<Gtk::Builder>& builder)
    : Gtk::DrawingArea(cobject)
{
    set_draw_func(sigc::mem_fun(*this, &GraphDrawingArea::on_draw));

    auto clickGesture = Gtk::GestureClick::create();

    clickGesture->signal_released().connect(
        sigc::mem_fun(*this, &GraphDrawingArea::gesture_released));

    add_controller(clickGesture);
}


void GraphDrawingArea::gesture_released(int n_press, double x, double y)
{
    printf("Clicks (%d) on: (%f, %f)\n", n_press, x, y);
}

void GraphDrawingArea::set_graph(const std::vector<Edge>& edges,
                                 const std::vector<Position>& positions)
{
    m_edges = edges;
    m_positions = positions;

    queue_draw();
}


void GraphDrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr,
                               int width, int height)
{
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->paint();

    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->set_line_width(1.0);

    for (const auto& edge: m_edges)
    {
        auto [start, end] = edge;

        cr->move_to(m_positions[start].first, m_positions[start].second);
        cr->line_to(m_positions[end].first, m_positions[end].second);
        cr->stroke();
    }

    cr->set_source_rgb(0.8, 0.0, 0.0);

    for (const auto& node: m_positions)
    {
        cr->arc(node.first, node.second, 5, 0, 2 * M_PI);
        cr->fill();
    }
}
