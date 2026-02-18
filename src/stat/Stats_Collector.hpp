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
};

class Stats_Collector {
protected:
  std::vector<Avalanche_Record> avalanche_records_;
  std::unordered_map<size_t, size_t> avalanche_sizes_;
  std::unordered_map<size_t, size_t> avalanche_origins_;
  std::vector<size_t> grains_counts_;

  void handle_avalanche(const Avalanche_Event &e) {
    avalanche_sizes_[e.topples_to_stabilize]++;
    avalanche_origins_[e.origin_vertex]++;

    avalanche_records_.push_back(
        {e.step_number, e.topples_to_stabilize, e.origin_vertex});
  }

  void handle_grains(const Grains_Count_Event &e) {
    grains_counts_.push_back(e.grains_count);
  }

public:
  explicit Stats_Collector(Simulation_Events &events) {
    events.avalanche.subscribe(
        [this](const Avalanche_Event &e) { handle_avalanche(e); });
    events.grains.subscribe(
        [this](const Grains_Count_Event &e) { handle_grains(e); });
  }

  // === GETTERS ===
  const std::vector<Avalanche_Record> &avalanche_records() const {
    return avalanche_records_;
  }

  const std::unordered_map<size_t, size_t> &avalanche_sizes() const {
    return avalanche_sizes_;
  }

  const std::unordered_map<size_t, size_t> &avalanche_origins() const {
    return avalanche_origins_;
  }

  const std::vector<size_t> &grains_counts() const { return grains_counts_; }
};

} // namespace ssoc::stat
