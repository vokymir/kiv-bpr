#pragma once

#include "Sim_Config.hpp"
#include "Vis_Config.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <vector>
namespace ssoc::graph {

/* Have vertices & edges. Data oriented memory layout for higher efficiency.
 * Last vertex is always a sink vertex.
 * */
class Graph {
private:
  size_t num_vertices_;
  size_t num_edges_;
  /* Store on which index in adj_vertices_ vector starts the list of neighbours
   * (=adjancent vertices) for current vertex.
   *
   * == Example:
   * num_vertices_ = 4
   * adj_offsets_ =
   * [0,2,2,4,6] <- at which index neighbour starts
   * adj_vertices_ =
   *  0 1 2 3 4 5 6   <- indexes
   *  0 0 2 2 3 3 3   <- whose neighbour list is it
   * [1,2,1,0,1,0,3]
   * */
  std::vector<size_t> adj_offsets_{};
  std::vector<size_t> adj_vertices_{};

  /* Sand count for every node.  */
  std::vector<int> sand_height_{};

  /* Store the x,y coordinates in 2D UI layout. */
  std::vector<std::pair<double, double>> layout_pos_{};

  Sim_Config sim_config_;
  Vis_Config vis_config_;

  // =====
  // get/set
public:
  size_t num_vertices() const { return num_vertices_; }
  void num_vertices(size_t count) { num_vertices_ = count; }

  size_t num_edges() const { return num_edges_; }
  void num_edges(size_t count) { num_edges_ = count; }

  std::vector<size_t> &adj_offsets() { return adj_offsets_; }
  const std::vector<size_t> &adj_offsets_const() const { return adj_offsets_; }

  std::vector<size_t> &adj_vertices() { return adj_vertices_; }
  const std::vector<size_t> &adj_vertices_const() const {
    return adj_vertices_;
  }

  std::vector<int> &sand_height() { return sand_height_; }
  const std::vector<int> &sand_height_const() const { return sand_height_; }

  std::vector<std::pair<double, double>> &layout_pos() { return layout_pos_; }
  const std::vector<std::pair<double, double>> &layout_pos_const() const {
    return layout_pos_;
  }

  void sim_cfg(Sim_Config cfg) { sim_config_ = cfg; }
  const Sim_Config &sim_cfg_const() const { return sim_config_; }

  void vis_cfg(Vis_Config cfg) { vis_config_ = cfg; }
  const Vis_Config &vis_cfg_const() const { return vis_config_; }

  // =====
  // smarter getters
public:
  // how many sand grains must be on the vertex for it to topple?
  int vertex_degree(size_t idx) const {
    if (idx >= adj_offsets_.size()) {
      throw std::out_of_range("vertex degree - wrong index");
    }

    const auto start = adj_offsets_[idx];
    const auto end = adj_offsets_[idx + 1];
    const int degree = static_cast<int>(end - start);

    return degree;
  }

  // how much sand is on vertex?
  int vertex_sand(size_t idx) const {
    if (idx >= sand_height_.size()) {
      throw std::out_of_range("vertex sand - wrong index");
    }
    return sand_height_[idx];
  }

  size_t grains_count() const {
    const auto &heights = sand_height_;

    if (heights.empty()) {
      return 0; // nothing to sum
    }

    // safe: take all except last element
    size_t n = heights.size() - 1;
    if (n == 0) {
      return 0; // only one element, ignore it
    }

    auto view = heights | std::views::take(n);

    return static_cast<size_t>(
        std::ranges::fold_left(view, 0, std::plus<int>()));
  }

  std::vector<size_t> get_neighbours(size_t vertex_idx) const {
    size_t start = adj_offsets_[vertex_idx];
    size_t end = adj_offsets_[vertex_idx + 1];

    std::vector<size_t> result;
    for (size_t idx = start; idx < end; ++idx) {
      result.push_back(adj_vertices_[idx]);
    }

    return result;
  }

  // =====
  // copy/move
public:
  Graph(const Graph &) = delete;
  Graph &operator=(const Graph &) = delete;

  Graph(Graph &&) noexcept = default;
  Graph &operator=(Graph &&) noexcept = default;

  // =====
  // constructors
public:
  Graph() {}
};

} // namespace ssoc::graph
