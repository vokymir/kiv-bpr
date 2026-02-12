#pragma once

#include <format>
#include <print>
#include <stdexcept>
#include <utility>
#include <vector>
namespace ssoc::graph {

/* Have vertices & edges. Data oriented memory layout for higher efficiency. */
class Graph {
private:
  int num_vertices_;
  int num_edges_;
  /* Store on which index in adj_vertices_ vector starts the list of neighbours
   * (=adjancent vertices) for current vertex.
   *
   * == Example:
   * num_vertices_ = 4
   * adj_offsets_ =
   * [0,2,2,4] <- at which index neighbour starts
   * adj_vertices_ =
   *  0 1 2 3 4 5 6   <- indexes
   *  0 0 2 2 3 3 3   <- whose neighbour list is it
   * [1,2,1,0,1,0,3]
   * */
  std::vector<int> adj_offsets_{};
  std::vector<int> adj_vertices_{};

  /* Sand count for every node. It's vectorized for batch runs, every run
   * having it's own vector. */
  std::vector<std::vector<int>> sand_height_{};

  // store where did sand fall in time
  std::vector<std::vector<int>> sand_history_{};

  /* Store the x,y coordinates in 2D UI layout. */
  std::vector<std::pair<double, double>> layout_pos_{};

  // =====
  // get/set
public:
  int num_vertices() const { return num_vertices_; }
  void num_vertices(int count) { num_vertices_ = count; }

  int num_edges() const { return num_edges_; }
  void num_edges(int count) { num_edges_ = count; }

  std::vector<int> &adj_offsets() { return adj_offsets_; }
  const std::vector<int> &adj_offsets() const { return adj_offsets_; }

  std::vector<int> &adj_vertices() { return adj_vertices_; }
  const std::vector<int> &adj_vertices() const { return adj_vertices_; }

  // batch index, default = 0
  std::vector<int> &sand_height(int idx = 0) {
    if (idx < 0) {
      throw std::out_of_range("sand_height index cannot be negative");
    }
    if (static_cast<int>(sand_height_.size()) <= idx) {
      sand_height_.resize(idx + 1); // ensure idx is valid
      sand_history_.resize(idx + 1);
    }
    return sand_height_[idx];
  }
  const std::vector<int> &sand_height_const(int idx = 0) const {
    if (idx < 0 || idx >= static_cast<int>(sand_height_.size())) {
      throw std::out_of_range("sand_height bad index");
    }

    return sand_height_[idx];
  }

  // batch index, default = 0
  std::vector<int> &sand_history(int idx = 0) {
    if (idx < 0) {
      throw std::out_of_range("sand_history index cannot be negative");
    }
    if (static_cast<int>(sand_history_.size()) <= idx) {
      throw std::out_of_range("sand_history index is too big");
    }
    return sand_history_[idx];
  }
  const std::vector<int> &sand_history_const(int idx = 0) const {
    if (idx < 0 || idx >= static_cast<int>(sand_history_.size())) {
      throw std::out_of_range("sand_history bad index");
    }

    return sand_history_[idx];
  }

  std::vector<std::pair<double, double>> &layout_pos() { return layout_pos_; }
  const std::vector<std::pair<double, double>> &layout_pos() const {
    return layout_pos_;
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
