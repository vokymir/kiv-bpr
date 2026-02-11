#include "Converter.hpp"
#include "igraph.h"
#include "igraph_constants.h"
#include "igraph_interface.h"
#include "igraph_lib.hpp"
#include <print>

namespace ssoc::graph::convert {

std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph) {
  auto graph = std::make_unique<Graph>();

  const igraph_integer_t node_count = igraph_vcount(&igraph);
  const igraph_integer_t edge_count = igraph_ecount(&igraph);

  // ===== Resize internal vectors =====
  // neighbour_idxs_ needs node_count entries
  graph->neighbour_idxs_.resize(node_count);

  // Precompute neighbours size
  graph->neighbours_.resize(
      2 * edge_count); // undirected graph: every edge stored twice

  // Positions vector (optional; set to 0,0 if igraph layout not provided)
  graph->positions_.resize(node_count, {0, 0});

  // Sand: initialize with empty vector (or 0 grains per node)
  graph->sand_.resize(1, std::vector<int>(node_count, 0));

  // ===== Fill neighbour lists =====
  igraph_integer_t idx = 0;
  for (igraph_integer_t v = 0; v < node_count; ++v) {
    graph->neighbour_idxs_[v] = idx;

    igraph_vector_int_t neighbors;
    igraph_vector_int_init(&neighbors, 0);
    if (igraph_neighbors(&igraph, &neighbors, v, IGRAPH_ALL, IGRAPH_NO_LOOPS,
                         IGRAPH_NO_MULTIPLE) != IGRAPH_SUCCESS) {
      igraph_vector_int_destroy(&neighbors);
      throw std::runtime_error("Failed to get neighbors");
    }

    std::print("{}: ", static_cast<int>(v));
    for (long n = 0; n < igraph_vector_int_size(&neighbors); ++n) {
      std::print("{}, ", VECTOR(neighbors)[n]);
      graph->neighbours_[idx++] = VECTOR(neighbors)[n];
    }
    std::print("\n");

    igraph_vector_int_destroy(&neighbors);
  }

  return graph;
}

std::unique_ptr<igraph_t, igraph_::igraph_Deleter>
to_igraph(const Graph &ssoc_graph) {}

} // namespace ssoc::graph::convert
