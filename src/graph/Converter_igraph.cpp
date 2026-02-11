#include "Converter.hpp"
#include "igraph.h"
#include "igraph_constants.h"
#include "igraph_interface.h"
#include "igraph_lib.hpp"

namespace ssoc::graph::convert {

// just resize the graphs internals to match the igraph
void to_ssoc__resize_internal_vectors(Graph &graph) {
  auto node_count = graph.num_vertices();
  auto edge_count = graph.num_edge();

  auto neighbour_idxs = graph.adj_offsets();
  auto &neighbours_arr = graph.adj_vertices();
  auto &positions = graph.layout_pos();
  auto &sand = graph.sand_height(0);

  neighbour_idxs.resize(node_count);

  // undirected = every edge twice
  neighbours_arr.resize(2 * edge_count);

  // if no layout provided, set all nodes to be at origin
  positions.resize(node_count, {0, 0});

  // initialize sand with 0 on every node
  sand.resize(node_count);
}

void to__fill_neighbour_lists(Graph &g, const igraph_t &ig) {
  auto node_count = g.num_vertices();

  auto neighbour_idxs = g.adj_offsets();
  auto &neighbours_arr = g.adj_vertices();

  igraph_integer_t idx = 0;
  for (igraph_integer_t v = 0; v < node_count; ++v) {
    neighbour_idxs[v] = idx;

    igraph_vector_int_t neighbors;
    igraph_vector_int_init(&neighbors, 0);
    if (igraph_neighbors(&ig, &neighbors, v, IGRAPH_ALL, IGRAPH_NO_LOOPS,
                         IGRAPH_NO_MULTIPLE) != IGRAPH_SUCCESS) {
      igraph_vector_int_destroy(&neighbors);
      throw std::runtime_error("Failed to get neighbors");
    }

    for (long n = 0; n < igraph_vector_int_size(&neighbors); ++n) {
      neighbours_arr[idx++] = VECTOR(neighbors)[n];
    }

    igraph_vector_int_destroy(&neighbors);
  }
}

std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph) {
  auto graph = std::make_unique<Graph>();

  auto node_count = igraph_vcount(&igraph);
  auto edge_count = igraph_ecount(&igraph);

  graph->num_vertices(node_count);
  graph->num_edge(edge_count);

  to_ssoc__resize_internal_vectors(*graph);

  to__fill_neighbour_lists(*graph, igraph);

  return graph;
}

// NOTE: won't be able to convert sand
std::unique_ptr<igraph_t, igraph_::igraph_Deleter>
to_igraph([[maybe_unused]] const Graph &ssoc_graph) {
  // TODO: this is only useful if allowed changing layout after generating graph
  // NOTE: [[maybe]] and null ptr is only to avoid warns
  return nullptr;
}

} // namespace ssoc::graph::convert
