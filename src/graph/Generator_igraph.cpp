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

  return graph;
}

} // namespace ssoc::graph::generate
