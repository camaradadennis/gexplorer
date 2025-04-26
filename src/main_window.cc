#include "graph_drawing_area.h"
#include "main_window.h"


MainWindow::MainWindow(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& refBuilder)
    : Gtk::ApplicationWindow(cobject)
{
    std::vector<GraphDrawingArea::Edge> edges = {
        {0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {2, 6}, // Connections from the root
        {3, 7}, {4, 8}, {5, 9}, {6, 10},               // Deeper connections
        {7, 11}, {8, 12}, {9, 13}, {10, 14}            // Connecting final layer
    };

    std::vector<GraphDrawingArea::Position> positions = {
        {300, 100}, // Node 0 position (root)
        {200, 200}, {400, 200}, // Level 1 nodes
        {150, 300}, {250, 300}, {350, 300}, {450, 300}, // Level 2 nodes
        {125, 400}, {175, 400}, {325, 400}, {425, 400}, // Level 3 nodes
        {100, 500}, {200, 500}, {300, 500}, {400, 500}  // Final level nodes
    };

    auto graphArea = Gtk::Builder::get_widget_derived<GraphDrawingArea>(
        refBuilder, "graphArea");

    graphArea->set_graph(edges, positions);
}
