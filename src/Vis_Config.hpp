#pragma once

#include <variant>
namespace ssoc {

// Graph Layout Algorithm
namespace gla_ {

enum struct FR2D_Accuracy {
  High,
  Low, // uses GRID variant of alg
  Auto // use GRID for LARGE graphs
};

struct Fruchterman_Reingold_2D {
  FR2D_Accuracy accuracy = FR2D_Accuracy::High;
};

struct Hidden_GLA {};

} // namespace gla_

using Graph_Layout_Algorithm =
    std::variant<gla_::Fruchterman_Reingold_2D, gla_::Hidden_GLA>;

/* Holds configuration for the visualisation of one simulation. Must be
 * serializable, to save specific config for future reference. */
struct Vis_Config {
  Graph_Layout_Algorithm gla;

  // if false, will show as colour
  bool show_as_size = true;

  // catch-all for all types of numbers
  bool show_numbers = true;
};

} // namespace ssoc
