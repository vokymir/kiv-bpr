#pragma once

#include "../Sim_Config.hpp"
#include "../graph/Graph.hpp"
#include "Visualizer.hpp"
#include "imgui.h"
#include <SDL3/SDL.h>
namespace ssoc::ui {

class UI {
private:
  SDL_Window *window_ = nullptr;
  SDL_GLContext gl_context_;
  ImVec4 bg_clr_ = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // graph window TEMP
  Visualizer vis_;
  bool show_vis_ = false;
  bool show_cfg_ = false;

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

  // draw one graph in a separate window
  void draw_graph(const graph::Graph &g);

  // set bool to true, if user clicked
  void draw_master_window(bool &generate_graph);

  void draw_config_window(Sim_Config &cfg);

private:
  // draw all fields of config without wrapping them inside window
  void draw(Sim_Config &cfg);
  void draw(Vis_Config &cfg);
  // === sim config
  void draw(Graph_Generation_Algorithm &gga);
  void draw(gga_::Square_Lattice_2D &cfg);
  void draw(gga_::Dummy_GGA &cfg);
  // === vis config
  void draw(Graph_Layout_Algorithm &gla);
  void draw(gla_::Fruchterman_Reingold_2D &cfg);
  void draw(gla_::Dummy_GLA &cfg);
};

} // namespace ssoc::ui
