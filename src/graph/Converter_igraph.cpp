#include "Converter.hpp"
#include "igraph.h"
#include "igraph_constants.h"
#include "igraph_interface.h"
#include "igraph_lib.hpp"

namespace ssoc::graph::convert {

std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph) {
  auto graph = std::make_unique<Graph>();

  const igraph_integer_t node_count = igraph_vcount(&igraph);
  const igraph_integer_t edge_count = igraph_ecount(&igraph);

  graph->node_count(node_count);

  // ===== Resize internal vectors =====
  // neighbour_idxs_ needs node_count entries
  auto neighbour_idxs = graph->neighbour_idxs();
  auto &neighbours_arr = graph->neighbours();
  auto &positions = graph->positions();
  auto &sand = graph->sand(0);

  neighbour_idxs.resize(node_count);

  // Precompute neighbours size
  neighbours_arr.resize(
      2 * edge_count); // undirected graph: every edge stored twice

  // Positions vector (optional; set to 0,0 if igraph layout not provided)
  positions.resize(node_count, {0, 0});

  // Sand: initialize with empty vector (or 0 grains per node)
  sand.resize(node_count);

  // ===== Fill neighbour lists =====
  igraph_integer_t idx = 0;
  for (igraph_integer_t v = 0; v < node_count; ++v) {
    neighbour_idxs[v] = idx;

    igraph_vector_int_t neighbors;
    igraph_vector_int_init(&neighbors, 0);
    if (igraph_neighbors(&igraph, &neighbors, v, IGRAPH_ALL, IGRAPH_NO_LOOPS,
                         IGRAPH_NO_MULTIPLE) != IGRAPH_SUCCESS) {
      igraph_vector_int_destroy(&neighbors);
      throw std::runtime_error("Failed to get neighbors");
    }

    for (long n = 0; n < igraph_vector_int_size(&neighbors); ++n) {
      neighbours_arr[idx++] = VECTOR(neighbors)[n];
    }

    igraph_vector_int_destroy(&neighbors);
  }

  return graph;
}

std::unique_ptr<igraph_t, igraph_::igraph_Deleter>
to_igraph(const Graph &ssoc_graph) {}

} // namespace ssoc::graph::convert
