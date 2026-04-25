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

void draw_sim_config_s(Sim_Config &cfg) { draw_gga_s(cfg.gga, cfg.sink_rule); }

void draw_gga_s(Graph_Generation_Algorithm &gga, Sink_Rule &rule) {
  constexpr const char *labels[] = {
      "Square Lattice",  "Erdös-Renyi G(n,m)", "Erdös-Renyi G(n,p)",
      "Barabasi-Albert", "Watts-Strogatz 2D",
  };

  size_t idx = gga.index();

  for (size_t i = 0; i < std::size(labels); ++i) {
    if (ImGui::RadioButton(labels[i], idx == i)) {
      switch (i) {
      case 0:
        gga = gga_::Square_Lattice{};
        break;
      case 1:
        gga = gga_::Erdos_Renyi_nm{};
        break;
      case 2:
        gga = gga_::Erdos_Renyi_np{};
        break;
      case 3:
        gga = gga_::Barabasi_Albert{};
        break;
      case 4:
        gga = gga_::Watts_Strogatz_2D{};
        break;

      default:
        gga = gga_::Square_Lattice{};
        break;
      }
    }
  }

  std::visit([](auto &alg) { draw_gga(alg); }, gga);
  draw_sink_rule(rule);
}

void draw_sink_rule(Sink_Rule &rule) {

  ImGui::SeparatorText("Sandpile Rules");

  const char *rule_names[] = {"Fill To N", "All Once", "As Many As Neighbours",
                              "Fill Up"};
  int current_rule = static_cast<int>(rule.type);

  // quality-of-life upgrade
  using t = Sink_Rule::Type;

  if (ImGui::Combo("Sink Rule", &current_rule, rule_names,
                   IM_ARRAYSIZE(rule_names))) {
    rule.type = static_cast<t>(current_rule);
  }
}

void draw_gga(gga_::Square_Lattice &cfg) {
  ImGui::SeparatorText("Dimensions");
  ImGui::InputInt("Width", &cfg.width);
  ImGui::InputInt("Height", &cfg.height);

  ImGui::SeparatorText("Boundary Conditions");
  ImGui::Checkbox("Circular on x axis", &cfg.circular_on_x);
  ImGui::Checkbox("Circular on y axis", &cfg.circular_on_y);
}

void draw_gga(gga_::Erdos_Renyi_nm &cfg) {

  ImGui::InputInt("n", &cfg.vertices);
  ImGui::TextDisabled("N = # vertices in final graph");

  ImGui::InputInt("m", &cfg.edges);
  ImGui::TextDisabled("m = # edges in final graph");
}

void draw_gga(gga_::Erdos_Renyi_np &cfg) {

  ImGui::InputInt("n", &cfg.vertices);
  ImGui::TextDisabled("N = # vertices in final graph");

  ImGui::InputDouble("p", &cfg.edge_probability);
  ImGui::TextDisabled("p = probability of edge existing");
}

void draw_gga(gga_::Barabasi_Albert &cfg) {

  ImGui::InputInt("n", &cfg.vertices);
  ImGui::TextDisabled("N = # vertices in final graph");

  ImGui::InputInt("m", &cfg.edges_per_node);
  ImGui::TextDisabled("m = # edges created for new vertex in each step");
}

void draw_gga(gga_::Watts_Strogatz_2D &cfg) {
  ImGui::InputInt("N", &cfg.vertices);
  ImGui::TextDisabled("N = # vertices");
  ImGui::InputInt("K", &cfg.neighbourhood_size);
  ImGui::TextDisabled("Vertex mean degree K, should be even integer.");

  ImGui::InputDouble("B (beta)", &cfg.p);
  ImGui::TextDisabled("Rewiring probability (if B=0 it's just square lattice "
                      "circular on both axis).");
}

// ============================================================================
// VISUAL

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
