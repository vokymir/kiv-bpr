#pragma once

#include "Vis_Config.hpp"
#include <variant>
namespace ssoc {

// Graph Generation Algorithm
namespace gga_ {

struct Square_Lattice_2D {
  int width;
  int height;
  bool circular_on_x;
  bool circular_on_y;
};

struct Dummy_GGA {
  int size;
  bool boolean;
  float ft;
};

} // namespace gga_

using Graph_Generation_Algorithm =
    std::variant<gga_::Square_Lattice_2D, gga_::Dummy_GGA>;

/* Holds configuration for the simulation. Must be serializable, to save
 * specific config for future reference. */
struct Sim_Config {
  struct Vis_Config visual;
  Graph_Generation_Algorithm gga;
};

} // namespace ssoc
