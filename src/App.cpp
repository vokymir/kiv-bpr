#include "App.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "graph/Generator.hpp"
#include "ui/Ui.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <memory>
#include <print>
#include <random>
#include <stdexcept>

namespace ssoc {

void App::init() {
  if (initialized_) {
    throw std::runtime_error("App double initialization.");
  }

  cfg_.gga = gga_::Square_Lattice_2D{3, 4, false, false};
  cfg_.visual.gla = gla_::Fruchterman_Reingold_2D{gla_::High};

  // g_ = graph::generate::igraph_from_config(cfg_);

  ui_ = std::make_unique<ui::UI>();

  rng_ = std::default_random_engine(rd_());

  initialized_ = true;
}

void App::run() {
  if (!initialized_) {
    init();
  }

  ui_->init();

  bool should_end = false;
  bool generate_graph = false;

  while (!should_end) {
    ui_->pollevs(should_end);

    if (generate_graph) {
      g_ = graph::generate::igraph_from_config(cfg_);
      set_dist();
      generate_graph = false;
    }

    ui_->begin_frame();
    // vvvvv

    ui_->draw_master_window(generate_graph);
    if (ui_->draw_stepping_control()) {
      step();
    }
    ui_->draw_config_window(cfg_);
    if (g_) {
      ui_->draw_graph(*g_);
    }

    // ^^^^^
    ui_->end_frame();
  }

  ui_->clear();
}

void App::step() {
  if (!g_) {
    std::print("Tried to step on not existing graph.\n");
    return;
  }

  int idx = dist_(rng_);

  g_->sand_height(0)[idx] += 1;

  check_topple(idx);
}

void App::check_topple(int idx) {
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
