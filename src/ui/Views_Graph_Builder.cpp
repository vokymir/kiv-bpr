#include "ui/Views.hpp"

namespace ssoc::ui::views {

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

} // namespace _detail
} // namespace ssoc::ui::views
