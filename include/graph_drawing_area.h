#ifndef GRAPH_DRAWING_AREA_H
#define GRAPH_DRAWING_AREA_H

#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>

class GraphDrawingArea final: public Gtk::DrawingArea
{
public:
    using Edge = std::pair<int, int>;
    using Position = std::pair<int, int>;

    GraphDrawingArea();
    GraphDrawingArea(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

    void set_graph(const std::vector<Edge>&, const std::vector<Position>&);

protected:
    void on_draw(const Cairo::RefPtr<Cairo::Context>&, int, int);

    void gesture_released(int, double, double);

private:
    std::vector<Edge> m_edges;
    std::vector<Position> m_positions;
};

#endif // GRAPH_DRAWING_AREA_H
