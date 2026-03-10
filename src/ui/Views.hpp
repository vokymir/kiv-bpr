#pragma once

#include "App_Utils.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "graph/Graph.hpp"
#include "stat/Stats_Collector.hpp"
#include "ui/Visualizer.hpp"
#include <cstddef>
#include <deque>
#include <imgui.h>
#include <memory>
namespace ssoc::ui::views {

[[nodiscard]] Master_Action draw_master_w(Master_State &state);

void draw_config_w(bool &show, Sim_Config &sim_cfg, Vis_Config &vis_cfg);

void draw_graph_w(bool &show, Visualizer &vis, const graph::Graph &g,
                  size_t last_vertex = 0,
                  const std::deque<size_t> &checking_topple_vertexes = {});

[[nodiscard]] Control_Action draw_control_w(bool &show);

void draw_stats_w(bool &show, const stat::Stats_Collector &sc);

// kinda PRIVATE
namespace _detail {

// = sim config
void draw_sim_config_s(Sim_Config &cfg);
void draw_gga_s(Graph_Generation_Algorithm &gga);
void draw_gga(gga_::Square_Lattice_2D &cfg);
void draw_gga(gga_::Watts_Strogatz_2D &cfg);

// = vis config
void draw_vis_config_s(Vis_Config &cfg);
void draw_gla_s(Graph_Layout_Algorithm &gla);
void draw_gla(gla_::Fruchterman_Reingold_2D &cfg);
void draw_gla(gla_::Hidden_GLA &cfg);

// = stats
void draw_stats_overview_s(const stat::Stats_Collector &sc);

// == avalanche size

// all neccesarry data taken from sc.avalanche_sizes()
struct Avalanche_Size_Plot_Data {
  std::vector<double> xs;
  std::vector<double> ys;
  // max key and count are not related
  size_t max_key = 0;
  size_t max_count = 0;
};
// get the struct from given sc.avalanche_sizes()
std::unique_ptr<Avalanche_Size_Plot_Data>
prepare_avalanche_data(const std::unordered_map<size_t, size_t> &size_map);
// actually render the ImPlot
void render_avalanche_size_plot(const Avalanche_Size_Plot_Data &data);

void draw_stats_avalanche_sizes_s(const stat::Stats_Collector &sc);

// == avalanche origin

struct Avalanche_Origin_Plot_Data {
  std::vector<double> hist;
  size_t max_vertex = 0;
  double max_freq = 0.0;
};

std::unique_ptr<Avalanche_Origin_Plot_Data>
prepare_origin_data(const std::unordered_map<size_t, size_t> &origin_map);

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
prepare_grains_data(const std::vector<size_t> &grains);

void render_grains_count_plot(const Grains_Count_Plot_Data &data);

void draw_stats_grains_counts_s(const stat::Stats_Collector &sc);

} // namespace _detail

} // namespace ssoc::ui::views
