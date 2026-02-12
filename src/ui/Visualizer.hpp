#pragma once

#include "../graph/Graph.hpp"
#include "imgui.h"
namespace ssoc::ui {

class Visualizer {
private:
  float zoom_ = 100.0f;
  ImVec2 pan_ = ImVec2(0.0f, 0.0f);
  bool is_dragging_ = false;

public:
  void show_window(const graph::Graph &g, bool &show);
};

} // namespace ssoc::ui
