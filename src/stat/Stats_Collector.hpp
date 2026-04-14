#pragma once

#include "stat/Simulation_Events.hpp"
#include "stat/events.hpp"
#include <cstddef>
#include <vector>
namespace ssoc::stat {

struct Avalanche_Record {
  size_t step;
  size_t size;
  size_t origin;
  int dissipated;
};

class Stats_Collector {
protected:
  // maybe useful in the future?
  std::vector<Avalanche_Record> avalanche_records_;
  // histogram of sizes-freq
  std::vector<size_t> avalanche_sizes_;
  // histogram of origins (vector as long as many vertices)
  std::vector<size_t> avalanche_origins_;
  // histogram, index = vertex_id, value = total grains added there
  std::vector<size_t> grain_dropped_counts_;
  // sequential, for each step the vertex_id (as many as sim steps)
  std::vector<size_t> grain_dropped_history_;
  // history of grain counts on the whole graph (as many as sim steps)
  std::vector<size_t> grains_total_counts_history_;

  size_t avalanche_max_size_ = 0;
  // sum of all sizes
  size_t avalanche_total_sizes_ = 0;
  // in fact a sim steps
  size_t grain_total_dropped_ = 0;
  // how many grains are in sink now
  size_t grain_total_dissipated_ = 0;

  void handle_avalanche(const Avalanche_Event &e) {
    size_t size = e.topples_to_stabilize;
    if (avalanche_sizes_.size() <= size) {
      avalanche_sizes_.resize(size + 1, 0);
    }
    avalanche_sizes_[size]++;

    size_t origin = e.origin_vertex;
    if (avalanche_origins_.size() <= origin) {
      avalanche_origins_.resize(origin + 1, 0);
    }
    avalanche_origins_[origin]++;

    avalanche_records_.push_back({e.step_number, e.topples_to_stabilize,
                                  e.origin_vertex, e.dissipated_grains_count});

    avalanche_total_sizes_ += e.topples_to_stabilize;
    avalanche_max_size_ = avalanche_max_size_ >= e.topples_to_stabilize
                              ? avalanche_max_size_
                              : e.topples_to_stabilize;

    grain_total_dissipated_ += static_cast<size_t>(e.dissipated_grains_count);
  }

  void handle_grains(const Grains_Count_Event &e) {
    grains_total_counts_history_.push_back(e.grains_count);
  }

  void handle_grain_dropped(const Grain_Dropped_Event &e) {
    size_t vertex_id = e.vertex_id;
    if (grain_dropped_counts_.size() <= vertex_id) {
      grain_dropped_counts_.resize(vertex_id + 1);
    }
    grain_dropped_counts_[vertex_id]++;

    grain_dropped_history_.push_back(vertex_id);

    grain_total_dropped_++;
  }

public:
  explicit Stats_Collector(Simulation_Events &events) {
    events.avalanche.subscribe(
        [this](const Avalanche_Event &e) { handle_avalanche(e); });
    events.grains_total.subscribe(
        [this](const Grains_Count_Event &e) { handle_grains(e); });
  }

  void reset() {
    avalanche_records_.clear();
    avalanche_sizes_.clear();
    avalanche_origins_.clear();
    grains_total_counts_history_.clear();

    avalanche_max_size_ = 0;
    avalanche_total_sizes_ = 0;
    grain_total_dropped_ = 0;
    grain_total_dissipated_ = 0;
  }

  // === GETTERS ===

  // maybe useful in the future?
  const std::vector<Avalanche_Record> &avalanche_records() const {
    return avalanche_records_;
  }

  // histogram of sizes-freq
  const std::vector<size_t> &avalanche_sizes() const {
    return avalanche_sizes_;
  }

  // histogram of origins (vector as long as many vertices)
  const std::vector<size_t> &avalanche_origins() const {
    return avalanche_origins_;
  }

  // histogram, index = vertex_id, value = total grains added there
  const std::vector<size_t> &grain_dropped_counts() const {
    return grain_dropped_counts_;
  }

  // sequential, for each step the vertex_id (as many as sim steps)
  const std::vector<size_t> &grain_dropped_history() const {
    return grain_dropped_history_;
  }

  // history of grain counts on the whole graph (as many as sim steps)
  const std::vector<size_t> &grains_total_counts_history() const {
    return grains_total_counts_history_;
  }

  size_t avalanche_max_size() const { return avalanche_max_size_; }

  // sum of all sizes
  size_t avalanche_total_sizes() const { return avalanche_total_sizes_; }

  // in fact a sim steps
  size_t grain_total_dropped() const { return grain_total_dropped_; }

  // how many grains are in sink now
  size_t grain_total_dissipated() const { return grain_total_dissipated_; }

  // at which step in simulation we are now
  size_t steps_count() const { return grain_total_dropped_; }
};

} // namespace ssoc::stat
