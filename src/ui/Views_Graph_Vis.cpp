#include "ui/Layout.hpp"
#include "ui/Views.hpp"

namespace ssoc::ui::views {

void draw_graph_visualization_window(
    bool &show, Vis_Config &vis_cfg, Visualizer &vis, graph::Graph &g,
    size_t last_vertex, const std::deque<size_t> &checking_topple_vertices) {
  layout::set_next_window(layout::Window::VISUALIZER);

  vis.show_window(g, show, last_vertex, checking_topple_vertices, vis_cfg);
}

} // namespace ssoc::ui::views
