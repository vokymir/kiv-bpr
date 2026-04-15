#include "ui/Views.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib> // for std::system
#include <functional>
#include <imgui.h>
#include <implot.h>
#include <memory>
#include <vector>
namespace ssoc::ui::views {

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

    ImPlot::PlotBars("Avalanche origins", x_origins.data(), data.origins.data(),
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

} // namespace _detail

} // namespace ssoc::ui::views
