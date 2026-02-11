#include "Converter.hpp"
#include "Generator.hpp"
#include "igraph.h"
#include <memory>
#include <stdexcept>

namespace ssoc::graph::generate {

std::unique_ptr<igraph_t, igraph_::igraph_Deleter>
generate_igraph(const ssoc::Sim_Config &cfg) {
  igraph_setup();

  igraph_::igraph_Deleter deleter;
  igraph_::unique_ptr_ ig(new igraph_t, deleter);

  // create dimension vector
  igraph_vector_int_t dim;
  igraph_vector_int_init(&dim, 2); // 2 dimensions
  VECTOR(dim)[0] = 4;              // rows
  VECTOR(dim)[1] = 6;              // columns

  // generate square lattice
  auto success =
      igraph_square_lattice(ig.get(), &dim, 4, false, false, nullptr);

  igraph_vector_int_destroy(&dim); // free dim

  if (success != IGRAPH_SUCCESS) {
    throw std::runtime_error("Failed to generate square lattice");
  }

  return ig;
}

std::unique_ptr<Graph> igraph_from_config(const ssoc::Sim_Config &cfg) {

  igraph_::unique_ptr_ igraph_graph = generate_igraph(cfg);

  std::unique_ptr<Graph> graph = graph::convert::to_ssoc_graph(*igraph_graph);

  igraph_matrix_t layout;
  igraph_matrix_init(&layout, 0, 0); // will be resized by the function

  igraph_error_t err =
      igraph_layout_fruchterman_reingold(igraph_graph.get(), &layout,
                                         false, // use_seed: false, random start
                                         500,   // niter: iterations
                                         0.1,   // start_temp
                                         IGRAPH_LAYOUT_NOGRID, // grid method
                                         nullptr,              // weights
                                         nullptr, nullptr,     // minx, maxx
                                         nullptr, nullptr      // miny, maxy
      );

  if (err != IGRAPH_SUCCESS) {
    igraph_matrix_destroy(&layout);
    throw std::runtime_error("Failed to compute Fruchterman-Reingold layout");
  }

  auto node_count = graph->node_count();
  auto &positions = graph->positions();

  // Copy layout to Graph::positions_
  positions.resize(node_count);
  for (long v = 0; v < node_count; ++v) {
    double x = MATRIX(layout, v, 0);
    double y = MATRIX(layout, v, 1);
    positions[v] = {x, y};
  }

  igraph_matrix_destroy(&layout);

  return graph;
}

} // namespace ssoc::graph::generate
