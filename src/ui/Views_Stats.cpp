#include "ui/Views.hpp"
#include <algorithm>
#include <cmath>
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

  _detail::draw_stats_grains_s(sc);

  ImGui::End();
}

namespace _detail {

// =========================
// INTERNAL HELPERS
// =========================

std::pair<double, double> fit_power_law(const std::vector<double> &xs,
                                        const std::vector<double> &ys) {

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

  return {a, b};
}

std::vector<double> make_power_law_fit(const std::vector<double> &xs, double a,
                                       double b) {

  std::vector<double> out;
  out.reserve(xs.size());

  for (double x : xs)
    out.push_back(std::exp(b) * std::pow(x, a));

  return out;
}

// =========================
// OVERVIEW
// =========================

void draw_stats_overview_s(const stat::Stats_Collector &sc) {
  ImGui::Text("Steps: %zu", sc.steps_count());
  ImGui::Text("Total avalanches: %zu", sc.avalanche_records().size());
  ImGui::Text("Total size sum: %zu", sc.avalanche_total_sizes());
  ImGui::Text("Max avalanche: %zu", sc.avalanche_max_size());
  ImGui::Text("Dissipated grains: %zu", sc.grain_total_dissipated());
}

// =========================
// AVALANCHE SIZE
// =========================

std::unique_ptr<AvalancheSizePlotModel>
build_avalanche_size_model(const std::vector<size_t> &input) {

  if (input.empty())
    return {};

  std::vector<double> xs, ys;

  for (size_t size = 1; size < input.size(); ++size) {
    size_t count = input[size];
    if (count > 0) {
      xs.push_back((double)size);
      ys.push_back((double)count);
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
    ImPlot::SetupAxes("Avalanche Size", "Count");
    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
    ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);

    ImPlot::PlotScatter("Data", m.xs.data(), m.ys.data(), (int)m.xs.size());

    auto fit = make_power_law_fit(m.xs, m.alpha, m.intercept);
    ImPlot::PlotLine("Fit", m.xs.data(), fit.data(), (int)m.xs.size());

    ImGui::Text("alpha = %.3f", m.alpha);

    ImPlot::EndPlot();
  }

  if (ImPlot::BeginPlot("##SizeLinear", ImVec2(-1, 250))) {
    ImPlot::SetupAxes("Size", "Count");
    ImPlot::PlotScatter("Data", m.xs.data(), m.ys.data(), (int)m.xs.size());
    ImPlot::EndPlot();
  }
}

void draw_stats_avalanche_sizes_s(const stat::Stats_Collector &sc) {

  const auto &data = sc.avalanche_sizes();
  if (data.empty())
    return;

  ImGui::Text("Distribution of Avalanche Sizes");
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");

  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Avalanche Size = toppled vertices");
  }

  auto model = build_avalanche_size_model(data);
  if (model)
    plot_avalanche_size(*model);
}

// =========================
// ORIGINS
// =========================

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
    ImPlot::SetupAxes("Vertex ID", "Count");

    ImPlot::PlotBars("Origins", m.hist.data(), (int)m.hist.size());

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

    double gv = (i < g.size()) ? (double)g[i] : 0.0;
    double ov = (i < o.size()) ? (double)o[i] : 0.0;

    if (gv == 0 && ov == 0)
      continue;

    x.push_back((double)i);
    grains.push_back(gv);
    origins.push_back(ov);
  }

  return std::make_unique<AvalancheOriginGroupedPlotModel>(
      AvalancheOriginGroupedPlotModel{std::move(x), std::move(grains),
                                      std::move(origins), n});
}

void plot_origin_grouped(const AvalancheOriginGroupedPlotModel &m) {

  if (ImPlot::BeginPlot("##OriginsGrouped", ImVec2(-1, 250))) {

    double w = 0.4;

    auto xg = m.x;
    auto xo = m.x;

    for (auto &v : xg)
      v -= w;
    for (auto &v : xo)
      v += w;

    ImPlot::PlotBars("Grains", xg.data(), m.grains.data(), (int)xg.size(), 0.4);
    ImPlot::PlotBars("Origins", xo.data(), m.origins.data(), (int)xo.size(),
                     0.8);

    ImPlot::EndPlot();
  }
}

void draw_stats_avalanche_origins_s(const stat::Stats_Collector &sc) {

  auto model = build_origin_model(sc.avalanche_origins());
  if (!model)
    return;

  ImGui::Text("Avalanche Origins");
  plot_origin(*model);
}

void section_avalanche_origins_grouped(const stat::Stats_Collector &sc) {

  auto model = build_origin_grouped_model(sc);
  if (!model)
    return;

  plot_origin_grouped(*model);
}

// =========================
// GRAINS
// =========================

std::unique_ptr<GrainsPlotModel>
build_grains_model(const std::vector<size_t> &g, int display, int win) {

  if (g.empty())
    return nullptr;

  size_t drop = g.size() > (size_t)display ? g.size() - display : 0;

  auto recent = g | std::views::drop(drop) |
                std::views::transform([](auto v) { return (double)v; }) |
                std::ranges::to<std::vector>();

  auto ma =
      g | std::views::slide(win) | std::views::transform([](auto w) {
        return std::ranges::fold_left(w, 0.0, std::plus{}) / (double)w.size();
      });

  auto moving_avg = ma | std::ranges::to<std::vector>();

  double maxv = std::ranges::max(recent);

  return std::make_unique<GrainsPlotModel>(GrainsPlotModel{
      std::move(recent), std::move(moving_avg), maxv, g.back()});
}

void plot_grains(const GrainsPlotModel &m) {

  if (ImPlot::BeginPlot("##GrainsLine", ImVec2(-1, 150))) {

    ImPlot::PlotLine("Grains", m.recent.data(), (int)m.recent.size());

    if (!m.moving_avg.empty())
      ImPlot::PlotLine("Average", m.moving_avg.data(),
                       (int)m.moving_avg.size());

    ImPlot::EndPlot();
  }
}

void draw_stats_grains_s(const stat::Stats_Collector &sc) {

  const auto &g = sc.grain_dropped_counts();
  if (g.empty())
    return;

  static int display = 200;
  static int win = 20;

  auto model = build_grains_model(g, display, win);

  if (model)
    plot_grains(*model);

  ImGui::InputInt("Last steps", &display);
  ImGui::InputInt("MA window", &win);
}

} // namespace _detail

} // namespace ssoc::ui::views
