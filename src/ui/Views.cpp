#include "ui/Views.hpp"
#include "App_Utils.hpp"
#include "Vis_Config.hpp"
#include <algorithm>
#include <cstddef>
#include <deque>
#include <imgui.h>
#include <implot.h>
#include <map>
#include <memory>
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
  const auto &size_map = sc.avalanche_sizes();
  if (size_map.empty()) {
    return;
  }

  // header
  ImGui::Text("Distribution of Avalanche Sizes");
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Avalanche Size = how many vertices were toppled");
  }

  auto plot_data = prepare_avalanche_data(size_map);
  if (plot_data) {
    render_avalanche_size_plot(*plot_data);
  }
}

void draw_stats_avalanche_origins_s(const stat::Stats_Collector &sc) {
  const auto &origin_map = sc.avalanche_origins();
  if (origin_map.empty()) {
    return;
  }

  ImGui::Text("Avalanche Origins");
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Distribution of where avalanches start.\n"
                      "Flat = Uniform distribution.\n"
                      "Spikes = Critical spots.");
  }

  auto plot_data = prepare_origin_data(origin_map);
  if (plot_data) {
    render_avalanche_origin_plot(*plot_data);
  }
}

void draw_stats_grains_counts_s(const stat::Stats_Collector &sc) {
  const auto &grains = sc.grains_counts();
  if (grains.empty())
    return;

  ImGui::Text("Grains Counts (Last 200 steps)");
  ImGui::SameLine();
  ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "  Latest: %zu",
                     grains.back());

  auto plot_data = prepare_grains_data(grains);
  if (plot_data) {
    render_grains_count_plot(*plot_data);
  }
}

std::unique_ptr<Avalanche_Size_Plot_Data>
prepare_avalanche_data(const std::unordered_map<size_t, size_t> &size_map) {
  if (size_map.empty()) {
    return {};
  }

  size_t max_key = std::ranges::max(size_map | std::views::keys);
  size_t max_count = std::ranges::max(size_map | std::views::values);
  size_t plot_range = max_key + 1;

  // generate XS (0 to plot_range)
  auto xs =
      std::views::iota(0u, plot_range) |
      std::views::transform([](auto i) { return static_cast<double>(i); }) |
      std::ranges::to<std::vector<double>>();

  // generate YS (look up counts, default to 0.0)
  auto ys =
      std::views::iota(0u, plot_range) |
      std::views::transform([&size_map](size_t i) {
        return size_map.contains(i) ? static_cast<double>(size_map.at(i)) : 0.0;
      }) |
      std::ranges::to<std::vector<double>>();

  return std::make_unique<Avalanche_Size_Plot_Data>(xs, ys, max_key, max_count);
}

std::unique_ptr<Avalanche_Origin_Plot_Data>
prepare_origin_data(const std::unordered_map<size_t, size_t> &origin_map) {
  if (origin_map.empty()) {
    return nullptr;
  }

  // Find the bounds in O(N)
  size_t max_vertex = std::ranges::max(origin_map | std::views::keys);
  size_t safe_size = max_vertex + 1;

  // Generate the dense histogram vector directly
  auto hist = std::views::iota(0u, safe_size) |
              std::views::transform([&origin_map](size_t i) {
                return origin_map.contains(i)
                           ? static_cast<double>(origin_map.at(i))
                           : 0.0;
              }) |
              std::ranges::to<std::vector<double>>();

  // Find the peak frequency for the Y-axis limit
  double max_freq =
      static_cast<double>(std::ranges::max(origin_map | std::views::values));

  return std::make_unique<Avalanche_Origin_Plot_Data>(hist, max_vertex,
                                                      max_freq);
}

std::unique_ptr<Grains_Count_Plot_Data>
prepare_grains_data(const std::vector<size_t> &grains) {
  if (grains.empty()) {
    return nullptr;
  }

  // Calculate how many old elements to ignore
  size_t max_display = 200;
  size_t drop_count =
      grains.size() > max_display ? grains.size() - max_display : 0;

  // drop skips the first N elements, creating a sliding window
  auto recent_hist = grains | std::views::drop(drop_count) |
                     std::views::transform(
                         [](size_t val) { return static_cast<double>(val); }) |
                     std::ranges::to<std::vector<double>>();

  double max_val = std::ranges::max(recent_hist);

  return std::make_unique<Grains_Count_Plot_Data>(recent_hist, max_val,
                                                  grains.back());
}

void render_avalanche_size_plot(const Avalanche_Size_Plot_Data &data) {
  ImPlot::BeginPlot("##SizeHist", ImVec2(-1, 300),
                    ImPlotFlags_NoMouseText | ImPlotFlags_NoLegend);

  ImPlot::SetupAxes("Avalanche Size", "Count", ImPlotAxisFlags_AutoFit,
                    ImPlotAxisFlags_AutoFit);

  // Dynamic scaling based on the visible data
  ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(data.xs.size()),
                          ImGuiCond_Appearing);
  ImPlot::SetupAxisLimits(ImAxis_Y1, 0,
                          static_cast<double>(data.max_count) * 1.1,
                          ImGuiCond_Appearing);

  ImPlot::PlotBars("Sizes", data.xs.data(), data.ys.data(),
                   static_cast<int>(data.xs.size()), 1.0);

  // Tooltip
  if (ImPlot::IsPlotHovered()) {
    ImPlotPoint pt = ImPlot::GetPlotMousePos();
    size_t idx = static_cast<size_t>(pt.x + 0.5);

    if (idx < data.ys.size()) {
      ImGui::BeginTooltip();
      ImGui::Text("Size: %zu", idx);
      ImGui::Text("Count: %.0f", data.ys[idx]);
      ImGui::EndTooltip();
    }
  }
  ImPlot::EndPlot();
}

void render_avalanche_origin_plot(const Avalanche_Origin_Plot_Data &data) {
  if (ImPlot::BeginPlot("##OriginsHist", ImVec2(-1, 200),
                        ImPlotFlags_NoMouseText | ImPlotFlags_NoLegend)) {

    ImPlot::SetupAxes("Vertex ID", "Count", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);

    ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(data.hist.size()),
                            ImGuiCond_Appearing);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0,
                            static_cast<double>(data.max_freq) * 1.1,
                            ImGuiCond_Appearing);

    ImPlot::PlotBars("Origins", data.hist.data(),
                     static_cast<int>(data.hist.size()), 1.0);

    if (ImPlot::IsPlotHovered()) {
      ImPlotPoint pt = ImPlot::GetPlotMousePos();
      size_t idx = static_cast<size_t>(pt.x + 0.5);

      if (idx < data.hist.size()) {
        ImGui::BeginTooltip();
        ImGui::Text("Vertex ID: %zu", idx);
        ImGui::Text("Count: %.0f", data.hist[idx]);
        ImGui::EndTooltip();
      }
    }
    ImPlot::EndPlot();
  }
}

void render_grains_count_plot(const Grains_Count_Plot_Data &data) {
  if (ImPlot::BeginPlot("##GrainsLine", ImVec2(-1, 150),
                        ImPlotFlags_NoMouseText | ImPlotFlags_NoLegend)) {

    ImPlot::SetupAxes("Recent Steps", "Grains", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);

    // X-axis represents the sliding window
    ImPlot::SetupAxisLimits(ImAxis_X1, 0,
                            static_cast<double>(data.recent_hist.size() - 1),
                            ImGuiCond_Appearing);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0,
                            static_cast<double>(data.max_val) * 1.1,
                            ImGuiCond_Appearing);

    // Plot as a continuous line
    ImPlot::PlotLine("Grains", data.recent_hist.data(),
                     static_cast<int>(data.recent_hist.size()));

    // Custom hover tooltip for the line graph
    if (ImPlot::IsPlotHovered()) {
      ImPlotPoint pt = ImPlot::GetPlotMousePos();
      size_t idx = static_cast<size_t>(pt.x + 0.5);

      if (idx < data.recent_hist.size()) {
        ImGui::BeginTooltip();
        ImGui::Text("Grains on grid: %.0f", data.recent_hist[idx]);

        // how many steps ago this was
        size_t steps_ago = data.recent_hist.size() - 1 - idx;
        ImGui::TextDisabled("(%zu steps ago)", steps_ago);

        ImGui::EndTooltip();
      }
    }
    ImPlot::EndPlot();
  }
}

} // namespace _detail

} // namespace ssoc::ui::views
