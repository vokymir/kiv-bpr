#pragma once

#include <stddef.h>
#include <variant>
namespace ssoc {

// Graph Generation Algorithm
namespace gga_ {

// NOTE: by having Sink_Rule inside the struct, different strategies could have
// different sink rules and still make sense
struct Square_Lattice_2D {
  enum class Sink_Rule {
    Fill_To_Four,
    All_Once,
  };

  int width = 3;
  int height = 4;
  bool circular_on_x = false;
  bool circular_on_y = false;
  Sink_Rule sink_rule = Sink_Rule::Fill_To_Four;
};

struct Watts_Strogatz_2D {
  enum class Sink_Rule {
    All_Once,
    As_Many_As_Nei,
  };

  int size = 3; // total vertexes are size*size (for 2D)
  int neighbourhood_size = 3;
  double p = 0.5; // rewiring probability
  Sink_Rule sink_rule = Sink_Rule::All_Once;
};

} // namespace gga_

using Graph_Generation_Algorithm =
    std::variant<gga_::Square_Lattice_2D, gga_::Watts_Strogatz_2D>;

/* Holds configuration for the simulation. Must be serializable, to save
 * specific config for future reference. */
struct Sim_Config {
  Graph_Generation_Algorithm gga;

  // is the sand-fall random? if not, specify which vertex should be sand thrown
  // at - modulo is used to prevent overflow
  bool random_sand_distribution = true;
  size_t specific_vertex_to_distribute = 0;
};

} // namespace ssoc
