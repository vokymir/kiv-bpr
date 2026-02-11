#include "Converter.hpp"
#include "igraph.h"
#include "igraph_constants.h"
#include "igraph_interface.h"
#include "igraph_lib.hpp"

namespace ssoc::graph::convert {

// just resize the graphs internals to match the igraph
void to_ssoc__resize_internal_vectors(Graph &g) {

  // as many offsets as vertices + 1 for the last one
  g.adj_offsets().resize(g.num_vertices() + 1);

  // undirected = every edge twice
  g.adj_vertices().resize(2 * g.num_edges());

  // if no layout provided, set all nodes to be at origin
  g.layout_pos().resize(g.num_vertices(), {0, 0});

  // initialize sand with 0 on every node
  // sand_height[0] because we only have one when converting from igraph
  g.sand_height(0).resize(g.num_vertices(), 0);
}

void to_ssoc__fill_neighbour_lists(Graph &g, const igraph_t &ig) {

  auto &adj_offsets = g.adj_offsets();
  auto &adj_vertices = g.adj_vertices();

  igraph_integer_t idx = 0;
  for (igraph_integer_t v = 0; v < g.num_vertices(); ++v) {
    adj_offsets[v] = idx;

    // find neighbours in igraph
    igraph_vector_int_t neighbours;
    igraph_vector_int_init(&neighbours, 0);

    if (igraph_neighbors(&ig, &neighbours, v, IGRAPH_ALL, IGRAPH_NO_LOOPS,
                         IGRAPH_NO_MULTIPLE) != IGRAPH_SUCCESS) {
      igraph_vector_int_destroy(&neighbours);
      throw std::runtime_error("Failed to get neighbors");
    }

    // copy neighbours from igraph format
    for (long n = 0; n < igraph_vector_int_size(&neighbours); ++n) {
      adj_vertices[idx++] = VECTOR(neighbours)[n];
    }

    // memory management
    igraph_vector_int_destroy(&neighbours);
  }
}

std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph) {
  auto graph = std::make_unique<Graph>();

  auto node_count = igraph_vcount(&igraph);
  auto edge_count = igraph_ecount(&igraph);

  graph->num_vertices(node_count);
  graph->num_edges(edge_count);

  to_ssoc__resize_internal_vectors(*graph);

  to_ssoc__fill_neighbour_lists(*graph, igraph);

  return graph;
}

// NOTE: won't be able to convert sand
igraph_::unique_ptr_ to_igraph([[maybe_unused]] const Graph &ssoc_graph) {
  // TODO: this is only useful if allowed changing layout after generating graph
  // NOTE: [[maybe]] and null ptr is only to avoid warns
  return nullptr;
}

} // namespace ssoc::graph::convert
