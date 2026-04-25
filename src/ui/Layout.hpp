#pragma once
#include <imgui.h>

// simple layout engine for dear imgui
// set/get grid cols,rows,padding
// set next window position relative, thanks to this
namespace ssoc::ui::layout {

// setters
void set_grid(int cols, int rows);
void set_padding(float padding);

// getters
int cols();
int rows();
float padding();

// ===
// OPTION 1: set window simply based on grid coordinates

// set the position of next window - uses grid coordinates and translates them
// into real ones
void set_next_window(int gx, int gy, int gw, int gh);

// alternative method of storing window info
struct Grid_Tile {
  int x, y, w, h;
};

void set_next_window(Grid_Tile tile);

// ===
// OPTION 2: use prepared window layout, saved in .cpp

// holds all window types
enum class Window { HELP, BUILDER, VISUALIZER, STATS, CTL_SIM, CTL_VIS };

// set next window based on given window type
void set_next_window(Window window);

// ===
// == helpers to make set_next_window happen

// calculate cell size on this particular device
ImVec2 cell_size();

// convert from grid position to real position
ImVec2 grid_to_pos(int x, int y);

// convert from grid size to real size
ImVec2 grid_to_size(int w, int h);

} // namespace ssoc::ui::layout
