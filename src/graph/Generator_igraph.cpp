#include "Converter.hpp"
#include "Generator.hpp"
#include "Simulation_Setup_Config.hpp"
#include "Visual_Setup_Config.hpp"
#include "igraph_lib.hpp"
#include <algorithm>
#include <cstddef>
#include <format>
#include <igraph.h>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <variant>

namespace ssoc::graph::generate {

std::unique_ptr<Graph> igraph_from_config(const Sim_Config &sim_cfg,
                                          const Vis_Config &vis_cfg) {
  igraph_::unique_ptr_ igraph_graph =
      igraph_lib::generate_igraph(sim_cfg.gga, sim_cfg.sink_rule);

  std::unique_ptr<Graph> graph = convert::to_ssoc_graph(*igraph_graph);

  // store cfg for later reference
  graph->sim_cfg(sim_cfg);
  graph->vis_cfg(vis_cfg);

  igraph_lib::generate_layout(*igraph_graph, *graph, vis_cfg.gla);

  return graph;
}

namespace igraph_lib {

// ===
// GENERATING
// ===

igraph_::unique_ptr_ generate_igraph(const Graph_Generation_Algorithm &gga,
                                     const Sink_Rule &rule) {
  igraph_setup();

  igraph_::igraph_Deleter deleter;
  igraph_::unique_ptr_ igp(new igraph_t, deleter);

  // choose according to config, if any error happens, will propagate up
  std::visit(
      [&igp, &rule](auto generation_alg) {
        using T = std::decay_t<decltype(generation_alg)>;
        if constexpr (std::is_same_v<T, gga_::Square_Lattice>) {
          square_lattice_2d_variant(*igp, generation_alg, rule);
        } else if constexpr (std::is_same_v<T, gga_::Watts_Strogatz_2D>) {
          watts_strogatz_2d_variant(*igp, generation_alg, rule);
        } else if constexpr (std::is_same_v<T, gga_::Erdos_Renyi_nm>) {
          erdos_renyi_edges_variant(*igp, generation_alg, rule);
        } else if constexpr (std::is_same_v<T, gga_::Erdos_Renyi_nm>) {
          erdos_renyi_prob_variant(*igp, generation_alg, rule);
        } else if constexpr (std::is_same_v<T, gga_::Erdos_Renyi_nm>) {
          barabasi_albert_variant(*igp, generation_alg, rule);
        } else {
          throw std::runtime_error("Unknown graph generation algorithm.\n");
        }
      },
      gga);

  return igp;
}

void add_sink(igraph_t &ig, const Sink_Rule &rule) {

  // = find the highest degree, only if necessary
  int max_degree = 0;
  if (rule.type == Sink_Rule::Type::Fill_Up) {
    max_degree = max_vertex_degree(ig);
  }

  // = add sink vertex
  auto num_v = igraph_vcount(&ig);
  igraph_add_vertices(&ig, 1, nullptr);
  auto sink_idx = num_v;

  // = get degrees of all vertices
  igraph_vector_int_t degrees;
  igraph_vector_int_init(&degrees, 0);

  igraph_degree(&ig, &degrees, igraph_vss_range(0, sink_idx), IGRAPH_ALL,
                IGRAPH_LOOPS);

  // = prepare all missing edges
  igraph_vector_int_t edges_to_add;
  igraph_vector_int_init(&edges_to_add, 0);

  for (igraph_integer_t i = 0; i < sink_idx; ++i) {
    auto degree = VECTOR(degrees)[i];

    int missing = 0;
    switch (rule.type) {
      using t = Sink_Rule::Type;

    case t::Fill_To_N:
      missing = std::max(0, rule.parameter - static_cast<int>(degree));
      break;
    case t::All_Once:
      missing = 1;
      break;
    case t::As_Many_As_Nei:
      missing = static_cast<int>(degree);
      break;
    case t::Fill_Up:
      missing = std::max(0, max_degree - static_cast<int>(degree));
      break;
    }

    for (int m = 0; m < missing; ++m) {
      igraph_vector_int_push_back(&edges_to_add, i);
      igraph_vector_int_push_back(&edges_to_add, sink_idx);
    }
  }

  // = add all missing edges
  igraph_add_edges(&ig, &edges_to_add, nullptr);

  // = cleanup
  igraph_vector_int_destroy(&edges_to_add);
  igraph_vector_int_destroy(&degrees);
}

int max_vertex_degree(igraph_t &ig) {
  igraph_vector_int_t degs;
  igraph_vector_int_init(&degs, 0);

  igraph_degree(&ig, &degs, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);

  igraph_integer_t max_deg = igraph_vector_int_max(&degs);

  igraph_vector_int_destroy(&degs);

  return static_cast<int>(max_deg);
}

void square_lattice_2d_variant(igraph_t &ig, const gga_::Square_Lattice &gga,
                               const Sink_Rule &rule) {
  // prepare instructions from gga
  igraph_vector_int_t dimensions;
  igraph_vector_int_init(&dimensions, 2); // always 2D
  VECTOR(dimensions)[0] = gga.width;
  VECTOR(dimensions)[1] = gga.height;

  igraph_vector_bool_t circularity;
  igraph_vector_bool_init(&circularity, 2);
  VECTOR(circularity)[0] = gga.circular_on_x;
  VECTOR(circularity)[1] = gga.circular_on_y;

  // let library generate
  auto err =
      igraph_square_lattice(&ig, &dimensions, 1, false, false, &circularity);

  // delete unuseful igraph structures
  igraph_vector_int_destroy(&dimensions);
  igraph_vector_bool_destroy(&circularity);

  // only throw if error occured
  if (err != IGRAPH_SUCCESS) {
    throw std::runtime_error(
        std::format("External library: igraph: square lattice error: {}",
                    igraph_strerror(err)));
  }

  add_sink(ig, rule);
}

void watts_strogatz_2d_variant(igraph_t &ig, const gga_::Watts_Strogatz_2D &gga,
                               const Sink_Rule &rule) {
  // let library generate
  auto err = igraph_watts_strogatz_game(
      &ig, 2, gga.size, gga.neighbourhood_size, gga.p, IGRAPH_SIMPLE_SW);

  if (err != IGRAPH_SUCCESS) {
    throw std::runtime_error(
        std::format("External library: igraph: watts-strogatz error: {}",
                    igraph_strerror(err)));
  }

  add_sink(ig, rule);
}

void erdos_renyi_edges_variant(igraph_t &ig, const gga_::Erdos_Renyi_nm &gga,
                               const Sink_Rule &rule) {
  auto err = igraph_erdos_renyi_game_gnm(&ig, gga.vertices, gga.edges,
                                         IGRAPH_UNDIRECTED, IGRAPH_NO_LOOPS,
                                         IGRAPH_EDGE_UNLABELED);

  if (err != IGRAPH_SUCCESS) {
    throw std::runtime_error(
        std::format("External library: igraph: erdos-renyi G(n,m) error: {}",
                    igraph_strerror(err)));
  }

  add_sink(ig, rule);
}

void erdos_renyi_prob_variant(igraph_t &ig, const gga_::Erdos_Renyi_np &gga,
                              const Sink_Rule &rule) {
  auto err = igraph_erdos_renyi_game_gnp(
      &ig, gga.vertices, gga.edge_probability, IGRAPH_UNDIRECTED,
      IGRAPH_NO_LOOPS, IGRAPH_EDGE_UNLABELED);

  if (err != IGRAPH_SUCCESS) {
    throw std::runtime_error(
        std::format("External library: igraph: erdos-renyi G(n,p) error: {}",
                    igraph_strerror(err)));
  }

  add_sink(ig, rule);
}

void barabasi_albert_variant(igraph_t &ig, const gga_::Barabasi_Albert &gga,
                             const Sink_Rule &rule) {
  // believe me, the parameters are just what is needed.
  // you can read about them here:
  // https://igraph.org/c/html/latest/igraph-Games.html#igraph_barabasi_game
  auto err = igraph_barabasi_game(&ig, gga.vertices, 1, gga.edges_per_node,
                                  nullptr, false, 1.0, IGRAPH_UNDIRECTED,
                                  IGRAPH_BARABASI_PSUMTREE, nullptr);

  if (err != IGRAPH_SUCCESS) {
    throw std::runtime_error(
        std::format("External library: igraph: barabasi-albert error: {}",
                    igraph_strerror(err)));
  }

  add_sink(ig, rule);
}

// =====
// LAYOUT SECTION
// =====

igraph_error_t
fruchterman_reingold_2d_variant(igraph_t &ig, igraph_matrix_t &layout,
                                const gla_::Fruchterman_Reingold_2D &gla) {
  bool use_seed = false;          // false means random start
  long iteration_count = 500;     // sensible default according to docs
  double start_temperature = 0.1; // longest distance to go in one iteration
  igraph_layout_grid_t grid_usage;
  switch (gla.accuracy) { // decide on whatever was in gla
  case gla_::FR2D_Accuracy::High:
    grid_usage = IGRAPH_LAYOUT_NOGRID;
    break;
  case gla_::FR2D_Accuracy::Low:
    grid_usage = IGRAPH_LAYOUT_GRID;
    break;
  case gla_::FR2D_Accuracy::Auto:
    grid_usage = IGRAPH_LAYOUT_AUTOGRID;
    break;
  }
  // not used
  igraph_vector_t *weights = nullptr;
  igraph_vector_t *min_x = nullptr;
  igraph_vector_t *max_x = nullptr;
  igraph_vector_t *min_y = nullptr;
  igraph_vector_t *max_y = nullptr;

  return igraph_layout_fruchterman_reingold(
      &ig, &layout, use_seed, iteration_count, start_temperature, grid_usage,
      weights, min_x, max_x, min_y, max_y);
}

void layout_igraph_to_graph(Graph &g, const igraph_matrix_t &layout) {
  if (std::holds_alternative<gla_::Hidden_GLA>(g.vis_cfg_const().gla)) {
    return;
  } // don't use valuable RAM space if not possible

  auto &positions = g.layout_pos();
  auto num_vertices = g.num_vertices();
  auto real_num_vertices = num_vertices - 1; // because of sink vertex
  auto max_row = static_cast<size_t>(layout.nrow);

  positions.resize(num_vertices);
  for (size_t v = 0; v < std::min(real_num_vertices, max_row); ++v) {
    const auto row = static_cast<long int>(v);

    double x = MATRIX(layout, row, 0);
    double y = MATRIX(layout, row, 1);

    positions[v] = {x, y};
  }

  // sink vertex is somewhere
  positions[real_num_vertices] = {0, 0};
}

void generate_layout(igraph_t &ig, Graph &g,
                     const Graph_Layout_Algorithm &gla) {
  // subgraph without sink
  igraph_t layout_view;
  igraph_vs_t all_but_sink;

  igraph_vs_range(&all_but_sink, 0, igraph_vcount(&ig) - 1);
  igraph_induced_subgraph(&ig, &layout_view, all_but_sink,
                          IGRAPH_SUBGRAPH_CREATE_FROM_SCRATCH);

  igraph_matrix_t layout;
  igraph_matrix_init(&layout, 0, 0); // will be resized by functions

  // delegate the work
  igraph_error_t err = IGRAPH_SUCCESS;
  std::visit(
      [&layout_view, &layout, &err](auto layout_alg) {
        using T = std::decay_t<decltype(layout_alg)>;
        if constexpr (std::is_same_v<T, gla_::Fruchterman_Reingold_2D>) {
          err =
              fruchterman_reingold_2d_variant(layout_view, layout, layout_alg);
        } else if constexpr (std::is_same_v<T, gla_::Hidden_GLA>) {
          err = IGRAPH_SUCCESS; // no visualization
        } else {
          err = IGRAPH_FAILURE;
        }
      },
      gla);

  igraph_vs_destroy(&all_but_sink);
  igraph_destroy(&layout_view);

  if (err != IGRAPH_SUCCESS) {
    igraph_matrix_destroy(&layout);
    throw std::runtime_error(
        std::format("Failed to compute layout: {}", igraph_strerror(err)));
  }

  layout_igraph_to_graph(g, layout);

  igraph_matrix_destroy(&layout);
}

} // namespace igraph_lib

} // namespace ssoc::graph::generate
