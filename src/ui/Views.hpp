#pragma once

#include "App_Utils.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "graph/Graph.hpp"
#include "ui/Visualizer.hpp"
#include <imgui.h>
namespace ssoc::ui::views {

[[nodiscard]] Master_Action draw_master_w(Master_State &state);

void draw_config_w(bool &show, Sim_Config &sim_cfg, Vis_Config &vis_cfg);

void draw_graph_w(bool &show, Visualizer &vis, const graph::Graph &g);

[[nodiscard]] Control_Action draw_control_w(bool &show);

// kinda PRIVATE
namespace _detail {

// = sim config
void draw_sim_config_s(Sim_Config &cfg);
void draw_gga_s(Graph_Generation_Algorithm &gga);
void draw_gga(gga_::Square_Lattice_2D &cfg);
void draw_gga(gga_::Dummy_GGA &cfg);

// = vis config
void draw_vis_config_s(Vis_Config &cfg);
void draw_gla_s(Graph_Layout_Algorithm &gla);
void draw_gla(gla_::Fruchterman_Reingold_2D &cfg);
void draw_gla(gla_::Dummy_GLA &cfg);

} // namespace _detail

} // namespace ssoc::ui::views
