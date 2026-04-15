#include "ui/Views.hpp"

namespace ssoc::ui::views {

void draw_graph_visualization_window(
    bool &show, Vis_Config &vis_cfg, Visualizer &vis, graph::Graph &g,
    size_t last_vertex, const std::deque<size_t> &checking_topple_vertices) {
  ImGui::SetNextWindowPos(ImVec2(5, 300), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(790, 480), ImGuiCond_FirstUseEver);

  vis.show_window(g, show, last_vertex, checking_topple_vertices, vis_cfg);
}

} // namespace ssoc::ui::views
