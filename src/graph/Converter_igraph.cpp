#include "Converter.hpp"

namespace ssoc::graph::convert {

std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph) {}

igraph_t to_igraph(const Graph &ssoc_graph) {}

} // namespace ssoc::graph::convert
