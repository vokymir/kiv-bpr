#include "Converter.hpp"
#include "igraph_lib.hpp"

namespace ssoc::graph::convert {

std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph) {}

std::unique_ptr<igraph_t, igraph_::igraph_Deleter>
to_igraph(const Graph &ssoc_graph) {}

} // namespace ssoc::graph::convert
