#include "App.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "graph/Generator.hpp"
#include "ui/Ui.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <memory>
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

  initialized_ = true;
}

void App::run() {
  if (!initialized_) {
    init();
  }

  ui_->init();

  bool should_end = false;

  while (!should_end) {
    ui_->pollevs(should_end);

    ui_->begin_frame();
    // vvvvv

    ui_->draw_master_window();
    ui_->draw_config_window(cfg_);
    if (g_) {
      ui_->draw_graph(*g_);
    }

    // ^^^^^
    ui_->end_frame();
  }

  ui_->clear();
}

} // namespace ssoc
