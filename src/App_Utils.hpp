#pragma once

namespace ssoc {

struct Master_State {
  bool show_graph_window = true;
  bool show_config_window = true;
  bool show_control_window = true;
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
