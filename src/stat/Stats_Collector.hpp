#pragma once

#include "stat/Simulation_Events.hpp"
#include "stat/events.hpp"
#include <cstddef>
#include <vector>
namespace ssoc::stat {

class Stats_Collector {
protected:
  std::vector<size_t> avalanche_sizes_;
  std::vector<size_t> avalanche_origins_;
  std::vector<size_t> avalanche_step_stemps_;
  std::vector<size_t> grains_counts_;

  void handle_avalanche(const Avalanche_Event &e) {
    avalanche_sizes_.push_back(e.topples_to_stabilize);
    avalanche_origins_.push_back(e.origin_vertex);
    avalanche_step_stemps_.push_back(e.step_number);
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
};

} // namespace ssoc::stat
