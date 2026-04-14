#pragma once

#include "stat/Event_Bus.hpp"
#include "stat/events.hpp"
namespace ssoc::stat {

struct Simulation_Events {
  Event_Bus<Grain_Dropped_Event> grain_dropped;
  Event_Bus<Avalanche_Event> avalanche;
  Event_Bus<Grains_Count_Event> grains_total;
};

} // namespace ssoc::stat
