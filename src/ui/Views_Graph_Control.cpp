#include "ui/Layout.hpp"
#include "ui/Views.hpp"

namespace ssoc::ui::views {

void draw_graph_control_window(bool &show, Visualizer_Config &cfg) {
  layout::set_next_window(layout::Window::CTL_VIS);
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

} // namespace ssoc::ui::views
