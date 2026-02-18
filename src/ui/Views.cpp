#include "ui/Views.hpp"
#include "App_Utils.hpp"
#include "Vis_Config.hpp"
#include <cstddef>
#include <imgui.h>
#include <variant>
namespace ssoc::ui::views {

Master_Action draw_master_w(Master_State &state) {
  Master_Action action = Master_Action::None;

  ImGui::SetNextWindowPos(ImVec2(31, 21), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(249, 189), ImGuiCond_FirstUseEver);
  ImGui::Begin("Master window", nullptr);

  ImGui::Checkbox("Show graph", &state.show_graph_window);
  ImGui::Checkbox("Show config", &state.show_config_window);
  ImGui::Checkbox("Show stepping", &state.show_control_window);

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

void draw_graph_w(bool &show, Visualizer &vis, const graph::Graph &g) {
  ImGui::SetNextWindowPos(ImVec2(385, 86), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(729, 571), ImGuiCond_FirstUseEver);

  vis.show_window(g, show);
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
  ImGui::InputInt("Width", &cfg.width);
  ImGui::InputInt("Height", &cfg.height);
  ImGui::Checkbox("Circular on x axis", &cfg.circular_on_x);
  ImGui::Checkbox("Circular on y axis", &cfg.circular_on_y);
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
