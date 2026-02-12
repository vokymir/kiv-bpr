#include "Visualizer.hpp"
#include "imgui.h"
#include <cstdio>

namespace ssoc::ui {

void Visualizer::show_window(const graph::Graph &g, bool &show) {
  ImGui::Begin("Graph Visualization", &show);

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

  auto node_count = g.num_vertices();
  auto &positions = g.layout_pos();
  auto &neighbour_idxs = g.adj_offsets();
  auto &neighbours = g.adj_vertices();

  // Draw Edges
  for (int v = 0; v < node_count; ++v) {
    ImVec2 p1 = to_screen(positions[v]);
    int start = neighbour_idxs[v];
    int end =
        (v + 1 < node_count) ? neighbour_idxs[v + 1] : (int)neighbours.size();

    for (int i = start; i < end; ++i) {
      draw_list->AddLine(p1, to_screen(positions[neighbours[i]]),
                         IM_COL32(150, 150, 150, 255));
    }
  }

  // Draw Nodes
  ImU32 color_normal = IM_COL32(200, 50, 50, 255);
  ImU32 color_last = IM_COL32(50, 150, 150, 255);
  ImU32 color_text = IM_COL32(255, 255, 255, 255);
  auto history = g.sand_history_const(0);
  auto last_idx = history[history.size() - 1];

  for (int v = 0; v < node_count; ++v) {
    int height = g.sand_height_const(0)[v];
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%i", height);

    auto pos = to_screen(positions[v]);

    // We keep the node radius constant-ish so they don't get giant
    // Or use: 5.0f * (zoom / 200.0f) if you want them to scale
    float circle_size = 5.0f * zoom_ / 200.0f * (height + 1);
    draw_list->AddCircleFilled(pos, circle_size,
                               last_idx != v ? color_normal : color_last);

    auto text_size = ImGui::CalcTextSize(buf);
    pos.x -= text_size.x / 2;
    pos.y -= text_size.y / 2;

    draw_list->AddText(pos, color_text, buf);
  }

  draw_list->PopClipRect();
  ImGui::End();
}

} // namespace ssoc::ui
