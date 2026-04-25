#include "Layout.hpp"

namespace ssoc::ui::layout {

// HARDCODED PRESET
static Grid_Tile get_tile(Window window) {
  switch (window) {
  case Window::HELP:
    return {0, 0, 8, 7};
  case Window::BUILDER:
    return {1, 1, 6, 5};
  case Window::VISUALIZER:
    return {0, 3, 5, 4};
  case Window::STATS:
    return {5, 0, 3, 7};
  case Window::CTL_VIS:
    return {0, 0, 2, 3};
  case Window::CTL_SIM:
    return {2, 0, 3, 3};
  default:
    return {0, 0, 1, 1};
  }
}

// VARIABLE SETTING

int g_cols = 8;
int g_rows = 5;
float g_padding = 5.0f;

void set_grid(int c, int r) {
  g_cols = c;
  g_rows = r;
}
void set_padding(float p) { g_padding = p; }

int cols() { return g_cols; }
int rows() { return g_rows; }
float padding() { return g_padding; }

// == !!! ==

void set_next_window(int gx, int gy, int gw, int gh) {
  ImGui::SetNextWindowPos(grid_to_pos(gx, gy), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(grid_to_size(gw, gh), ImGuiCond_FirstUseEver);
}

void set_next_window(Grid_Tile tile) {
  set_next_window(tile.x, tile.y, tile.w, tile.h);
}

void set_next_window(Window window) {
  set_grid(8, 7);
  set_padding(5.0f);

  set_next_window(get_tile(window));
}

// == helpers

ImVec2 cell_size() {
  ImVec2 screen = ImGui::GetIO().DisplaySize;

  // don't overlap the menu bar
  float menu_bar_h = ImGui::GetFrameHeight();
  float usable_h = screen.y - menu_bar_h;

  float cell_w = (screen.x - static_cast<float>(g_cols + 1) * g_padding) /
                 static_cast<float>(g_cols);

  float cell_h = (usable_h - static_cast<float>(g_rows + 1) * g_padding) /
                 static_cast<float>(g_rows);

  return ImVec2(cell_w, cell_h);
}

ImVec2 grid_to_pos(int x, int y) {
  ImVec2 cell = cell_size();
  float menu_bar_h = ImGui::GetFrameHeight();

  return ImVec2(g_padding + static_cast<float>(x) * (cell.x + g_padding),
                menu_bar_h + g_padding +
                    static_cast<float>(y) * (cell.y + g_padding));
}

ImVec2 grid_to_size(int w, int h) {
  ImVec2 cell = cell_size();

  return ImVec2(
      static_cast<float>(w) * cell.x + static_cast<float>(w - 1) * g_padding,
      static_cast<float>(h) * cell.y + static_cast<float>(h - 1) * g_padding);
}

} // namespace ssoc::ui::layout
