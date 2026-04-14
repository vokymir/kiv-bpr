#pragma once

#include "stat/Simulation_Events.hpp"
#include "stat/events.hpp"
#include <cstddef>
#include <unordered_map>
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
  std::vector<Avalanche_Record> avalanche_records_;
  std::unordered_map<size_t, size_t> avalanche_sizes_;
  std::unordered_map<size_t, size_t> avalanche_origins_;
  std::vector<size_t> grain_dropped_counts_;
  size_t avalanche_total_sizes_ = 0;
  size_t avalanche_max_size_ = 0;

  std::vector<size_t> grains_counts_;

  void handle_avalanche(const Avalanche_Event &e) {
    avalanche_sizes_[e.topples_to_stabilize]++;
    avalanche_origins_[e.origin_vertex]++;

    avalanche_records_.push_back({e.step_number, e.topples_to_stabilize,
                                  e.origin_vertex, e.dissipated_grains_count});

    avalanche_total_sizes_ += e.topples_to_stabilize;
    avalanche_max_size_ = avalanche_max_size_ >= e.topples_to_stabilize
                              ? avalanche_max_size_
                              : e.topples_to_stabilize;
  }

  void handle_grains(const Grains_Count_Event &e) {
    grains_counts_.push_back(e.grains_count);
  }

  void handle_grain_dropped(const Grain_Dropped_Event &e) {
    grain_dropped_counts_.resize(e.vertex_id);
    grain_dropped_counts_[e.vertex_id]++;
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
    grains_counts_.clear();
    avalanche_total_sizes_ = 0;
    avalanche_max_size_ = 0;
  }

  // === GETTERS ===
  const std::vector<Avalanche_Record> &avalanche_records() const {
    return avalanche_records_;
  }

  const std::unordered_map<size_t, size_t> &avalanche_sizes() const {
    return avalanche_sizes_;
  }
  size_t avalanche_max_size() const { return avalanche_max_size_; }
  size_t avalanche_total_sizes() const { return avalanche_total_sizes_; }

  const std::unordered_map<size_t, size_t> &avalanche_origins() const {
    return avalanche_origins_;
  }

  const std::vector<size_t> &grains_counts() const { return grains_counts_; }
};

} // namespace ssoc::stat
