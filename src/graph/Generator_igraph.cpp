#include "Converter.hpp"
#include "Generator.hpp"
#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include "igraph_lib.hpp"
#include <algorithm>
#include <format>
#include <igraph.h>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <variant>

namespace ssoc::graph::generate {

// generate 2D square lattice based on config->gga instruction
void square_lattice_2d_variant(igraph_t &ig,
                               const gga_::Square_Lattice_2D &gga) {
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

  // SINK RULES

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
    switch (gga.sink_rule) {
    case gga_::Square_Lattice_2D::Sink_Rule::All_Once:
      missing = 1;
      break;
    case gga_::Square_Lattice_2D::Sink_Rule::Fill_To_Four:
      missing = std::max(0, 4 - static_cast<int>(degree));
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

// generate igraph
// - delegate the generation to specialized method based on configs' gga
igraph_::unique_ptr_ generate_igraph(const Graph_Generation_Algorithm &gga) {
  igraph_setup();

  igraph_::igraph_Deleter deleter;
  igraph_::unique_ptr_ igp(new igraph_t, deleter);

  // choose according to config, if any error happens, will propagate up
  std::visit(
      [&igp](auto generation_alg) {
        using T = std::decay_t<decltype(generation_alg)>;
        if constexpr (std::is_same_v<T, gga_::Square_Lattice_2D>) {
          square_lattice_2d_variant(*igp, generation_alg);
        } else {
          throw std::runtime_error("Unknown graph generation algorithm.\n");
        }
      },
      gga);

  return igp;
}

// =====
// LAYOUT SECTION
// =====

// generate the layout of igraph g and save it to matrix layout
// use instruction stored in gla
// return error code - for better error propagation and memory work (only one
// free in mother function)
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

// convert layout from igraph to graph
void layout_igraph_to_graph(Graph &g, const igraph_matrix_t &layout) {
  auto &positions = g.layout_pos();
  auto num_vertices = g.num_vertices();
  auto real_num_vertices = num_vertices - 1; // because of sink vertex

  positions.resize(num_vertices);
  for (size_t v = 0; v < real_num_vertices; ++v) {
    const auto row = static_cast<long int>(v);

    double x = MATRIX(layout, row, 0);
    double y = MATRIX(layout, row, 1);

    positions[v] = {x, y};
  }

  // sink vertex is somewhere
  positions[real_num_vertices] = {0, 0};
}

// - decide which algorithm to use & delegate the work
// - save the layout from igraph format to graph
// - free any igraph remaining memory
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

std::unique_ptr<Graph> igraph_from_config(const Sim_Config &sim_cfg,
                                          const Vis_Config &vis_cfg) {
  igraph_::unique_ptr_ igraph_graph = generate_igraph(sim_cfg.gga);

  std::unique_ptr<Graph> graph = convert::to_ssoc_graph(*igraph_graph);

  generate_layout(*igraph_graph, *graph, vis_cfg.gla);

  return graph;
}

} // namespace ssoc::graph::generate
