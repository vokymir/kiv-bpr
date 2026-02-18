#pragma once

#include "Graph.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include <memory>
namespace ssoc::graph::generate {

/* Adding support for new generators:
 * add the function:
 * std::unique_ptr<Graph> (lib-name)_from_config(const ssoc::Sim_Config &cfg);
 *
 * and that function must generate the graph using library or whatever,
 * optionaly use corresponding converter and return ssoc graph
 * */

// =====
// Library: igraph
// =====

// 1. generate igraph based on config
// 2. convert to ssoc::Graph
// 3. generate layout for that graph
std::unique_ptr<Graph> igraph_from_config(const Sim_Config &sim_cfg,
                                          const Vis_Config &vis_cfg);

} // namespace ssoc::graph::generate
