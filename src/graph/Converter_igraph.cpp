#include "Converter.hpp"

namespace ssoc::graph::convert {

void igraph_Deleter::operator()(igraph_t *g) const {
  if (g) {
    igraph_destroy(g);
    delete g;
  }
}

std::unique_ptr<Graph> to_ssoc_graph(const igraph_t &igraph) {}

std::unique_ptr<igraph_t, igraph_Deleter> to_igraph(const Graph &ssoc_graph) {}

} // namespace ssoc::graph::convert
