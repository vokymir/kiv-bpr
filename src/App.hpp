#pragma once

#include "Sim_Config.hpp"
#include "graph/Graph.hpp"
#include "ui/Ui.hpp"
#include <memory>
namespace ssoc {

/* Orchestrates everything. */
class App {
private:
  Sim_Config cfg_{};
  std::unique_ptr<graph::Graph> g_{};
  ui::UI ui_{};

public:
  void run();

private:
  void init();
};

} // namespace ssoc
