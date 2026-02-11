#include "Visualizer.hpp"
#include "imgui.h"

namespace ssoc::ui {

void Visualizer::show_window(const graph::Graph &g) {
  ImGui::Begin("Graph Visualization");

  ImGui::Text("zoom: %f", zoom_);

  ImVec2 origin = ImGui::GetCursorScreenPos();
  ImVec2 size = ImGui::GetContentRegionAvail();
  ImVec2 center = ImVec2(origin.x + size.x * 0.5f, origin.y + size.y * 0.5f);
  ImVec2 footer = ImVec2(origin.x + size.x, origin.y + size.y);

  // Capture input
  ImGui::InvisibleButton("##canvas", size);
  const bool is_hovered = ImGui::IsItemHovered();
  const bool is_active = ImGui::IsItemActive();
  ImGuiIO &io = ImGui::GetIO();

  // 1. Zoom Logic (Start 'zoom' at ~200.0f in your constructor for [-1,1]
  // layouts!)
  if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    pan_.x += io.MouseDelta.x;
    pan_.y += io.MouseDelta.y;
  }

  if (is_hovered && io.MouseWheel != 0.0f) {
    float old_zoom = zoom_;
    zoom_ *= (io.MouseWheel > 0) ? 1.01f : 0.99f;

    // Pan adjustment to zoom toward mouse
    pan_.x = (io.MousePos.x - center.x) -
             ((io.MousePos.x - center.x) - pan_.x) * (zoom_ / old_zoom);
    pan_.y = (io.MousePos.y - center.y) -
             ((io.MousePos.y - center.y) - pan_.y) * (zoom_ / old_zoom);
  }

  // 2. Rendering
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(origin, footer, true);

  // Transform: Screen Center + Pan + (WorldCoord * Zoom)
  auto to_screen = [&](std::pair<double, double> pos) {
    return ImVec2(center.x + pan_.x + (float)pos.first * zoom_,
                  center.y + pan_.y + (float)pos.second * zoom_);
  };

  // Draw Edges
  for (int v = 0; v < g.node_count_; ++v) {
    ImVec2 p1 = to_screen(g.positions_[v]);
    int start = g.neighbour_idxs_[v];
    int end = (v + 1 < g.node_count_) ? g.neighbour_idxs_[v + 1]
                                      : (int)g.neighbours_.size();

    for (int i = start; i < end; ++i) {
      draw_list->AddLine(p1, to_screen(g.positions_[g.neighbours_[i]]),
                         IM_COL32(150, 150, 150, 255));
    }
  }

  // Draw Nodes
  for (int v = 0; v < g.node_count_; ++v) {
    // We keep the node radius constant-ish so they don't get giant
    // Or use: 5.0f * (zoom / 200.0f) if you want them to scale
    draw_list->AddCircleFilled(to_screen(g.positions_[v]), 5.0f,
                               IM_COL32(200, 50, 50, 255));
  }

  draw_list->PopClipRect();
  ImGui::End();
}

} // namespace ssoc::ui
