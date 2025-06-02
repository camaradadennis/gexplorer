#ifndef GRAPH_DRAWING_AREA_H
#define GRAPH_DRAWING_AREA_H

#include "graph.h"

#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/gestureclick.h>

#include <optional>
#include <memory>    // for unique_ptr
#include <utility>   // for pair
#include <vector>


class GraphDrawingArea final: public Gtk::DrawingArea
{
public:
    GraphDrawingArea(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

    void set_graph(std::unique_ptr<Graph>);
    bool set_src_vertex_id(std::size_t);
    bool set_tgt_vertex_id(std::size_t);

private:
    void on_draw(const Cairo::RefPtr<Cairo::Context>&, int, int);

    void on_gesture_drag_begin(double, double);
    void on_gesture_drag_update(double, double);
    void on_gesture_pressed(int, double, double);
    bool on_gesture_scroll(double, double);

    Glib::RefPtr<Gtk::GestureClick> m_click_gesture;

    double m_scale_factor{ 1.0 };
    double m_offset_x{ 0.0 };
    double m_offset_y{ 0.0 };
    double m_drag_start_x{ 0.0 };
    double m_drag_start_y{ 0.0 };

    void set_src_vertex(const Graph::VertexT&);
    void set_tgt_vertex(const Graph::VertexT&);

    std::unique_ptr<Graph> m_graph{ nullptr };
    std::optional<Graph::VertexT> m_src_vertex{};
    std::optional<Graph::VertexT> m_tgt_vertex{};
    std::vector<Graph::VertexT> m_path;
};

#endif // GRAPH_DRAWING_AREA_H
