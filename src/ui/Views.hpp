#pragma once

#include "App_Utils.hpp"
#include "Simulation_Setup_Config.hpp"
#include "Visual_Setup_Config.hpp"
#include "graph/Graph.hpp"
#include "stat/Stats_Collector.hpp"
#include "ui/Visualizer.hpp"
#include <cstddef>
#include <deque>
#include <imgui.h>
#include <memory>
namespace ssoc::ui::views {
/* Implementations are spread into separate .cpp files. Hint to which file is
 * provided in form of:
 *
 * =====>
 * (almost NAME OF FILE)
 *
 * It also works as visual indication of separation of concerns.
 * */

// =====>
// MENU

// menu uses Master State to allow user manually opening/closing individual
// windows
void draw_menu(Master_State &state);

// =====>
// HELP

// onboarding + overall help & about us info
void draw_welcome_help_window(bool &show);

// =====>
// GRAPH BUILDER

// allow user build their own graph, return if user clicked CREATE button
[[nodiscard]] Master_Action draw_graph_builder_windows(bool &show,
                                                       Sim_Config &sim_cfg,
                                                       Vis_Config &vis_cfg);

// =====>
// GRAPH VIS

// graph is modifiable (user using mouse to move vertices)
void draw_graph_visualization_window(
    bool &show, Vis_Config &vis_cfg, Visualizer &vis, graph::Graph &g,
    size_t last_vertex = 0,
    const std::deque<size_t> &checking_topple_vertices = {});

// =====>
// GRAPH CONTROL

// allow user to control visualization via UI
void draw_graph_control_window(bool &show, Visualizer_Config &cfg);

// =====>
// SIM CONTROL

// allow for controlling the simulation (stepping, rendering, etc.) via UI
[[nodiscard]] Control_Action draw_simulation_control_window(bool &show,
                                                            Master_State &state,
                                                            Sim_Config &cfg);

// =====>
// STATS

// show all wonderful statistics
void draw_stats_window(bool &show, const stat::Stats_Collector &sc);

// this namespace contain further implementation details which should not
// clutter the API of this file
namespace _detail {

// =====>
// HELP

struct HelpLayoutState {
  float split_x = 0.5f;
  float split_y = 0.5f;
};

void draw_help_title();

void draw_help_top(HelpLayoutState &s, float h, float w);
// welcome
void draw_help_top_left(float left_w);
// what is SOC
void draw_help_top_right();

void draw_help_bottom(HelpLayoutState &s, float h, float w);
// how to use
void draw_help_bottom_left(float left_w);
// about
void draw_help_bottom_right();

// =====>
// GRAPH BUILDER

// = sim config

// render simulation configuration (the left column)
void draw_sim_config_s(Sim_Config &cfg);
// draw the section in Sim Config which is relevant to graph generation
// algorithm
void draw_gga_s(Graph_Generation_Algorithm &gga);
// specific algorithm: square lattice
void draw_gga(gga_::Square_Lattice_2D &cfg);
// specific algorithm: watts strogatz
void draw_gga(gga_::Watts_Strogatz_2D &cfg);

// = vis config

// render visualization configuration (the right column)
void draw_vis_config_s(Vis_Config &cfg);
// draw the section which is relevant to graph layout algorithm
void draw_gla_s(Graph_Layout_Algorithm &gla);
// specific algorithm: fruchterman reingold
void draw_gla(gla_::Fruchterman_Reingold_2D &cfg);
// specific algorithm: hidden layout
void draw_gla(gla_::Hidden_GLA &cfg);

// =====>
// STATS

// ===
// OVERVIEW

void draw_stats_overview_s(const stat::Stats_Collector &sc);

// ===
// AVALANCHE SIZE

struct AvalancheSizePlotModel {
  std::vector<double> xs;
  std::vector<double> ys;

  double alpha = 0.0;
  double intercept = 0.0;
};

std::unique_ptr<AvalancheSizePlotModel>
build_avalanche_size_model(const std::vector<size_t> &input);

void plot_avalanche_size(const AvalancheSizePlotModel &model);

void draw_stats_avalanche_sizes_s(const stat::Stats_Collector &sc);

// ===
// AVALANCHE ORIGINS

struct AvalancheOriginPlotModel {
  std::vector<double> hist;
  size_t max_vertex = 0;
  size_t max_freq = 0;
};

struct AvalancheOriginGroupedPlotModel {
  std::vector<double> x;
  std::vector<double> grains;
  std::vector<double> origins;

  size_t max_vertex = 0;
};

std::unique_ptr<AvalancheOriginPlotModel>
build_origin_model(const std::vector<size_t> &histogram);

void plot_origin(const AvalancheOriginPlotModel &model);

std::unique_ptr<AvalancheOriginGroupedPlotModel>
build_origin_grouped_model(const stat::Stats_Collector &sc);

void plot_origin_grouped(const AvalancheOriginGroupedPlotModel &model);

void draw_stats_avalanche_origins_s(const stat::Stats_Collector &sc);
void section_avalanche_origins_grouped(const stat::Stats_Collector &sc);

// ===
// GRAINS

struct GrainsPlotModel {
  std::vector<double> recent;
  std::vector<double> moving_avg;

  double max_val = 0.0;
  size_t latest = 0;
};

std::unique_ptr<GrainsPlotModel>
build_grains_model(const std::vector<size_t> &grains, int display_count,
                   int window_size);

void plot_grains(const GrainsPlotModel &model);

void draw_stats_grains_s(const stat::Stats_Collector &sc);

// ===
// INTERNAL HELPERS

std::pair<double, double> fit_power_law(const std::vector<double> &xs,
                                        const std::vector<double> &ys);

std::vector<double> make_power_law_fit(const std::vector<double> &xs,
                                       double alpha, double intercept);

} // namespace _detail

} // namespace ssoc::ui::views
