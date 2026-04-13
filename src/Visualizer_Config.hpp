#pragma once

namespace ssoc {

struct Visualizer_Config {
  float zoom_speed = 0.01f;
  float vertex_base_size = 5.0f;

  // if false, will show as colour
  bool show_as_size = true;

  // catch-all for all types of numbers
  bool show_numbers = true;
};
} // namespace ssoc
