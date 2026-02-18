#include "App.hpp"
#include "App_Utils.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "graph/Generator.hpp"
#include "ui/Views.hpp"
#include "ui/Window_Context.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <cstddef>
#include <memory>
#include <print>
#include <random>
#include <stdexcept>

namespace ssoc {

void App::init() {
  if (initialized_) {
    throw std::runtime_error("App double initialization.");
  }

  sim_cfg_.gga = gga_::Square_Lattice_2D{3, 4, false, false};
  vis_cfg_.gla = gla_::Fruchterman_Reingold_2D{gla_::FR2D_Accuracy::High};

  // start with SOME graph
  g_ = graph::generate::igraph_from_config(sim_cfg_, vis_cfg_);

  win_context_ = std::make_unique<ui::Window_Context>();

  rng_ = std::default_random_engine(rd_());

  initialized_ = true;
}

void App::run() {
  if (!initialized_) {
    init();
  }

  win_context_->init();

  bool should_end = false;
  bool generate_graph = false;

  while (!should_end) {
    win_context_->pollevs(should_end);

    if (generate_graph) {
      g_ = graph::generate::igraph_from_config(sim_cfg_, vis_cfg_);
      set_dist();
      generate_graph = false;
    }

    win_context_->begin_frame();
    // vvvvv

    master_action(ui::views::draw_master_w(master_state_));

    if (master_state_.show_config_window) {
      ui::views::draw_config_w(master_state_.show_config_window, sim_cfg_,
                               vis_cfg_);
    }
    if (master_state_.show_graph_window) {
      ui::views::draw_graph_w(master_state_.show_graph_window, vis_, *g_);
    }
    if (master_state_.show_control_window) {
      control_action(
          ui::views::draw_control_w(master_state_.show_control_window));
    }

    if (running_) {
      periodic_step();
    }

    // ^^^^^
    win_context_->end_frame(bg_clr_);
  }

  win_context_->shutdown();
}

void App::master_action(Master_Action action) {
  switch (action) {
  case Master_Action::None:
    return;
  case Master_Action::Generate_Graph:
    g_ = graph::generate::igraph_from_config(sim_cfg_, vis_cfg_);
    master_state_.show_graph_window = true;
    break;
  }
}

void App::control_action(Control_Action action) {
  if (!g_) {
    std::print("Tried to step on not existing graph.\n");
    return;
  }

  switch (action) {
  case Control_Action::None:
    return;
  case Control_Action::Step_In:
    step_in();
    break;
  case Control_Action::Step_Over:
    step_over();
    break;
  case Control_Action::Run_Until_Avalanche:
    step_run_until_avalanche();
    break;
  case Control_Action::Run:
    step_run();
    break;
  case Control_Action::Stop:
    running_ = false;
    break;
  }
}

size_t App::drop_sand() {
  auto &heights = g_->sand_height(0);
  auto idx = dist_(rng_) % heights.size();

  g_->sand_history(0).push_back(idx);
  heights[idx] += 1;

  check_toppling_.push(idx);

  return idx;
}

void App::step_in() {
  if (check_topple(-1) > 0) {
    return;
  }

  drop_sand();
  check_topple(-1);
}

void App::step_over() {
  if (check_topple(0) > 0) {
    return;
  }

  drop_sand();
  check_topple(0);
}

void App::step_run_until_avalanche() {
  running_ = true;
  stop_cond_ = [this]() { return last_toppled_ > 1; };
}

void App::step_run() {
  running_ = true;
  stop_cond_ = []() { return false; };
}

void App::periodic_step() {
  if (stop_cond_) {
    running_ = false;
    return;
  }
  drop_sand();
  last_toppled_ = check_topple(0);
}

size_t App::check_topple(int option) {
  if (check_toppling_.size() == 0) {
    return 0;
  }
  size_t checked = 0, toppled = 0;

  auto should_continue = [&]() -> bool {
    // don't index inside empty queue
    if (check_toppling_.size() == 0) {
      return false;
    }

    if (option == 0) { // until empty
      return check_toppling_.size() > 0;
    } else if (option > 0) { // at most checked times
      return checked < static_cast<size_t>(option);
    } else if (option < 0) { // until at most option vertices are toppled
      return toppled < static_cast<size_t>(-option);
    }

    return false;
  };

  auto &adj_off = g_->adj_offsets();
  auto &adj_vert = g_->adj_vertices();
  auto &heights = g_->sand_height(0);
  do {
    // which one to check
    auto idx = check_toppling_.front();
    check_toppling_.pop();

    // check if higher sandpile than neighbour count
    auto neighbour_count = adj_off[idx + 1] - adj_off[idx];
    if (heights[idx] >= static_cast<int>(neighbour_count)) {
      heights[idx] -= static_cast<int>(neighbour_count);

      // sprinkle sand to neighbours
      auto start = adj_vert[adj_off[idx]];
      for (auto i = start; i < start + neighbour_count; ++i) {
        heights[i] += 1;
      }
    }

  } while (should_continue());

  return toppled;
}

} // namespace ssoc
