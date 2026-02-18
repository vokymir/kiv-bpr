#pragma once

#include "App_Utils.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "graph/Graph.hpp"
#include "stat/Simulation_Events.hpp"
#include "stat/Stats_Collector.hpp"
#include "ui/Visualizer.hpp"
#include "ui/Window_Context.hpp"
#include <cstddef>
#include <deque>
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

  // CONTROL vars
  bool running_ = false;
  Run_Mode run_mode_ = Run_Mode::Until_Avalanche;
  std::deque<size_t> to_topple_{};

  // STATS
  stat::Simulation_Events events_;
  stat::Stats_Collector stats_;
  // transient (temporary) stats
  size_t current_step_ = 0;
  size_t current_vertex_;
  size_t avalanche_topples_ = 0;

public:
  // hook-up stats gathering machinery
  App() : stats_(events_) {}

  void run();

private:
  // possible to defer initialization...
  void init();

  // set random number to correspond to graph vert count
  void set_dist() {
    // 0-(N-1) is standard convertion
    // -1 is for the last vertex is a sink = don't want to choose it for sand
    dist_ = std::uniform_int_distribution<size_t>(0, g_->num_vertices() - 2);
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
