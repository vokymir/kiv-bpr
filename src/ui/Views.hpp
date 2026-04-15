#pragma once

#include "App_Utils.hpp"
#include "Simulation_Setup_Config.hpp"
#include "Visual_Setup_Config.hpp"
#include "graph/Graph.hpp"
#include "stat/Stats_Collector.hpp"
#include "ui/Visualizer.hpp"
#include <cmath>
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
void draw_stats_overview_s(const stat::Stats_Collector &sc);

inline std::pair<double, double> fit_power_law(const std::vector<double> &xs,
                                               const std::vector<double> &ys) {
  // log(y) = a log(x) + b  => a = alpha

  double Sx = 0, Sy = 0, Sxx = 0, Sxy = 0;
  size_t n = 0;

  for (size_t i = 0; i < xs.size(); ++i) {
    double x = xs[i];
    double y = ys[i];

    if (x <= 0 || y <= 0)
      continue;

    double lx = std::log(x);
    double ly = std::log(y);

    Sx += lx;
    Sy += ly;
    Sxx += lx * lx;
    Sxy += lx * ly;
    ++n;
  }

  if (n < 2)
    return {0.0, 0.0};

  double denom = n * Sxx - Sx * Sx;
  if (denom == 0)
    return {0.0, 0.0};

  double a = (n * Sxy - Sx * Sy) / denom;
  double b = (Sy - a * Sx) / n;

  return {a, b}; // alpha, intercept
}
static std::vector<double> make_fit_line(const std::vector<double> &xs,
                                         double a, double b) {
  std::vector<double> yfit;
  yfit.reserve(xs.size());

  for (double x : xs) {
    yfit.push_back(std::exp(b) * std::pow(x, a));
  }

  return yfit;
}

// == avalanche size

// all neccesarry data taken from sc.avalanche_sizes()
struct Avalanche_Size_Plot_Data {
  std::vector<double> xs;
  std::vector<double> ys;

  double alpha = 0.0;
  double intercept = 0.0;
};
// get the struct from given sc.avalanche_sizes()
std::unique_ptr<Avalanche_Size_Plot_Data>
prepare_avalanche_data(const std::vector<size_t> &input_data);
// actually render the ImPlot
void render_avalanche_size_plot(const Avalanche_Size_Plot_Data &data);

void draw_stats_avalanche_sizes_s(const stat::Stats_Collector &sc);

// == avalanche origin

struct Avalanche_Origin_Plot_Data {
  // must be double ImPlot::PlotBars
  std::vector<double> hist;
  size_t max_vertex = 0;
  size_t max_freq = 0.0;
};

struct Avalanche_Origin_Grouped_Plot_Data {
  std::vector<double> x;
  std::vector<double> grains;
  std::vector<double> origins;

  size_t max_vertex = 0;
};
void render_avalanche_origin_grouped_plot(
    const Avalanche_Origin_Grouped_Plot_Data &data);
std::unique_ptr<Avalanche_Origin_Grouped_Plot_Data>
prepare_origin_grouped_data(const stat::Stats_Collector &sc);
void draw_stats_grouped_origins(const stat::Stats_Collector &sc);

std::unique_ptr<Avalanche_Origin_Plot_Data>
prepare_origin_data(const std::vector<size_t> &histogram);

void render_avalanche_origin_plot(const Avalanche_Origin_Plot_Data &data);

void draw_stats_avalanche_origins_s(const stat::Stats_Collector &sc);

// == grains

struct Grains_Count_Plot_Data {
  std::vector<double> recent_hist;
  std::vector<double> moving_avg;
  double max_val = 0.0;
  size_t latest_val = 0;
};

std::unique_ptr<Grains_Count_Plot_Data>
prepare_grains_data(const std::vector<size_t> &grains, int display_grains,
                    int win_size);

void render_grains_count_plot(const Grains_Count_Plot_Data &data);

void draw_stats_grains_counts_s(const stat::Stats_Collector &sc);

} // namespace _detail

} // namespace ssoc::ui::views
