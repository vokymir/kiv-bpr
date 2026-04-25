#pragma once

#include "Graph.hpp"
#include "Simulation_Setup_Config.hpp"
#include "Visual_Setup_Config.hpp"
#include "graph/igraph_lib.hpp"
#include <igraph.h>
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

namespace igraph_lib {

// ===
// GENERATING
// ===

// generate igraph
// - delegate the generation to specialized method based on configs' gga
igraph_::unique_ptr_ generate_igraph(const Graph_Generation_Algorithm &gga,
                                     Sink_Rule &rule);

// generate square lattice based on config->gga instruction
void square_lattice_2d_variant(igraph_t &ig, const gga_::Square_Lattice &gga,
                               Sink_Rule &rule);

// generate WS (in 2 dimensions) from config
void watts_strogatz_2d_variant(igraph_t &ig, const gga_::Watts_Strogatz_2D &gga,
                               Sink_Rule &rule);

void erdos_renyi_edges_variant(igraph_t &ig, const gga_::Erdos_Renyi_nm &gga,
                               Sink_Rule &rule);

void erdos_renyi_prob_variant(igraph_t &ig, const gga_::Erdos_Renyi_np &gga,
                              Sink_Rule &rule);

void barabasi_albert_variant(igraph_t &ig, const gga_::Barabasi_Albert &gga,
                             Sink_Rule &rule);

// ===
// LAYOUT
// ===

// generate the layout of igraph g and save it to matrix layout
// use instruction stored in gla
// return error code - for better error propagation and memory work (only one
// free in mother function)
igraph_error_t
fruchterman_reingold_2d_variant(igraph_t &ig, igraph_matrix_t &layout,
                                const gla_::Fruchterman_Reingold_2D &gla);

// convert layout from igraph to graph
void layout_igraph_to_graph(Graph &g, const igraph_matrix_t &layout);

// - decide which algorithm to use & delegate the work
// - save the layout from igraph format to graph
// - free any igraph remaining memory
void generate_layout(igraph_t &ig, Graph &g, const Graph_Layout_Algorithm &gla);

} // namespace igraph_lib

} // namespace ssoc::graph::generate
