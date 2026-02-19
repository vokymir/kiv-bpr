#include "ui/Views.hpp"
#include "App_Utils.hpp"
#include "Vis_Config.hpp"
#include <algorithm>
#include <cstddef>
#include <deque>
#include <imgui.h>
#include <map>
#include <string>
#include <variant>
#include <vector>
namespace ssoc::ui::views {

Master_Action draw_master_w(Master_State &state) {
  Master_Action action = Master_Action::None;

  ImGui::SetNextWindowPos(ImVec2(31, 21), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(249, 189), ImGuiCond_FirstUseEver);
  ImGui::Begin("Master window", nullptr);

  ImGui::Checkbox("Show graph", &state.show_graph_window);
  ImGui::Checkbox("Show config", &state.show_config_window);
  ImGui::Checkbox("Show stepping", &state.show_control_window);
  ImGui::Checkbox("Show stats", &state.show_stats_window);

  if (ImGui::Button("Generate graph")) {
    action = Master_Action::Generate_Graph;
  }

  ImGui::End();

  return action;
}

void draw_config_w(bool &show, Sim_Config &sim_cfg, Vis_Config &vis_cfg) {
  ImGui::SetNextWindowPos(ImVec2(22, 233), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(352, 459), ImGuiCond_FirstUseEver);
  ImGui::Begin("Config window", &show);

  if (ImGui::CollapsingHeader("General configuration")) {
    _detail::draw_sim_config_s(sim_cfg);
  }

  if (ImGui::CollapsingHeader("Visual configuration")) {
    _detail::draw_vis_config_s(vis_cfg);
  }

  ImGui::End();
}

void draw_graph_w(bool &show, Visualizer &vis, const graph::Graph &g,
                  size_t last_vertex,
                  const std::deque<size_t> &checking_topple_vertexes) {
  ImGui::SetNextWindowPos(ImVec2(385, 86), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(729, 571), ImGuiCond_FirstUseEver);

  vis.show_window(g, show, last_vertex, checking_topple_vertexes);
}

Control_Action draw_control_w(bool &show) {
  Control_Action action = Control_Action::None;

  ImGui::SetNextWindowPos(ImVec2(423, 16), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(562, 54), ImGuiCond_FirstUseEver);
  ImGui::Begin("Stepping control", &show);

  if (ImGui::Button("Step In")) {
    action = Control_Action::Step_In;
  }

  ImGui::SameLine();
  if (ImGui::Button("Step Over")) {
    action = Control_Action::Step_Over;
  }

  ImGui::SameLine();
  if (ImGui::Button("Run right until avalanche happened")) {
    action = Control_Action::Run_Until_Avalanche;
  }

  ImGui::SameLine();
  if (ImGui::Button("Run forever")) {
    action = Control_Action::Run;
  }

  ImGui::SameLine();
  if (ImGui::Button("Stop")) {
    action = Control_Action::Stop;
  }

  ImGui::End();

  return action;
}

void draw_stats_w(bool &show, const ssoc::stat::Stats_Collector &sc) {
  ImGui::Begin("Stats", &show);

  _detail::draw_stats_overview_s(sc);
  ImGui::Separator();

  _detail::draw_stats_avalanche_sizes_s(sc);
  ImGui::Separator();

  _detail::draw_stats_avalanche_origins_s(sc);
  ImGui::Separator();

  _detail::draw_stats_grains_counts_s(sc);
  ImGui::End();
}

namespace _detail {

void draw_sim_config_s(Sim_Config &cfg) { draw_gga_s(cfg.gga); }

void draw_gga_s(Graph_Generation_Algorithm &gga) {
  constexpr const char *labels[] = {
      "Square Lattice 2D",
      "Dummy",
  };

  size_t idx = gga.index();

  for (size_t i = 0; i < std::size(labels); ++i) {
    if (ImGui::RadioButton(labels[i], idx == i)) {
      switch (i) {
      case 0:
        gga = gga_::Square_Lattice_2D{};
        break;
      case 1:
        gga = gga_::Dummy_GGA{};
        break;

      default:
        gga = gga_::Square_Lattice_2D{};
        break;
      }
    }
  }

  std::visit([](auto &alg) { draw_gga(alg); }, gga);
}

void draw_gga(gga_::Square_Lattice_2D &cfg) {
  ImGui::SeparatorText("Dimensions");
  ImGui::InputInt("Width", &cfg.width);
  ImGui::InputInt("Height", &cfg.height);

  ImGui::SeparatorText("Boundary Conditions");
  ImGui::Checkbox("Circular on x axis", &cfg.circular_on_x);
  ImGui::Checkbox("Circular on y axis", &cfg.circular_on_y);

  ImGui::SeparatorText("Sandpile Rules");

  const char *rule_names[] = {"Fill To Four", "All Once"};
  int current_rule = static_cast<int>(cfg.sink_rule);

  if (ImGui::Combo("Sink Rule", &current_rule, rule_names,
                   IM_ARRAYSIZE(rule_names))) {
    cfg.sink_rule =
        static_cast<gga_::Square_Lattice_2D::Sink_Rule>(current_rule);
  }

  if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
    ImGui::BeginTooltip();
    if (cfg.sink_rule == gga_::Square_Lattice_2D::Sink_Rule::Fill_To_Four) {
      ImGui::Text(
          "Ensures all nodes have degree 4 by adding edges to the sink.");
    } else {
      ImGui::Text("Adds exactly one edge to the sink for every node.");
    }
    ImGui::EndTooltip();
  }
}

void draw_gga(gga_::Dummy_GGA &cfg) {
  ImGui::InputInt("Size", &cfg.size);
  ImGui::Checkbox("Bool", &cfg.boolean);
  ImGui::InputFloat("FT", &cfg.ft);
}

void draw_vis_config_s(Vis_Config &cfg) { draw_gla_s(cfg.gla); }

void draw_gla_s(Graph_Layout_Algorithm &gla) {
  constexpr const char *labels[] = {
      "Fruchterman Reingold 2D",
      "Dummy ui",
  };

  size_t idx = gla.index();

  for (size_t i = 0; i < std::size(labels); ++i) {
    if (ImGui::RadioButton(labels[i], idx == i)) {
      switch (i) {
      case 0:
        gla = gla_::Fruchterman_Reingold_2D{};
        break;
      case 1:
        gla = gla_::Dummy_GLA{};
        break;

      default:
        gla = gla_::Fruchterman_Reingold_2D{};
        break;
      }
    }
  }

  std::visit([](auto &alg) { draw_gla(alg); }, gla);
}

void draw_gla(gla_::Fruchterman_Reingold_2D &cfg) {
  size_t acc = static_cast<size_t>(cfg.accuracy);

  ImGui::Text("Accuracy");
  if (ImGui::RadioButton("High", acc == 0)) {
    acc = 0;
  }
  if (ImGui::RadioButton("Low", acc == 1)) {
    acc = 1;
  }
  if (ImGui::RadioButton("Auto", acc == 2)) {
    acc = 2;
  }

  cfg.accuracy = static_cast<gla_::FR2D_Accuracy>(acc);
}

void draw_gla(gla_::Dummy_GLA &cfg) { ImGui::InputInt("x", &cfg.x); }

void draw_stats_overview_s(const stat::Stats_Collector &sc) {
  const auto &records = sc.avalanche_records();

  ImGui::TextDisabled("Summary");
  ImGui::Text("Steps: %zu", records.empty() ? 0 : records.back().step);
  ImGui::Text("Total avalanches: %zu", records.size());

  if (records.empty()) {
    ImGui::Text("Waiting for data...");
  } else {
    double avg_size = static_cast<double>(sc.avalanche_total_sizes()) /
                      static_cast<double>(records.size());

    ImGui::Text("Average Size: %.2f", avg_size);
    ImGui::Text("Max Size:     %zu", sc.avalanche_max_size());
  }
}

void draw_stats_avalanche_sizes_s(const stat::Stats_Collector &sc) {
  // --- 2. Histogram: Avalanche SIZES ---
  // Problem: Unordered map is random. Size index can be massive (sparse).
  // Solution: Sort by size, and clamp display if needed, or show a dense plot.

  const auto &size_map = sc.avalanche_sizes();
  if (!size_map.empty()) {
    ImGui::Text("Distribution of Sizes (X=Size, Y=Count)");

    // 1. Find the actual max size to determine plot range
    // usage of std::map ensures keys are sorted: 1, 2, 3...
    std::map<size_t, size_t> sorted_sizes(size_map.begin(), size_map.end());

    size_t max_key = sorted_sizes.rbegin()->first;
    size_t max_count = 0;
    for (const auto &pair : sorted_sizes)
      max_count = std::max(max_count, pair.second);

    // SAFETY: If max_key is huge (e.g. > 2000), plotting index-by-index is
    // too heavy for standard ImGui::PlotHistogram.
    // For accurate stats, we visualize the first N sizes, or we need a proper
    // log-plot library. Here we cap at 500 for UI performance, or dynamic if
    // small.
    static const size_t MAX_PLOT_BARS = 500;
    size_t plot_range = std::min(max_key + 1, MAX_PLOT_BARS);

    std::vector<float> hist_data(plot_range, 0.0f);

    for (const auto &[size, count] : sorted_sizes) {
      if (size < plot_range) {
        hist_data[size] = static_cast<float>(count);
      }
    }

    std::string overlay = "Max Count: " + std::to_string(max_count);
    ImGui::PlotHistogram("##SizeHist", hist_data.data(),
                         static_cast<int>(hist_data.size()), 0, overlay.c_str(),
                         0.0f, static_cast<float>(max_count),
                         ImVec2(-1, 80) // -1 width = fill window
    );

    // TOOLTIP: Hover to see exact stats
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("X axis = Avalanche Size (capped at %zu)", plot_range);
      ImGui::Text("Y axis = Frequency");
      if (max_key >= MAX_PLOT_BARS) {
        ImGui::TextColored(ImVec4(1, 0.5, 0, 1),
                           "Warning: Long tail truncated (Max size: %zu)",
                           max_key);
      }
      ImGui::EndTooltip();
    }
  }
}

void draw_stats_avalanche_origins_s(const stat::Stats_Collector &sc) {
  // --- 3. Histogram: Avalanche ORIGINS ---
  // Problem: unordered_map order is undefined.
  // Solution: Sort by Vertex Index so the graph represents physical space 0..N

  const auto &origin_map = sc.avalanche_origins();
  if (!origin_map.empty()) {
    ImGui::Text("Avalanche Origins (X=Vertex ID, Y=Count)");

    // Sort by Origin ID (Key)
    std::map<size_t, size_t> sorted_origins(origin_map.begin(),
                                            origin_map.end());

    if (!sorted_origins.empty()) {
      size_t max_vertex = sorted_origins.rbegin()->first;

      // Create a dense vector for plotting 0 to MaxVertex
      // Only practical if vertex count is reasonable (< ~2000).
      // If you have 1M vertices, you need binning. Assuming < 2000 for display:
      std::vector<float> origin_hist;
      size_t safe_size = std::min(max_vertex + 1, (size_t)2000);
      origin_hist.resize(safe_size, 0.0f);

      float max_freq = 0.0f;
      for (const auto &[origin, count] : sorted_origins) {
        if (origin < safe_size) {
          origin_hist[origin] = static_cast<float>(count);
          if (static_cast<float>(count) > max_freq)
            max_freq = static_cast<float>(count);
        }
      }

      ImGui::PlotHistogram("##Origins", origin_hist.data(),
                           static_cast<int>(origin_hist.size()), 0, nullptr,
                           0.0f, max_freq, ImVec2(-1, 80));

      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Distribution of where avalanches start.\nFlat = "
                          "Uniform randomness.\nSpikes = Critical spots.");
      }
    }
  }
}

void draw_stats_grains_counts_s(const stat::Stats_Collector &sc) {

  // --- 4. Recent Grains (Time Series) ---
  // Problem: Vector grows indefinitely.
  // Solution: Only plot the last N items.

  const auto &grains = sc.grains_counts();
  if (!grains.empty()) {
    ImGui::Text("Recent Grains Dropped (Last 200 steps)");

    int count = static_cast<int>(grains.size());
    int offset = 0;
    const int max_display = 200;

    // Determine start pointer to only show last N
    const size_t *data_ptr = grains.data();
    int display_count = count;

    if (count > max_display) {
      data_ptr += (count - max_display);
      display_count = max_display;
    }

    // Convert only the slice to float for ImGui
    std::vector<float> recent_hist(display_count);
    float max_val = 0.0f;
    for (int i = 0; i < display_count; ++i) {
      recent_hist[i] = static_cast<float>(data_ptr[i]);
      if (recent_hist[i] > max_val)
        max_val = recent_hist[i];
    }

    ImGui::PlotLines("##Grains", recent_hist.data(), display_count, 0, nullptr,
                     0.0f,
                     max_val * 1.1f, // Add 10% headroom
                     ImVec2(-1, 60));

    // Display latest value next to plot
    ImGui::SameLine();
    ImGui::Text("%zu", grains.back());
  }
}

} // namespace _detail

} // namespace ssoc::ui::views
