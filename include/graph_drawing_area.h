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
    using SignalChangedSelection = sigc::signal<void()>;

    GraphDrawingArea(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

    void set_graph(std::unique_ptr<Graph>);
    bool has_graph() const;
    bool set_src_vertex_id(std::size_t);
    bool set_tgt_vertex_id(std::size_t);
    void set_editable(bool);

    void save_to(const std::string&, int, int);

    std::optional<std::size_t> get_src_vertex_id() const;
    std::optional<std::size_t> get_tgt_vertex_id() const;
    std::optional<std::size_t> get_num_vertices() const;
    std::optional<std::size_t> get_num_on_path() const;
    std::optional<double> get_path_distance() const;

    SignalChangedSelection signal_changed_selection();

private:
    void on_draw(const Cairo::RefPtr<Cairo::Context>&, int, int);

    void on_drag_begin(double, double);
    void on_drag_update(double, double);
    void on_click(int, double, double, Glib::RefPtr<Gtk::GestureClick>&);
    bool on_scroll(double, double);
    bool on_key_pressed(guint, guint, Gdk::ModifierType);

    bool m_editable{ false };

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
    std::optional<double> m_path_distance;
    std::vector<Graph::VertexT> m_path;

    SignalChangedSelection m_signal_changed_selection;
};

#endif // GRAPH_DRAWING_AREA_H
