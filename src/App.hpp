#pragma once

#include "graph/Graph.hpp"
#include "ui/Ui.hpp"
namespace ssoc {

/* Orchestrates everything. */
class App {
private:
  ui::UI ui_{};

public:
  void run(const graph::Graph &g);
};

} // namespace ssoc
