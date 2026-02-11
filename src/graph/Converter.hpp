#pragma once

#include "Graph.hpp"
#include "igraph.h"
#include "igraph_lib.hpp"
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

std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph);

std::unique_ptr<igraph_t, igraph_::igraph_Deleter>
to_igraph(const Graph &ssoc_graph);

} // namespace ssoc::graph::convert
