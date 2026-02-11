#pragma once

#include <format>
#include <stdexcept>
#include <utility>
#include <vector>
namespace ssoc::graph {

/* Have nodes & edges. Data oriented memory layout for higher efficiency. */
class Graph {
private:
  /* How many nodes are in a graph. */
  int node_count_;
  /* Store on which index in neighbours_ vector starts the list of neighbours
   * for current node.
   *
   * == Example:
   * node_count = 4
   * neighbour_idxs_ =
   * [0,2,2,4] <- at which index neighbour starts
   * neighbours_ =
   *  0 1 2 3 4 5 6   <- indexes
   *  0 0 2 2 3 3 3   <- whose neighbour list is it
   * [1,2,1,0,1,0,3]
   * */
  std::vector<int> neighbour_idxs_;
  /* Neighbours list, to know how many neighbours for node exists, look into
   * neighbour_idxs_ list. */
  std::vector<int> neighbours_;

  /* Sand count for every node. It's vectorized for batch runs, every run
   * having it's own vector. */
  std::vector<std::vector<int>> sand_;

  /* Store the x,y coordinates in 2D UI layout. */
  std::vector<std::pair<double, double>> positions_;

public:
  int node_count() const { return node_count_; }
  void node_count(int count) { node_count_ = count; }

  std::vector<int> &neighbour_idxs() { return neighbour_idxs_; }
  const std::vector<int> &neighbour_idxs() const { return neighbour_idxs_; }

  std::vector<int> &neighbours() { return neighbours_; }
  const std::vector<int> &neighbours() const { return neighbours_; }

  std::vector<int> &sand(int idx) {
    while (static_cast<int>(sand_.size()) <= idx) {
      sand_.push_back({});
    }
    return sand_[idx];
  }

  std::vector<std::pair<double, double>> &positions() { return positions_; }
  const std::vector<std::pair<double, double>> &positions() const {
    return positions_;
  }
};

} // namespace ssoc::graph
