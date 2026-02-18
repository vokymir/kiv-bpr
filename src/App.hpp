#pragma once

#include "App_Utils.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "graph/Graph.hpp"
#include "ui/Visualizer.hpp"
#include "ui/Window_Context.hpp"
#include <cstddef>
#include <memory>
#include <random>
namespace ssoc {

/* Orchestrates everything. */
class App {
private:
  bool initialized_ = false;

  // CONFIGURATION
  Sim_Config sim_cfg_;
  Vis_Config vis_cfg_;
  Master_State master_state_;
  std::unique_ptr<graph::Graph> g_;

  // UI
  std::unique_ptr<ui::Window_Context> win_context_;
  ImVec4 bg_clr_ = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  ui::Visualizer vis_;

  // RANDOM
  std::random_device rd_;
  std::default_random_engine rng_;
  std::uniform_int_distribution<size_t> dist_;

public:
  void run();

private:
  void init();

  void set_dist() {
    dist_ = std::uniform_int_distribution<size_t>(0, g_->num_vertices() - 1);
  }

  void master_action(Master_Action action);
  void control_action(Control_Action action);

  // if graph exists, do one step
  void step();

  void check_topple(size_t idx);
};

} // namespace ssoc
