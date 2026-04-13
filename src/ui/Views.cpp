#include "ui/Views.hpp"
#include "App_Utils.hpp"
#include "Visual_Setup_Config.hpp"
#include <algorithm>
#include <cstddef>
#include <deque>
#include <functional>
#include <imgui.h>
#include <implot.h>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>
namespace ssoc::ui::views {

void draw_menu(Master_State &state) {

  if (ImGui::BeginMainMenuBar()) {

    if (ImGui::BeginMenu("Windows")) {

      ImGui::MenuItem("Show help", nullptr, &state.show_help_window);
      ImGui::MenuItem("Show graph builder", nullptr,
                      &state.show_builder_window);

      ImGui::MenuItem("Show graph visualization", nullptr,
                      &state.show_visualization_window);
      ImGui::MenuItem("Show graph visualization configuration", nullptr,
                      &state.show_visualization_config_window);

      ImGui::MenuItem("Show simulation control", nullptr,
                      &state.show_simulation_control_window);
      ImGui::MenuItem("Show stats", nullptr, &state.show_stats_window);

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void draw_welcome_help_window(bool &show) {
  ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(1200, 700), ImGuiCond_FirstUseEver);
  ImGui::Begin("Welcome | Help", &show);

  ImGui::Text(
      "Here will be briefly explained what this program is for, general info "
      "about SOC/sandpile model. Another section would be for help - "
      "explaining how to use it and what means things. Lastly, short 'About' "
      "section with link to github repo where this programs source code and "
      "documentation (thesis) is freely available.");

  ImGui::End();
}

Master_Action draw_graph_builder_windows(bool &show, Sim_Config &sim_cfg,
                                         Vis_Config &vis_cfg) {
  Master_Action state = Master_Action::None;

  ImGui::SetNextWindowPos(ImVec2(200, 150), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);
  ImGui::Begin("Graph Builder", &show);

  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10, 8));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

  if (ImGui::BeginTable("config_table", 2,
                        ImGuiTableFlags_Resizable |
                            ImGuiTableFlags_SizingStretchSame |
                            ImGuiTableFlags_BordersInnerV)) {

    // Left column
    ImGui::TableNextColumn();
    ImGui::Text("Simulation configuration");
    ImGui::Separator();
    ImGui::Spacing();
    _detail::draw_sim_config_s(sim_cfg);

    // Right column
    ImGui::TableNextColumn();
    ImGui::Text("Visualization configuration");
    ImGui::Separator();
    ImGui::Spacing();
    _detail::draw_vis_config_s(vis_cfg);

    ImGui::EndTable();
  }

  ImGui::PopStyleVar(2);

  ImGui::Spacing();
  ImGui::Spacing();

  // centered big button
  float button_width = 200.0f;
  float avail = ImGui::GetContentRegionAvail().x;
  float offset = (avail - button_width) * 0.5f;

  if (offset > 0)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

  if (ImGui::Button("Generate Graph", ImVec2(button_width, 40))) {
    state = Master_Action::Generate_Graph;
    show = false; // after generating, hide itself
  }

  ImGui::End();
  return state;
}

void draw_graph_visualization_window(
    bool &show, Vis_Config &vis_cfg, Visualizer &vis, graph::Graph &g,
    size_t last_vertex, const std::deque<size_t> &checking_topple_vertices) {
  ImGui::SetNextWindowPos(ImVec2(5, 300), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(790, 480), ImGuiCond_FirstUseEver);

  vis.show_window(g, show, last_vertex, checking_topple_vertices, vis_cfg);
}

void draw_graph_control_window(bool &show, Visualizer_Config &cfg) {
  ImGui::SetNextWindowPos(ImVec2(5, 25), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(290, 270), ImGuiCond_FirstUseEver);
  ImGui::Begin("Visualization Control", &show);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 10));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));

  ImGui::Text("View");
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SliderFloat("Zoom speed", &cfg.zoom_speed, 0.001f, 1.0f, "%.3f",
                     ImGuiSliderFlags_Logarithmic);

  ImGui::SliderFloat("Vertex size", &cfg.vertex_base_size, 0.1f, 100.0f);

  ImGui::Spacing();

  ImGui::Text("Display");
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Scale nodes by sand count", &cfg.show_as_size);
  ImGui::Checkbox("Show vertex labels", &cfg.show_numbers);

  ImGui::PopStyleVar(2);
  ImGui::End();
}

Control_Action draw_simulation_control_window(bool &show, Master_State &state,
                                              Sim_Config &cfg) {
  Control_Action action = Control_Action::None;

  ImGui::SetNextWindowPos(ImVec2(300, 25), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(495, 270), ImGuiCond_FirstUseEver);
  ImGui::Begin("Simulation Control", &show);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 8));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

  //  ROW 1: Actions
  ImGui::Text("Simulation");
  ImGui::Separator();

  if (ImGui::Button("Step In")) {
    action = Control_Action::Step_In;
  }
  ImGui::SameLine();

  if (ImGui::Button("Step Over")) {
    action = Control_Action::Step_Over;
  }
  ImGui::SameLine();

  if (ImGui::Button("Run until avalanche")) {
    action = Control_Action::Run_Until_Avalanche;
  }
  ImGui::SameLine();

  if (ImGui::Button("Run")) {
    action = Control_Action::Run;
  }
  ImGui::SameLine();

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
  if (ImGui::Button("Stop")) {
    action = Control_Action::Stop;
  }
  ImGui::PopStyleColor();

  ImGui::Spacing();
  ImGui::Spacing();

  // ROW 2: Config
  ImGui::Text("Configuration");
  ImGui::Separator();

  ImGui::PushItemWidth(140);
  ImGui::InputInt("Render freq", &state.draw_every);
  ImGui::PopItemWidth();

  ImGui::SameLine();

  ImGui::Checkbox("Random sand", &cfg.random_sand_distribution);

  if (!cfg.random_sand_distribution) {
    ImGui::SameLine();
    ImGui::PushItemWidth(180);
    ImGui::InputScalar("Vertex", ImGuiDataType_U64,
                       &cfg.specific_vertex_to_distribute);
    ImGui::PopItemWidth();
  }

  ImGui::Spacing();
  ImGui::Spacing();

  // ROW 3: Graph
  ImGui::Text("Graph");
  ImGui::Separator();

  if (ImGui::Button("Regenerate (same)")) {
    action = Control_Action::Generate_The_Same_Graph;
  }

  ImGui::SameLine();

  if (ImGui::Button("New graph...")) {
    action = Control_Action::Launch_Builder;
  }

  ImGui::PopStyleVar(2);
  ImGui::End();

  return action;
}

void draw_stats_window(bool &show, const ssoc::stat::Stats_Collector &sc) {
  ImGui::SetNextWindowPos(ImVec2(800, 25), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(460, 755), ImGuiCond_FirstUseEver);
  ImGui::Begin("Statistics", &show);

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
      "Watts Strogatz 2D",
  };

  size_t idx = gga.index();

  for (size_t i = 0; i < std::size(labels); ++i) {
    if (ImGui::RadioButton(labels[i], idx == i)) {
      switch (i) {
      case 0:
        gga = gga_::Square_Lattice_2D{};
        break;
      case 1:
        gga = gga_::Watts_Strogatz_2D{};
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

  if (cfg.circular_on_x && cfg.circular_on_y &&
      cfg.sink_rule == gga_::Square_Lattice_2D::Sink_Rule::Fill_To_Four) {
    ImGui::TextColored(
        ImVec4(1.f, 0.f, 0.f, 1.f),
        "Fill To Four in combination with circular on both axis can cause "
        "program crash. There isn't any vertex connected to the sink resulting "
        "in infinite grain'pocalypse.");
  }
}

void draw_gga(gga_::Watts_Strogatz_2D &cfg) {
  ImGui::TextUnformatted(
      "This model is constructed in two steps:\n1. build square lattice (N "
      "vertices) circular on both X and Y axis (have K neighbours)\n2. for "
      "every vertex, examine all it's rightmost edges (K/2) and with "
      "probability (B) rewire them to any random vertex\n* more info on "
      "wikipedia: "
      "https://en.wikipedia.org/wiki/Watts%E2%80%93Strogatz_model#Algorithm");
  ImGui::SeparatorText("Dimensions");
  ImGui::InputInt("sqrt(N)", &cfg.size);
  ImGui::TextDisabled("#vertices = N ( = sqrt(N)^2 for 2D ), it's the same "
                      "number as in square lattice");
  ImGui::InputInt("K", &cfg.neighbourhood_size);
  ImGui::TextDisabled("Vertex mean degree K, should be even integer.");

  ImGui::InputDouble("B (beta)", &cfg.p);
  ImGui::TextDisabled("Rewiring probability (if B=0 it's just square lattice "
                      "circular on both axis).");

  ImGui::SeparatorText("Sandpile Rules");

  const char *rule_names[] = {"All Once", "As Many As Neighbours"};
  int current_rule = static_cast<int>(cfg.sink_rule);

  if (ImGui::Combo("Sink Rule", &current_rule, rule_names,
                   IM_ARRAYSIZE(rule_names))) {
    cfg.sink_rule =
        static_cast<gga_::Watts_Strogatz_2D::Sink_Rule>(current_rule);
  }
}

void draw_vis_config_s(Vis_Config &cfg) { draw_gla_s(cfg.gla); }

void draw_gla_s(Graph_Layout_Algorithm &gla) {
  constexpr const char *labels[] = {
      "Fruchterman Reingold 2D",
      "Hidden",
  };

  size_t idx = gla.index();

  for (size_t i = 0; i < std::size(labels); ++i) {
    if (ImGui::RadioButton(labels[i], idx == i)) {
      switch (i) {
      case 0:
        gla = gla_::Fruchterman_Reingold_2D{};
        break;
      case 1:
        gla = gla_::Hidden_GLA{};
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

void draw_gla([[maybe_unused]] gla_::Hidden_GLA &cfg) {}

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

  ImGui::Text("Grains Counts");
  ImGui::SameLine();
  ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "  Latest: %zu",
                     grains.back());

  static int display_grains = 200;
  static int win_size = 20; // window for moving average calculations

  auto plot_data = prepare_grains_data(grains, display_grains, win_size);
  if (plot_data) {
    if (plot_data->moving_avg.size() > 0) {
      ImGui::SameLine();
      ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "  Trend: %f",
                         plot_data->moving_avg.back());
    }

    render_grains_count_plot(*plot_data);
  }

  ImGui::InputInt("Last steps shown", &display_grains);
  ImGui::InputInt("Moving average window size", &win_size);
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
prepare_grains_data(const std::vector<size_t> &grains, int max_grains,
                    int win_size) {
  if (grains.empty()) {
    return nullptr;
  }

  size_t max_display = static_cast<size_t>(max_grains);
  size_t window_size = static_cast<size_t>(win_size);
  size_t drop_count =
      grains.size() > max_display ? grains.size() - max_display : 0;

  // Prepare raw data slice
  auto recent_hist = grains | std::views::drop(drop_count) |
                     std::views::transform(
                         [](size_t val) { return static_cast<double>(val); }) |
                     std::ranges::to<std::vector<double>>();

  // Prepare Moving Average
  auto ma_view = grains | std::views::slide(window_size) |
                 std::views::transform([](auto window) {
                   double sum =
                       std::ranges::fold_left(window, 0.0, std::plus<double>());
                   return sum / static_cast<double>(window.size());
                 });

  // We need to drop from the MA view to align it with the recent_hist
  // Note: slide view is shorter than the original by (window_size - 1)
  size_t ma_total_size =
      (grains.size() >= window_size) ? (grains.size() - window_size + 1) : 0;
  size_t ma_drop =
      ma_total_size > max_display ? ma_total_size - max_display : 0;

  auto moving_avg = ma_view | std::views::drop(ma_drop) |
                    std::ranges::to<std::vector<double>>();

  double max_val = std::ranges::max(recent_hist);

  return std::make_unique<Grains_Count_Plot_Data>(
      std::move(recent_hist), std::move(moving_avg), max_val, grains.back());
}

void render_avalanche_size_plot(const Avalanche_Size_Plot_Data &data) {
  if (ImPlot::BeginPlot("##SizeHist", ImVec2(-1, 300),
                        ImPlotFlags_NoMouseText | ImPlotFlags_NoLegend)) {

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
    ImPlot::SetupAxisLimits(ImAxis_X1, 0,
                            static_cast<double>(data.recent_hist.size() - 1),
                            ImGuiCond_Appearing);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, data.max_val * 1.1,
                            ImGuiCond_Appearing);

    // Draw raw data
    ImPlot::PlotLine("Grains", data.recent_hist.data(),
                     static_cast<int>(data.recent_hist.size()));

    // Draw Moving Average
    if (!data.moving_avg.empty()) {
      // We calculate the X-offset because MA might have fewer points than raw
      // data if the simulation just started
      double x_offset = static_cast<double>(data.recent_hist.size()) -
                        static_cast<double>(data.moving_avg.size());

      ImPlot::PushStyleColor(ImPlotCol_PlotBg, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
      ImPlot::PlotLine("Average", data.moving_avg.data(),
                       static_cast<int>(data.moving_avg.size()), 1.0, x_offset,
                       ImPlotSpec());
      ImPlot::PopStyleColor(ImPlotCol_PlotBg);
    }

    if (ImPlot::IsPlotHovered()) {
      ImPlotPoint pt = ImPlot::GetPlotMousePos();
      size_t idx = static_cast<size_t>(pt.x + 0.5);

      if (idx < data.recent_hist.size()) {
        ImGui::BeginTooltip();
        ImGui::Text("Grains: %.0f", data.recent_hist[idx]);

        // Find corresponding MA index
        size_t ma_idx_offset = data.recent_hist.size() - data.moving_avg.size();
        if (idx >= ma_idx_offset &&
            (idx - ma_idx_offset) < data.moving_avg.size()) {
          ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "Trend: %.2f",
                             data.moving_avg[idx - ma_idx_offset]);
        }

        ImGui::EndTooltip();
      }
    }
    ImPlot::EndPlot();
  }
}

} // namespace _detail

} // namespace ssoc::ui::views
