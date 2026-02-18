#include "ui/Views.hpp"
#include "App_Utils.hpp"
#include "Vis_Config.hpp"
#include <cstddef>
#include <deque>
#include <imgui.h>
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

void draw_stats_w(bool &show, const stat::Stats_Collector &sc) {
  ImGui::Begin("Stats", &show);

  ImGui::Text("Total avalanches: %zu", sc.avalanche_records().size());

  // Average avalanche size
  size_t total_size = 0;
  for (const auto &rec : sc.avalanche_records())
    total_size += rec.size;
  float avg_size = sc.avalanche_records().empty()
                       ? 0.0f
                       : float(total_size) / sc.avalanche_records().size();
  ImGui::Text("Average avalanche size: %.2f", avg_size);

  // Max avalanche size
  size_t max_size = 0;
  for (const auto &rec : sc.avalanche_records())
    max_size = std::max(max_size, rec.size);
  ImGui::Text("Max avalanche size: %zu", max_size);

  ImGui::Separator();

  // 2️⃣ Histogram of avalanche sizes
  const auto &size_map = sc.avalanche_sizes();
  if (!size_map.empty()) {
    std::vector<float> hist_data;
    std::vector<std::string> labels;
    hist_data.reserve(size_map.size());
    labels.reserve(size_map.size());

    for (const auto &[size, count] : size_map) {
      hist_data.push_back(float(count));
      labels.push_back(std::to_string(size));
    }

    ImGui::Text("Avalanche Size Histogram:");
    ImGui::PlotHistogram("Sizes", hist_data.data(),
                         static_cast<int>(hist_data.size()),
                         0,       // offset
                         nullptr, // overlay text
                         0.0f,    // min scale
                         float(*std::max_element(hist_data.begin(),
                                                 hist_data.end())), // max scale
                         ImVec2(0, 150)                             // size
    );
  }

  ImGui::Separator();

  // 3️⃣ Optional: Histogram of avalanche origins
  const auto &origin_map = sc.avalanche_origins();
  if (!origin_map.empty()) {
    std::vector<float> origin_hist(origin_map.size());
    std::vector<std::string> origin_labels(origin_map.size());
    size_t idx = 0;
    for (const auto &[origin, count] : origin_map) {
      origin_hist[idx] = float(count);
      origin_labels[idx] = std::to_string(origin);
      ++idx;
    }

    ImGui::Text("Avalanche Origins Histogram:");
    ImGui::PlotHistogram(
        "Origins", origin_hist.data(), static_cast<int>(origin_hist.size()), 0,
        nullptr, 0.0f,
        float(*std::max_element(origin_hist.begin(), origin_hist.end())),
        ImVec2(0, 120));
  }

  // 4️⃣ Optional: recent grains dropped
  const auto &grains = sc.grains_counts();
  if (!grains.empty()) {
    ImGui::Text("Recent grains dropped:");
    std::vector<float> grains_hist(grains.begin(), grains.end());
    ImGui::PlotHistogram(
        "Grains", grains_hist.data(), static_cast<int>(grains_hist.size()), 0,
        nullptr, 0.0f,
        float(*std::max_element(grains_hist.begin(), grains_hist.end())),
        ImVec2(0, 100));
  }

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

} // namespace _detail

} // namespace ssoc::ui::views
