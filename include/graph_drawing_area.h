#ifndef GRAPH_DRAWING_AREA_H
#define GRAPH_DRAWING_AREA_H

#include "graph.h"

#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/gestureclick.h>

#include <memory>    // for unique_ptr
#include <utility>   // for pair
#include <vector>


class GraphDrawingArea final: public Gtk::DrawingArea
{
public:
    GraphDrawingArea(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

    void set_graph(std::unique_ptr<Graph>);

private:
    void on_draw(const Cairo::RefPtr<Cairo::Context>&, int, int);

    void set_src_vertex(Graph::vertex_t);
    void set_tgt_vertex(Graph::vertex_t);

    void on_gesture_pressed(int, double, double);
    void on_gesture_drag_update(double, double);
    bool on_gesture_scroll(double, double);

    std::unique_ptr<Graph> m_graph{ nullptr };

    double m_scale_factor{ 1.0 };
    double m_offset_x{ 0.0 };
    double m_offset_y{ 0.0 };

    Glib::RefPtr<Gtk::GestureClick> m_click_gesture;
    std::pair<Graph::vertex_t, Graph::vertex_t> m_src_tgt;
    std::vector<Graph::vertex_t> m_path;
    bool m_path_set{false};
};

#endif // GRAPH_DRAWING_AREA_H
