#include "Converter.hpp"
#include "Generator.hpp"
#include "igraph_lib.hpp"
#include <memory>

namespace ssoc::graph::generate {

std::unique_ptr<igraph_t, igraph_::igraph_Deleter>
generate_igraph(const ssoc::Sim_Config &cfg) {
  igraph_setup();

  igraph_::igraph_Deleter deleter;
  igraph_::unique_ptr_ ig(new igraph_t, deleter);

  return ig;
}

std::unique_ptr<Graph> igraph_from_config(const ssoc::Sim_Config &cfg) {

  igraph_::unique_ptr_ igraph_graph = generate_igraph(cfg);

  std::unique_ptr<Graph> graph = graph::convert::to_ssoc_graph(*igraph_graph);

  return graph;
}

} // namespace ssoc::graph::generate
