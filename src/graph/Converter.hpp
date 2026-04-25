#pragma once

#include "Graph.hpp"
#include "igraph_lib.hpp"
#include <igraph.h>
#include <memory>
namespace ssoc::graph::convert {

/* Adding support for new converters:
 * add two functions:
 * std::unique_ptr<Graph> to_ssoc_graph(const TYPE&library_name);
 * std::unique_ptr<TYPE[, custom_deleter]> to_library_name(const
 * Graph&ssoc_graph);
 * */

// =====
// Library: igraph
// =====

// convert any igraph to ssoc graph format
std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph);

// convert any ssoc graph to igraph format
// must not be simple unique_ptr because it needs custom deleter
igraph_::unique_ptr_ to_igraph(const Graph &ssoc_graph);

namespace igraph_lib {
// just resize the graphs internals to match the igraph
void to_ssoc__resize_internal_vectors(Graph &g);

void to_ssoc__fill_neighbour_lists(Graph &g, const igraph_t &ig);

} // namespace igraph_lib

} // namespace ssoc::graph::convert
