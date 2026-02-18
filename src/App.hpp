#pragma once

#include "App_Utils.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "graph/Graph.hpp"
#include "ui/Visualizer.hpp"
#include "ui/Window_Context.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <queue>
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

  // CONTROL vars
  bool running_ = false;
  size_t last_toppled_ = 0;
  std::function<bool()> stop_cond_ = []() -> bool { return true; };
  std::queue<size_t> to_topple_{};

public:
  void run();

private:
  void init();

  // set random number to correspond to graph vert count
  void set_dist() {
    dist_ = std::uniform_int_distribution<size_t>(0, g_->num_vertices() - 1);
  }

  void master_action(Master_Action action);
  void control_action(Control_Action action);

  // add one sand to history&heights + in the check_toppling queue
  // return index
  size_t drop_sand();
  // if avalanche is in progress, only proceed one step
  void step_in();
  // step over avalanches in one step
  void step_over();
  // periodically update graph until avalanches is found
  void step_run_until_avalanche();
  // periodically update graph until STOPped
  void step_run();
  // do the periodic update
  void periodic_step();

  // return how many vertices were topled
  // may add new vertices to check_toppling_
  // OPTIONS
  //
  //  (avalanche)
  //  0         => topple until the queue is empty
  //
  //  (??? = degree(vert))
  //  1 - N     => check at most N vertices in the queue
  //
  //  (step in = -1)
  //  -1 - N    => topple until at most N vertices were toppled
  size_t check_topple(int option);
};

} // namespace ssoc
