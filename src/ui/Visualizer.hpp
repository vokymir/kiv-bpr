#pragma once

#include "graph/Graph.hpp"
#include <cstddef>
#include <deque>
#include <imgui.h>
#include <utility>
namespace ssoc::ui {

class Visualizer {
private:
  // editable in UI
  float zoom_speed_ = 0.01f;
  float vertex_base_size_ = 5.0f;

  // editable via mouse
  float zoom_ = 100.0f;
  ImVec2 pan_ = ImVec2(0.0f, 0.0f);
  bool is_dragging_ = false;

  // static for every iteration
  ImVec2 origin_;
  ImVec2 size_;
  ImVec2 center_;
  ImVec2 footer_;

private:
  // === EVERYTHING BEFORE RENDERING

  // setup all canvas-related parameters which must be recalculated every
  // iteration
  void setup_canvas();
  // if user is panning, do it (assume the last item added is canvas)
  void pan();
  // if user is zooming, do it (assume the last item added is canvas)
  void zoom();

  // === HELPERS FOR RENDERING

  // helper to transform to screen coordinates
  ImVec2 to_screen(std::pair<double, double> pos);
  // helper to get vertex circle size
  float circle_size(int height);

  // === RENDERING

  // draw everything onto the drawing list
  void draw(ImDrawList *draw_list, const graph::Graph &g, size_t last_vertex,
            const std::deque<size_t> &maybe_toppling);
  // draw all edges (except to sink)
  void draw_edges(ImDrawList *draw_list, const graph::Graph &g);
  // draw all vertexes (except sink), their grain height & id
  void draw_vertexes(ImDrawList *draw_list, const graph::Graph &g,
                     size_t last_vertex);
  // draw info about vertexes which might be toppled in this avalanche
  void draw_topple_vertexes(ImDrawList *draw_list, const graph::Graph &g,
                            const std::deque<size_t> &maybe_toppling);

public:
  void show_window(const graph::Graph &g, bool &show, size_t last_vertex,
                   const std::deque<size_t> &checking_topple_vertexes);
};

} // namespace ssoc::ui
