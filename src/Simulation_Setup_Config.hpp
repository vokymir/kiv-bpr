#pragma once

#include <stddef.h>
#include <variant>
namespace ssoc {

// Graph Generation Algorithm
namespace gga_ {

struct Square_Lattice {
  int width = 3;
  int height = 4;
  bool circular_on_x = false;
  bool circular_on_y = false;
};

struct Watts_Strogatz_2D {
  int size = 3; // total vertices are size*size (for 2D)
  int neighbourhood_size = 3;
  double p = 0.5; // rewiring probability
};

struct Erdos_Renyi_nm {
  int vertices = 9;
  int edges = 3;
};

struct Erdos_Renyi_np {
  int vertices = 9;
  double edge_probability = 0.5;
};

struct Barabasi_Albert {
  int vertices = 9;
  int edges_per_node = 3;
};

} // namespace gga_

using Graph_Generation_Algorithm =
    std::variant<gga_::Square_Lattice, gga_::Watts_Strogatz_2D,
                 gga_::Erdos_Renyi_nm, gga_::Erdos_Renyi_np,
                 gga_::Barabasi_Albert>;

// Strategy for how many times a sink should be connected to all vertices.
// parameter IS ONLY used for N in type Fill_To_N. otherwise it may be
// overwritten to any value
struct Sink_Rule {
  enum class Type {
    Fill_To_N,
    All_Once,
    As_Many_As_Nei,
    Fill_Up,
  };

  Type type;
  int parameter;

  Sink_Rule() : type(Type::All_Once) {}
  Sink_Rule(Type t, int p) : type(t), parameter(p) {}
};

/* Holds configuration for the simulation. Must be serializable, to save
 * specific config for future reference. */
struct Sim_Config {
  Graph_Generation_Algorithm gga;
  Sink_Rule sink_rule = Sink_Rule();

  // is the sand-fall random? if not, specify which vertex should be sand thrown
  // at - modulo is used to prevent overflow
  bool random_sand_distribution = true;
  size_t specific_vertex_to_distribute = 0;
};

} // namespace ssoc
