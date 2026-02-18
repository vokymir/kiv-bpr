#pragma once

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
  Graph_Generation_Algorithm gga;
};

} // namespace ssoc
