#pragma once

#include "Sim_Config.hpp"
#include "graph/Graph.hpp"
#include "ui/Ui.hpp"
#include <memory>
#include <random>
namespace ssoc {

/* Orchestrates everything. */
class App {
private:
  bool initialized_ = false;
  Sim_Config cfg_;
  std::unique_ptr<graph::Graph> g_;
  std::unique_ptr<ui::UI> ui_;
  std::random_device rd_;
  std::default_random_engine rng_;
  std::uniform_int_distribution<int> dist_;

public:
  void run();

private:
  void init();

  void set_dist() {
    dist_ = std::uniform_int_distribution<int>(0, g_->num_vertices() - 1);
  }

  // if graph exists, do one step
  void step();
};

} // namespace ssoc
