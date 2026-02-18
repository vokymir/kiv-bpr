#pragma once

#include "stat/Event_Bus.hpp"
#include "stat/events.hpp"
namespace ssoc::stat {

struct Simulation_Events {
  Event_Bus<Avalanche_Event> avalanche;
  Event_Bus<Grains_Count_Event> grains;
};

} // namespace ssoc::stat
