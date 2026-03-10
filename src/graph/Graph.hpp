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

  /* Sand count for every node. It's vectorized for batch runs, every run
   * having it's own vector. */
  std::vector<std::vector<int>> sand_height_{};

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

  // batch index, default = 0
  std::vector<int> &sand_height(size_t idx = 0) {
    if (sand_height_.size() <= idx) {
      sand_height_.resize(idx + 1, std::vector<int>(num_vertices_ + 1, 0));
    }
    return sand_height_[idx];
  }
  const std::vector<int> &sand_height_const(size_t idx = 0) const {
    if (idx >= sand_height_.size()) {
      throw std::out_of_range("sand_height bad index");
    }

    return sand_height_[idx];
  }

  std::vector<std::pair<double, double>> &layout_pos() { return layout_pos_; }
  const std::vector<std::pair<double, double>> &layout_pos() const {
    return layout_pos_;
  }

  void sim_cfg(Sim_Config cfg) { sim_config_ = cfg; }
  const Sim_Config &sim_cfg() const { return sim_config_; }

  void vis_cfg(Vis_Config cfg) { vis_config_ = cfg; }
  const Vis_Config &vis_cfg() const { return vis_config_; }

  // =====
  // smarter getters
public:
  size_t grains_count(size_t idx) const {
    if (idx >= sand_height_.size()) {
      throw std::out_of_range("grains count - wrong batch index");
    }

    const auto &heights = sand_height_[idx];

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
