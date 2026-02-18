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
  switch (action) {
  case Control_Action::None:
    return;
  case Control_Action::Step:
    step();
    break;
  }
}

void App::step() {
  if (!g_) {
    std::print("Tried to step on not existing graph.\n");
    return;
  }

  auto &heights = g_->sand_height(0);

  auto idx = dist_(rng_) % heights.size();

  g_->sand_history(0).push_back(idx);
  heights[idx] += 1;

  check_topple(idx);
}

void App::check_topple(size_t idx) {
  auto &sand = g_->sand_height(0);

  if (sand[idx] >= 4) {
    sand[idx] -= 4;
    auto neigh_start = g_->adj_offsets()[idx];
    auto neigh_end = g_->adj_offsets()[idx + 1];

    auto adj = g_->adj_vertices();

    for (auto i = neigh_start; i < neigh_end; ++i) {
      auto idxx = adj[i];
      sand[idxx] += 1;
      check_topple(idxx);
    }
  }
}

} // namespace ssoc
