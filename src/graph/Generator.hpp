#pragma once

#include "../Sim_Config.hpp"
#include "Graph.hpp"
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

std::unique_ptr<Graph> igraph_from_config(const ssoc::Sim_Config &cfg);

} // namespace ssoc::graph::generate
