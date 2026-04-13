#pragma once

#include <algorithm>
#include <cstdlib>
namespace ssoc {

enum class Run_Mode {
  Until_Avalanche,
  Forever,
};

struct Master_State {
  bool show_help_window = true;

  bool show_visualization_window = true;
  bool show_visualization_config_window = true;

  bool show_builder_window = true;
  bool show_simulation_control_window = true;
  bool show_stats_window = true;

  int draw_every = 1; // draw every Xth iteration - can greatly sped up things

  int draw_every_safe() {
    draw_every = std::max(1, std::abs(draw_every));
    return draw_every;
  }
};

enum struct Master_Action {
  None,
  Generate_Graph,
};

enum struct Control_Action {
  None,
  Step_In,
  Step_Over,
  Run_Until_Avalanche,
  Run,
  Stop,
};

} // namespace ssoc
