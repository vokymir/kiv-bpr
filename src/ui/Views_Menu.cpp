#include "ui/Views.hpp"

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

} // namespace ssoc::ui::views
