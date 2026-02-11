#pragma once

#include "../graph/Graph.hpp"
#include "Visualizer.hpp"
#include "imgui.h"
#include <SDL3/SDL.h>
namespace ssoc::ui {

class UI {
private:
  SDL_Window *window_;
  SDL_GLContext gl_context_;
  ImVec4 bg_clr_ = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // graph window TEMP
  Visualizer vis_;

public:
  // initialize everything the UI (ImGui, OpenGL & SDL) need
  void init();
  // poll events in every iteration of main loop
  // set given bool to true if app should end
  void pollevs(bool &should_end);
  // begin the ImGui frame
  void begin_frame();
  // end the ImGui frame & render
  void end_frame();
  // clear all data after ending the mainloop
  void clear();

  void draw_graph(const ssoc::graph::Graph &g);
};

} // namespace ssoc::ui
