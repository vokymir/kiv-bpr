#include "Converter.hpp"
#include "Generator.hpp"
#include "igraph.h"
#include <memory>

namespace ssoc::graph::generate {

std::unique_ptr<igraph_t> generate_igraph(const ssoc::Sim_Config &cfg) {}

std::unique_ptr<Graph> igraph_from_config(const ssoc::Sim_Config &cfg) {

  std::unique_ptr<igraph_t> igraph_graph = generate_igraph(cfg);

  std::unique_ptr<Graph> graph = graph::convert::to_ssoc_graph(*igraph_graph);

  return graph;
}

} // namespace ssoc::graph::generate
