#include "Converter.hpp"
#include "Generator.hpp"
#include "igraph.h"
#include <format>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <variant>

namespace ssoc::graph::generate {

// generate 2D square lattice based on config->gga instruction
void square_lattice_2d_variant(igraph_t &g, gga_::Square_Lattice_2D gga) {
  // prepare instructions from gga
  igraph_vector_int_t dimensions;
  igraph_vector_int_init(&dimensions, 2); // always 2D
  VECTOR(dimensions)[0] = gga.width;
  VECTOR(dimensions)[1] = gga.height;

  igraph_vector_bool_t circularity;
  igraph_vector_bool_init(&circularity, 2);
  VECTOR(circularity)[0] = gga.circular_on_x;
  VECTOR(circularity)[1] = gga.circular_on_y;

  // let library generate
  auto err =
      igraph_square_lattice(&g, &dimensions, 1, false, false, &circularity);

  // delete unuseful igraph structures
  igraph_vector_int_destroy(&dimensions);
  igraph_vector_bool_destroy(&circularity);

  // only throw if error occured
  if (err != IGRAPH_SUCCESS) {
    throw std::runtime_error(
        std::format("External library: igraph: square lattice error: {}",
                    igraph_strerror(err)));
  }
}

// generate igraph
// - delegate the generation to specialized method based on configs' gga
std::unique_ptr<igraph_t, igraph_::igraph_Deleter>
generate_igraph(Graph_Generation_Algorithm gga) {
  igraph_setup();

  igraph_::igraph_Deleter deleter;
  igraph_::unique_ptr_ ig(new igraph_t, deleter);

  // choose according to config, if any error happens, will propagate up
  std::visit(
      [&ig](auto generation_alg) {
        using T = std::decay_t<decltype(generation_alg)>;
        if constexpr (std::is_same_v<T, gga_::Square_Lattice_2D>) {
          square_lattice_2d_variant(*ig, generation_alg);
        } else {
          throw std::runtime_error("Unknown graph generation algorithm.\n");
        }
      },
      gga);

  return ig;
}

// =====
// LAYOUT SECTION
// =====

// generate the layout of igraph g and save it to matrix layout
// use instruction stored in gla
// return error code - for better error propagation and memory work (only one
// free in mother function)
igraph_error_t
fruchterman_reingold_2d_variant(igraph_t &g, igraph_matrix_t &layout,
                                gla_::Fruchterman_Reingold_2D gla) {
  bool use_seed = false;          // false means random start
  long iteration_count = 500;     // sensible default according to docs
  double start_temperature = 0.1; // longest distance to go in one iteration
  igraph_layout_grid_t grid_usage;
  switch (gla.accuracy) { // decide on whatever was in gla
  case gla_::High:
    grid_usage = IGRAPH_LAYOUT_NOGRID;
    break;
  case gla_::Low:
    grid_usage = IGRAPH_LAYOUT_GRID;
    break;
  case gla_::Auto:
    grid_usage = IGRAPH_LAYOUT_AUTOGRID;
    break;
  }
  // not used
  igraph_vector_t *weights = nullptr;
  igraph_vector_t *min_x = nullptr;
  igraph_vector_t *max_x = nullptr;
  igraph_vector_t *min_y = nullptr;
  igraph_vector_t *max_y = nullptr;

  return igraph_layout_fruchterman_reingold(
      &g, &layout, use_seed, iteration_count, start_temperature, grid_usage,
      weights, min_x, max_x, min_y, max_y);
}

// convert layout from igraph to graph
void layout_igraph_to_graph(const igraph_matrix_t &layout, Graph &graph) {
  std::vector<std::pair<double, double>> positions = graph.positions();
  auto node_count = graph.node_count();

  positions.resize(node_count);
  for (long v = 0; v < node_count; ++v) {
    double x = MATRIX(layout, v, 0);
    double y = MATRIX(layout, v, 1);
    positions[v] = {x, y};
  }
}

// - decide which algorithm to use & delegate the work
// - save the layout from igraph format to graph
// - free any igraph remaining memory
void generate_layout(igraph_t &igraph_graph, Graph &graph,
                     Graph_Layout_Algorithm gla) {
  igraph_matrix_t layout;
  igraph_matrix_init(&layout, 0, 0); // will be resized by functions

  // delegate the work
  igraph_error_t err = IGRAPH_SUCCESS;
  std::visit(
      [&igraph_graph, &layout, &err](auto layout_alg) {
        using T = std::decay_t<decltype(layout_alg)>;
        if constexpr (std::is_same_v<T, gla_::Fruchterman_Reingold_2D>) {
          err =
              fruchterman_reingold_2d_variant(igraph_graph, layout, layout_alg);
        } else {
          igraph_matrix_destroy(&layout);
          throw std::runtime_error("Unknown graph layout algorithm.");
        }
      },
      gla);

  if (err != IGRAPH_SUCCESS) {
    igraph_matrix_destroy(&layout);
    throw std::runtime_error(
        std::format("Failed to compute layout: {}", igraph_strerror(err)));
  }

  layout_igraph_to_graph(layout, graph);

  igraph_matrix_destroy(&layout);
}

std::unique_ptr<Graph> igraph_from_config(const ssoc::Sim_Config &cfg) {
  igraph_::unique_ptr_ igraph_graph = generate_igraph(cfg.gga);

  std::unique_ptr<Graph> graph = graph::convert::to_ssoc_graph(*igraph_graph);

  generate_layout(*igraph_graph, *graph, cfg.visual.gla);

  return graph;
}

} // namespace ssoc::graph::generate
