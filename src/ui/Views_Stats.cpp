#include "ui/Views.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib> // for std::system
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
  _detail::draw_stats_avalanche_origins_grouped_s(sc);
  ImGui::Separator();

  _detail::draw_stats_grains_s(sc);

  ImGui::End();
}

namespace _detail {

// ===
// INTERNAL HELPERS

std::pair<double, double> fit_power_law(const std::vector<double> &xs,
                                        const std::vector<double> &ys) {
  // function to fit: y = a * x^(-k)
  //
  // alpha = -k
  // C = a
  //
  // taking logs: log(y) = alpha * log(x) + log(C)

  double sum_log_x = 0.0;
  double sum_log_y = 0.0;
  double sum_log_x2 = 0.0;
  double sum_log_xy = 0.0;

  auto is_valid_point = [](double x, double y) { return x > 0.0 && y > 0.0; };
  size_t valid_count = 0;

  for (size_t i = 0; i < xs.size(); ++i) {
    const double x = xs[i];
    const double y = ys[i];

    // log undefined for non-positive values
    if (!is_valid_point(xs[i], ys[i])) {
      continue;
    }

    const double log_x = std::log(x);
    const double log_y = std::log(y);

    sum_log_x += log_x;
    sum_log_y += log_y;
    sum_log_x2 += log_x * log_x;
    sum_log_xy += log_x * log_y;

    ++valid_count;
  }

  // too little values
  if (valid_count < 2) {
    return {0.0, 0.0};
  }

  // valid count, but to prevent repeated static cast
  const double n = static_cast<double>(valid_count);

  const double denominator = n * sum_log_x2 - sum_log_x * sum_log_x;
  // cannot divide by zero
  if (denominator == 0.0) {
    return {0.0, 0.0};
  }

  const double alpha = (n * sum_log_xy - sum_log_x * sum_log_y) / denominator;

  const double intercept = (sum_log_y - alpha * sum_log_x) / n;

  return {alpha, intercept}; // intercept = log(C)
}

std::vector<double> make_power_law_fit(const std::vector<double> &xs,
                                       double alpha, double log_intercept) {
  // reconstruct: y = C * x^alpha
  // where C = exp(log_intercept)

  const double C = std::exp(log_intercept);

  std::vector<double> fitted;
  fitted.reserve(xs.size());

  for (double x : xs) {
    fitted.push_back(C * std::pow(x, alpha));
  }

  return fitted;
}

// ===
// OVERVIEW

void draw_stats_overview_s(const stat::Stats_Collector &sc) {
  ImGui::Text("Steps: %zu", sc.steps_count());
  ImGui::Text("Total avalanches: %zu", sc.avalanche_records().size());
  ImGui::Text("Total size sum: %zu", sc.avalanche_total_sizes());
  ImGui::Text("Max avalanche: %zu", sc.avalanche_max_size());
  ImGui::Text("Dissipated grains: %zu", sc.grain_total_dissipated());
}

// ===
// AVALANCHE SIZE

std::unique_ptr<AvalancheSizePlotModel>
build_avalanche_size_model(const std::vector<size_t> &input) {

  if (input.empty())
    return {};

  std::vector<double> xs, ys;

  for (size_t size = 1; size < input.size(); ++size) {
    size_t count = input[size];
    if (count > 0) {
      xs.push_back(static_cast<double>(size));
      ys.push_back(static_cast<double>(count));
    }
  }

  auto model = std::make_unique<AvalancheSizePlotModel>(
      AvalancheSizePlotModel{std::move(xs), std::move(ys)});

  auto [a, b] = fit_power_law(model->xs, model->ys);
  model->alpha = a;
  model->intercept = b;

  return model;
}

void plot_avalanche_size(const AvalancheSizePlotModel &m) {

  if (m.xs.empty())
    return;

  if (ImPlot::BeginPlot("##SizeLogLog", ImVec2(-1, 250))) {
    ImPlot::SetupAxes("Avalanche Size", "Count", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);
    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
    ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);

    ImPlot::PlotScatter("Data", m.xs.data(), m.ys.data(),
                        static_cast<int>(m.xs.size()));

    auto fit = make_power_law_fit(m.xs, m.alpha, m.intercept);
    ImPlot::PlotLine("Fit", m.xs.data(), fit.data(),
                     static_cast<int>(m.xs.size()));

    ImGui::Text("alpha = %.3f", m.alpha);

    ImPlot::EndPlot();
  }

  if (ImPlot::BeginPlot("##SizeLinear", ImVec2(-1, 250))) {
    ImPlot::SetupAxes("Size", "Count", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);
    ImPlot::PlotScatter("Data", m.xs.data(), m.ys.data(),
                        static_cast<int>(m.xs.size()));
    ImPlot::EndPlot();
  }
}

void draw_stats_avalanche_sizes_s(const stat::Stats_Collector &sc) {

  const auto &data = sc.avalanche_sizes();
  if (data.empty())
    return;

  ImGui::Text("Distribution of Avalanche Sizes");

  auto model = build_avalanche_size_model(data);
  if (model)
    plot_avalanche_size(*model);
}

// ===
// ORIGINS

std::unique_ptr<AvalancheOriginPlotModel>
build_origin_model(const std::vector<size_t> &hist) {

  if (hist.empty())
    return nullptr;

  size_t max_freq = *std::max_element(hist.begin(), hist.end());

  return std::make_unique<AvalancheOriginPlotModel>(
      AvalancheOriginPlotModel{std::vector<double>(hist.begin(), hist.end()),
                               hist.size() - 1, max_freq});
}

void plot_origin(const AvalancheOriginPlotModel &m) {

  if (ImPlot::BeginPlot("##OriginsHist", ImVec2(-1, 200))) {

    ImPlot::SetupAxes("Vertex ID", "Count", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);

    ImPlot::PlotBars("Origins", m.hist.data(), static_cast<int>(m.hist.size()));

    ImPlot::EndPlot();
  }
}

std::unique_ptr<AvalancheOriginGroupedPlotModel>
build_origin_grouped_model(const stat::Stats_Collector &sc) {

  const auto &g = sc.grain_dropped_counts();
  const auto &o = sc.avalanche_origins();

  size_t n = std::max(g.size(), o.size());

  std::vector<double> x, grains, origins;

  for (size_t i = 0; i < n; ++i) {

    double gv = (i < g.size()) ? static_cast<double>(g[i]) : 0.0;
    double ov = (i < o.size()) ? static_cast<double>(o[i]) : 0.0;

    if (gv == 0 && ov == 0)
      continue;

    x.push_back(static_cast<double>(i));
    grains.push_back(gv);
    origins.push_back(ov);
  }

  return std::make_unique<AvalancheOriginGroupedPlotModel>(
      AvalancheOriginGroupedPlotModel{std::move(x), std::move(grains),
                                      std::move(origins), n});
}

void plot_origin_grouped(const AvalancheOriginGroupedPlotModel &m) {

  if (ImPlot::BeginPlot("##OriginsGrouped", ImVec2(-1, 250))) {

    ImPlot::SetupAxes("Vertex ID", "Count", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);

    double w = 0.4;

    auto xg = m.x;
    auto xo = m.x;

    for (auto &v : xg) {
      v -= w;
    }
    for (auto &v : xo) {
      v += w;
    }

    ImPlot::PlotBars("Grains", xg.data(), m.grains.data(),
                     static_cast<int>(xg.size()), 0.4);
    ImPlot::PlotBars("Origins", xo.data(), m.origins.data(),
                     static_cast<int>(xo.size()), 0.8);

    ImPlot::EndPlot();
  }
}

void draw_stats_avalanche_origins_s(const stat::Stats_Collector &sc) {

  auto model = build_origin_model(sc.avalanche_origins());
  if (!model) {
    return;
  }

  ImGui::Text("Avalanche Origins");
  plot_origin(*model);
}

void draw_stats_avalanche_origins_grouped_s(const stat::Stats_Collector &sc) {

  auto model = build_origin_grouped_model(sc);
  if (!model) {
    return;
  }

  plot_origin_grouped(*model);
}

// ===
// GRAINS

std::unique_ptr<GrainsPlotModel>
build_grains_model(const std::vector<size_t> &grains, int display,
                   int window_size) {
  if (grains.empty())
    return nullptr;

  const std::size_t display_sz = static_cast<std::size_t>(display);
  const std::size_t win = static_cast<std::size_t>(window_size);

  // FULL moving average over entire history
  std::vector<double> full_ma;
  full_ma.reserve(grains.size());

  double sum = 0.0;

  for (std::size_t i = 0; i < grains.size(); ++i) {
    const double v = static_cast<double>(grains[i]);
    sum += v;

    if (i >= win)
      sum -= static_cast<double>(grains[i - win]);

    const std::size_t len = std::min(i + 1, win);
    full_ma.push_back(sum / static_cast<double>(len));
  }

  // Slice BOTH series consistently (last N points)
  const std::size_t drop =
      (grains.size() > display_sz) ? grains.size() - display_sz : 0;

  std::vector<double> recent;
  std::vector<double> ma;

  recent.reserve(std::min(display_sz, grains.size()));
  ma.reserve(std::min(display_sz, grains.size()));

  for (std::size_t i = drop; i < grains.size(); ++i) {
    recent.push_back(static_cast<double>(grains[i]));
    ma.push_back(full_ma[i]);
  }

  return std::make_unique<GrainsPlotModel>(
      GrainsPlotModel{std::move(recent), std::move(ma)});
}

void plot_grains(const GrainsPlotModel &model) {
  if (ImPlot::BeginPlot("##GrainsLine", ImVec2(-1, 150),
                        ImPlotFlags_NoMouseText | ImPlotFlags_NoLegend)) {
    ImPlot::SetupAxes("Steps (relative)", "Grains", ImPlotAxisFlags_AutoFit,
                      ImPlotAxisFlags_AutoFit);

    const int n = static_cast<int>(model.recent.size());

    const double xstart = -(n - 1);
    const double xscale = 1.0;

    ImPlot::PlotLine("Grains", model.recent.data(), n, xscale, xstart);

    ImPlot::PlotLine("Average", model.moving_avg.data(), n, xscale, xstart);

    ImPlot::EndPlot();
  }
}

void draw_stats_grains_s(const stat::Stats_Collector &sc) {

  const auto &g = sc.grains_total_counts_history();
  if (g.empty()) {
    return;
  }

  static int display = 200;
  static int win = 20;

  auto model = build_grains_model(g, display, win);

  if (model) {
    plot_grains(*model);
  }

  ImGui::InputInt("Last steps", &display);
  ImGui::InputInt("MA window", &win);
}

} // namespace _detail

} // namespace ssoc::ui::views
