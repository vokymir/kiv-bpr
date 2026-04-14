#include "ui/Views.hpp"
#include "App_Utils.hpp"
#include "Visual_Setup_Config.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib> // for std::system
#include <deque>
#include <functional>
#include <imgui.h>
#include <implot.h>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
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
  ImGui::SetNextWindowPos(ImVec2(150, 50), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowBgAlpha(1.0f);

  ImGui::Begin("Welcome | Help", &show, ImGuiWindowFlags_NoCollapse);

  // shortcuts
  if (ImGui::IsKeyPressed(ImGuiKey_Q) || (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
                                          ImGui::IsKeyPressed(ImGuiKey_Q))) {
    show = false;
  }

  _detail::HelpLayoutState state;

  _detail::draw_help_title();

  ImVec2 a = ImGui::GetContentRegionAvail();

  // -20 is just to prevent not enough region
  float top_h = a.y * state.split_y - 20.0f;
  float bot_h = a.y - top_h - 20.0f;

  draw_help_top(state, top_h, a.x);

  ImGui::InvisibleButton("split_h", ImVec2(-1, 6));
  if (ImGui::IsItemActive())
    state.split_y = std::clamp(
        state.split_y + ImGui::GetIO().MouseDelta.y / a.y, 0.2f, 0.8f);

  draw_help_bottom(state, bot_h, a.x);

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

  _detail::draw_stats_grouped_origins(sc);

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
  ImGui::Text("Steps: %zu", sc.steps_count());
  ImGui::Text("Total avalanches: %zu", sc.avalanche_records().size());
  ImGui::Text("Total size sum: %zu", sc.avalanche_total_sizes());
  ImGui::Text("Max avalanche: %zu", sc.avalanche_max_size());
  ImGui::Text("Dissipated grains: %zu", sc.grain_total_dissipated());
}

void draw_stats_avalanche_sizes_s(const stat::Stats_Collector &sc) {
  const auto &input_data = sc.avalanche_sizes();
  if (input_data.empty()) {
    return;
  }

  // header
  ImGui::Text("Distribution of Avalanche Sizes");
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Avalanche Size = how many vertices were toppled");
  }

  auto plot_data = prepare_avalanche_data(input_data);
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

void draw_stats_grouped_origins(const stat::Stats_Collector &sc) {
  ImGui::Text("Avalanche Origins");
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Distribution of where avalanches start.\n"
                      "Flat = Uniform distribution.\n"
                      "Spikes = Critical spots.");
  }

  auto plot_data = prepare_origin_grouped_data(sc);
  if (plot_data) {
    render_avalanche_origin_grouped_plot(*plot_data);
  }
}

void draw_stats_grains_counts_s(const stat::Stats_Collector &sc) {
  const auto &grains = sc.grain_dropped_counts();
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
prepare_avalanche_data(const std::vector<size_t> &input_data) {

  if (input_data.empty()) {
    return {};
  }

  std::vector<double> xs;
  std::vector<double> ys;

  xs.reserve(input_data.size());
  ys.reserve(input_data.size());

  // skip size 0
  for (size_t size = 1; size < input_data.size(); ++size) {

    size_t count = input_data[size];

    // log-scale safe: remove zeros
    if (count > 0 && size > 0) {
      xs.push_back(static_cast<double>(size));
      ys.push_back(static_cast<double>(count));
    }
  }

  auto data =
      std::make_unique<Avalanche_Size_Plot_Data>(std::move(xs), std::move(ys));

  auto [a, b] = fit_power_law(data->xs, data->ys);
  data->alpha = a;
  data->intercept = b;

  return data;
}

std::unique_ptr<Avalanche_Origin_Plot_Data>
prepare_origin_data(const std::vector<size_t> &histogram) {

  if (histogram.empty()) {
    return nullptr;
  }

  size_t max_vertex = histogram.size() - 1;

  size_t max_freq = 0.0;
  for (auto v : histogram) {
    if (v > max_freq)
      max_freq = v;
  }

  std::vector<double> hist_double(histogram.begin(), histogram.end());

  return std::make_unique<Avalanche_Origin_Plot_Data>(std::move(hist_double),
                                                      max_vertex, max_freq);
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

std::unique_ptr<Avalanche_Origin_Grouped_Plot_Data>
prepare_origin_grouped_data(const stat::Stats_Collector &sc) {
  const auto &g = sc.grain_dropped_counts();
  const auto &o = sc.avalanche_origins();

  if (g.empty() && o.empty())
    return nullptr;

  size_t n = std::max(g.size(), o.size());

  std::vector<double> x;
  std::vector<double> grains;
  std::vector<double> origins;

  x.reserve(n);
  grains.reserve(n);
  origins.reserve(n);

  for (size_t i = 0; i < n; ++i) {

    double gv = (i < g.size()) ? (double)g[i] : 0.0;
    double ov = (i < o.size()) ? (double)o[i] : 0.0;

    // optional: skip empty vertices
    if (gv == 0.0 && ov == 0.0)
      continue;

    x.push_back((double)i);
    grains.push_back(gv);
    origins.push_back(ov);
  }

  return std::make_unique<Avalanche_Origin_Grouped_Plot_Data>(
      std::move(x), std::move(grains), std::move(origins), n);
}

void render_avalanche_size_plot(const Avalanche_Size_Plot_Data &data) {
  const auto &xs = data.xs;
  const auto &ys = data.ys;

  if (xs.empty())
    return;

  // ===== LOG-LOG PLOT =====
  if (ImPlot::BeginPlot("##SizeLogLog", ImVec2(-1, 250))) {

    ImPlot::SetupAxes("Avalanche Size", "Count", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);

    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
    ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);

    ImPlot::PlotScatter("Data", xs.data(), ys.data(), (int)xs.size());

    // FIT LINE
    auto fit = make_fit_line(xs, data.alpha, data.intercept);
    ImPlot::PlotLine("Power law fit", xs.data(), fit.data(), (int)xs.size());

    ImGui::Text("alpha = %.3f", data.alpha);

    ImPlot::EndPlot();
  }

  // ===== LINEAR PLOT =====
  if (ImPlot::BeginPlot("##SizeLinear", ImVec2(-1, 250))) {

    ImPlot::SetupAxes("Size", "Count", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);

    ImPlot::PlotScatter("Data", xs.data(), ys.data(), (int)xs.size());

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

    ImPlot::PlotBars("Origins", data.hist.data(), (int)data.hist.size());

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
void render_avalanche_origin_grouped_plot(
    const Avalanche_Origin_Grouped_Plot_Data &data) {
  if (ImPlot::BeginPlot("##OriginsGrouped", ImVec2(-1, 250))) {

    ImPlot::SetupAxes("Vertex ID", "Count", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);

    double w = 0.4; // bar half-spacing

    std::vector<double> x_grains = data.x;
    std::vector<double> x_origins = data.x;

    for (auto &x : x_grains)
      x -= w;
    for (auto &x : x_origins)
      x += w;

    // TODO: grains dropped is EMPTY???
    ImPlot::PlotBars("Grains dropped", x_grains.data(), data.grains.data(),
                     (int)x_grains.size(), 0.4);

    ImPlot::PlotBars("Avalanche origins", x_origins.data(),
    data.origins.data(),
                     (int)x_origins.size(), 0.8);

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

// HELP WINDOW SECTION:

void draw_help_title() {
  const char *title = "Sandpile Simulation (SOC)";

  float tw = ImGui::CalcTextSize(title).x;
  ImGui::SetCursorPosX((ImGui::GetWindowWidth() - tw) * 0.5f);

  ImGui::Text("%s", title);
  ImGui::Separator();
  ImGui::Spacing();
}

void draw_help_top_left(float left_w) {
  ImGui::BeginChild("welcome", ImVec2(left_w, 0), true);

  ImGui::Text("Welcome");
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::TextWrapped("TODO: here will be some short onboarding, info for who "
                     "this app is.\n");

  ImGui::Spacing();

  ImGui::TextWrapped("You can close this window using shortcut: [CTRL-q]");

  ImGui::TextDisabled(
      "Tip: You can reopen this window from the main menu at [Windows].");

  ImGui::EndChild();
}

void draw_help_top_right() {
  ImGui::BeginChild("soc", ImVec2(0, 0), true);

  ImGui::Text("What is SOC?");
  ImGui::Separator();

  ImGui::TextWrapped(
      "TODO: here short description of SOC & its relation to sandpile");

  ImGui::EndChild();
}

void draw_help_top(HelpLayoutState &s, float h, float w) {
  ImGui::BeginChild("top", ImVec2(0, h));

  float left_w = w * s.split_x;

  _detail::draw_help_top_left(left_w);

  ImGui::SameLine();

  ImGui::InvisibleButton("split_top", ImVec2(6, h));
  if (ImGui::IsItemActive())
    s.split_x =
        std::clamp(s.split_x + ImGui::GetIO().MouseDelta.x / w, 0.2f, 0.8f);

  ImGui::SameLine();

  _detail::draw_help_top_right();

  ImGui::EndChild();
}

void draw_help_bottom_left(float left_w) {
  ImGui::BeginChild("usage", ImVec2(left_w, 0), true);

  ImGui::Text("How to use");
  ImGui::Separator();

  ImGui::TextWrapped(
      "TODO: detail explanation of individual functionalities in app");

  ImGui::EndChild();
}

void draw_help_bottom_right() {
  ImGui::BeginChild("about", ImVec2(0, 0), true);

  ImGui::Text("About");
  ImGui::Separator();

  ImGui::TextWrapped("TODO: Why this app was created. all info on GH:");

  ImGui::Spacing();

  auto open_url = [&](const std::string &url) {
#ifdef _WIN32
    std::system(("start " + url).c_str());
#elif __APPLE__
    std::system(("open " + url).c_str());
#else
    std::system(("xdg-open " + url).c_str());
#endif
  };

  if (ImGui::Button("Open GitHub repository")) {
    open_url("https://github.com/vokymir/kiv-bpr");
  }

  ImGui::EndChild();
}

void draw_help_bottom(HelpLayoutState &s, float h, float w) {
  ImGui::BeginChild("bottom", ImVec2(0, h));

  float left_w = w * s.split_x;

  _detail::draw_help_bottom_left(left_w);

  ImGui::SameLine();

  ImGui::InvisibleButton("split_bottom", ImVec2(6, h));
  if (ImGui::IsItemActive())
    s.split_x =
        std::clamp(s.split_x + ImGui::GetIO().MouseDelta.x / w, 0.2f, 0.8f);

  ImGui::SameLine();

  _detail::draw_help_bottom_right();

  ImGui::EndChild();
}

} // namespace _detail

} // namespace ssoc::ui::views
