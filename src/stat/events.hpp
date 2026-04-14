#pragma once

#include <cstddef>
namespace ssoc::stat {

struct Grain_Dropped_Event {
  size_t vertex_id;
};

struct Avalanche_Event {
  size_t topples_to_stabilize;
  size_t origin_vertex;
  size_t step_number;
  int dissipated_grains_count;
};

struct Grains_Count_Event {
  size_t grains_count;
};

} // namespace ssoc::stat
