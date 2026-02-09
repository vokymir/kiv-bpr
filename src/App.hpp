#pragma once

#include <SDL3/SDL_video.h>
namespace ssoc {

/* Orchestrates everything. */
class App {
private:
  SDL_Window *window_;
  SDL_GLContext gl_context_;
  bool running_ = false;

public:
  void run();

private:
  void setup_SDL();
  void setup_openGL();
  void setup_ImGui();
  void main_loop();
  void cleanup();
};

} // namespace ssoc
