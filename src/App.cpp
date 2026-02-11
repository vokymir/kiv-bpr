#include "App.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "graph/Generator.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

namespace ssoc {

void App::init() {
  cfg_.gga = gga_::Square_Lattice_2D{3, 4, false, false};
  cfg_.visual.gla = gla_::Fruchterman_Reingold_2D{gla_::High};

  g_ = graph::generate::igraph_from_config(cfg_);
}

void App::run() {
  ui_.init();

  bool should_end = false;

  while (!should_end) {
    ui_.pollevs(should_end);

    ui_.begin_frame();
    // vvvvv

    ui_.draw_graph(*g_);

    // ^^^^^
    ui_.end_frame();
  }

  ui_.clear();
}

} // namespace ssoc
