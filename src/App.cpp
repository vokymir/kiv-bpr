#include "App.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

void ssoc::App::run() {
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
