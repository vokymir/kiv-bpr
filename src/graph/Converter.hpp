#pragma once

#include "Graph.hpp"
#include "igraph_datatype.h"
#include <memory>
namespace ssoc::graph::convert {

/* Adding support for new converters:
 * add two functions:
 * std::unique_ptr<Graph> to_ssoc_graph(const TYPE&library_name);
 * TYPE to_library_name(const Graph&ssoc_graph);
 * */

std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph);
igraph_t to_igraph(const Graph &ssoc_graph);

} // namespace ssoc::graph::convert
