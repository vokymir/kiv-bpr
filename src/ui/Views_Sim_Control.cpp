#include "ui/Views.hpp"

namespace ssoc::ui::views {

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

} // namespace ssoc::ui::views
