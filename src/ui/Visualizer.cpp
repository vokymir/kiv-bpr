#include "Visualizer.hpp"
#include "Vis_Config.hpp"
#include "graph/Graph.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <deque>
#include <imgui.h>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace ssoc::ui {

void Visualizer::setup_canvas() {
  origin_ = ImGui::GetCursorScreenPos();
  size_ = ImGui::GetContentRegionAvail();
  center_ = ImVec2(origin_.x + size_.x * 0.5f, origin_.y + size_.y * 0.5f);
  footer_ = ImVec2(origin_.x + size_.x, origin_.y + size_.y);
}

void Visualizer::pan() {
  const bool is_active = ImGui::IsItemActive();
  const bool is_dragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

  if (!is_active || !is_dragging) {
    return;
  }

  ImGuiIO &io = ImGui::GetIO();
  pan_.x += io.MouseDelta.x;
  pan_.y += io.MouseDelta.y;
}

void Visualizer::zoom() {
  const bool is_hovered = ImGui::IsItemHovered();
  ImGuiIO &io = ImGui::GetIO();

  if (!is_hovered || io.MouseWheel == 0.0f) {
    return;
  }

  float old_zoom = zoom_;
  zoom_ *= 1.f + (io.MouseWheel > 0 ? zoom_speed_ : -zoom_speed_);

  // have zoom treshold - not to stay on zero
  if (zoom_ < 0.001f) {
    zoom_ = 0.001f;
  }

  // adjust pan to zoom towards the mouse
  ImVec2 mouse_offset = {io.MousePos.x - center_.x, io.MousePos.y - center_.y};
  pan_.x = mouse_offset.x - (mouse_offset.x - pan_.x) * (zoom_ / old_zoom);
  pan_.y = mouse_offset.y - (mouse_offset.y - pan_.y) * (zoom_ / old_zoom);
}

ImVec2 Visualizer::to_screen(std::pair<double, double> pos) {
  return ImVec2(center_.x + pan_.x + static_cast<float>(pos.first) * zoom_,
                center_.y + pan_.y + static_cast<float>(pos.second) * zoom_);
}

std::pair<double, double> Visualizer::from_screen(const ImVec2 &coord) {
  return {(coord.x - center_.x - pan_.x) / zoom_,
          (coord.y - center_.y - pan_.y) / zoom_};
}

float Visualizer::circle_size(int height) {
  return vertex_base_size_ * static_cast<float>(height + 1) * zoom_ / 100.0f;
}

void Visualizer::draw(ImDrawList *draw_list, const graph::Graph &g,
                      size_t last_vertex,
                      const std::deque<size_t> &maybe_toppling) {
  draw_edges(draw_list, g);
  draw_vertexes(draw_list, g, last_vertex);
  draw_topple_vertexes(draw_list, g, maybe_toppling);
}

void Visualizer::draw_edges(ImDrawList *draw_list, const graph::Graph &g) {
  const size_t vert_count = g.num_vertices() - 1;
  // only because we know that this is the case - if the sink vertex was at
  // another place, we would need to change it here
  const size_t sink = vert_count;

  const auto &positions = g.layout_pos_const();
  const ImU32 edge_color = IM_COL32(150, 150, 150, 255);

  for (size_t v = 0; v < vert_count; ++v) {
    const ImVec2 pt = to_screen(positions[v]);

    for (const auto neigh : g.get_neighbours(v)) {
      if (neigh == sink) {
        continue;
      }

      // avoid drawing every edge twice
      if (neigh < v) {
        continue;
      }

      draw_list->AddLine(pt, to_screen(positions[neigh]), edge_color);
    }
  }
}

void Visualizer::draw_vertexes(ImDrawList *draw_list, const graph::Graph &g,
                               size_t last_vertex) {
  const size_t vert_count = g.num_vertices() - 1;

  const auto &positions = g.layout_pos_const();
  const auto &heights = g.sand_height_const(0);

  const ImU32 color_normal = IM_COL32(200, 50, 50, 255);
  const ImU32 color_last = IM_COL32(50, 150, 150, 255);
  const ImU32 color_text_H = IM_COL32(255, 255, 255, 255); // height
  const ImU32 color_text_I = IM_COL32(3, 252, 57, 255);    // id

  for (size_t v = 0; v < vert_count; ++v) {
    int height = heights[v];
    auto pos = to_screen(positions[v]);

    draw_list->AddCircleFilled(pos, circle_size(height),
                               last_vertex == v ? color_last : color_normal);

    char buf[32];
    std::snprintf(buf, sizeof(buf), "%i", height);
    auto text_size = ImGui::CalcTextSize(buf);
    pos.x -= text_size.x / 2;

    draw_list->AddText(pos, color_text_H, buf);
    pos.x += text_size.x / 2; // revert to base pos

    std::snprintf(buf, sizeof(buf), "%zu", v);
    text_size = ImGui::CalcTextSize(buf);
    pos.x -= text_size.x / 2;
    pos.y -= text_size.y;

    draw_list->AddText(pos, color_text_I, buf);
  }
}

void Visualizer::draw_topple_vertexes(
    ImDrawList *draw_list, const graph::Graph &g,
    const std::deque<size_t> &maybe_toppling) {
  const size_t sink = g.num_vertices() - 1;
  const auto &positions = g.layout_pos_const();
  const auto &heights = g.sand_height_const();

  const ImU32 color_candidate = IM_COL32(170, 255, 0, 255);

  std::unordered_map<size_t, std::vector<size_t>> grouped;

  // one vertex might be checked multiple times
  for (auto [i, x] : std::views::enumerate(maybe_toppling)) {
    grouped[x].push_back(static_cast<size_t>(i));
  }

  for (auto &[vertex, checking_order] : grouped) {
    if (vertex == sink) {
      continue;
    }

    auto pos = to_screen(positions[vertex]);
    int height = heights[vertex];

    float line_thickness = 1.0f * zoom_ / 100.0f;
    draw_list->AddCircle(pos, circle_size(height), color_candidate, 0,
                         line_thickness);

    size_t min_order =
        *std::min_element(checking_order.begin(), checking_order.end());

    char buf[32];
    // shift by 1 to be 1-indexed (more human readable)
    std::snprintf(buf, sizeof(buf), "%zu", min_order + 1);
    auto text_size = ImGui::CalcTextSize(buf);
    pos.x -= circle_size(height) + text_size.x;
    pos.y -= text_size.y / 2;

    draw_list->AddText(pos, color_candidate, buf);
  }
}

void Visualizer::move_vertex(graph::Graph &g) {
  if (!ImGui::IsMouseDragging(ImGuiMouseButton_Right) ||
      !ImGui::IsMousePosValid()) {
    return;
  }

  const auto mouse_pos = ImGui::GetMousePos();
  auto &positions = g.layout_pos();

  // find nearest vertex (should be moving)
  size_t nearest_vertex = 0;
  float nearest_dst = INFINITY;
  for (size_t i = 0; i < positions.size(); ++i) {
    const auto pos = to_screen(positions[i]);

    const float dst = std::sqrt((mouse_pos.x - pos.x) * (mouse_pos.x - pos.x) +
                                (mouse_pos.y - pos.y) * (mouse_pos.y - pos.y));

    if (dst < nearest_dst) {
      nearest_dst = dst;
      nearest_vertex = i;
    }
  }

  // jump-move vertex to mouse position
  positions[nearest_vertex] = from_screen(mouse_pos);
}

void Visualizer::show_window(
    graph::Graph &g, bool &show, size_t last_vertex,
    const std::deque<size_t> &checking_topple_vertexes) {
  ImGui::Begin("Graph Visualization", &show);

  if (std::holds_alternative<gla_::Hidden_GLA>(g.vis_cfg_const().gla)) {
    ImGui::Text("Empty visualization.");
    ImGui::End();
    return;
  }

  ImGui::Text("zoom: %f", static_cast<double>(zoom_));
  ImGui::SliderFloat("Zoom speed", &zoom_speed_, 0.001f, 1.0f);
  ImGui::SliderFloat("Vertex base size", &vertex_base_size_, 0.001f, 10.0f);

  setup_canvas();
  ImGui::InvisibleButton("##canvas", size_);
  pan();
  zoom();

  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(origin_, footer_, true);

  draw(draw_list, g, last_vertex, checking_topple_vertexes);
  move_vertex(g); // this one line allows moving vertices

  draw_list->PopClipRect();
  ImGui::End();
}

} // namespace ssoc::ui
